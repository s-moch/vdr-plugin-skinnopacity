#include "menuitem.h"
#include <string>
#include <sstream>
#include <algorithm>


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

void cNopacityMenuItem::SetGeometry(int index, int top, int left, int width, int height) {
    this->index = index;
    this->top = top;
    this->left = left;
    this->width = width;
    this->height = height;
}

void cNopacityMenuItem::CreatePixmap() {
    pixmap = osd->CreatePixmap(3, cRect(left, top + index * (height + left), width, height));
}

void cNopacityMenuItem::CreatePixmapIcon(void) {
    pixmapIcon = osd->CreatePixmap(5, cRect(left, top + index * (height + left), width, height));
    pixmapIcon->Fill(clrTransparent);
}

void cNopacityMenuItem::CreatePixmapTextScroller(int totalWidth) {
    pixmapTextScroller = osd->CreatePixmap(4, cRect(left, top + index * (height + left), width, height), cRect(0, 0, totalWidth+10, height));
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
    pixmap->Fill(Theme.Color(clrMenuItem));
    pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    cImageLoader imgLoader;
    if (!drawn) {
        if (imgLoader.LoadIcon(icon, config.iconHeight)) {
            if (pixmapIcon == NULL) {
                pixmapIcon = osd->CreatePixmap(5, cRect(left, top + index * (height + left), config.menuItemLogoWidth, config.menuItemLogoWidth));
                pixmapIcon->Fill(clrTransparent);
            }
            pixmapIcon->DrawImage(cPoint(1, (height - config.iconHeight) / 2), imgLoader.GetImage());
        }
        drawn = true;
    }
    std::string delimiter = del;
    delimiter.erase(delimiter.find_last_not_of("-")+1);
    int x = config.iconHeight + 3;
    int y = (height - font->Height()) / 2;
    pixmap->DrawText(cPoint(x, y), delimiter.c_str(), Theme.Color(clrMenuFontMenuItemSep), clrTransparent, font);
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

// cNopacityMainMenuItem  -------------
cNopacityMainMenuItem::cNopacityMainMenuItem(cOsd *osd, const char *text, bool sel) : cNopacityMenuItem (osd, text, sel) {
}

cNopacityMainMenuItem::~cNopacityMainMenuItem(void) {
}

std::string cNopacityMainMenuItem::items[16] = { "Schedule", "Channels", "Timers", "Recordings", "Setup", "Commands",
                                                "OSD", "EPG", "DVB", "LNB", "CAM", "Recording", "Replay", "Miscellaneous", "Plugins", "Restart"};

cString cNopacityMainMenuItem::GetIconName() {
    std::string element = *menuEntry;
    for (int i=0; i<16; i++) {
        std::string s = trVDR(items[i].c_str());
        if (s == element)
            return items[i].c_str();
    }
    return menuEntry;
}

void cNopacityMainMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + config.iconHeight + 10;
    int pixmapWidth = width - config.iconHeight - 10;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + left), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
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
    int spaceLeft = left;
    if (hasIcon)
        spaceLeft += config.iconHeight;
    int totalTextWidth = width - spaceLeft;
    int numberWidth = font->Width("xxx");
    int textWidth = font->Width(*menuEntry);
    if ((numberWidth +  textWidth) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(numberWidth + textWidth, totalTextWidth);
        strEntryFull = strEntry.c_str();
        cTextWrapper twEntry;
        std::stringstream sstrEntry;
        twEntry.Set(strEntry.c_str(), font, width - spaceLeft - numberWidth);
        sstrEntry << twEntry.GetLine(0) << "...";
        strEntry = sstrEntry.str();
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
    pixmap->Fill(Theme.Color(clrMenuBorder));
    int handleBgrd = (current)?handleBackgrounds[3]:handleBackgrounds[2];
    pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    if (selectable) {
        cString cIcon = GetIconName();
        if (!drawn) {
            cImageLoader imgLoader;
            if (imgLoader.LoadIcon(*cIcon, config.iconHeight)) {
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
    } else {
        DrawDelimiter(*itemTabs[1], "Channelseparator", handleBgrd);
    }
}

// cNopacityScheduleMenuItem  -------------

cNopacityScheduleMenuItem::cNopacityScheduleMenuItem(cOsd *osd, const cEvent *Event, const cChannel *Channel, eTimerMatch TimerMatch, bool sel, eMenuCategory category) 
: cNopacityMenuItem (osd, "", sel) {
    this->category = category;
    this->Event = Event;
    this->Channel = Channel;
    this->TimerMatch = TimerMatch;
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
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + left), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityScheduleMenuItem::CreateText() {
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
    int spaceLeft = left;
    if (hasIcon)
        spaceLeft += config.menuItemLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strTitle.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        scrollTitle = true;
        totalTextWidth = max(font->Width(strTitle.c_str()), totalTextWidth);
        strTitleFull = strTitle.c_str();
        strSubTitleFull = strSubTitle.c_str();
        cTextWrapper twTitle;
        std::stringstream sstrTitle;
        twTitle.Set(strTitle.c_str(), font, width - spaceLeft);
        sstrTitle << twTitle.GetLine(0) << "...";
        strTitle = sstrTitle.str();
    }
    if (fontSmall->Width(strSubTitle.c_str()) > (width - spaceLeft)) {
        if (!scrollable) {
            scrollable = true;
            strTitleFull = strTitle.c_str();
            strSubTitleFull = strSubTitle.c_str();
        }
        scrollSubTitle = true;
        totalTextWidth = max(fontSmall->Width(strSubTitle.c_str()), totalTextWidth);
        cTextWrapper twSubtitle;
        std::stringstream sstrSubtitle;
        twSubtitle.Set(strSubTitle.c_str(), fontSmall, width - spaceLeft);
        sstrSubtitle << twSubtitle.GetLine(0) << "...";
        strSubTitle = sstrSubtitle.str();
    }
    return totalTextWidth;

}

void cNopacityScheduleMenuItem::SetTextFull(void) {
    pixmapTextScroller->Fill(clrTransparent);
    if (scrollTitle)
        pixmapTextScroller->DrawText(cPoint(5, titleY), strTitleFull.c_str(), Theme.Color(clrMenuFontMenuItemTitle), clrTransparent, font);
    if (scrollSubTitle)
        pixmapTextScroller->DrawText(cPoint(5, titleY + font->Height() - 2), strSubTitleFull.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::SetTextShort(void) {
    pixmapTextScroller->Fill(clrTransparent);
    if (scrollTitle)
        pixmapTextScroller->DrawText(cPoint(5, titleY), strTitle.c_str(), Theme.Color(clrMenuFontMenuItemTitle), clrTransparent, font);
    if (scrollSubTitle)
        pixmapTextScroller->DrawText(cPoint(5, titleY + font->Height() - 2), strSubTitle.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, fontSmall);
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
            SetTextShort();
            drawn = true;
        }
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
        if (current) {
            infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow);
            infoTextWindow->SetGeometry(textWindow);
            infoTextWindow->SetText(Event->Description());
            infoTextWindow->Start();
        }
    }
}

void cNopacityScheduleMenuItem::DrawBackground(int textLeft) {
    int handleBgrd = (current)?handleBackgrounds[5]:handleBackgrounds[4];
    pixmap->Fill(Theme.Color(clrMenuBorder));
    pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    pixmap->DrawText(cPoint(textLeft, 3), strDateTime.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, font);
    if (TimerMatch == tmFull) {
        cImageLoader imgLoader;
        if (imgLoader.LoadIcon("activetimer", 64, 64)) {
            pixmapIcon->DrawImage(cPoint(width - 66, 2), imgLoader.GetImage());
        }
    } else if (TimerMatch == tmPartial) {
        cImageLoader imgLoader;
        if (imgLoader.LoadIcon("activetimersmall", 32, 32)) {
            pixmapIcon->DrawImage(cPoint(width - 34, 2), imgLoader.GetImage());
        }
    }
    if (!scrollTitle)
        pixmap->DrawText(cPoint(textLeft, titleY), strTitle.c_str(), Theme.Color(clrMenuFontMenuItemTitle), clrTransparent, font);
    if (!scrollSubTitle)
        pixmap->DrawText(cPoint(textLeft, titleY + font->Height() - 2), strSubTitle.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::DrawLogo(int logoWidth, int logoHeight) {
    if (Channel && Channel->Name()) {
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(Channel->Name(), logoWidth, logoHeight)) {
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

cNopacityChannelMenuItem::cNopacityChannelMenuItem(cOsd *osd, const cChannel *Channel, bool sel) : cNopacityMenuItem (osd, "", sel) {
    this->Channel = Channel;
}

cNopacityChannelMenuItem::~cNopacityChannelMenuItem(void) {
}

void cNopacityChannelMenuItem::CreatePixmapTextScroller(int totalWidth) {
    int pixmapLeft = left + config.menuItemLogoWidth + 10;
    int pixmapWidth = width - config.menuItemLogoWidth - 10;
    int drawPortWidth = totalWidth + 10;
    pixmapTextScroller = osd->CreatePixmap(4, cRect(pixmapLeft, top + index * (height + left), pixmapWidth, height), cRect(0, 0, drawPortWidth, height));
    pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityChannelMenuItem::CreateText() {
    strEntry = Channel->Name();
}

int cNopacityChannelMenuItem::CheckScrollable(bool hasIcon) {
    int spaceLeft = left;
    if (hasIcon)
        spaceLeft += config.menuItemLogoWidth;
    int totalTextWidth = width - spaceLeft;
    if (font->Width(strEntry.c_str()) > (width - spaceLeft)) {
        scrollable = true;
        totalTextWidth = max(font->Width(strEntry.c_str()), totalTextWidth);
        strEntryFull = strEntry.c_str();
        cTextWrapper twEntry;
        std::stringstream sstrEntry;
        twEntry.Set(strEntry.c_str(), font, width - spaceLeft);
        sstrEntry << twEntry.GetLine(0) << "...";
        strEntry = sstrEntry.str();
    }
    return totalTextWidth;
}

void cNopacityChannelMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(5, (height - font->Height())/2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityChannelMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(5, (height - font->Height())/2), strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityChannelMenuItem::Render() {
    
    int handleBgrd = (current)?handleBackgrounds[5]:handleBackgrounds[4];
    
    if (selectable) {                           //Channels
        pixmap->Fill(Theme.Color(clrMenuBorder));
        int logoWidth = config.menuItemLogoWidth;
        int logoHeight = config.menuItemLogoHeight;
        pixmap->DrawImage(cPoint(1, 1), handleBgrd);
        if (!drawn) {
            cImageLoader imgLoader;
            if (imgLoader.LoadLogo(Channel->Name(), logoWidth, logoHeight)) {
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
    } else {                                    //Channelseparators
        DrawDelimiter(Channel->Name(), "Channelseparator", handleBgrd);
    }
}

// cNopacityDefaultMenuItem  -------------

cNopacityDefaultMenuItem::cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool sel) : cNopacityMenuItem (osd, text, sel) {
    scrollCol = -1;
}

cNopacityDefaultMenuItem::~cNopacityDefaultMenuItem(void) {
}

void cNopacityDefaultMenuItem::SetTextFull(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, (height - font->Height()) / 2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityDefaultMenuItem::SetTextShort(void) {
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapTextScroller->Fill(clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, (height - font->Height()) / 2), strEntry.c_str(), clrFont, clrTransparent, font);
}

int cNopacityDefaultMenuItem::CheckScrollable(bool hasIcon) {
    if (!selectable)
        return 0;
    int colWidth = 0;
    int colTextWidth = 0; 
    for (int i=0; i<numTabs; i++) {
        if (tabWidth[i] > 0) {
            colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
            colTextWidth = font->Width(*itemTabs[i]);
            if (colTextWidth > colWidth) {
                cTextWrapper itemTextWrapped;
                scrollable = true;
                scrollCol = i;
                strEntryFull = *itemTabs[i];
                itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
                strEntry = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
            }
            break;
        } else
            break;
    }
    if (scrollable) {
        pixmapTextScroller = osd->CreatePixmap(4, cRect(left + tabWidth[scrollCol], top + index * (height + left), tabWidth[scrollCol+numTabs], height), cRect(0, 0, colTextWidth+10, height));
        pixmapTextScroller->Fill(clrTransparent);
    }
    return 0;
}

void cNopacityDefaultMenuItem::Render() {
    pixmap->Fill(Theme.Color(clrMenuBorder));
    int handleBgrd = (current)?handleBackgrounds[1]:handleBackgrounds[0];
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    int colWidth = 0;
    int colTextWidth = 0; 
    cString itemText("");
    std::stringstream sstrText;
    for (int i=0; i<numTabs; i++) {
        if (tabWidth[i] > 0) {
            if (selectable) {
                if (i != scrollCol) {
                    colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
                    colTextWidth = font->Width(*itemTabs[i]);
                    if (colTextWidth > colWidth) {
                        cTextWrapper itemTextWrapped;
                        itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
                        itemText = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
                    } else {
                        itemText = itemTabs[i];
                    }
                    pixmap->DrawText(cPoint(tabWidth[i], (height - font->Height()) / 2), *itemText, clrFont, clrTransparent, font);
                } else {
                    if (!Running())
                        SetTextShort();
                }
            } else {
                sstrText << *itemTabs[i];
            }
        } else
            break;
    }
    if (!selectable) {
        pixmap->DrawText(cPoint(1, (height - font->Height()) / 2), sstrText.str().c_str(), clrFont, clrTransparent, font);
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
    int handleBgrd = (current)?handleBackgrounds[1]:handleBackgrounds[0];
    pixmap->DrawImage(cPoint(1, 1), handleBgrd);
    pixmap->DrawText(cPoint(5, (height - font->Height())/2), Text, Theme.Color(clrTracksFontButtons), clrTransparent, font);
}
