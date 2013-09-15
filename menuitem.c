#include "menuitem.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>

// cNopacityMenuItem  -------------

cNopacityMenuItem::cNopacityMenuItem(cOsd *osd, const char *text, bool sel) {
    this->osd = osd;
    drawn = false;
    Text = text;
    selectable = sel;
    current = false;
    wasCurrent = false;
    scrollable = false;
    itemTabs = NULL;
    tabWidth = NULL;
    pixmapIcon = NULL;
    pixmapTextScroller = NULL;
    infoTextWindow = NULL;
}

cNopacityMenuItem::~cNopacityMenuItem(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    delete [] itemTabs;
    delete [] tabWidth;
    osd->DestroyPixmap(pixmap);
    if (pixmapIcon) {
        osd->DestroyPixmap(pixmapIcon);
    }
    if (pixmapTextScroller) {
        osd->DestroyPixmap(pixmapTextScroller);
    }
    if (infoTextWindow) {
        delete infoTextWindow;
    }
}

void cNopacityMenuItem::SetCurrent(bool cur) {
    wasCurrent = current;
    current = cur;
} 

void cNopacityMenuItem::SetGeometry(int index, int top, int left, int width, int height, int spaceMenu) {
    this->index = index;
    this->top = top;
    this->left = left;
    this->width = width;
    this->height = height;
    this->spaceMenu = spaceMenu;
}

void cNopacityMenuItem::CreatePixmap() {
    pixmap = osd->CreatePixmap(3, cRect(left, top + index * (height + spaceMenu), width, height));
}

void cNopacityMenuItem::CreatePixmapIcon(void) {
    pixmapIcon = osd->CreatePixmap(5, cRect(left, top + index * (height + spaceMenu), width, height));
    pixmapIcon->Fill(clrTransparent);
}

void cNopacityMenuItem::CreatePixmapTextScroller(int totalWidth) {
    pixmapTextScroller = osd->CreatePixmap(4, cRect(left, top + index * (height + spaceMenu), width, height), cRect(0, 0, totalWidth+10, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityMenuItem::SetTabs(cString *tabs, int *tabWidths, int numtabs) {
    if (itemTabs)
        delete [] itemTabs;
    if (tabWidth)
        delete [] tabWidth;
    itemTabs = tabs;
    tabWidth = tabWidths;
    numTabs = numtabs;
}

void cNopacityMenuItem::SetBackgrounds(int *handleBackgrounds) {
    this->handleBackgrounds = handleBackgrounds;
}

void cNopacityMenuItem::DrawDelimiter(const char *del, const char *icon, int handleBgrd) {
    pixmap->Fill(Theme.Color(clrSeparatorBorder));
    if (handleBgrd >= 0)
        pixmap->DrawImage(cPoint(1, 1), handleBackgrounds[handleBgrd]);
    else
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrMenuItem));

    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrSeparatorBorder));
    cImageLoader imgLoader;
    if (!drawn) {
        if (imgLoader.LoadIcon(icon, config.iconHeight)) {
            if (pixmapIcon == NULL) {
                pixmapIcon = osd->CreatePixmap(5, cRect(left, top + index * (height + spaceMenu), config.menuItemLogoWidth, config.menuItemLogoWidth));
                pixmapIcon->Fill(clrTransparent);
            }
            pixmapIcon->DrawImage(cPoint(1, (height - config.iconHeight) / 2), imgLoader.GetImage());
        }
        drawn = true;
    }
    std::string delimiter = del;
    try {
        if (delimiter.find_first_not_of("-") > 0)
            delimiter.erase(0, delimiter.find_first_not_of("-")+1);
        if (delimiter.find_last_not_of("-") != std::string::npos)
            delimiter.erase(delimiter.find_last_not_of("-")+1);
    } catch (...) {}
    int x = config.iconHeight + 3;
    int y = (height - font->Height()) / 2;
    tColor clrFontBack = (config.doBlending)?(clrTransparent):Theme.Color(clrMenuItem);
    pixmap->DrawText(cPoint(x, y), delimiter.c_str(), Theme.Color(clrMenuFontMenuItemSep), clrFontBack, font);
}

void cNopacityMenuItem::Action(void) {
    int scrollDelay = config.menuScrollDelay * 1000;
    DoSleep(scrollDelay);
    cPixmap::Lock();
    if (Running())
        SetTextFull();
    cPixmap::Unlock();
    int drawPortX;
    int FrameTime = config.menuScrollFrameTime;
    int maxX = pixmapTextScroller->DrawPort().Width() - pixmapTextScroller->ViewPort().Width();
    bool doSleep = false;
    while (Running()) {
        if (doSleep) {
            DoSleep(scrollDelay);
            doSleep = false;
        }
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        drawPortX = pixmapTextScroller->DrawPort().X();
        drawPortX -= 1;
        cPixmap::Unlock();
        if (abs(drawPortX) > maxX) {
            DoSleep(scrollDelay);
            drawPortX = 0;
            doSleep = true;
        }
        cPixmap::Lock();
        if (Running())
            pixmapTextScroller->SetDrawPortPoint(cPoint(drawPortX, 0));
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running())
            osd->Flush();
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
    }
}

void cNopacityMenuItem::DoSleep(int duration) {
    int sleepSlice = 10;
    for (int i = 0; Running() && (i*sleepSlice < duration); i++)
        cCondWait::SleepMs(sleepSlice);
}

void cNopacityMenuItem::DrawRoundedCorners(tColor borderColor) {
    int radius = config.cornerRadius;
    if (radius < 3)
        return;
    pixmap->DrawEllipse(cRect(0,0,radius,radius), borderColor, -2);
    pixmap->DrawEllipse(cRect(-1,-1,radius,radius), clrTransparent, -2);

    pixmap->DrawEllipse(cRect(width-radius,0,radius,radius), borderColor, -1);
    pixmap->DrawEllipse(cRect(width-radius+1,-1,radius,radius), clrTransparent, -1);

    pixmap->DrawEllipse(cRect(0,height-radius,radius,radius), borderColor, -3);
    pixmap->DrawEllipse(cRect(-1,height-radius+1,radius,radius), clrTransparent, -3);

    pixmap->DrawEllipse(cRect(width-radius,height-radius,radius,radius), borderColor, -4);
    pixmap->DrawEllipse(cRect(width-radius+1,height-radius+1,radius,radius), clrTransparent, -4);
}

// cNopacityMainMenuItem  -------------
cNopacityMainMenuItem::cNopacityMainMenuItem(cOsd *osd, const char *text, bool sel, bool setup) : cNopacityMenuItem (osd, text, sel) {
    this->isSetup = setup;
}

cNopacityMainMenuItem::~cNopacityMainMenuItem(void) {
}

void cNopacityMainMenuItem::DrawBackground(void) {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    if (config.doBlending) {
        int handleBgrd;
        if (!isSetup)
            handleBgrd = (current)?handleBackgrounds[3]:handleBackgrounds[2];
        else
            handleBgrd = (current)?handleBackgrounds[13]:handleBackgrounds[12];
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    } else {
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), col);
    }
    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrMenuBorder));
}

std::string cNopacityMainMenuItem::items[16] = { "Schedule", "Channels", "Timers", "Recordings", "Setup", "Commands",
                                                "OSD", "EPG", "DVB", "LNB", "CAM", "Recording", "Replay", "Miscellaneous", "Plugins", "Restart"};

cString cNopacityMainMenuItem::GetIconName() {
    std::string element = *menuEntry;
    //check for standard menu entries
    for (int i=0; i<16; i++) {
        std::string s = trVDR(items[i].c_str());
        if (s == element) {
            cString menuIcon = cString::sprintf("menuIcons/%s", items[i].c_str());
            return menuIcon;
        }
    }
    //check for special main menu entries "stop recording", "stop replay"
    std::string stopRecording = skipspace(trVDR(" Stop recording "));
    std::string stopReplay = skipspace(trVDR(" Stop replaying"));
    try {
        if (element.substr(0, stopRecording.size()) == stopRecording)
            return "menuIcons/StopRecording";
        if (element.substr(0, stopReplay.size()) == stopReplay)
            return "menuIcons/StopReplay";
    } catch (...) {}
    //check for Plugins
    for (int i = 0; ; i++) {
        cPlugin *p = cPluginManager::GetPlugin(i);
        if (p) {
            const char *mainMenuEntry = p->MainMenuEntry();
            if (mainMenuEntry) {
                std::string plugMainEntry = mainMenuEntry;
                try {
                    if (element.substr(0, plugMainEntry.size()) == plugMainEntry) {
                        return cString::sprintf("pluginIcons/%s", p->Name());
                    }
                } catch (...) {}
            } 
        } else
            break;
    }
    return cString::sprintf("extraIcons/%s", *menuEntry);
}

void cNopacityMainMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + 10;
    if (config.useMenuIcons)
        pixmapLeft += config.iconHeight;
    int pixmapWidth = width - 10;
    if (config.useMenuIcons)
        pixmapWidth -= config.iconHeight;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityMainMenuItem::CreateText() {
    std::string text = skipspace(Text);
    bool found = false;
    bool doBreak = false;
    size_t i = 0;
    for (; i < text.length(); i++) {
        char s = text.at(i);
        if (i==0) {
            //if text directly starts with nonnumeric, break
            if (!(s >= '0' && s <= '9')) {
                break;
            }
        }
        if (found) {
            if (!(s >= '0' && s <= '9')) {
                doBreak = true;
            }           
        }
        if (s >= '0' && s <= '9') {
            found = true;
        }
        if (doBreak)
            break;
        if (i>4)
            break;
    }
    if (found) {
        menuNumber = skipspace(text.substr(0,i).c_str());
        menuEntry = skipspace(text.substr(i).c_str());
    } else {
        menuNumber = "";
        menuEntry = text.c_str();       
    }
    strEntry = *menuEntry;
}

int cNopacityMainMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += config.iconHeight;
    int totalTextWidth = width - spaceLeft;
    int numberWidth = font->Width("xxx");
    int textWidth = font->Width(*menuEntry);
    if ((numberWidth +  textWidth) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(numberWidth + textWidth, totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, width - spaceLeft - numberWidth, font);
    }
    return totalTextWidth;
}

void cNopacityMainMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = 0;
    int numberTotalWidth = font->Width("xxx");
    int numberWidth = font->Width(*menuNumber);
    pixmapTextScroller->DrawText(cPoint(x + (numberTotalWidth - numberWidth)/2, (height - font->Height())/2), *menuNumber, clrFont, clrTransparent, font);
    x += numberTotalWidth;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height())/2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityMainMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = 0;
    int numberTotalWidth = font->Width("xxx");
    int numberWidth = font->Width(*menuNumber);
    pixmapTextScroller->DrawText(cPoint(x + (numberTotalWidth - numberWidth)/2, (height - font->Height())/2), *menuNumber, clrFont, clrTransparent, font);
    x += numberTotalWidth;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height())/2), strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityMainMenuItem::Render() {
    DrawBackground();
    if (selectable) {
        if (config.useMenuIcons) {
            cString cIcon = GetIconName();
            if (!drawn) {
                cImageLoader imgLoader;
                if (imgLoader.LoadIcon(*cIcon, config.iconHeight)) {
                    pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
                }
                drawn = true;
            }
        }
        SetTextShort();
        if (current && scrollable && !Running() && config.menuScrollSpeed) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
    } else {
        DrawDelimiter(strEntry.c_str(), "skinIcons/channeldelimiter", (config.doBlending)?-1:(isSetup?12:2));
    }
}

// cNopacityScheduleMenuItem  -------------

cNopacityScheduleMenuItem::cNopacityScheduleMenuItem(cOsd *osd, const cEvent *Event, const cChannel *Channel, eTimerMatch TimerMatch, bool sel, eMenuCategory category, cRect *vidWin) 
                            : cNopacityMenuItem (osd, "", sel) {
    this->category = category;
    this->Event = Event;
    this->Channel = Channel;
    this->TimerMatch = TimerMatch;
    this->vidWin = vidWin;
    strDateTime = "";
    strTitle = "";
    strSubTitle = "";
    strTitleFull = "";
    strSubTitleFull = "";
    scrollTitle = false;
    scrollSubTitle = false;
}

cNopacityScheduleMenuItem::~cNopacityScheduleMenuItem(void) {
}

void cNopacityScheduleMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int drawPortWidth = totalWidth + 10;
    int pixmapLeft = left;
    int pixmapWidth = width;
    if (Channel) {
        pixmapLeft += config.menuItemLogoWidth + 5;
        pixmapWidth = pixmapWidth - config.menuItemLogoWidth - 5;
    }
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityScheduleMenuItem::CreateText() {
    if (!Event)
        return;
    std::stringstream sstrDateTime;
    time_t now = time(NULL);
    std::string dayNow = *WeekDayName(now);
    std::string dayEvent = *WeekDayName(Event->StartTime());
    if ((Event->StartTime() - now > 86400) || (dayNow != dayEvent))
        sstrDateTime << dayEvent << " ";
    sstrDateTime << *Event->GetTimeString();
    sstrDateTime << " - " << *Event->GetEndTimeString();
    strDateTime = sstrDateTime.str();
    if (Event->Title())
        strTitle = Event->Title();
    if (Event->ShortText())
        strSubTitle = Event->ShortText();
}

int cNopacityScheduleMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += config.menuItemLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strTitle.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        scrollTitle = true;
        totalTextWidth = max(font->Width(strTitle.c_str()), totalTextWidth);
        strTitleFull = strTitle.c_str();
        strSubTitleFull = strSubTitle.c_str();
        strTitle = CutText(strTitle, width - spaceLeft, font);
    }
    if (fontSmall->Width(strSubTitle.c_str()) > (width - spaceLeft)) {
        if (!scrollable) {
            scrollable = true;
            strTitleFull = strTitle.c_str();
            strSubTitleFull = strSubTitle.c_str();
        }
        scrollSubTitle = true;
        totalTextWidth = max(fontSmall->Width(strSubTitle.c_str()), totalTextWidth);
        strSubTitle = CutText(strSubTitle, width - spaceLeft, fontSmall);
    }
    return totalTextWidth;

}

void cNopacityScheduleMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(5, titleY), strTitleFull.c_str(), clrFont, clrTransparent, font);
    pixmapTextScroller->DrawText(cPoint(5, titleY + font->Height() - 2), strSubTitleFull.c_str(), clrFont, clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(5, titleY), strTitle.c_str(), clrFont, clrTransparent, font);
    pixmapTextScroller->DrawText(cPoint(5, titleY + font->Height() - 2), strSubTitle.c_str(), clrFont, clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::Render() {
    int logoWidth = config.menuItemLogoWidth;
    int logoHeight = config.menuItemLogoHeight;
    textLeft = 5;
    if (Channel && Channel->Name())
        textLeft = logoWidth + 10;
    
    if (selectable) {
        titleY = (height - font->Height())/2 - 2;
        DrawBackground(textLeft);
        DrawRemaining(textLeft, height*7/8, width - textLeft - 10);
        if (!drawn) {
            DrawLogo(logoWidth, logoHeight);
            drawn = true;
        }
        SetTextShort();
        if (current && scrollable && !Running() && config.menuScrollSpeed) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        if (current && Event) {
            if (config.menuSchedulesWindowMode == 0) {
                //window mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetText(Event->Description());
                infoTextWindow->SetPoster(Event, false);
                infoTextWindow->Start();
            } else {
                //fullscreen mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetPoster(Event, false, true);
                infoTextWindow->SetEvent(Event);
            }
        }
    } else {
        if (Event) {
            DrawDelimiter(Event->Title(), "skinIcons/daydelimiter", (config.doBlending)?4:-1);
        } else if (Channel) {
            DrawDelimiter(Channel->Name(), "skinIcons/channeldelimiter", (config.doBlending)?4:-1);
        }
    }
}

void cNopacityScheduleMenuItem::DrawBackground(int textLeft) {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    if (config.doBlending) {
        int handleBgrd = (current)?handleBackgrounds[5]:handleBackgrounds[4];
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    } else {
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), col);
    }
    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrMenuBorder));
    if (TimerMatch == tmFull) {
        cImageLoader imgLoader;
        if (imgLoader.LoadIcon("skinIcons/activetimer", 64, 64)) {
            pixmapIcon->DrawImage(cPoint(width - 66, 2), imgLoader.GetImage());
        }
    } else if (TimerMatch == tmPartial) {
        cImageLoader imgLoader;
        if (imgLoader.LoadIcon("skinIcons/activetimersmall", 32, 32)) {
            pixmapIcon->DrawImage(cPoint(width - 34, 2), imgLoader.GetImage());
        }
    }
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    tColor clrFontBack = (config.doBlending)?(clrTransparent):((current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem));
    pixmap->DrawText(cPoint(textLeft, 3), strDateTime.c_str(), clrFont, clrFontBack, font);
}

void cNopacityScheduleMenuItem::DrawLogo(int logoWidth, int logoHeight) {
    if (Channel && Channel->Name()) {
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(Channel->Name(), logoWidth, logoHeight)) {
            pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
        } else if (imgLoader.LoadLogo(*(Channel->GetChannelID().ToString()), logoWidth, logoHeight)) {
            pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
        } else {
            cTextWrapper channel;
            channel.Set(Channel->Name(), font, logoWidth);
            int lines = channel.Lines();
            int lineHeight = height / 3;
            int heightChannel = lines * lineHeight;
            int y = (heightChannel>height)?0:(height-heightChannel)/2;
            for (int line = 0; line < lines; line++) {
                pixmapIcon->DrawText(cPoint((logoWidth - font->Width(channel.GetLine(line)))/2, y+lineHeight*line), channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, font);
            }   
        }
    }
}

void cNopacityScheduleMenuItem::DrawRemaining(int x, int y, int width) {
    if (!Event)
        return;
    time_t now = time(NULL);
    if ((now < Event->StartTime()) || (now > Event->EndTime()))
        return;
    int seen = now - Event->StartTime();
    int total = Event->EndTime() - Event->StartTime();
    if (total == 0)
        return;
    double percentSeen = (double)seen/total;
    pixmap->DrawEllipse(cRect(x, y, 7, 7), Theme.Color(clrProgressBarBack));
    pixmap->DrawEllipse(cRect(x+width, y, 7, 7), Theme.Color(clrProgressBarBack));
    pixmap->DrawRectangle(cRect(x+4, y, width-1, 7), Theme.Color(clrProgressBarBack));
    pixmap->DrawEllipse(cRect(x+1, y+1, 5, 5), Theme.Color(clrProgressBar));
    if (percentSeen > 0.0)
        pixmap->DrawEllipse(cRect(x+(width*percentSeen), y+1, 5, 5), Theme.Color(clrProgressBar));
    pixmap->DrawRectangle(cRect(x+4, y+1, (width-1)*percentSeen, 5), Theme.Color(clrProgressBar));
}

// cNopacityChannelMenuItem  -------------

cNopacityChannelMenuItem::cNopacityChannelMenuItem(cOsd *osd, const cChannel *Channel, bool sel, cRect *vidWin) : cNopacityMenuItem (osd, "", sel) {
    this->Channel = Channel;
    this->vidWin = vidWin;
    strEntry = "";
    strEntryFull = "";
    strChannelSource = "";
    strChannelInfo = "";
    strEpgInfo = "";
    strEpgInfoFull = "";
    strTimeInfo = "";
}

cNopacityChannelMenuItem::~cNopacityChannelMenuItem(void) {
}

void cNopacityChannelMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + config.menuItemLogoWidth + 10;
    int pixmapWidth = width - config.menuItemLogoWidth - 10;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityChannelMenuItem::CreateText() {
    strEntry = cString::sprintf("%d %s", Channel->Number(), Channel->Name());
    if (config.menuChannelDisplayMode == 0) {
        const cSource *source = Sources.Get(Channel->Source());
        if (source)
            strChannelSource = cString::sprintf("%s - %s", *cSource::ToString(source->Code()),  source->Description());
        else
            strChannelSource = "";
        strChannelInfo = cString::sprintf("%s %d, %d MHz", tr("Transp."), Channel->Transponder(), Channel->Frequency()/1000);
    } else {
        readCurrentEPG();
    }
}

int cNopacityChannelMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += config.menuItemLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strEntry.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strEntry.c_str()), totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, width - spaceLeft, font);
    } else 
        strEntryFull = strEntry;
        
    if (fontSmall->Width(strEpgInfo.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(fontSmall->Width(strEpgInfo.c_str()), totalTextWidth);
        strEpgInfoFull = strEpgInfo.c_str();
        strEpgInfo = CutText(strEpgInfo, width - spaceLeft, fontSmall);
    } else
        strEpgInfoFull = strEpgInfo;
    
    return totalTextWidth;
}

void cNopacityChannelMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int heightChannelName = 0;
    if (config.menuChannelDisplayMode == 2) {
        heightChannelName = (height - font->Height())/2;
    } else {
        heightChannelName = (height/2 - font->Height())/2;
    }
    pixmapTextScroller->DrawText(cPoint(5, heightChannelName), strEntryFull.c_str(), clrFont, clrTransparent, font);
    if (config.menuChannelDisplayMode == 1) {
        int heightTimeInfo = 0;
        int heightEPGInfo = 0;
        if (config.menuChannelDisplayTime) {
            heightTimeInfo = height/2 + (height/4 - fontSmall->Height())/2;
            heightEPGInfo = 3*height/4 + (height/4 - fontSmall->Height())/2;
        } else {
            heightEPGInfo = height/2 + (height/4 - fontSmall->Height())/2;
        }
        if (config.menuChannelDisplayTime) {
            pixmapTextScroller->DrawText(cPoint(5, heightTimeInfo), strTimeInfo.c_str(), clrFont, clrTransparent, fontSmall);
        }
        pixmapTextScroller->DrawText(cPoint(5, heightEPGInfo), strEpgInfoFull.c_str(), clrFont, clrTransparent, fontSmall);
    }
}

void cNopacityChannelMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int heightChannelName = 0;
    if (config.menuChannelDisplayMode == 2) {
        heightChannelName = (height - font->Height())/2;
    } else {
        heightChannelName = (height/2 - font->Height())/2;
    }
    pixmapTextScroller->DrawText(cPoint(5, heightChannelName), strEntry.c_str(), clrFont, clrTransparent, font);
    if (config.menuChannelDisplayMode == 1) {
        int heightTimeInfo = 0;
        int heightEPGInfo = 0;
        if (config.menuChannelDisplayTime) {
            heightTimeInfo = height/2 + (height/4 - fontSmall->Height())/2;
            heightEPGInfo = 3*height/4 + (height/4 - fontSmall->Height())/2;
        } else {
            heightEPGInfo = height/2 + (height/4 - fontSmall->Height())/2;
        }
        if (config.menuChannelDisplayTime) {
            pixmapTextScroller->DrawText(cPoint(5, heightTimeInfo), strTimeInfo.c_str(), clrFont, clrTransparent, fontSmall);
        }
        pixmapTextScroller->DrawText(cPoint(5, heightEPGInfo), strEpgInfo.c_str(), clrFont, clrTransparent, fontSmall);
    }
}

void cNopacityChannelMenuItem::DrawBackground(void) {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    if (config.doBlending) {
        int handleBgrd = (current)?handleBackgrounds[7]:handleBackgrounds[6];
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    } else {
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), col);
    }
    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrMenuBorder));
    
    if (config.menuChannelDisplayMode == 0) {
        int encryptedSize = height/4-2;
        int sourceX = config.menuItemLogoWidth + 15;
        tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
        tColor clrFontBack = (config.doBlending)?(clrTransparent):((current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem)); 
        pixmap->DrawText(cPoint(sourceX, 3*height/4 + (height/4 - fontSmall->Height())/2), *strChannelInfo, clrFont, clrFontBack, fontSmall);
        if (Channel->Ca()) {
            cImageLoader imgLoader;
            if (imgLoader.LoadIcon("skinIcons/encrypted", encryptedSize)) {
                pixmapIcon->DrawImage(cPoint(sourceX, height/2+1), imgLoader.GetImage());
                sourceX += encryptedSize + 10;
            }
        }
        pixmap->DrawText(cPoint(sourceX, height/2 + (height/4 - fontSmall->Height())/2), *strChannelSource, clrFont, clrFontBack, fontSmall);
    }
}

void cNopacityChannelMenuItem::readCurrentEPG(void) {
    cSchedulesLock schedulesLock;
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
    const cSchedule *Schedule = NULL;
	Schedule = schedules->GetSchedule(Channel);
	if (!Schedule) {
		strEpgInfo = tr("No EPG Information found");
        strTimeInfo = "";
	} else {
        const cEvent *PresentEvent = Schedule->GetPresentEvent();
        if (!PresentEvent) {
            strEpgInfo = tr("No EPG Information found");
            strTimeInfo = "";
        } else {
            strEpgInfo = PresentEvent->Title();
            strTimeInfo = *cString::sprintf("%s - %s:", *PresentEvent->GetTimeString(), *PresentEvent->GetEndTimeString());
        }
    }
}

std::string cNopacityChannelMenuItem::readEPG(void) {
    std::stringstream sstrText;
    cSchedulesLock schedulesLock;
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
    const cSchedule *Schedule = NULL;
	Schedule = schedules->GetSchedule(Channel);
	if (!Schedule) {
		sstrText << tr("No EPG Information found");
	} else {
        const cEvent *PresentEvent = Schedule->GetPresentEvent();
        int i=0;
        if (!PresentEvent) {
            sstrText << tr("No EPG Information found");
        } else {
            for (const cEvent *event = PresentEvent; event; event = Schedule->Events()->Next(event)) {
                if (event) {
                    sstrText << *event->GetTimeString();
                    sstrText << " ";
                    sstrText << event->Title();
                    if (event->ShortText() && (strlen(event->ShortText()) > 1))
                        sstrText << " ~ " << event->ShortText();
                    i++;
                    if (i < config.numEPGEntriesChannelsMenu)
                        sstrText << "\n";
                    else
                        break;
                }
            }
        }
    }
    return sstrText.str();
}

void cNopacityChannelMenuItem::Render() {
    if (selectable) {                           //Channels
        DrawBackground();
        int logoWidth = config.menuItemLogoWidth;
        int logoHeight = config.menuItemLogoHeight;
        if (!drawn) {
            cImageLoader imgLoader;
            if (imgLoader.LoadLogo(Channel->Name(), logoWidth, logoHeight)) {
                pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
            } else if (imgLoader.LoadLogo(*(Channel->GetChannelID().ToString()), logoWidth, logoHeight)) {
                pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
            } 
            drawn = true;
        }
        SetTextShort();
        if (current && scrollable && !Running() && config.menuScrollSpeed) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        if (current && Channel && (config.menuChannelDisplayMode == 0)) {
            infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
            infoTextWindow->SetGeometry(textWindow);
            infoTextWindow->SetText(readEPG().c_str());
            infoTextWindow->Start();
        }
    } else {                                    //Channelseparators
        DrawDelimiter(Channel->Name(), "skinIcons/channeldelimiter", (config.doBlending)?6:-1);
    }
}

// cNopacityTimerMenuItem  -------------

cNopacityTimerMenuItem::cNopacityTimerMenuItem(cOsd *osd, const cTimer *Timer, bool sel) : cNopacityMenuItem (osd, "", sel) {
    this->Timer = Timer;
}

cNopacityTimerMenuItem::~cNopacityTimerMenuItem(void) {
}

void cNopacityTimerMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + config.menuItemLogoWidth + 10;
    int pixmapWidth = width - config.menuItemLogoWidth - 10;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityTimerMenuItem::CreateText() {
    const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(Timer->File(), FOLDERDELIMCHAR);
    if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
        File++;
    else
        File = Timer->File();
    strEntry = File;
    strDateTime = CreateDate();
}

std::string cNopacityTimerMenuItem::CreateDate(void) {
    cString day, dayName("");
    if (Timer->WeekDays())
        day = Timer->PrintDay(0, Timer->WeekDays(), false);
    else if (Timer->Day() - time(NULL) < 28 * SECSINDAY) {
        day = itoa(Timer->GetMDay(Timer->Day()));
        dayName = WeekDayName(Timer->Day());
    } else {
        struct tm tm_r;
        time_t Day = Timer->Day();
        localtime_r(&Day, &tm_r);
        char buffer[16];
        strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_r);
        day = buffer;
    }
    cString dateString;
    if (Timer->Channel())
        dateString = *cString::sprintf("%d - %s%s%s (%02d:%02d-%02d:%02d)", Timer->Channel()->Number(), *dayName, *dayName && **dayName ? " " : "", *day, Timer->Start() / 100, Timer->Start() % 100, Timer->Stop() / 100, Timer->Stop() % 100);
    else
        dateString = *cString::sprintf("%s%s%s (%02d:%02d-%02d:%02d)", *dayName, *dayName && **dayName ? " " : "", *day, Timer->Start() / 100, Timer->Start() % 100, Timer->Stop() / 100, Timer->Stop() % 100);
    return *dateString;
}

int cNopacityTimerMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = spaceMenu;
    if (hasIcon)
        spaceLeft += config.menuItemLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strEntry.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strEntry.c_str()), totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, width - spaceLeft, font);
    }
    return totalTextWidth;
}

void cNopacityTimerMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, height/2 + (height/2 - font->Height())/2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityTimerMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, height/2 + (height/2 - font->Height())/2), strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityTimerMenuItem::DrawBackground(int textLeft) {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    if (config.doBlending) {
        int handleBgrd = (current)?handleBackgrounds[11]:handleBackgrounds[10];
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    } else {
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), col);
    }
    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrMenuBorder));
    int iconSize = height/2;
    cString iconName("");
    bool firstDay = false;
    if (!(Timer->HasFlags(tfActive)))
        iconName = "skinIcons/timerInactive";
    else if (Timer->FirstDay()) {
        iconName = "skinIcons/timerActive";
        firstDay = true;
    } else if (Timer->Recording())
        iconName = "skinIcons/timerRecording";
    else 
        iconName = "skinIcons/timerActive";
    
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon(iconName, iconSize)) {
        pixmapIcon->DrawImage(cPoint(textLeft, 0), imgLoader.GetImage());
    }
    cString dateTime("");
    if (firstDay)
        dateTime = cString::sprintf("! %s", strDateTime.c_str());
    else
        dateTime = strDateTime.c_str();
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    tColor clrFontBack = (config.doBlending)?(clrTransparent):((current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem));
    pixmap->DrawText(cPoint(textLeft + iconSize, (height/2 - fontSmall->Height())/2), *dateTime, clrFont, clrFontBack, fontSmall);
}

void cNopacityTimerMenuItem::Render() {
    textLeft = config.menuItemLogoWidth + 10;
    if (selectable) {                           
        DrawBackground(textLeft);
        int logoWidth = config.menuItemLogoWidth;
        int logoHeight = config.menuItemLogoHeight;
        if (!drawn) {
            DrawLogo(logoWidth, logoHeight);
            drawn = true;
        }
        if (!Running())
            SetTextShort();
        if (current && scrollable && !Running() && config.menuScrollSpeed) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
    }
}

void cNopacityTimerMenuItem::DrawLogo(int logoWidth, int logoHeight) {
    if (Timer && Timer->Channel() && Timer->Channel()->Name()) {
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(Timer->Channel()->Name(), logoWidth, logoHeight)) {
            pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
        } else if (imgLoader.LoadLogo(*(Timer->Channel()->GetChannelID().ToString()), logoWidth, logoHeight)) {
            pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
        } else {
            cTextWrapper channel;
            channel.Set(Timer->Channel()->Name(), font, logoWidth);
            int lines = channel.Lines();
            int lineHeight = height / 3;
            int heightChannel = lines * lineHeight;
            int y = (heightChannel>height)?0:(height-heightChannel)/2;
            for (int line = 0; line < lines; line++) {
                pixmapIcon->DrawText(cPoint((logoWidth - font->Width(channel.GetLine(line)))/2, y+lineHeight*line), channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, font);
            }   
        }
    }
}
// cNopacityRecordingMenuItem  -------------

cNopacityRecordingMenuItem::cNopacityRecordingMenuItem(cOsd *osd, const cRecording *Recording, bool sel, bool isFolder, int Level, int Total, int New, cRect *vidWin) : cNopacityMenuItem (osd, "", sel) {
    this->Recording = Recording;
    this->isFolder = isFolder;
    this->Level = Level;
    this->Total = Total;
    this->New = New;
    this->vidWin = vidWin;
    posterWidth = 0;
    posterHeight = 0;
    hasPoster = false;
}

cNopacityRecordingMenuItem::~cNopacityRecordingMenuItem(void) {
}

void cNopacityRecordingMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = 0;
    int pixmapWidth = 0;
    int drawPortWidth = totalWidth + 10;

    if (isFolder) {
        pixmapLeft = left + 10 + config.menuRecFolderSize;
        pixmapWidth = width - 10 - config.menuRecFolderSize;
    } else {
        pixmapLeft = left + 10;
        pixmapWidth = width - 10;
        if (hasPoster) {
            pixmapLeft += posterWidth+10;
            pixmapWidth -= (posterWidth+10);
        }
    }
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + spaceMenu), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityRecordingMenuItem::CreateText() {
    std::string recName = Recording->Name();
    try {
        std::vector<std::string> tokens;
        std::istringstream f(recName.c_str());
        std::string s;    
        while (std::getline(f, s, FOLDERDELIMCHAR)) {
            tokens.push_back(s);
        }
        strRecName = tokens.at(Level);
        if (!isFolder && Recording->IsEdited()) {
            strRecName = strRecName.substr(1);
        }
    } catch (...) {
        strRecName = recName.c_str();
    }
}

void cNopacityRecordingMenuItem::SetPoster(void) {
    const cRecordingInfo *info = Recording->Info();
    if (info) {
        const cEvent *event = info->GetEvent();
        static cPlugin *pTVScraper = cPluginManager::GetPlugin("tvscraper");
        if (pTVScraper && event) {
            poster.event = event;
            poster.isRecording = true;
            if (pTVScraper->Service("TVScraperGetPoster", &poster)) {
                hasPoster = true;
                int posterWidthOrig = poster.media.width;
                int posterHeightOrig = poster.media.height;
                if ((posterWidthOrig > 10) && (posterHeightOrig > 10)) {
                    posterHeight = height - 10;
                    posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
                } else {
                    hasPoster = false;
                    posterHeight = 0;
                    posterWidth = 0;
                }
            } else {
                hasPoster = false;
                posterHeight = 0;
                posterWidth = 0;
            }
        }
    }
}

int cNopacityRecordingMenuItem::CheckScrollable(bool hasIcon) {
    int totalWidth = 0;
    if (isFolder)
        totalWidth = CheckScrollableFolder();
    else
        totalWidth = CheckScrollableRecording();
    return totalWidth;
}

int cNopacityRecordingMenuItem::CheckScrollableRecording(void) {
    int spaceLeft = spaceMenu;
    if (hasPoster)
        spaceLeft += posterWidth + 15;
    int iconWidth = 0;
    if (Recording->IsNew())
        iconWidth += font->Height() + 10;
    if (Recording->IsEdited())
        iconWidth += font->Height() + 5;
    int totalTextWidth = width - spaceLeft;
    strRecNameFull = strRecName.c_str();
    if (font->Width(strRecName.c_str()) + iconWidth > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strRecName.c_str()) + iconWidth, totalTextWidth);
        strRecName = CutText(strRecName, width - spaceLeft - iconWidth, font);
    }
    return totalTextWidth;
}

int cNopacityRecordingMenuItem::CheckScrollableFolder(void) {
    int spaceLeft = spaceMenu + config.menuRecFolderSize;
    int totalTextWidth = width - spaceLeft;
    strRecNameFull = strRecName.c_str();
    if (font->Width(strRecName.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strRecName.c_str()), totalTextWidth);
        strRecName = CutText(strRecName, width - spaceLeft, font);
    }
    return totalTextWidth;
}


void cNopacityRecordingMenuItem::SetTextFull(void) {
    if (isFolder)
        SetTextFullFolder();
    else
        SetTextFullRecording();
}

void cNopacityRecordingMenuItem::DrawBackground(void) {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    if (config.doBlending) {
        int handleBgrd = (current)?handleBackgrounds[9]:handleBackgrounds[8];
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    } else {
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), col);
    }
    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrMenuBorder));
}

void cNopacityRecordingMenuItem::SetTextFullFolder(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    if (hasPoster)
        DrawPoster();
    else
        DrawFolderIcon();
    int heightRecName = (2*height/3 - font->Height())/2 + 10;
    pixmapTextScroller->DrawText(cPoint(0, heightRecName), strRecNameFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityRecordingMenuItem::SetTextFullRecording(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int textLeft = 5;
    if (Recording->IsNew()) {
        textLeft += DrawRecordingNewIcon();
    }
    if (Recording->IsEdited()) {
        textLeft += DrawRecordingEditedIcon(textLeft);
    }
    int heightRecName = (height / 2 - font->Height())/2;
    pixmapTextScroller->DrawText(cPoint(textLeft, heightRecName), strRecNameFull.c_str(), clrFont, clrTransparent, font);
}


void cNopacityRecordingMenuItem::SetTextShort(void) {
    if (isFolder)
        SetTextShortFolder();
    else
        SetTextShortRecording();
}

void cNopacityRecordingMenuItem::SetTextShortFolder(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    if (hasPoster)
        DrawPoster();
    else
        DrawFolderIcon();
    int heightRecName = (2*height/3 - font->Height())/2 + 10;
    pixmapTextScroller->DrawText(cPoint(0, heightRecName), strRecName.c_str(), clrFont, clrTransparent, font);
}

void cNopacityRecordingMenuItem::SetTextShortRecording(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int textLeft = 5;
    if (Recording->IsNew()) {
        textLeft += DrawRecordingNewIcon();
    }
    if (Recording->IsEdited()) {
        textLeft += DrawRecordingEditedIcon(textLeft);
    }
    int heightRecName = (height / 2 - font->Height())/2;
    pixmapTextScroller->DrawText(cPoint(textLeft, heightRecName), strRecName.c_str(), clrFont, clrTransparent, font);
}


int cNopacityRecordingMenuItem::DrawRecordingNewIcon(void) {
    int iconNewSize = font->Height();
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("skinIcons/newrecording", iconNewSize)) {
        int iconHeight = (height/2 - iconNewSize)/2;
        pixmapTextScroller->DrawImage(cPoint(1, iconHeight), imgLoader.GetImage());
    }
    return iconNewSize;
}

int cNopacityRecordingMenuItem::DrawRecordingEditedIcon(int startLeft) {
    int iconEditedSize = font->Height() - 10;
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("skinIcons/recordingcutted", iconEditedSize)) {
        int iconHeight = (height/2 - iconEditedSize)/2;
        pixmapTextScroller->DrawImage(cPoint(startLeft + 5, iconHeight), imgLoader.GetImage());
    }
    return iconEditedSize + 5;
}

void cNopacityRecordingMenuItem::DrawFolderIcon(void) {
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("skinIcons/recordingfolder", config.menuRecFolderSize)) {
        pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
    }
}

void cNopacityRecordingMenuItem::DrawRecDateTime(void) {
    int iconDateTimeSize = config.menuRecFolderSize / 2;
    int iconHeight = height/2 + (height/2 - iconDateTimeSize)/2;
    int left = 10;
    if (hasPoster) {
        left += posterWidth + 10;
    }
    if (!drawn) {
        cImageLoader imgLoader;
        if (imgLoader.LoadIcon("skinIcons/recordingdatetime", iconDateTimeSize)) {
            pixmapIcon->DrawImage(cPoint(left, iconHeight), imgLoader.GetImage());
        }
        drawn = true;
    }
    pixmapIcon->DrawRectangle(cRect(iconHeight+left, 0, width-iconHeight, height), clrTransparent);
    const cEvent *Event = NULL;
    Event = Recording->Info()->GetEvent();
    cString strDateTime("");
    cString strDuration("");
    if (Event) {
        std::string strDate = *(Event->GetDateString());
        cString strTime = Event->GetTimeString();
        if (strDate.find("1970") != std::string::npos) {
            time_t start = Recording->Start();
            strDateTime = cString::sprintf("%s %s", *DateString(start),*TimeString(start));
        } else {
            strDateTime = cString::sprintf("%s - %s", strDate.c_str(), *strTime);
        }
        int duration = Event->Duration() / 60;
        int recDuration = Recording->LengthInSeconds();
        recDuration = (recDuration>0)?(recDuration / 60):0;
        strDuration = cString::sprintf("%s: %d %s, %s: %d %s", tr("Duration"), duration, tr("min"), tr("recording"), recDuration, tr("min"));
    }

    int textHeight = height/2 + (height/4 - fontSmall->Height())/2;
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapIcon->DrawText(cPoint(iconDateTimeSize + 10 + left, textHeight), *strDateTime, clrFont, clrTransparent, fontSmall);
    textHeight += height/4;
    pixmapIcon->DrawText(cPoint(iconDateTimeSize + 10 + left, textHeight), *strDuration, clrFont, clrTransparent, fontSmall);

}

void cNopacityRecordingMenuItem::DrawFolderNewSeen(void) {
    int textHeight = 2*height/3 + (height/3 - fontSmall->Height())/2 - 10;
    cString strTotalNew = cString::sprintf("%d %s (%d %s)", Total, (Total > 1)?tr("recordings"):tr("recording"), New, tr("new"));
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapIcon->DrawText(cPoint(config.menuRecFolderSize + 10, textHeight), *strTotalNew, clrFont, clrTransparent, fontSmall);
}

void cNopacityRecordingMenuItem::DrawPoster(void) {
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(poster.media.path.c_str(), posterWidth, posterHeight)) {
        pixmapIcon->DrawImage(cPoint(10, 5), imgLoader.GetImage());
    }
}

void cNopacityRecordingMenuItem::Render() {
    if (selectable) {                           
        DrawBackground();
        if (isFolder) {
            DrawFolderNewSeen();
            SetTextShort();
        } else {
            if (hasPoster) {
                DrawPoster();
            }
            DrawRecDateTime();
            SetTextShort();
        }
        if (current && scrollable && !Running() && config.menuScrollSpeed) {
            Start();
        }
        if (wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetTextShort();
            Cancel(-1);
        }
        if (!isFolder) {
            if (wasCurrent)
                if (infoTextWindow) {
                    delete infoTextWindow;
                    infoTextWindow = NULL;
                }
            if (current) {
                if (config.menuRecordingsWindowMode == 0) {
                    //window mode
                    infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                    infoTextWindow->SetGeometry(textWindow);
                    infoTextWindow->SetText(Recording->Info()->Description());
                    infoTextWindow->SetPoster(Recording->Info()->GetEvent(), true);
                    infoTextWindow->Start();
                } else {
                    //fullscreen mode
                    infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                    infoTextWindow->SetGeometry(textWindow);
                    infoTextWindow->SetPoster(Recording->Info()->GetEvent(), true, true);
                    infoTextWindow->SetRecording(Recording);
                }
            }
        }
    }
}


// cNopacityDefaultMenuItem  -------------

cNopacityDefaultMenuItem::cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool sel) : cNopacityMenuItem (osd, text, sel) {
    scrollCol = -1;
}

cNopacityDefaultMenuItem::~cNopacityDefaultMenuItem(void) {
}

bool cNopacityDefaultMenuItem::CheckProgressBar(const char *text) {
    if (strlen(text) > 5 
        && text[0] == '[' 
        && ((text[1] == '|')||(text[1] == ' ')) 
        && ((text[2] == '|')||(text[2] == ' ')) 
        && text[strlen(text) - 1] == ']')
        return true;
    return false;
}

void cNopacityDefaultMenuItem::DrawBackground(void) {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    if (config.doBlending) {
        int handleBgrd = (current)?handleBackgrounds[1]:handleBackgrounds[0];
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    } else {
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), col);
    }
    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrMenuBorder));
}

void cNopacityDefaultMenuItem::DrawProgressBar(int x, int width, const char *bar, tColor color) {
    const char *p = bar + 1;
    bool isProgressbar = true;
    int total = 0;
    int now = 0;
    for (; *p != ']'; ++p) {
        if (*p == ' ' || *p == '|') {
            ++total;
            if (*p == '|')
                ++now;
        } else {
            isProgressbar = false;
            break;
        }
    }
    if (isProgressbar) {
        pixmap->DrawRectangle(cRect(x+5, height/4, width-10, height/2), color);
        pixmap->DrawRectangle(cRect(x+7, height/4+2, width-14, height/2-4), Theme.Color(clrMenuItemBlend));
        double progress = (double)now/(double)total;
        pixmap->DrawRectangle(cRect(x+8, height/4+3, (width-16)*progress, height/2-6), color);
        
    }
}

void cNopacityDefaultMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = (scrollCol == 0)?5:0;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height()) / 2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityDefaultMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    int x = (scrollCol == 0)?5:0;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height()) / 2), strEntry.c_str(), clrFont, clrTransparent, font);
}

int cNopacityDefaultMenuItem::CheckScrollable(bool hasIcon) {
    if (!selectable)
        return 0;
    int colWidth = 0;
    int colTextWidth = 0; 
    for (int i=0; i<numTabs; i++) {
        if (tabWidth[i] > 0) {
            if (CheckProgressBar(*itemTabs[i]))
                continue;
            colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
            colTextWidth = font->Width(*itemTabs[i]);
            if (colTextWidth > colWidth) {
                cTextWrapper itemTextWrapped;
                scrollable = true;
                scrollCol = i;
                strEntryFull = *itemTabs[i];
                itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
                strEntry = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
                break;
            }
        } else
            break;
    }
    if (scrollable) {
        pixmapTextScroller = osd->CreatePixmap(4, cRect(left + tabWidth[scrollCol], top + index * (height + spaceMenu), tabWidth[scrollCol+numTabs], height), cRect(0, 0, colTextWidth+10, height));
        pixmapTextScroller->Fill(clrTransparent);
    }
    return 0;
}

bool cNopacityDefaultMenuItem::DrawHeaderElement(void) {
    const char *c = Text + 3;
    while (*c == '-')
        c++;
    if (*c == ' ' || *c == '\t') {
        // it's a headline
        while (*c == ' ' || *c == '\t') // find start of headline text
            c++;

        // find end of headline text
        char *c2 = (char*)(c + strlen(c) - 1);
        while (*c2 == '-')
            c2--;
        while (*c2 == ' ' || *c2 == '\t')
            c2--;
        *(c2 + 1) = 0;

        int left = 5 + tabWidth[0];
        tColor clrFontBack = (config.doBlending)?(clrTransparent):((current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem));
	pixmap->DrawText(cPoint(left, (height - font->Height()) / 2), c, Theme.Color(clrMenuFontMenuItemSep), clrFontBack, font);
        return true;
    }
    return false;
}

void cNopacityDefaultMenuItem::Render() {
    DrawBackground();
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    tColor clrFontBack = (config.doBlending)?(clrTransparent):((current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem));
    if (!selectable && (strncmp(Text, "---", 3) == 0)) {
        if (DrawHeaderElement())
            return;
    }
    int colWidth = 0;
    int colTextWidth = 0; 
    cString itemText("");
    for (int i=0; i<numTabs; i++) {
        if (tabWidth[i] > 0) {
            colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
            int posX = tabWidth[i];
            if (CheckProgressBar(*itemTabs[i])) {
                    DrawProgressBar(posX, colWidth, *itemTabs[i], clrFont);
            } else if (i != scrollCol) {
                colTextWidth = font->Width(*itemTabs[i]);
                if (colTextWidth > colWidth) {
                    cTextWrapper itemTextWrapped;
                    if (selectable) {
                        itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
                        itemText = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
                    } else {
                        itemTextWrapped.Set(*itemTabs[i], font, colWidth);
                        itemText = cString::sprintf("%s", itemTextWrapped.GetLine(0));
                    }
                } else {
                    itemText = itemTabs[i];
                }
                if (i==0) posX += 5;
                pixmap->DrawText(cPoint(posX, (height - font->Height()) / 2), *itemText, clrFont, clrFontBack, font);
            } else {
                if (!Running())
                    SetTextShort();
            }
        } else
            break;
    }

    if (current && scrollable && !Running() && config.menuScrollSpeed) {
        Start();
    }
    if (wasCurrent && !current && scrollable && Running()) {
        pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
        SetTextShort();
        Cancel(-1);
    }
}

// cNopacityTrackMenuItem  -------------

cNopacityTrackMenuItem::cNopacityTrackMenuItem(cOsd *osd, const char *text) : cNopacityMenuItem (osd, text, true) {
}

cNopacityTrackMenuItem::~cNopacityTrackMenuItem(void) {
}

void cNopacityTrackMenuItem::Render() {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    if (config.doBlending) {
        int handleBgrd = (current)?handleBackgrounds[1]:handleBackgrounds[0];
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    } else {
        tColor col = (current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem);
        pixmap->DrawRectangle(cRect(1, 1, width-2, height-2), col);
    }
    if (config.roundedCorners)
        DrawRoundedCorners(Theme.Color(clrMenuBorder));
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrTracksFontButtons);
    tColor clrFontBack = (config.doBlending)?(clrTransparent):((current)?Theme.Color(clrMenuItemHigh):Theme.Color(clrMenuItem));
    pixmap->DrawText(cPoint(5, (height - font->Height())/2), Text, clrFont, clrFontBack, font);
}
