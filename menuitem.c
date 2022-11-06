#include "menuitem.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include "config.h"
#include "helpers.h"
#include "imageloader.h"

// cNopacityMenuItem  -------------

cNopacityMenuItem::cNopacityMenuItem(cOsd *osd, const char *text, bool sel) : cThread("MenuItem") {
    this->osd = osd;
    drawn = false;
    Text = text;
    selectable = sel;
    current = false;
    wasCurrent = false;
    scrollable = false;
    itemTabs = NULL;
    tabWidth = NULL;
    infoTextWindow = NULL;
}

cNopacityMenuItem::~cNopacityMenuItem(void) {
    Cancel(-1);
    if (infoTextWindow) {
        delete infoTextWindow;
    }
    while (Active())
        cCondWait::SleepMs(10);
    delete [] itemTabs;
    delete [] tabWidth;
    osd->DestroyPixmap(pixmapBackground);
    osd->DestroyPixmap(pixmapStatic);
    osd->DestroyPixmap(pixmapTextScroller);
    osd->DestroyPixmap(pixmapForeground);
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
    textLeft = spaceMenu;
    textRight = spaceMenu;
}

void cNopacityMenuItem::CreatePixmaps(bool createPixmapFg) {
    pixmapBackground = CreatePixmap(osd, "pixmapBackground", 3, cRect(left, top + index * (height + spaceMenu), width, height));
    if (config.GetValue("displayType") == dtGraphical && createPixmapFg) {
        pixmapForeground = CreatePixmap(osd, "pixmapForeground", 6, cRect(left, top + index * (height + spaceMenu), width, height));
    }
    pixmapStatic = CreatePixmap(osd, "pixmapStatic", 5, cRect(left, top + index * (height + spaceMenu), width, height));
    PixmapFill(pixmapStatic, clrTransparent);
}

void cNopacityMenuItem::CreatePixmapTextScroller(int totalWidth, int pixmapLeft, int pixmapWidth) {
    int pixmapleft = (pixmapLeft) ? left + pixmapLeft : left;
    int pixmapwidth =(pixmapWidth) ? pixmapWidth : width;
    pixmapTextScroller = CreatePixmap(osd, "pixmapTextScroller", 4, cRect(pixmapleft,
                                                                          top + index * (height + spaceMenu),
                                                                          pixmapwidth, height),
                                                                    cRect(0, 0, totalWidth, height));
    PixmapFill(pixmapTextScroller, clrTransparent);
}

void cNopacityMenuItem::SetAlpha(int alpha) {
    PixmapSetAlpha(pixmapBackground, alpha);
    PixmapSetAlpha(pixmapForeground, alpha);
    PixmapSetAlpha(pixmapStatic, alpha);
    PixmapSetAlpha(pixmapTextScroller, alpha);
    if (infoTextWindow) infoTextWindow->SetAlpha(alpha);
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

void cNopacityMenuItem::DrawBackground(eSkinElementType type, eSkinElementType fgType) {
    if (!pixmapBackground)
        return;

    if (config.GetValue("displayType") == dtBlending) {
        PixmapFill(pixmapBackground, Theme.Color(clrMenuBorder));
        cImage *back = imgCache->GetSkinElement(type);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            PixmapFill(pixmapBackground, clrTransparent);
        }
        if (fgType != seNone) {
            cImage *fore = imgCache->GetSkinElement(fgType);
            if (pixmapForeground && fore) {
                pixmapForeground->DrawImage(cPoint(0, 0), *fore);
            } else {
                PixmapFill(pixmapForeground, clrTransparent);
            }
        }
    } else {
        PixmapFill(pixmapBackground, Theme.Color(clrMenuBorder));
        tColor col = (current) ? Theme.Color(clrMenuItemHigh) : Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width - 2, height - 2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }
}

void cNopacityMenuItem::DrawDelimiter(const char *del, const char *icon, eSkinElementType seType) {
    if (!pixmapBackground)
        goto Next;

    if ((config.GetValue("displayType") == dtBlending) && (seType != seNone)) {
        PixmapFill(pixmapBackground, Theme.Color(clrSeparatorBorder));
        cImage *back = imgCache->GetSkinElement(seType);
        if (back)
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrSeparatorBorder));
    } else if ((config.GetValue("displayType") == dtGraphical) && (seType != seNone)) {
        cImage *back = imgCache->GetSkinElement(seType);
        if (back)
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        else
            PixmapFill(pixmapBackground, clrTransparent);
        PixmapFill(pixmapForeground, clrTransparent);
    } else {
        PixmapFill(pixmapBackground, Theme.Color(clrSeparatorBorder));
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrMenuItem));
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrSeparatorBorder));
    }
Next:
    if (!pixmapStatic)
        return;
    if (!drawn) {
        cImage *imgIcon = imgCache->GetSkinIcon(icon, height-2*geoManager->menuSpace, height-2*geoManager->menuSpace);
        if (imgIcon) {
/*            if (pixmapStatic == NULL) {
                pixmapStatic = CreatePixmap(osd, "pixmapStatic", 5, cRect(left, top + index * (height + spaceMenu), width, height));
                pixmapStatic->Fill(clrTransparent);
            }*/
            pixmapStatic->DrawImage(cPoint(geoManager->menuSpace, geoManager->menuSpace), *imgIcon);
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
    int x = height + 3;
    int y = (height - font->Height()) / 2;
    pixmapStatic->DrawText(cPoint(x, y), delimiter.c_str(), Theme.Color(clrMenuFontMenuItemSep), clrTransparent, font);
}

void cNopacityMenuItem::Action(void) {
    if (!pixmapTextScroller)
        return;

    bool carriageReturn = (config.GetValue("scrollMode") == 0) ? true : false;
    int scrollDelta = 1;
    int scrollDelay = config.GetValue("menuScrollDelay") * 1000;
    DoSleep(scrollDelay);
    cPixmap::Lock();
    if (Running())
        SetTextFull();
    cPixmap::Unlock();
    int drawPortX;

    int FrameTime = 0;
    if (config.GetValue("menuScrollSpeed") == 1)
        FrameTime = 50;
    else if (config.GetValue("menuScrollSpeed") == 2)
        FrameTime = 30;
    else if (config.GetValue("menuScrollSpeed") == 3)
        FrameTime = 15;
    if (!Running())
        return;
    int maxX = pixmapTextScroller->DrawPort().Width() - pixmapTextScroller->ViewPort().Width();
    bool doSleep = false;
    while (Running()) {
        if (doSleep) {
            DoSleep(scrollDelay);
            doSleep = false;
        }
        if (!Running())
            return;
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        drawPortX = pixmapTextScroller->DrawPort().X();
        drawPortX -= scrollDelta;
        cPixmap::Unlock();

        if (abs(drawPortX) > maxX) {
            DoSleep(scrollDelay);
            if (carriageReturn)
                drawPortX = 0;
            else {
                scrollDelta *= -1;
                drawPortX -= scrollDelta;
            }
            doSleep = true;
        }
        if (!carriageReturn && (drawPortX == 0)) {
            scrollDelta *= -1;
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
    int radius = config.GetValue("cornerRadius");
    if (!pixmapBackground || radius < 3)
        return;

    pixmapBackground->DrawEllipse(cRect(0,0,radius,radius), borderColor, -2);
    pixmapBackground->DrawEllipse(cRect(-1,-1,radius,radius), clrTransparent, -2);

    pixmapBackground->DrawEllipse(cRect(width-radius,0,radius,radius), borderColor, -1);
    pixmapBackground->DrawEllipse(cRect(width-radius+1,-1,radius,radius), clrTransparent, -1);

    pixmapBackground->DrawEllipse(cRect(0,height-radius,radius,radius), borderColor, -3);
    pixmapBackground->DrawEllipse(cRect(-1,height-radius+1,radius,radius), clrTransparent, -3);

    pixmapBackground->DrawEllipse(cRect(width-radius,height-radius,radius,radius), borderColor, -4);
    pixmapBackground->DrawEllipse(cRect(width-radius+1,height-radius+1,radius,radius), clrTransparent, -4);
}

void cNopacityMenuItem::DrawChannelLogoBackground(void) {
    if (!pixmapBackground || !config.GetValue("menuChannelLogoBackground"))
        return;
    int logoWidth = geoManager->menuLogoWidth;
    pixmapBackground->DrawRectangle(cRect(spaceMenu, spaceMenu, logoWidth, height - 2 * spaceMenu), Theme.Color(clrMenuChannelLogoBack));
}

void cNopacityMenuItem::DrawLogo(const cChannel *Channel, int logoWidth, int logoHeight, cFont *font, bool drawText) {
    if (!pixmapStatic)
        return;

    cImage *logo = imgCache->GetLogo(ctLogoMenuItem, Channel);
    if (logo) {
        pixmapStatic->DrawImage(cPoint(2 + (logoWidth - logo->Width()) / 2, (logoHeight - logo->Height()) / 2), *logo);
    } else if (drawText) {
        cTextWrapper channel;
        channel.Set(Channel->Name(), fontSmall, logoWidth);
        int lines = channel.Lines();
        int lineHeight = height / 3;
        int heightChannel = std::min(3, lines) * lineHeight;
        int y = spaceMenu + (height - heightChannel) / 2;
        for (int line = 0; line < lines; line++) {
            int x = spaceMenu + (logoWidth - fontSmall->Width(channel.GetLine(line))) / 2;
            pixmapStatic->DrawText(cPoint(x, y + lineHeight * line),
                                   channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, fontSmall);
        }
    }
}

// cNopacityMainMenuItem  -------------
cNopacityMainMenuItem::cNopacityMainMenuItem(cOsd *osd, const char *text, bool sel, bool setup) : cNopacityMenuItem (osd, text, sel) {
    this->isSetup = setup;
    font = fontManager->menuItemLarge;
}

void cNopacityMainMenuItem::DrawBackgroundMainMenu(void) {
    if (!pixmapBackground)
        return;

    cImage *back = NULL;
    if ((config.GetValue("displayType") == dtBlending) || (config.GetValue("displayType") == dtGraphical)){
        eSkinElementType type;
        if (!isSetup)
            type = (current)?seMainHigh:seMain;
        else
            type = (current)?seSetupHigh:seSetup;
        back = imgCache->GetSkinElement(type);
        if (back) {
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        }
    }
    if (config.GetValue("displayType") == dtBlending) {
        PixmapFill(pixmapBackground, Theme.Color(clrMenuBorder));
        if (back) {
            pixmapBackground->DrawImage(cPoint(1, 1), *back);
        }
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    } else if (config.GetValue("displayType") == dtGraphical) {
        if (back) {
            pixmapBackground->DrawImage(cPoint(0, 0), *back);
        } else {
            PixmapFill(pixmapBackground, clrTransparent);
        }
        eSkinElementType menuButtonTop;
        if (!isSetup)
            menuButtonTop = seMainTop;
        else
            menuButtonTop = seSetupTop;
        cImage *fore = imgCache->GetSkinElement(menuButtonTop);
        if (pixmapForeground && fore) {
            pixmapForeground->DrawImage(cPoint(0, 0), *fore);
        } else {
            PixmapFill(pixmapForeground, clrTransparent);
        }
    } else {
        PixmapFill(pixmapBackground, Theme.Color(clrMenuBorder));
        tColor col = (current) ? Theme.Color(clrMenuItemHigh) : Theme.Color(clrMenuItem);
        pixmapBackground->DrawRectangle(cRect(1, 1, width-2, height-2), col);
        if (config.GetValue("roundedCorners"))
            DrawRoundedCorners(Theme.Color(clrMenuBorder));
    }
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

int cNopacityMainMenuItem::CheckScrollable1(int maxwidth) {
    int totalTextWidth = maxwidth;
    int numberWidth = font->Width("xxx");
    int textWidth = font->Width(*menuEntry);
    if ((numberWidth + textWidth) > (maxwidth)) {
        scrollable = true;
        totalTextWidth = std::max(numberWidth + textWidth, totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, maxwidth - numberWidth, font);
    }
    return totalTextWidth;
}

void cNopacityMainMenuItem::SetText(bool full) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    int x = 0;
    int numberTotalWidth = font->Width("xxx");
    int numberWidth = font->Width(*menuNumber);
    pixmapTextScroller->DrawText(cPoint(x + (numberTotalWidth - numberWidth) / 2, (height - font->Height()) / 2), *menuNumber, clrFont, clrTransparent, font);
    x += numberTotalWidth;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height()) / 2), (full) ? strEntryFull.c_str() : strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityMainMenuItem::Render(bool initial, bool fadeout) {
    textLeft = 2 * spaceMenu;
    DrawBackgroundMainMenu();
    if (selectable) {
        if (!drawn) {
            if (config.GetValue("useMenuIcons")) {
                cString cIcon = GetIconName();
                cImage *imgIcon = imgCache->GetMenuIcon(*cIcon);
                if (pixmapStatic && imgIcon)
                    pixmapStatic->DrawImage(cPoint(spaceMenu, spaceMenu), *imgIcon);
                textLeft += geoManager->menuMainMenuIconSize + spaceMenu;
            }
            int pixmapWidth = width - textLeft - 2 * spaceMenu;
            int textWidth = CheckScrollable1(pixmapWidth);
            if (textWidth > 0)
                CreatePixmapTextScroller(textWidth, textLeft, pixmapWidth);
            drawn = true;
        }
        if (!Running())
            SetText();
        if (config.GetValue("animation") && config.GetValue("menuScrollSpeed")) {
            if (current && scrollable && !Running())
                Start();
        }
        if (pixmapTextScroller && wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetText();
            Cancel(-1);
        }
    } else {
        DrawDelimiter(strEntry.c_str(), "skinIcons/channeldelimiter", (config.GetValue("displayType") != dtFlat) ? seNone : (isSetup ? seSetup : seMain));
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
    font = fontManager->menuItemSchedule;
    fontSmall = fontManager->menuItemScheduleSmall;
    fontEPGWindow = fontManager->menuEPGInfoWindow;
    fontEPGWindowLarge = fontManager->menuEPGInfoWindowLarge;
}

void cNopacityScheduleMenuItem::CreateText() {
    if (!Event)
        return;
    std::stringstream sstrDateTime;
    time_t now = time(NULL);
    std::string dayNow = *WeekDayName(now);
    std::string dayEvent = *WeekDayName(Event->StartTime());
    if ((Event->StartTime() - now > 86400) || (dayNow != dayEvent))
        sstrDateTime << dayEvent << " " << *ShortDateString(Event->StartTime()) << " ";
    sstrDateTime << *Event->GetTimeString();
    sstrDateTime << " - " << *Event->GetEndTimeString();
    strDateTime = sstrDateTime.str();
    if (Event->Title())
        strTitle = Event->Title();
    if (Event->ShortText())
        strSubTitle = Event->ShortText();
}

int cNopacityScheduleMenuItem::CheckScrollable1(int maxwidth) {
    int totalTextWidth = maxwidth;
    if (font->Width(strTitle.c_str()) > (maxwidth)) {
        scrollable = true;
        scrollTitle = true;
        totalTextWidth = std::max(font->Width(strTitle.c_str()), totalTextWidth);
        strTitleFull = strTitle.c_str();
        strSubTitleFull = strSubTitle.c_str();
        strTitle = CutText(strTitle, maxwidth, font);
    }
    if (!config.GetValue("menuSchedulesShowShortText"))
        return totalTextWidth;
    if (fontSmall->Width(strSubTitle.c_str()) > (maxwidth)) {
        if (!scrollable) {
            scrollable = true;
            strTitleFull = strTitle.c_str();
            strSubTitleFull = strSubTitle.c_str();
        }
        scrollSubTitle = true;
        totalTextWidth = std::max(fontSmall->Width(strSubTitle.c_str()), totalTextWidth);
        strSubTitle = CutText(strSubTitle, maxwidth, fontSmall);
    }
    return totalTextWidth;

}

void cNopacityScheduleMenuItem::SetText(bool full) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, titleY), (full) ? strTitleFull.c_str() : strTitle.c_str(), clrFont, clrTransparent, font);
    if (!config.GetValue("menuSchedulesShowShortText"))
        return;
    pixmapTextScroller->DrawText(cPoint(0, subTitleY), (full) ? strSubTitleFull.c_str() : strSubTitle.c_str(), clrFont, clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::DrawRemaining(int x, int y, int width) {
    if (!config.GetValue("menuSchedulesShowProgressBar") || !pixmapBackground || !Event)
        return;

    time_t now = time(NULL);
    if ((now < Event->StartTime()) || (now > Event->EndTime()))
        return;
    int seen = now - Event->StartTime();
    int total = Event->EndTime() - Event->StartTime();
    if (total == 0)
        return;
    double percentSeen = (double)seen/total;

    tColor clrBack = (current) ? Theme.Color(clrProgressBarBackHigh) : Theme.Color(clrProgressBarBack);
    tColor clrBar = (current) ? Theme.Color(clrProgressBarHigh) : Theme.Color(clrProgressBar);
    pixmapBackground->DrawEllipse(cRect(x, y, 7, 7), clrBack);
    pixmapBackground->DrawEllipse(cRect(x + width, y, 7, 7), clrBack);
    pixmapBackground->DrawRectangle(cRect(x + 4, y, width - 1, 7), clrBack);
    pixmapBackground->DrawEllipse(cRect(x + 1, y + 1, 5, 5), clrBar);

    if (percentSeen > 0.0)
        pixmapBackground->DrawEllipse(cRect(x + (width * percentSeen), y + 1, 5, 5), clrBar);
    pixmapBackground->DrawRectangle(cRect(x + 4, y + 1, (width - 1) * percentSeen, 5), clrBar);
}

void cNopacityScheduleMenuItem::DrawStatic(int textLeft) {
    if (!pixmapStatic)
        return;

    if (TimerMatch == tmFull) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/activetimer", 64, 64);
        if (imgIcon) {
            int iconLeft = width - (spaceMenu + 64);
            pixmapStatic->DrawImage(cPoint(iconLeft, (pixmapStatic->ViewPort().Height() - imgIcon->Height()) / 2), *imgIcon);
            iconwidth = width - iconLeft;
        }
    }
    if (TimerMatch == tmPartial) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/activetimersmall", 32, 32);
        if (imgIcon) {
            int iconLeft = width - (spaceMenu + 32);
            pixmapStatic->DrawImage(cPoint(iconLeft, (pixmapStatic->ViewPort().Height() - imgIcon->Height()) / 2), *imgIcon);
            iconwidth = width - iconLeft;
        }
    }
    if (!config.GetValue("menuSchedulesShowTime"))
        return;
    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(textLeft, dateTimeY), strDateTime.c_str(), clrFont, clrTransparent, font);
}

void cNopacityScheduleMenuItem::SetY(void) {
    dateTimeY = 0;
    titleY = (height - font->Height()) / 2;
    subTitleY = titleY + font->Height() - 2;
    progressBarY = 7 * height / 8;
    if (config.GetValue("menuSchedulesShowTime")) {
        if (!config.GetValue("menuSchedulesShowShortText")) {
            dateTimeY = (7 * height / 16 - font->Height()) / 2;
            titleY = 7 * height / 16 + dateTimeY;
        }
        if (!config.GetValue("menuSchedulesShowProgressBar") && !config.GetValue("menuSchedulesShowShortText")) {
            dateTimeY = 2 * (height / 2 - font->Height()) / 3;
            titleY = height / 2 + (height / 2 - font->Height()) / 3;
        }
    }
    if (!config.GetValue("menuSchedulesShowTime") && config.GetValue("menuSchedulesShowShortText")) {
        titleY = (3 * height / 4 - font->Height()) / 2;
        subTitleY = titleY + font->Height() + 2;
    }
}

void cNopacityScheduleMenuItem::Render(bool initial, bool fadeout) {
    textLeft = 2 * spaceMenu;

    if (selectable) {
        if (!drawn)
            SetY();
        eSkinElementType type = (current) ? seSchedulesHigh : seSchedules;
        DrawBackground(type, seSchedulesTop);
        if (Channel && Channel->Name()) {
	    DrawChannelLogoBackground();
            int logoWidth = geoManager->menuLogoWidth;
            int logoHeight = geoManager->menuLogoHeight;
            if (!drawn) {
                DrawLogo(Channel, logoWidth, logoHeight, font, true);
            }
            textLeft += logoWidth + spaceMenu;
        }
        DrawStatic(textLeft);	
        if (!drawn) {
            int pixmapWidth = width - textLeft - iconwidth - 2 * spaceMenu;
            int textWidth = CheckScrollable1(pixmapWidth);
            if (textWidth > 0) {
                CreatePixmapTextScroller(textWidth, textLeft, pixmapWidth);
            }
            drawn = true;
        }
        int progressBarDelta = 0;
        if (config.GetValue("displayType") == dtGraphical && textLeft < 20)
            progressBarDelta = 10;
        DrawRemaining(textLeft + progressBarDelta, progressBarY, width - textLeft - 20 - progressBarDelta);
        if (!Running())
            SetText();
        if (config.GetValue("animation") && config.GetValue("menuScrollSpeed")) {
            if (current && scrollable && !Running())
                Start();
        }
        if (pixmapTextScroller && wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetText();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        if (current && Event) {
            if (config.GetValue("menuSchedulesWindowMode") == 0) {
              if (config.GetValue("animation") && !fadeout) {
                //window mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetText(Event->Description());
                infoTextWindow->SetPoster(Event, NULL);
                infoTextWindow->SetInitial(initial);
                infoTextWindow->Start();
              }
            } else {
                //fullscreen mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetPoster(Event, NULL, true);
                infoTextWindow->SetEvent(Event);
            }
        }
    } else {
        if (Event) {
            DrawDelimiter(Event->Title(), "skinIcons/daydelimiter", (config.GetValue("displayType") != dtFlat) ? seSchedules : seNone);
            return;
        }
       	if (Channel) {
            DrawDelimiter(Channel->Name(), "skinIcons/channeldelimiter", (config.GetValue("displayType") != dtFlat) ? seSchedules : seNone);
        }
    }
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
    font = fontManager->menuItemChannel;
    fontSmall = fontManager->menuItemChannelSmall;
    fontEPGWindow = fontManager->menuEPGInfoWindow;
}

void cNopacityChannelMenuItem::CreateText() {
    strEntry = cString::sprintf("%d %s", Channel->Number(), Channel->Name());
    if (config.GetValue("menuChannelDisplayMode") == 0) {
        const cSource *source = Sources.Get(Channel->Source());
        if (source)
            strChannelSource = cString::sprintf("%s - %s", *cSource::ToString(source->Code()),  source->Description());
        else
            strChannelSource = "";
        strChannelInfo = cString::sprintf("%s %d, %d MHz", tr("Transp."), Channel->Transponder(), Channel->Frequency() / 1000);
    } else {
        readCurrentEPG();
    }
}

int cNopacityChannelMenuItem::CheckScrollable1(int maxwidth) {
    int totalTextWidth = maxwidth;
    if (font->Width(strEntry.c_str()) > (maxwidth)) {
        scrollable = true;
        totalTextWidth = std::max(font->Width(strEntry.c_str()), totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, maxwidth, font);
    } else
        strEntryFull = strEntry;

    if (fontSmall->Width(strEpgInfo.c_str()) > (maxwidth)) {
        scrollable = true;
        totalTextWidth = std::max(fontSmall->Width(strEpgInfo.c_str()), totalTextWidth);
        strEpgInfoFull = strEpgInfo.c_str();
        strEpgInfo = CutText(strEpgInfo, maxwidth, fontSmall);
    } else
        strEpgInfoFull = strEpgInfo;

    return totalTextWidth;
}

void cNopacityChannelMenuItem::SetText(bool full) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    int heightChannelName = 0;
    if (config.GetValue("menuChannelDisplayMode") == 2) {
        heightChannelName = (height - font->Height()) / 2;
    } else {
        heightChannelName = (height / 2 - font->Height()) / 2;
    }
    pixmapTextScroller->DrawText(cPoint(0, heightChannelName), (full) ? strEntryFull.c_str() : strEntry.c_str(), clrFont, clrTransparent, font);

    if (!(config.GetValue("menuChannelDisplayMode") == 1))
        return;

    int heightTimeInfo = 0;
    int heightEPGInfo = 0;
    if (config.GetValue("menuChannelDisplayTime")) {
        heightTimeInfo = height / 2 + (height / 4 - fontSmall->Height()) / 2;
        heightEPGInfo = 3 * height / 4 + (height / 4 - fontSmall->Height()) / 2;
    } else {
        heightEPGInfo = height / 2 + (height / 4 - fontSmall->Height()) / 2;
    }
    if (config.GetValue("menuChannelDisplayTime")) {
        pixmapTextScroller->DrawText(cPoint(0, heightTimeInfo), strTimeInfo.c_str(), clrFont, clrTransparent, fontSmall);
    }
    pixmapTextScroller->DrawText(cPoint(0, heightEPGInfo), (full) ? strEpgInfoFull.c_str() : strEpgInfo.c_str(), clrFont, clrTransparent, fontSmall);
}

void cNopacityChannelMenuItem::DrawStatic(void) {
    if (!pixmapStatic || !config.GetValue("menuChannelDisplayMode") == 0)
        return;

    int x = textLeft + spaceMenu;
    int encryptedSize = height / 4 - 2;
    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(x, 3 * height / 4 + (height / 4 - fontSmall->Height()) / 2), *strChannelInfo, clrFont, clrTransparent, fontSmall);
    if (Channel->Ca()) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/encrypted", encryptedSize, encryptedSize);
        if (imgIcon) {
            pixmapStatic->DrawImage(cPoint(x, height / 2 + 1), *imgIcon);
            x += encryptedSize + 10;
        }
    }
    pixmapStatic->DrawText(cPoint(x, height / 2 + (height / 4 - fontSmall->Height()) / 2), *strChannelSource, clrFont, clrTransparent, fontSmall);
}

void cNopacityChannelMenuItem::readCurrentEPG(void) {
    LOCK_SCHEDULES_READ;
    const cSchedule *Schedule = Schedules->GetSchedule(Channel);
    if (!Schedule) {
        strEpgInfo = tr("No EPG Information found");
        strTimeInfo = "";
    } else {
        const cEvent *PresentEvent = Schedule->GetPresentEvent();
        if (!PresentEvent) {
            strEpgInfo = tr("No EPG Information found");
            strTimeInfo = "";
        } else if (PresentEvent->Title()) {
            strEpgInfo = PresentEvent->Title();
            strTimeInfo = *cString::sprintf("%s - %s:", *PresentEvent->GetTimeString(), *PresentEvent->GetEndTimeString());
        } else {
            strEpgInfo = "";
            strTimeInfo = "";
        }
    }
}

std::string cNopacityChannelMenuItem::readEPG(void) {
    std::stringstream sstrText;
    LOCK_SCHEDULES_READ;
    const cSchedule *Schedule = Schedules->GetSchedule(Channel);
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
                    if (i < config.GetValue("numEPGEntriesChannelsMenu"))
                        sstrText << "\n";
                    else
                        break;
                }
            }
        }
    }
    return sstrText.str();
}

void cNopacityChannelMenuItem::Render(bool initial, bool fadeout) {
    int logoWidth = geoManager->menuLogoWidth;
    int logoHeight = geoManager->menuLogoHeight;
    textLeft = logoWidth + 3 * spaceMenu;
    if (selectable) { //Channels
        eSkinElementType type = (current) ? seChannelsHigh : seChannels;
        DrawBackground(type, seChannelsTop);
        DrawStatic();
        DrawChannelLogoBackground();
        if (!drawn) {
            if (Channel && Channel->Name())
                DrawLogo(Channel, logoWidth, logoHeight, font);
            int pixmapWidth = width - textLeft - 2 * spaceMenu;
            int textWidth = CheckScrollable1(pixmapWidth);
            if (textWidth > 0)
                CreatePixmapTextScroller(textWidth, textLeft, pixmapWidth);
            drawn = true;
        }
        if (!Running())
            SetText();
        if (config.GetValue("animation") && config.GetValue("menuScrollSpeed")) {
            if (current && scrollable && !Running())
                Start();
        }
        if (pixmapTextScroller && wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetText();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        if (current && Channel && (config.GetValue("menuChannelDisplayMode") == 0)) {
          if (config.GetValue("animation") && !fadeout) {
            infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
            infoTextWindow->SetGeometry(textWindow);
            infoTextWindow->SetText(readEPG().c_str());
            infoTextWindow->SetInitial(initial);
            infoTextWindow->Start();
          }
        }
    } else { //Channelseparators
        DrawDelimiter(Channel->Name(), "skinIcons/channeldelimiter", (config.GetValue("displayType")!=dtFlat )?seChannels:seNone);
    }
}

// cNopacityTimerMenuItem  -------------

cNopacityTimerMenuItem::cNopacityTimerMenuItem(cOsd *osd, const cTimer *Timer, bool sel, cRect *vidWin) : cNopacityMenuItem (osd, "", sel) {
    this->Timer = Timer;
    this->vidWin = vidWin;
    font = fontManager->menuItemTimers;
    fontSmall = fontManager->menuItemTimersSmall;
    fontEPGWindow = fontManager->menuEPGInfoWindow;
    fontEPGWindowLarge = fontManager->menuEPGInfoWindowLarge;
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

int cNopacityTimerMenuItem::CheckScrollable1(int maxwidth) {
    int totalTextWidth = maxwidth;
    if (font->Width(strEntry.c_str()) > (maxwidth)) {
        scrollable = true;
        totalTextWidth = std::max(font->Width(strEntry.c_str()), totalTextWidth);
        strEntryFull = strEntry.c_str();
        strEntry = CutText(strEntry, maxwidth, font);
    }
    return totalTextWidth;
}

void cNopacityTimerMenuItem::SetText(bool full) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    pixmapTextScroller->DrawText(cPoint(0, height / 2 + (height / 2 - font->Height()) / 2), (full) ? strEntryFull.c_str() : strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityTimerMenuItem::DrawStatic(int textLeft) {
    if (!pixmapStatic)
        return;

    int iconSize = height / 2;
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

    cImage *imgIcon = imgCache->GetSkinIcon(*iconName, iconSize, iconSize);
    if (imgIcon)
        pixmapStatic->DrawImage(cPoint(textLeft, 0), *imgIcon);

    cString dateTime("");
    if (firstDay)
        dateTime = cString::sprintf("! %s", strDateTime.c_str());
    else
        dateTime = strDateTime.c_str();
    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(textLeft + iconSize + spaceMenu, (height / 2 - fontSmall->Height()) / 2), *dateTime, clrFont, clrTransparent, fontSmall);
}

void cNopacityTimerMenuItem::Render(bool initial, bool fadeout) {
    int logoWidth = geoManager->menuLogoWidth;
    int logoHeight = geoManager->menuLogoHeight;
    textLeft = logoWidth + 3 * spaceMenu;
    if (selectable) {
        eSkinElementType type = (current) ? seTimersHigh : seTimers;
        DrawBackground(type, seTimersTop);

        DrawStatic(textLeft);
        DrawChannelLogoBackground();
        if (!drawn) {
            if (Timer && Timer->Channel() && Timer->Channel()->Name())
                DrawLogo(Timer->Channel(), logoWidth, logoHeight, font, true);
            int pixmapWidth = width - textLeft - 2 * spaceMenu;
            int textWidth = CheckScrollable1(pixmapWidth);
            if (textWidth > 0)
                CreatePixmapTextScroller(textWidth, textLeft, pixmapWidth);
            drawn = true;
        }
        if (!Running())
            SetText();
        if (config.GetValue("animation") && config.GetValue("menuScrollSpeed")) {
            if (current && scrollable && !Running())
                Start();
        }
        if (pixmapTextScroller && wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetText();
            Cancel(-1);
        }
        if (wasCurrent)
            if (infoTextWindow) {
                delete infoTextWindow;
                infoTextWindow = NULL;
            }
        const cEvent *Event = Timer->Event();
        if (current && Event) {
            if (config.GetValue("menuTimersWindowMode") == 0) {
              if (config.GetValue("animation") && !fadeout) {
                //window mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetText(Event->Description());
                infoTextWindow->SetPoster(Event, NULL, false);
                infoTextWindow->SetInitial(initial);
                infoTextWindow->Start();
              }
            } else {
                //fullscreen mode
                infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                infoTextWindow->SetGeometry(textWindow);
                infoTextWindow->SetPoster(Event, NULL, true);
                infoTextWindow->SetEvent(Event);
            }
        }
    }
}

// cNopacityRecordingMenuItem  -------------

cNopacityRecordingMenuItem::cNopacityRecordingMenuItem(cOsd *osd, const cRecording *Recording, bool sel, int Level, int Total, int New, cRect *vidWin) : cNopacityMenuItem (osd, "", sel) {
    this->Recording = Recording;
    this->isFolder = (Total > 0) ? true : false;
    this->Level = Level;
    this->Total = Total;
    this->New = New;
    this->vidWin = vidWin;
    font = fontManager->menuItemRecordings;
    fontSmall = fontManager->menuItemRecordingsSmall;
    fontEPGWindow = fontManager->menuEPGInfoWindow;
    fontEPGWindowLarge = fontManager->menuEPGInfoWindowLarge;
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
    posterHeight = height - 10;
    posterWidth = config.GetValue("posterWidth") * ((double)posterHeight / (double)config.GetValue("posterHeight"));
    //check first if manually set poster exists
    cString posterFound;
    cImageLoader imgLoader;
    hasManualPoster = imgLoader.SearchRecordingPoster(Recording->FileName(), posterFound);
    if (hasManualPoster)
        manualPosterPath = posterFound;
    //no manually set poster found, check scraper
    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper && (config.GetValue("scraperInfo") == 1)) {
        thumb.event = NULL;
        thumb.recording = Recording;
        if (pScraper->Service("GetPosterThumb", &thumb)) {
            hasThumb = true;
        } else {
            hasThumb = false;
        }
    }
}

int cNopacityRecordingMenuItem::CheckScrollable1(int maxwidth) {
    int totalTextWidth = maxwidth;
    strRecNameFull = strRecName.c_str();
    if (font->Width(strRecName.c_str()) > (maxwidth)) {
        scrollable = true;
	totalTextWidth = std::max(font->Width(strRecName.c_str()), totalTextWidth);
        strRecName = CutText(strRecName, (maxwidth), font);
    }
    return totalTextWidth;
}

void cNopacityRecordingMenuItem::SetText(bool full) {
    if (isFolder)
        SetTextFolder(full);
    else
        SetTextRecording(full);
}

void cNopacityRecordingMenuItem::SetTextFolder(bool full) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    if (config.GetValue("useFolderPoster") && (hasPoster || hasThumb || hasManualPoster))
        DrawPoster();
    else
        DrawFolderIcon();
    int heightRecName = (2 * height / 3 - font->Height()) / 2;
//    int heightRecName = (7 * height / 10 - font->Height()) / 2;
    pixmapTextScroller->DrawText(cPoint(0, heightRecName), (full) ? strRecNameFull.c_str() : strRecName.c_str(), clrFont, clrTransparent, font);
}

void cNopacityRecordingMenuItem::SetTextRecording(bool full) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    int heightRecName = (height - font->Height()) / 2;
    if (config.GetValue("menuRecordingsShowLine2") && config.GetValue("menuRecordingsShowLine3"))
        heightRecName = (height / 2 - font->Height()) / 2;
    else if (config.GetValue("menuRecordingsShowLine2") || config.GetValue("menuRecordingsShowLine3"))
        heightRecName = (2 * height / 3 - font->Height()) / 2;
    pixmapTextScroller->DrawText(cPoint(0, heightRecName), (full) ? strRecNameFull.c_str() : strRecName.c_str(), clrFont, clrTransparent, font);
}

void cNopacityRecordingMenuItem::DrawRecordingIcons(void) {
    if (!pixmapStatic)
        return;

    int iconSize = height / 3;
    int iconX = width - spaceMenu;
    int iconY = height - iconSize - fontSmall->Height();

    cImage *imgIconNew = imgCache->GetSkinIcon("skinIcons/newrecording", iconSize, iconSize);
    if (imgIconNew && Recording->IsNew()) {
        iconX -= iconSize;
        pixmapStatic->DrawImage(cPoint(iconX, iconY), *imgIconNew);
    }

    cImage *imgIconCut = imgCache->GetSkinIcon("skinIcons/recordingcutted", iconSize, iconSize);
    if (imgIconCut && Recording->IsEdited()) {
        iconX -= iconSize;
        pixmapStatic->DrawImage(cPoint(iconX, iconY), *imgIconCut);
    }

#if (APIVERSNUM >= 20505)
    cImage *imgIconErr = imgCache->GetSkinIcon("skinIcons/recordingerror", iconSize, iconSize);
    const cRecordingInfo *Info = Recording->Info();
    if (imgIconErr && Info && (Info->Errors() > 0)) {
        iconX -= iconSize;
        pixmapStatic->DrawImage(cPoint(iconX, iconY), *imgIconErr);
    }
#endif
    iconwidth = width - iconX;
}

void cNopacityRecordingMenuItem::DrawFolderIcon(void) {
    if (!pixmapStatic)
        return;

    cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/recfolder", posterWidth, posterHeight);
    if (imgIcon) {
        int y = (height - imgIcon->Height()) / 2;
        pixmapStatic->DrawImage(cPoint(2 * spaceMenu, y), *imgIcon);
    }
}

void cNopacityRecordingMenuItem::DrawRecDateTime(void) {
    if (!pixmapStatic)
        return;

    const cEvent *Event = NULL;
    Event = Recording->Info()->GetEvent();
    cString strDateTime("");
    cString strDuration("");
    cString strError("");
#if (APIVERSNUM >= 20505)
    //Errors
    const cRecordingInfo *info = Recording->Info();
    if (info->Errors() >= (1 - config.GetValue("menuRecordingsDisplayError0"))) {
        strError = cString::sprintf("%s: %d ", tr("errors"), info->Errors());
    }
#endif
    if (Event) {
        std::string strDate = *(Event->GetDateString());
        cString strTime = Event->GetTimeString();
        if (strDate.find("1970") != std::string::npos) {
            time_t start = Recording->Start();
            strDateTime = cString::sprintf("%s %s", *DateString(start), *TimeString(start));
        } else {
            strDateTime = cString::sprintf("%s - %s", strDate.c_str(), *strTime);
        }
        if (config.GetValue("menuRecordingsErrorMode") == 1 && !isempty(strError))
            strDateTime = cString::sprintf("%s, %s", *strDateTime, *strError);
        int duration = Event->Duration() / 60;
        int recDuration = Recording->LengthInSeconds();
        recDuration = (recDuration > 0) ? (recDuration / 60) : 0;
	strDuration = cString::sprintf("%s: %d %s, %s: %d %s", tr("Duration"), duration, tr("min"), tr("recording"), recDuration, tr("min"));
	if (config.GetValue("menuRecordingsErrorMode") == 2 && !isempty(strError))
            strDuration = cString::sprintf("%s, %s", *strDuration, *strError);
    }
    int textleft = textLeft + spaceMenu / 2;
    int textHeight = 0;
    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    if (config.GetValue("menuRecordingsShowLine2") && config.GetValue("menuRecordingsShowLine3"))
        textHeight = height / 2 + (height / 4 - fontSmall->Height()) / 2;
    else if (config.GetValue("menuRecordingsShowLine2") || config.GetValue("menuRecordingsShowLine3"))
        textHeight = 2 * height / 3 + (height / 3 - fontSmall->Height()) / 2;
    if (config.GetValue("menuRecordingsShowLine2")) {
        pixmapStatic->DrawText(cPoint(textleft, textHeight), *strDateTime, clrFont, clrTransparent, fontSmall);
        textHeight += height / 4;
    }
    if (config.GetValue("menuRecordingsShowLine3")) {
        pixmapStatic->DrawText(cPoint(textleft, textHeight), *strDuration, clrFont, clrTransparent, fontSmall);
    }
}

void cNopacityRecordingMenuItem::DrawFolderNewSeen(void) {
    if (!pixmapStatic)
        return;

    int textleft = textLeft + spaceMenu / 2;
    int textHeight = 7 * height / 10;
    cString strTotalNew = cString::sprintf("%d %s (%d %s)", Total, (Total > 1) ? tr("recordings") : tr("recording"), New, tr("new"));
    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    pixmapStatic->DrawText(cPoint(textleft, textHeight), *strTotalNew, clrFont, clrTransparent, fontSmall);
}

void cNopacityRecordingMenuItem::DrawPoster(void) {
    if (!pixmapStatic)
        return;

    cImageLoader imgLoader;
    bool posterDrawn = false;
    if (hasManualPoster) {
        if (imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
            posterDrawn = true;
        }
    } else if (hasThumb) {
        if (imgLoader.LoadPoster(thumb.poster.path.c_str(), posterWidth, posterHeight)) {
            posterDrawn = true;
        }
    }
    if (posterDrawn) {
        cImage img = imgLoader.GetImage();
        pixmapStatic->DrawImage(cPoint(2 * spaceMenu, (height - img.Height()) /2), img);
        return;
    }
    cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/defaultPoster", posterWidth, posterHeight);
    if (imgIcon)
        pixmapStatic->DrawImage(cPoint(2 * spaceMenu, (height - imgIcon->Height()) /2), *imgIcon);
}

void cNopacityRecordingMenuItem::Render(bool initial, bool fadeout) {
    textLeft = posterWidth + 5 * spaceMenu;
    if (selectable) {
        eSkinElementType type = (current) ? seRecordingsHigh : seRecordings;
        DrawBackground(type, seRecordingsTop);

        if (isFolder) {
            DrawFolderNewSeen();
        } else {
            DrawPoster();
            DrawRecDateTime();
            DrawRecordingIcons();
        }
        if (!drawn) {
            int pixmapWidth = width - textLeft - iconwidth - 2 * spaceMenu;
            int textWidth = CheckScrollable1(pixmapWidth);
            if (textWidth > 0)
                CreatePixmapTextScroller(textWidth, textLeft, pixmapWidth);
            drawn = true;
        }
        if (!Running())
            SetText();
        if (config.GetValue("animation") && config.GetValue("menuScrollSpeed")) {
            if (current && scrollable && !Running())
                Start();
        }
        if (pixmapTextScroller && wasCurrent && !current && scrollable && Running()) {
            pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
            SetText();
            Cancel(-1);
        }
        if (!isFolder) {
            if (wasCurrent)
                if (infoTextWindow) {
                    delete infoTextWindow;
                    infoTextWindow = NULL;
                }
            if (current) {
                if (config.GetValue("menuRecordingsWindowMode") == 0) {
                  if (config.GetValue("animation") && !fadeout) {
                    //window mode
                    infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, vidWin);
                    infoTextWindow->SetGeometry(textWindow);
                    infoTextWindow->SetText(Recording->Info()->Description());
                    if (!infoTextWindow->SetManualPoster(Recording))
                        infoTextWindow->SetPoster(NULL, Recording);
                    infoTextWindow->SetInitial(initial);
                    infoTextWindow->Start();
                  }
                } else {
                    //fullscreen mode
                    infoTextWindow = new cNopacityTextWindow(osd, fontEPGWindow, fontEPGWindowLarge);
                    infoTextWindow->SetGeometry(textWindow);
                    if (!infoTextWindow->SetManualPoster(Recording, true))
                        infoTextWindow->SetPoster(NULL, Recording, true);
                    infoTextWindow->SetRecording(Recording);
                }
            }
        }
    }
}


// cNopacityDefaultMenuItem  -------------

cNopacityDefaultMenuItem::cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool sel) : cNopacityMenuItem (osd, text, sel) {
    scrollCol = -1;
    font = fontManager->menuItemDefault;
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
    if (pixmapStatic && isProgressbar) {
        pixmapStatic->DrawRectangle(cRect(x + 5, height / 4, width - 10, height / 2), color);
        pixmapStatic->DrawRectangle(cRect(x + 7, height / 4 + 2, width - 14, height / 2 - 4), clrTransparent);
        double progress = (double)now/(double)total;
        pixmapStatic->DrawRectangle(cRect(x + 8, height / 4 + 3, (width - 16)*progress, height / 2 - 6), color);

    }
}

void cNopacityDefaultMenuItem::SetTextFull(void) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    int x = (scrollCol == 0) ? 5 : 0;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height()) / 2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityDefaultMenuItem::SetTextShort(void) {
    if (!pixmapTextScroller)
        return;

    tColor clrFont = (current) ? Theme.Color(clrMenuFontMenuItemHigh) : Theme.Color(clrMenuFontMenuItem);
    PixmapFill(pixmapTextScroller, clrTransparent);
    int x = (scrollCol == 0) ? 5 : 0;
    pixmapTextScroller->DrawText(cPoint(x, (height - font->Height()) / 2), strEntry.c_str(), clrFont, clrTransparent, font);
}

int cNopacityDefaultMenuItem::CheckScrollable(bool hasIcon) {
    if (!selectable)
        return 0;
    scrollable = false;
    int colWidth = 0;
    int colTextWidth = 0;
    for (int i = 0; i < numTabs; i++) {
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
        if (!pixmapTextScroller) {
            pixmapTextScroller = CreatePixmap(osd, "pixmapTextScroller", 4, cRect(left + tabWidth[scrollCol], top + index * (height + spaceMenu), tabWidth[scrollCol+numTabs], height),
                                                                            cRect(0, 0, colTextWidth+10, height));
            PixmapFill(pixmapTextScroller, clrTransparent);
        }
    } else {
        if (pixmapTextScroller) {
            while (Running()) {
                Cancel(-1);
                DoSleep(10);
            }
            osd->DestroyPixmap(pixmapTextScroller);
            pixmapTextScroller = NULL;
            scrollCol = -1;
        }
    }
    return 0;
}

bool cNopacityDefaultMenuItem::DrawHeaderElement(void) {
    if (!pixmapStatic)
        return false;

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
        pixmapStatic->DrawText(cPoint(left, (height - font->Height()) / 2), c, Theme.Color(clrMenuFontMenuItemSep), clrTransparent, font);
        return true;
    }
    return false;
}

void cNopacityDefaultMenuItem::Render(bool initial, bool fadeout) {

    eSkinElementType type = (current) ? seDefaultHigh : seDefault;
    DrawBackground(type, seNone);

    tColor clrFont;
    if (current)
        clrFont = Theme.Color(clrMenuFontMenuItemHigh);
    else if (!selectable)
        clrFont = Theme.Color(clrMenuFontMenuItemSep);
    else
        clrFont = Theme.Color(clrMenuFontMenuItem);

    if (!selectable && (strncmp(Text, "---", 3) == 0)) {
        if (DrawHeaderElement())
            return;
    }
    int colWidth = 0;
    int colTextWidth = 0;
    cString itemText("");

    if (!pixmapStatic)
        goto Next;
        
    PixmapFill(pixmapStatic, clrTransparent);

    for (int i = 0; i < numTabs; i++) {
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
                pixmapStatic->DrawText(cPoint(posX, (height - font->Height()) / 2), *itemText, clrFont, clrTransparent, font);
            } else {
                if (!Running())
                    SetTextShort();
            }
        } else
            break;
    }

Next:
    if (config.GetValue("animation") && config.GetValue("menuScrollSpeed")) {
        if (current && scrollable && !Running())
            Start();
    }
    if (pixmapTextScroller && wasCurrent && !current && scrollable && Running()) {
        pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
        SetTextShort();
        Cancel(-1);
    }
}

// cNopacityTrackMenuItem  -------------

cNopacityTrackMenuItem::cNopacityTrackMenuItem(cOsd *osd, const char *text) : cNopacityMenuItem (osd, text, true) {
    font = fontManager->trackText;
}

cNopacityTrackMenuItem::~cNopacityTrackMenuItem(void) {
}

void cNopacityTrackMenuItem::Render(bool initial, bool fadeout) {

    eSkinElementType type = (current) ? seTracksHigh : seTracks;
    DrawBackground(type, seTracksTop);

    if (!pixmapStatic)
        return;

    tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrTracksFontButtons);
    pixmapStatic->DrawText(cPoint(5, (height - font->Height()) / 2), Text, clrFont, clrTransparent, font);
}
