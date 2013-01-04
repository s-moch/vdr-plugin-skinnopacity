#include "displaymenuview.h"

cNopacityTimer::cNopacityTimer(cOsd *osd) {
    this->osd = osd;
}

cNopacityTimer::~cNopacityTimer(void) {
    osd->DestroyPixmap(pixmap);
}

cNopacityDisplayMenuView::cNopacityDisplayMenuView() {
    diskUsageAlert = 95;
    pixmapStatus = NULL;
    pixmapHeaderIcon = NULL;
}

cNopacityDisplayMenuView::~cNopacityDisplayMenuView(void) {
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapHeaderLogo);
    osd->DestroyPixmap(pixmapHeaderLabel);
    osd->DestroyPixmap(pixmapDate);
    osd->DestroyPixmap(pixmapFooter);
    osd->DestroyPixmap(pixmapContent);
    osd->DestroyPixmap(pixmapScrollbar);    
    osd->DestroyPixmap(pixmapDiskUsage);
    osd->DestroyPixmap(pixmapDiskUsageIcon);
    osd->DestroyPixmap(pixmapDiskUsageLabel);
    if (pixmapHeaderIcon)
        osd->DestroyPixmap(pixmapHeaderIcon);
    
    delete fontHeader;
    delete fontDate;
    delete fontMenuitemLarge;
    delete fontMenuitemSchedule;
    delete fontMenuitemScheduleSmall;
    delete fontMenuitemRecordings;
    delete fontMenuitemRecordingsSmall;
    delete fontMenuitemDefault;
    delete fontDiskUsage;
    delete fontTimers;
    delete fontTimersHead;
    delete fontButtons;
    delete fontMessage;
    delete fontEPGInfoWindow;
}

cOsd *cNopacityDisplayMenuView::createOsd(void) {
    osdLeft = cOsd::OsdLeft();
    osdTop = cOsd::OsdTop();
    osdWidth = cOsd::OsdWidth();
    osdHeight = cOsd::OsdHeight();
    osd = CreateOsd(osdLeft, osdTop, osdWidth, osdHeight);
    return osd;
}

void cNopacityDisplayMenuView::SetGeometry(void) {
    spaceMenu = 5;
    widthScrollbar = 20;
    dateWidth = osdWidth * 0.3;
    headerHeight = osdHeight * config.headerHeight / 100;
    footerHeight = osdHeight * config.footerHeight / 100;
    contentHeight = osdHeight - headerHeight - footerHeight;
    contentWidthNarrow = osdWidth * config.menuWidthNarrow / 100;
    contentWidthFull = osdWidth - widthScrollbar - spaceMenu;
    menuItemWidthDefault = contentWidthFull - 4 * spaceMenu;
    menuItemWidthMain = contentWidthNarrow - 4*spaceMenu;
    menuItemHeightMain = config.iconHeight + 2;
    menuItemHeightSchedule = config.menuItemLogoHeight + 2;
    menuItemHeightDefault = contentHeight / config.numDefaultMenuItems - spaceMenu;
    menuItemHeightRecordings = config.menuRecFolderSize + 2;
    diskUsageWidth = diskUsageHeight = timersWidth = osdWidth  * config.menuWidthRightItems / 100;
    buttonsBorder = 10;
    buttonWidth = (osdWidth / 4) - 2 * buttonsBorder;
    buttonHeight = footerHeight - 3 * buttonsBorder;
    messageWidth = 0.8 * osdWidth;
    messageHeight = 0.1 * osdHeight;
    SetDescriptionTextWindowSize();
}

void cNopacityDisplayMenuView::SetDescriptionTextWindowSize(void) {
    int x = 2 * spaceMenu + contentWidthNarrow + widthScrollbar;
    int height = config.menuHeightInfoWindow * (contentHeight - 2*spaceMenu) / 100;
    int y = headerHeight + (contentHeight - height - spaceMenu);
    int width = osdWidth - x - spaceMenu;
    textWindowSize = cRect(x,y,width,height);
}

void cNopacityDisplayMenuView::CreatePixmaps(void) {
    pixmapHeader = osd->CreatePixmap(1, cRect(0, 0, osdWidth, headerHeight));
    pixmapHeaderLogo = osd->CreatePixmap(-1, cRect(0, 0, config.menuHeaderLogoWidth, config.menuHeaderLogoHeight));
    pixmapHeaderLabel = osd->CreatePixmap(2, cRect(0, 0, osdWidth - dateWidth, headerHeight));
    pixmapDate = osd->CreatePixmap(2, cRect(osdWidth - dateWidth, 0, dateWidth, headerHeight));
    pixmapFooter = osd->CreatePixmap(1, cRect(0, osdHeight-footerHeight, osdWidth, footerHeight));
    pixmapContent = osd->CreatePixmap(1, cRect(0, headerHeight, osdWidth, contentHeight),
                                         cRect(0, 0, osdWidth + contentWidthFull - contentWidthNarrow, contentHeight));
    pixmapScrollbar = osd->CreatePixmap(2, cRect(contentWidthNarrow, headerHeight + spaceMenu, widthScrollbar, contentHeight - 2 * spaceMenu));
    pixmapDiskUsage = osd->CreatePixmap(2, cRect(osdWidth - diskUsageWidth - 10, headerHeight + spaceMenu, diskUsageWidth, diskUsageHeight));
    pixmapDiskUsageIcon = osd->CreatePixmap(3, cRect((osdWidth - diskUsageWidth - 10) + (diskUsageWidth)/8, headerHeight, diskUsageWidth*3/4, diskUsageWidth*3/4));
    pixmapDiskUsageLabel = osd->CreatePixmap(3, cRect(osdWidth - diskUsageWidth - 10, headerHeight + spaceMenu, diskUsageWidth, diskUsageHeight));
    pixmapHeaderLogo->Fill(clrTransparent);
    pixmapHeaderLabel->Fill(clrTransparent);
    pixmapDiskUsage->Fill(clrTransparent);
    pixmapDiskUsageIcon->Fill(clrTransparent);
    pixmapDiskUsageLabel->Fill(clrTransparent);
    if (config.menuFadeTime) {
        pixmapHeader->SetAlpha(0);
        pixmapHeaderLogo->SetAlpha(0);
        pixmapHeaderLabel->SetAlpha(0);
        pixmapDate->SetAlpha(0);
        pixmapFooter->SetAlpha(0);
        pixmapContent->SetAlpha(0);
        pixmapScrollbar->SetAlpha(0);
        pixmapDiskUsage->SetAlpha(0);
        pixmapDiskUsageIcon->SetAlpha(0);
        pixmapDiskUsageLabel->SetAlpha(0);
    }
}

void cNopacityDisplayMenuView::SetPixmapAlpha(int Alpha) {
    pixmapHeader->SetAlpha(Alpha);
    pixmapHeaderLogo->SetAlpha(Alpha);
    pixmapHeaderLabel->SetAlpha(Alpha);
    pixmapDate->SetAlpha(Alpha);
    pixmapContent->SetAlpha(Alpha);
    pixmapFooter->SetAlpha(Alpha);
    pixmapScrollbar->SetAlpha(Alpha);
    pixmapDiskUsage->SetAlpha(Alpha);
    pixmapDiskUsageIcon->SetAlpha(Alpha);
    pixmapDiskUsageLabel->SetAlpha(Alpha);
}

void cNopacityDisplayMenuView::CreateFonts(void) {
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 2 + config.fontHeader);
    fontDate = cFont::CreateFont(config.fontName, headerHeight / 2 + config.fontDate);
    fontMenuitemLarge = cFont::CreateFont(config.fontName, menuItemHeightMain/3 + 4 + config.fontMenuitemLarge);
    fontMenuitemSchedule = cFont::CreateFont(config.fontName, menuItemHeightSchedule / 4 + 5 + config.fontMenuitemSchedule);
    fontMenuitemScheduleSmall = cFont::CreateFont(config.fontName, menuItemHeightSchedule / 4 - 5 + config.fontMenuitemScheduleSmall);
    fontMenuitemRecordings = cFont::CreateFont(config.fontName, menuItemHeightRecordings / 2 - 14 + config.fontMenuitemRecordings);
    fontMenuitemRecordingsSmall = cFont::CreateFont(config.fontName, menuItemHeightRecordings / 4 - 3 + config.fontMenuitemRecordingsSmall);
    fontMenuitemDefault = cFont::CreateFont(config.fontName, menuItemHeightDefault * 2 / 3 + config.fontMenuitemDefault);
    fontDiskUsage = cFont::CreateFont(config.fontName, (diskUsageHeight/4)/2 - 2 + config.fontDiskUsage);
    fontTimersHead = cFont::CreateFont(config.fontName, (contentHeight - 3*spaceMenu - diskUsageHeight) / 25 + config.fontTimersHead);
    fontTimers = cFont::CreateFont(config.fontName, (contentHeight - 3*spaceMenu - diskUsageHeight) / 25 - 6 + config.fontTimers);
    fontButtons = cFont::CreateFont(config.fontName, buttonHeight*0.8 + config.fontButtons);
    fontMessage = cFont::CreateFont(config.fontName, messageHeight / 3 + config.fontMessageMenu);
    fontEPGInfoWindow = cFont::CreateFont(config.fontName, (config.menuHeightInfoWindow *  contentHeight / 100)/ 6 + config.fontEPGInfoWindow);
}

cFont *cNopacityDisplayMenuView::GetMenuItemFont(eMenuCategory menuCat) {
    switch (menuCat) {
        case mcUnknown:
            return fontMenuitemDefault;
        case mcMain:
            return fontMenuitemLarge;
        case mcSchedule:
            return fontMenuitemSchedule;
        case mcChannel:
            return fontMenuitemLarge;
        case mcRecording:
            return fontMenuitemRecordings;
        default:
            return fontMenuitemDefault;     
    }
    return fontMenuitemDefault;
}

cFont *cNopacityDisplayMenuView::GetMenuItemFontSmall(eMenuCategory menuCat) {
    switch (menuCat) {
        case mcSchedule:
            return fontMenuitemScheduleSmall;
        case mcRecording:
            return fontMenuitemRecordingsSmall;
        default:
            return fontMenuitemScheduleSmall;     
    }
    return fontMenuitemScheduleSmall;
}

cFont *cNopacityDisplayMenuView::GetEPGWindowFont(void) {
    return fontEPGInfoWindow;
}

void cNopacityDisplayMenuView::GetMenuItemSize(eMenuCategory menuCat, cPoint *itemSize) {
    int itemWidth = 0;
    int itemHeight = 0;
    switch (menuCat) {
        case mcUnknown:
            itemWidth = menuItemWidthDefault;
            itemHeight = menuItemHeightDefault;
            break;
        case mcMain:
            itemWidth = menuItemWidthMain;
            itemHeight = menuItemHeightMain;
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            itemWidth = menuItemWidthMain;
            itemHeight = menuItemHeightSchedule;
            break;
        case mcChannel:
            itemWidth = menuItemWidthMain;
            itemHeight = menuItemHeightSchedule;
            break;
        case mcRecording:
            itemWidth = menuItemWidthMain;
            itemHeight = menuItemHeightRecordings;
            break;
        default:
            itemWidth = menuItemWidthDefault;
            itemHeight = menuItemHeightDefault;
            break;
    }
    itemSize->Set(itemWidth, itemHeight);
}

int cNopacityDisplayMenuView::GetMaxItems(eMenuCategory menuCat) {
    int maxItems = 0;
    switch (menuCat) {
        case mcMain:
        case mcSetup:
            maxItems = contentHeight / (menuItemHeightMain + spaceMenu);
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
        case mcChannel:
            maxItems = contentHeight / (menuItemHeightSchedule + spaceMenu);
            break;
        case mcRecording:
            maxItems = contentHeight / (menuItemHeightRecordings + spaceMenu);
            break;
        default:
            maxItems = config.numDefaultMenuItems;      
    }
    return maxItems;
}

int cNopacityDisplayMenuView::GetMenuTop(int numItems, int itemHeight) {
    return headerHeight + (contentHeight - numItems*(itemHeight + spaceMenu))/2;
}

void cNopacityDisplayMenuView::SetAvrgFontWidth(void) {
    avrgFontWidth = fontMenuitemDefault->Width("x")+3;
}

void cNopacityDisplayMenuView::SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5) {
    mytabs[0] = 1;
    mytabs[1] = Tab1 ? mytabs[0] + Tab1 : 0;
    mytabs[2] = Tab2 ? mytabs[1] + Tab2 : 0;
    mytabs[3] = Tab3 ? mytabs[2] + Tab3 : 0;
    mytabs[4] = Tab4 ? mytabs[3] + Tab4 : 0;
    mytabs[5] = Tab5 ? mytabs[4] + Tab5 : 0;
    if (Tab2) {
        for (int i = 1; i < cSkinDisplayMenu::MaxTabs; i++)
        mytabs[i] *= avrgFontWidth;
    } else if (Tab1) {
        mytabs[1] = GetEditableWidth();
    }
}

int cNopacityDisplayMenuView::GetEditableWidth(void) {
    return contentWidthFull*0.5;
}

int cNopacityDisplayMenuView::GetTextAreaWidth(void) {
    return contentWidthFull - 2*spaceMenu;
}

const cFont *cNopacityDisplayMenuView::GetTextAreaFont(bool FixedFont) {
    return cFont::CreateFont(config.fontName, contentHeight / 25 + config.fontDetailView);
}

void cNopacityDisplayMenuView::CreateBackgroundImages(int *handleBackgrounds, int *handleButtons) {
    cImageLoader imgLoader;
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), menuItemWidthDefault-2, menuItemHeightDefault-2);
    handleBackgrounds[0] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), menuItemWidthDefault-2, menuItemHeightDefault-2);
    handleBackgrounds[1] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), menuItemWidthMain-2, menuItemHeightMain-2);
    handleBackgrounds[2] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), menuItemWidthMain-2, menuItemHeightMain-2);
    handleBackgrounds[3] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), menuItemWidthMain-2, menuItemHeightSchedule-2);
    handleBackgrounds[4] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), menuItemWidthMain-2, menuItemHeightSchedule-2);
    handleBackgrounds[5] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), menuItemWidthMain-2, menuItemHeightRecordings-2);
    handleBackgrounds[6] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), menuItemWidthMain-2, menuItemHeightRecordings-2);
    handleBackgrounds[7] = cOsdProvider::StoreImage(imgLoader.GetImage());

    tColor lutBg[] = { Theme.Color(clrButtonRed), Theme.Color(clrButtonGreen), Theme.Color(clrButtonYellow), Theme.Color(clrButtonBlue) };

    imgLoader.DrawBackground(Theme.Color(clrMenuBack), lutBg[Setup.ColorKey0], buttonWidth-4, buttonHeight-4);
    handleButtons[0] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuBack), lutBg[Setup.ColorKey1], buttonWidth-4, buttonHeight-4);
    handleButtons[1] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuBack), lutBg[Setup.ColorKey2], buttonWidth-4, buttonHeight-4);
    handleButtons[2] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuBack), lutBg[Setup.ColorKey3], buttonWidth-4, buttonHeight-4);
    handleButtons[3] = cOsdProvider::StoreImage(imgLoader.GetImage());
}

void cNopacityDisplayMenuView::DrawBorderDecoration() {
    cImageLoader imgLoader;
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHighBlend), Theme.Color(clrMenuBack), osdWidth, headerHeight);
    pixmapHeader->DrawImage(cPoint(0,0), imgLoader.GetImage());
    pixmapFooter->Fill(Theme.Color(clrMenuBack));
    
    int borderWidth = 2;
    int radius = 10;
    
    pixmapContent->Fill(clrTransparent);
    pixmapContent->DrawRectangle(cRect(0, 0, contentWidthFull-radius, contentHeight), Theme.Color(clrMenuBack));
    pixmapContent->DrawRectangle(cRect(contentWidthFull, 0, osdWidth - contentWidthNarrow, borderWidth), Theme.Color(clrMenuBorder));
    pixmapContent->DrawRectangle(cRect(contentWidthFull, contentHeight - borderWidth, osdWidth - contentWidthNarrow, borderWidth), Theme.Color(clrMenuBorder));
    pixmapContent->DrawRectangle(cRect(contentWidthFull - radius, 0, radius, radius), Theme.Color(clrMenuBack));
    pixmapContent->DrawEllipse(cRect(contentWidthFull - radius, 0, radius, radius), Theme.Color(clrMenuBorder),2);
    pixmapContent->DrawEllipse(cRect(contentWidthFull - radius + borderWidth, borderWidth, radius-borderWidth, radius-borderWidth), clrTransparent, 2);
    pixmapContent->DrawRectangle(cRect(contentWidthFull-radius, radius, borderWidth, contentHeight - 2*radius), Theme.Color(clrMenuBorder));
    pixmapContent->DrawRectangle(cRect(contentWidthFull - radius, contentHeight - radius, radius, radius), Theme.Color(clrMenuBack));
    pixmapContent->DrawEllipse(cRect(contentWidthFull - radius, contentHeight - radius, radius, radius), Theme.Color(clrMenuBorder),3);
    pixmapContent->DrawEllipse(cRect(contentWidthFull - radius + borderWidth, contentHeight - radius, radius-borderWidth, radius-borderWidth), clrTransparent, 3);

}

void cNopacityDisplayMenuView::AdjustContentBackground(bool contentNarrow, bool contentNarrowLast) {
    if (contentNarrow) {
        pixmapContent->SetDrawPortPoint(cPoint(contentWidthNarrow - contentWidthFull, 0));
        if (config.scalePicture) {
            // ask output device to scale down
            cRect availableRect(
                osdLeft + contentWidthNarrow + widthScrollbar + 2 * spaceMenu,
                osdTop + headerHeight,
                contentWidthFull - osdLeft - contentWidthNarrow - widthScrollbar - 4 * spaceMenu,
                contentHeight);// - osdTop - headerHeight);
            cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
        }
    } else {
        pixmapContent->SetDrawPortPoint(cPoint(0, 0));
        if (config.scalePicture) {
            // ask output device to restore full size
            cDevice::PrimaryDevice()->CanScaleVideo(cRect::Null);
        }
    }
    if (contentNarrow != contentNarrowLast) {
        osd->DestroyPixmap(pixmapScrollbar);
        int contentWidth = (contentNarrow)?contentWidthNarrow:contentWidthFull;
        pixmapScrollbar = osd->CreatePixmap(2, cRect(contentWidth , headerHeight + spaceMenu, widthScrollbar, osdHeight - headerHeight - footerHeight - 2*spaceMenu));
    }
}

void cNopacityDisplayMenuView::DrawHeaderLogo(void) {
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("vdrlogo", config.menuHeaderLogoWidth, config.menuHeaderLogoHeight)) {
        pixmapHeaderLogo->DrawImage(cPoint(0,0), imgLoader.GetImage());
    }
}

int cNopacityDisplayMenuView::ShowHeaderLogo(bool show) {
    if (show) {
        pixmapHeaderLogo->SetLayer(2);
    } else {
        pixmapHeaderLogo->SetLayer(-1);
    }
    return config.menuHeaderLogoWidth + spaceMenu;
}

int cNopacityDisplayMenuView::DrawHeaderIcon(eMenuCategory menuCat) {
    cString icon;
    bool drawIcon = true;
    switch (menuCat) {
        case mcScheduleNow:
        case mcScheduleNext:
            icon = "Schedule";
            break;
        case mcChannel:
            icon = "Channels";
            break;
        case mcTimer:
            icon = "Timers";
            break;
        case mcRecording:
            icon = "Recordings";
            break;
        case mcSetup:
            icon = "Setup";
            break;
        case mcCommand:
            icon = "Commands";
            break;
        default:
            drawIcon = false;
    }
    
    int left = 0;
    if (drawIcon) {
        pixmapHeaderIcon = osd->CreatePixmap(2, cRect(0, 0, config.headerIconHeight, config.headerIconHeight));
        pixmapHeaderIcon->Fill(clrTransparent);
        cImageLoader imgLoader;
        if (imgLoader.LoadIcon(*icon, config.headerIconHeight)) {
            pixmapHeaderIcon->DrawImage(cPoint(0,0), imgLoader.GetImage());
            left = config.headerIconHeight + spaceMenu;
        }
    }
    return left;
}

int cNopacityDisplayMenuView::ShowHeaderIconChannelLogo(const char *Title) {
    int left = 0;
    pixmapHeaderIcon = osd->CreatePixmap(2, cRect(0, 0, config.menuItemLogoWidth, config.menuItemLogoHeight));
    pixmapHeaderIcon->Fill(clrTransparent);
    std::string channel = Title;
    if (channel.length() == 0)
        return left;
    std::string remove = trVDR("Schedule");
    try {
        remove.append(" - ");
        channel.erase(0, remove.length());
    } catch (...) {}
    cImageLoader imgLoader;
    if (imgLoader.LoadLogo(channel.c_str(), config.menuItemLogoWidth, config.menuItemLogoHeight)) {
        pixmapHeaderIcon->DrawImage(cPoint(0, 0), imgLoader.GetImage());
        left =  config.menuItemLogoWidth + spaceMenu;
    }
    return left;
}

void cNopacityDisplayMenuView::DestroyHeaderIcon(void) {
    if (pixmapHeaderIcon) {
        osd->DestroyPixmap(pixmapHeaderIcon);
        pixmapHeaderIcon = NULL;
    }
}

void cNopacityDisplayMenuView::DrawHeaderLabel(int left, cString label) {
    pixmapHeaderLabel->Fill(clrTransparent);
    pixmapHeaderLabel->DrawText(cPoint(left, ((headerHeight - 10) - fontHeader->Height()) / 2), *label, Theme.Color(clrMenuFontHeader), clrTransparent, fontHeader);

}

void cNopacityDisplayMenuView::DrawDate(bool initial) {
    cString date = DayDateTime();
    if (initial || strcmp(date, lastDate)) {
        pixmapDate->Fill(clrTransparent);
        int dateW = fontDate->Width(date);
        pixmapDate->DrawText(cPoint(dateWidth - dateW - 2*spaceMenu, (headerHeight - fontDate->Height()) / 2), date, Theme.Color(clrMenuFontDate), clrTransparent, fontDate);
        lastDate = date;
    }
}

void cNopacityDisplayMenuView::DrawDiskUsage(void) {
    int iconWidth = diskUsageWidth * 3/4;
    int DiskUsage = cVideoDiskUsage::UsedPercent();
    bool DiskAlert = DiskUsage > diskUsageAlert;
    tColor bgColor = DiskAlert ? Theme.Color(clrDiskAlert) : Theme.Color(clrMenuBack);
    pixmapDiskUsage->Fill(Theme.Color(clrMenuBorder));
    pixmapDiskUsage->DrawRectangle(cRect(2,2,diskUsageWidth-4, diskUsageHeight-4), bgColor);
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("DiskUsage", iconWidth)) {
        cImage icon = imgLoader.GetImage();
        pixmapDiskUsageIcon->DrawImage(cPoint(0,0), icon);
    }
    pixmapDiskUsageLabel->Fill(clrTransparent);
    cString usage = cString::sprintf("%s: %d%%", tr("Disc"), DiskUsage);
    cString rest = cString::sprintf("%02d:%02dh %s", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60, tr("free"));
    pixmapDiskUsageLabel->DrawText(cPoint((diskUsageWidth - fontDiskUsage->Width(*usage))/2, diskUsageHeight - 2*fontDiskUsage->Height() - spaceMenu), *usage, Theme.Color(clrMenuFontDiscUsage), clrTransparent, fontDiskUsage);
    pixmapDiskUsageLabel->DrawText(cPoint((diskUsageWidth - fontDiskUsage->Width(*rest))/2, diskUsageHeight - fontDiskUsage->Height() - spaceMenu), *rest, Theme.Color(clrMenuFontDiscUsage), clrTransparent, fontDiskUsage);
    pixmapDiskUsage->SetLayer(2);
    pixmapDiskUsageIcon->SetLayer(3);
    pixmapDiskUsageLabel->SetLayer(4);
}

void cNopacityDisplayMenuView::ShowDiskUsage(bool show) {
    if (show) {
        pixmapDiskUsage->SetLayer(2);
        pixmapDiskUsageIcon->SetLayer(3);
        pixmapDiskUsageLabel->SetLayer(4);
    } else {
        pixmapDiskUsage->SetLayer(-1);
        pixmapDiskUsageIcon->SetLayer(-1);
        pixmapDiskUsageLabel->SetLayer(-1);
    }
}

void cNopacityDisplayMenuView::DrawButton(const char *text, int handleImage, tColor borderColor, int num) {
    int top = 2*buttonsBorder;
    int left = num * buttonWidth + (2*num + 1) * buttonsBorder;
    pixmapFooter->DrawRectangle(cRect(left, top, buttonWidth, buttonHeight), borderColor);
    pixmapFooter->DrawImage(cPoint(left+2, top+2), handleImage);
    int textWidth = fontButtons->Width(text);
    int textHeight = fontButtons->Height();
    pixmapFooter->DrawText(cPoint(left + (buttonWidth-textWidth)/2, top + (buttonHeight-textHeight)/2), text, Theme.Color(clrMenuFontButton), clrTransparent, fontButtons);
}

void cNopacityDisplayMenuView::ClearButton(int num) {
    int top = 2*buttonsBorder;
    int left = num * buttonWidth + (2*num + 1) * buttonsBorder;
    pixmapFooter->DrawRectangle(cRect(left, top, buttonWidth, buttonHeight), Theme.Color(clrMenuBack));
}

int cNopacityDisplayMenuView::GetTimersInitHeight(void) {
    int initHeight = headerHeight + 2*spaceMenu;
    if (config.showDiscUsage)
        initHeight += diskUsageHeight;
    return initHeight;
}

int cNopacityDisplayMenuView::GetTimersMaxHeight(void) {
    return headerHeight + contentHeight;
}

cNopacityTimer *cNopacityDisplayMenuView::DrawTimer(const cTimer *Timer, int y) {
    const cChannel *Channel = Timer->Channel();
    const cEvent *Event = Timer->Event();
    cString channelName(""), title("");
    if (Channel) {
        channelName = Channel->Name();
    }
    if (Event) {
        title = Event->Title();
    }
    cTextWrapper titleLines;
    titleLines.Set(*title, fontTimers, timersWidth-10);
    
    cString Date;
    if (Timer->Recording()) {
        Date = cString::sprintf("-%s", *TimeString(Timer->StopTime()));
    } else {
        time_t Now = time(NULL);
        cString Today = WeekDayName(Now);
        cString Time = TimeString(Timer->StartTime());
        cString Day = WeekDayName(Timer->StartTime());
        if (Timer->StartTime() > Now + 6 * SECSINDAY)
            Date = DayDateTime(Timer->StartTime());
        else if (strcmp(Day, Today) != 0)
            Date = cString::sprintf("%s %s", *Day, *Time);
        else
            Date = Time;
        if (Timer->Flags() & tfVps)
            Date = cString::sprintf("VPS %s", *Date);
    }
    
    int numLines = titleLines.Lines();
    int lineHeight = fontTimers->Height();
    int timerHeight = (numLines + 2)*lineHeight + spaceMenu;
    
    cNopacityTimer *t = new cNopacityTimer(osd);
    t->pixmap = osd->CreatePixmap(2, cRect(osdWidth - timersWidth - 10, y, timersWidth, timerHeight));
    
    cImageLoader imgLoader;
    if(Timer->Recording()) {
        t->pixmap->Fill(Theme.Color(clrDiskAlert));
        imgLoader.DrawBackground(Theme.Color(clrDiskAlert), Theme.Color(clrMenuItemHigh), timersWidth-2, timerHeight-2);
        t->pixmap->DrawImage(cPoint(1,1), imgLoader.GetImage());
    } else {
        t->pixmap->Fill(Theme.Color(clrMenuBorder));
        imgLoader.DrawBackground(Theme.Color(clrMenuItemHighBlend), Theme.Color(clrMenuItemHigh), timersWidth-2, timerHeight-2);
        t->pixmap->DrawImage(cPoint(1,1), imgLoader.GetImage());
    }
    t->pixmap->DrawText(cPoint(5, 0), *Date, Theme.Color(clrMenuFontTimersHeader), clrTransparent, fontTimersHead);
    t->pixmap->DrawText(cPoint(5, lineHeight+2), *channelName, Theme.Color(clrMenuFontTimersHeader), clrTransparent, fontTimers);
    int yStart = 2*lineHeight + 3;
    for (int line=0; line<numLines; line++)
        t->pixmap->DrawText(cPoint(5, yStart+line*(lineHeight-2)), titleLines.GetLine(line), Theme.Color(clrMenuFontTimers), clrTransparent, fontTimers);
        
    return t;
}

void cNopacityDisplayMenuView::DrawScrollbar(double Height, double Offset) {
    int totalHeight = pixmapScrollbar->ViewPort().Height() - 6;
    int height = Height * totalHeight;
    int offset = Offset * totalHeight;
    pixmapScrollbar->Fill(Theme.Color(clrMenuScrollBar));
    pixmapScrollbar->DrawRectangle(cRect(2,2,widthScrollbar-4,totalHeight+2), Theme.Color(clrMenuScrollBarBack));
    pixmapScrollbar->DrawRectangle(cRect(3,3 + offset,widthScrollbar-6,height), Theme.Color(clrMenuScrollBar));
}

void cNopacityDisplayMenuView::ClearScrollbar(void) {
    pixmapScrollbar->Fill(clrTransparent);
}


void cNopacityDisplayMenuView::DrawMessage(eMessageType Type, const char *Text) {
    tColor col = Theme.Color(clrMessageStatus);
    switch (Type) {
        case mtStatus: 
            col = Theme.Color(clrMessageStatus);
            break;
        case mtInfo:
            col = Theme.Color(clrMessageInfo);
            break;
        case mtWarning:
            col = Theme.Color(clrMessageWarning);
            break;
        case mtError:
            col = Theme.Color(clrMessageError);
            break;
    }
    if (pixmapStatus) {
        ClearMessage();
    }
    pixmapStatus = osd->CreatePixmap(5, cRect(0.1*osdWidth, 0.8*osdHeight, messageWidth, messageHeight));
    pixmapStatus->Fill(col);
    cImageLoader imgLoader;
    imgLoader.DrawBackground2(Theme.Color(clrMenuBack), col, messageWidth-2, messageHeight-2);
    pixmapStatus->DrawImage(cPoint(1, 1), imgLoader.GetImage());
    int textWidth = fontMessage->Width(Text);
    pixmapStatus->DrawText(cPoint((messageWidth - textWidth) / 2, (messageHeight - fontMessage->Height()) / 2), Text, Theme.Color(clrMenuFontMessages), clrTransparent, fontMessage);
}

void cNopacityDisplayMenuView::ClearMessage(void) {
    if (pixmapStatus) {
        osd->DestroyPixmap(pixmapStatus);
        pixmapStatus = NULL;
    }
}

void cNopacityDisplayMenuView::SetDetailViewSize(eDetailViewType detailViewType, cNopacityMenuDetailView *detailView) {
    int width = 0;
    int height = 0;
    int top = 0;
    int contentBorder = 20;
    int detailHeaderHeight = 0;
    
    switch (detailViewType) {
        case dvEvent:
            detailHeaderHeight = max(config.detailViewLogoHeight, config.epgImageHeight)+4;
            break;
        case dvRecording:
            detailHeaderHeight = config.epgImageHeight + 4;
            break;
        case dvText:
            detailHeaderHeight = 0;         
            break;
        default:
            break;
    }
    width = contentWidthFull - 2*spaceMenu;
    height = contentHeight;
    top = headerHeight;
    detailView->SetGeometry(width, height, top, contentBorder, detailHeaderHeight);
}
