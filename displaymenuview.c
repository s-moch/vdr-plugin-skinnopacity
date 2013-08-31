#include "displaymenuview.h"

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
    if (pixmapRssFeed)
        osd->DestroyPixmap(pixmapRssFeed);
    if (pixmapRssFeedBackground)
        osd->DestroyPixmap(pixmapRssFeedBackground);
    if (pixmapRssFeedIcon)
        osd->DestroyPixmap(pixmapRssFeedIcon);
    delete fontHeader;
    delete fontDate;
    delete fontMenuitemLarge;
    delete fontMenuitemSchedule;
    delete fontMenuitemScheduleSmall;
    delete fontMenuitemChannel;
    delete fontMenuitemChannelSmall;
    delete fontMenuitemRecordings;
    delete fontMenuitemRecordingsSmall;
    delete fontMenuitemTimers;
    delete fontMenuitemTimersSmall;
    delete fontMenuitemDefault;
    delete fontDiskUsage;
    delete fontDiskUsagePercent;
    delete fontTimers;
    delete fontTimersHead;
    delete fontButtons;
    delete fontMessage;
    delete fontEPGInfoWindow;
    delete fontRssFeed;
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
    rssFeedHeight = (config.displayRSSFeed)?(osdHeight * config.rssFeedHeight / 100):0;
    contentHeight = osdHeight - headerHeight - footerHeight - rssFeedHeight;
    contentWidthMain = osdWidth * config.menuWidthMain / 100;
    contentWidthSchedules = osdWidth * config.menuWidthSchedules / 100;
    contentWidthChannels = osdWidth * config.menuWidthChannels / 100;
    contentWidthTimers = osdWidth * config.menuWidthTimers / 100;
    contentWidthRecordings = osdWidth * config.menuWidthRecordings / 100;
    contentWidthSetup = osdWidth * config.menuWidthSetup / 100;
    contentWidthFull = osdWidth - widthScrollbar - spaceMenu;
    contentWidthMinimum = Minimum(contentWidthMain, 
                                  contentWidthSchedules, 
                                  contentWidthChannels,
                                  contentWidthTimers,
                                  contentWidthRecordings,
                                  contentWidthSetup);
    menuItemWidthDefault = contentWidthFull - 4 * spaceMenu;
    menuItemWidthMain = contentWidthMain - 4*spaceMenu;
    menuItemWidthSchedule = contentWidthSchedules - 4*spaceMenu;
    menuItemWidthChannel = contentWidthChannels - 4*spaceMenu;
    menuItemWidthTimer = contentWidthTimers - 4*spaceMenu;
    menuItemWidthRecording = contentWidthRecordings - 4*spaceMenu;
    menuItemWidthSetup = contentWidthSetup - 4*spaceMenu;
    menuItemHeightMain = config.iconHeight + 2;
    menuItemHeightSchedule = config.menuItemLogoHeight + 2;
    menuItemHeightDefault = contentHeight / config.numDefaultMenuItems - spaceMenu;
    menuItemHeightRecordings = config.menuRecFolderSize + 2;
    diskUsageWidth = diskUsageHeight = osdWidth  * config.menuSizeDiskUsage / 100;
    timersWidth = osdWidth  * config.menuWidthRightItems / 100;
    buttonsBorder = footerHeight / 6;
    buttonWidth = (osdWidth / 4) - 2 * buttonsBorder;
    buttonHeight = footerHeight - 3 * buttonsBorder;
    messageWidth = 0.8 * osdWidth;
    messageHeight = 0.1 * osdHeight;
    SetDescriptionTextWindowSize();
}

void cNopacityDisplayMenuView::SetDescriptionTextWindowSize(void) {
    int xSchedules =  (config.menuAdjustLeft) ? (2 * spaceMenu + contentWidthSchedules + widthScrollbar)  : (spaceMenu);
    int xRecordings = (config.menuAdjustLeft) ? (2 * spaceMenu + contentWidthRecordings + widthScrollbar) : (spaceMenu);
    int xChannels = (config.menuAdjustLeft) ? (2 * spaceMenu + contentWidthChannels + widthScrollbar) : (spaceMenu);
    int height = config.menuHeightInfoWindow * (contentHeight - 2*spaceMenu) / 100;
    int y = headerHeight + (contentHeight - height - spaceMenu);
    int widthSchedules = (config.menuAdjustLeft)  ? (osdWidth - xSchedules - spaceMenu)  : (osdWidth - contentWidthSchedules - widthScrollbar - 2 * spaceMenu);
    int widthRecordings = (config.menuAdjustLeft) ? (osdWidth - xRecordings - spaceMenu) : (osdWidth - contentWidthRecordings - widthScrollbar - 2 * spaceMenu);
    int widthChannels = (config.menuAdjustLeft) ? (osdWidth - xChannels - spaceMenu) : (osdWidth - contentWidthChannels - widthScrollbar - 2 * spaceMenu);
    textWindowSizeSchedules = cRect(xSchedules,y,widthSchedules,height);
    textWindowSizeRecordings = cRect(xRecordings,y,widthRecordings,height);
    textWindowSizeChannels = cRect(xChannels,y,widthChannels,height);
}

cRect *cNopacityDisplayMenuView::GetDescriptionTextWindowSize(eMenuCategory menuCat) {
    switch (menuCat) {
        case mcSchedule:
            return &textWindowSizeSchedules;
        case mcRecording:
            return &textWindowSizeRecordings;
        case mcChannel:
            return &textWindowSizeChannels;
        default:
            return NULL;
    }
    return NULL;
}

int cNopacityDisplayMenuView::GetContentWidth(eMenuCategory menuCat) {
    switch (menuCat) {
        case mcMain:
            if (config.narrowMainMenu)
                return contentWidthMain;
            else
                return contentWidthFull;
        case mcSetup:
            if (config.narrowSetupMenu)
                return contentWidthSetup;
            else
                return contentWidthFull;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            if (config.narrowScheduleMenu)
                return contentWidthSchedules;
            else
                return contentWidthFull;
        case mcChannel:
            if (config.narrowChannelMenu)
                return contentWidthChannels;
            else
                return contentWidthFull;
        case mcTimer:
            if (config.narrowTimerMenu)
                return contentWidthTimers;
            else
                return contentWidthFull;
        case mcRecording:
            if (config.narrowRecordingMenu)
                return contentWidthRecordings;
            else
                return contentWidthFull;
        default:
            return contentWidthFull;     
    }
    return contentWidthFull;
}


void cNopacityDisplayMenuView::CreatePixmaps(void) {
    pixmapHeader = osd->CreatePixmap(1, cRect(0, 0, osdWidth, headerHeight));
    int dateX = (config.menuAdjustLeft) ? (osdWidth - dateWidth) : 0;
    pixmapDate = osd->CreatePixmap(2, cRect(dateX, 0, dateWidth, headerHeight));
    int logoX = (config.menuAdjustLeft) ? 0 : (osdWidth - config.menuHeaderLogoWidth);
    pixmapHeaderLogo = osd->CreatePixmap(-1, cRect(logoX, 0, config.menuHeaderLogoWidth, config.menuHeaderLogoHeight));
    int labelX = (config.menuAdjustLeft) ? 0 : dateWidth;
    pixmapHeaderLabel = osd->CreatePixmap(2, cRect(labelX, 0, osdWidth - dateWidth, headerHeight));
    pixmapFooter = osd->CreatePixmap(1, cRect(0, osdHeight - rssFeedHeight - footerHeight, osdWidth, footerHeight));
    int drawPortWidth = osdWidth + contentWidthFull - contentWidthMinimum;
    pixmapContent = osd->CreatePixmap(1, cRect(0, headerHeight, osdWidth, contentHeight),
                                         cRect(0, 0, drawPortWidth, contentHeight));
    int diskUsageX = (config.menuAdjustLeft) ? (osdWidth - diskUsageWidth - 10) : 10;
    pixmapDiskUsage = osd->CreatePixmap(2, cRect(diskUsageX, headerHeight + spaceMenu, diskUsageWidth, diskUsageHeight));
    pixmapDiskUsageIcon = osd->CreatePixmap(3, cRect(diskUsageX + 2, headerHeight + spaceMenu + 2, diskUsageWidth - 4, diskUsageHeight - 4));
    pixmapDiskUsageLabel = osd->CreatePixmap(4, cRect(diskUsageX + 2, headerHeight + spaceMenu + 2, diskUsageWidth - 4, diskUsageHeight - 4));
    int scrollbarX = (config.menuAdjustLeft) ? contentWidthMain : (osdWidth - contentWidthMain);
    pixmapScrollbar = osd->CreatePixmap(2, cRect(scrollbarX, headerHeight + spaceMenu, widthScrollbar, contentHeight - 2 * spaceMenu));

    if (config.displayRSSFeed) {
        pixmapRssFeedBackground = osd->CreatePixmap(1, cRect(0, headerHeight + contentHeight + footerHeight, osdWidth, rssFeedHeight));
        pixmapRssFeed = osd->CreatePixmap(2, cRect(0, headerHeight + contentHeight + footerHeight, osdWidth, rssFeedHeight));
        pixmapRssFeedIcon = osd->CreatePixmap(3, cRect(0, headerHeight + contentHeight + footerHeight, rssFeedHeight, rssFeedHeight));
    } else {
        pixmapRssFeedBackground = NULL;
        pixmapRssFeed = NULL;
        pixmapRssFeedIcon = NULL;
    }
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
    fontMenuitemChannel = cFont::CreateFont(config.fontName, menuItemHeightSchedule / 3 + config.fontMenuitemChannel);
    fontMenuitemChannelSmall = cFont::CreateFont(config.fontName, menuItemHeightSchedule / 5 - 2 + config.fontMenuitemChannelSmall);
    fontMenuitemRecordings = cFont::CreateFont(config.fontName, menuItemHeightRecordings / 2 - 14 + config.fontMenuitemRecordings);
    fontMenuitemRecordingsSmall = cFont::CreateFont(config.fontName, menuItemHeightRecordings / 4 - 3 + config.fontMenuitemRecordingsSmall);
    fontMenuitemTimers = cFont::CreateFont(config.fontName, menuItemHeightSchedule / 3 + config.fontMenuitemTimers);
    fontMenuitemTimersSmall = cFont::CreateFont(config.fontName, menuItemHeightSchedule / 4 - 3 + config.fontMenuitemTimersSmall);
    fontMenuitemDefault = cFont::CreateFont(config.fontName, menuItemHeightDefault * 2 / 3 + config.fontMenuitemDefault);
    fontDiskUsage = cFont::CreateFont(config.fontName, diskUsageHeight/6 - 2 + config.fontDiskUsage);
    fontDiskUsagePercent = cFont::CreateFont(config.fontName, diskUsageHeight/5 - 4 + config.fontDiskUsagePercent);
    fontTimersHead = cFont::CreateFont(config.fontName, (contentHeight - 3*spaceMenu - diskUsageHeight) / 25 + config.fontTimersHead);
    fontTimers = cFont::CreateFont(config.fontName, (contentHeight - 3*spaceMenu - diskUsageHeight) / 25 - 6 + config.fontTimers);
    fontButtons = cFont::CreateFont(config.fontName, buttonHeight*0.8 + config.fontButtons);
    fontMessage = cFont::CreateFont(config.fontName, messageHeight / 3 + config.fontMessageMenu);
    fontEPGInfoWindow = cFont::CreateFont(config.fontName, contentHeight / 30 + config.fontEPGInfoWindow);
    fontRssFeed = cFont::CreateFont(config.fontName, (rssFeedHeight / 2) + 3 + config.fontRssFeed);
}

cFont *cNopacityDisplayMenuView::GetMenuItemFont(eMenuCategory menuCat) {
    switch (menuCat) {
        case mcUnknown:
            return fontMenuitemDefault;
        case mcMain:
        case mcSetup:
            return fontMenuitemLarge;
        case mcSchedule:
            return fontMenuitemSchedule;
        case mcChannel:
            return fontMenuitemChannel;
        case mcTimer:
            return fontMenuitemTimers;
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
        case mcChannel:
            return fontMenuitemChannelSmall;
        case mcTimer:
            return fontMenuitemTimersSmall;
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
        case mcSetup:
            itemWidth = menuItemWidthSetup;
            itemHeight = menuItemHeightMain;
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            itemWidth = menuItemWidthSchedule;
            itemHeight = menuItemHeightSchedule;
            break;
        case mcChannel:
            itemWidth = menuItemWidthChannel;
            itemHeight = menuItemHeightSchedule;
            break;
        case mcTimer:
            itemWidth = menuItemWidthTimer;
            itemHeight = menuItemHeightSchedule;
            break;
        case mcRecording:
            itemWidth = menuItemWidthRecording;
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
        case mcTimer:
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

int cNopacityDisplayMenuView::GetMenuItemLeft(int itemWidth) {
    int menuLeft = spaceMenu;
    if (!config.menuAdjustLeft)
        menuLeft = osdWidth - itemWidth - spaceMenu;
    return menuLeft;
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
    if (!config.doBlending)
        return;
    cImageLoader imgLoader;
    //Default Menus
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), menuItemWidthDefault-2, menuItemHeightDefault-2);
    handleBackgrounds[0] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), menuItemWidthDefault-2, menuItemHeightDefault-2);
    handleBackgrounds[1] = cOsdProvider::StoreImage(imgLoader.GetImage());
    //Main Menu
    cPoint itemSize;
    GetMenuItemSize(mcMain, &itemSize);
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[2] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[3] = cOsdProvider::StoreImage(imgLoader.GetImage());
    //Schedules Menu
    GetMenuItemSize(mcSchedule, &itemSize);
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[4] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[5] = cOsdProvider::StoreImage(imgLoader.GetImage());
    //Channels Menu
    GetMenuItemSize(mcChannel, &itemSize);
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[6] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[7] = cOsdProvider::StoreImage(imgLoader.GetImage());
    //Recordings Menu
    GetMenuItemSize(mcRecording, &itemSize);
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[8] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[9] = cOsdProvider::StoreImage(imgLoader.GetImage());
    //Timers Menu
    GetMenuItemSize(mcTimer, &itemSize);
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[10] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[11] = cOsdProvider::StoreImage(imgLoader.GetImage());
    //Setup Menu
    GetMenuItemSize(mcSetup, &itemSize);
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[12] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), itemSize.X()-2, itemSize.Y()-2);
    handleBackgrounds[13] = cOsdProvider::StoreImage(imgLoader.GetImage());
    //Color Buttons
    imgLoader.DrawBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonRed), buttonWidth-4, buttonHeight-4);
    handleButtons[0] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonGreen), buttonWidth-4, buttonHeight-4);
    handleButtons[1] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonYellow), buttonWidth-4, buttonHeight-4);
    handleButtons[2] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonBlue), buttonWidth-4, buttonHeight-4);
    handleButtons[3] = cOsdProvider::StoreImage(imgLoader.GetImage());
}

void cNopacityDisplayMenuView::DrawBorderDecoration() {
    if (config.doBlending) {
        cImageLoader imgLoader;
        bool mirrorHeader = (config.menuAdjustLeft) ? false : true;
        imgLoader.DrawBackground(Theme.Color(clrMenuHeaderBlend), Theme.Color(clrMenuHeader), osdWidth, headerHeight, mirrorHeader);
        pixmapHeader->DrawImage(cPoint(0,0), imgLoader.GetImage());
    } else 
        pixmapHeader->Fill(Theme.Color(clrMenuBack));
    pixmapFooter->Fill(Theme.Color(clrMenuBack));
    
    int borderWidth = 2;
    int radius = 10;

    pixmapContent->Fill(clrTransparent);

    if (config.menuAdjustLeft) {
        //Background
        pixmapContent->DrawRectangle(cRect(0, 0, contentWidthFull - radius, contentHeight), Theme.Color(clrMenuBack));
        //Upper and lower Corner Square
        pixmapContent->DrawRectangle(cRect(contentWidthFull - radius, 0, radius, radius), Theme.Color(clrMenuBack));
        pixmapContent->DrawRectangle(cRect(contentWidthFull - radius, contentHeight - radius, radius, radius), Theme.Color(clrMenuBack));
        //Upper line
        pixmapContent->DrawRectangle(cRect(contentWidthFull, 0, osdWidth - contentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Lower Line
        pixmapContent->DrawRectangle(cRect(contentWidthFull, contentHeight - borderWidth, osdWidth - contentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Vertical line
        pixmapContent->DrawRectangle(cRect(contentWidthFull-radius, radius, borderWidth, contentHeight - 2*radius), Theme.Color(clrMenuBorder));
        if (radius-borderWidth > 2) {
            //Upper Ellipse
            pixmapContent->DrawEllipse(cRect(contentWidthFull - radius, 0, radius, radius), Theme.Color(clrMenuBorder),2);
            pixmapContent->DrawEllipse(cRect(contentWidthFull - radius + borderWidth, borderWidth, radius-borderWidth, radius-borderWidth), clrTransparent, 2);
            //Lower Ellipse
            pixmapContent->DrawEllipse(cRect(contentWidthFull - radius, contentHeight - radius, radius, radius), Theme.Color(clrMenuBorder),3);
            pixmapContent->DrawEllipse(cRect(contentWidthFull - radius + borderWidth, contentHeight - radius, radius-borderWidth, radius-borderWidth), clrTransparent, 3);
        }
    } else {
        //Background
        pixmapContent->DrawRectangle(cRect(osdWidth - contentWidthMinimum + radius, 0, contentWidthFull - radius, contentHeight), Theme.Color(clrMenuBack));
        //Upper and lower Corner Square
        pixmapContent->DrawRectangle(cRect(osdWidth - contentWidthMinimum, 0, radius, radius), Theme.Color(clrMenuBack));
        pixmapContent->DrawRectangle(cRect(osdWidth - contentWidthMinimum, contentHeight - radius, radius, radius), Theme.Color(clrMenuBack));
        //Upper line
        pixmapContent->DrawRectangle(cRect(0, 0, osdWidth - contentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Lower Line
        pixmapContent->DrawRectangle(cRect(0, contentHeight - borderWidth, osdWidth - contentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Vertical line
        pixmapContent->DrawRectangle(cRect(osdWidth - contentWidthMinimum + radius - borderWidth, radius, borderWidth, contentHeight - 2*radius), Theme.Color(clrMenuBorder));
        if (radius-borderWidth > 2) {
            //Upper Ellipse
            pixmapContent->DrawEllipse(cRect(osdWidth - contentWidthMinimum, 0, radius, radius), Theme.Color(clrMenuBorder),1);
            pixmapContent->DrawEllipse(cRect(osdWidth - contentWidthMinimum, borderWidth, radius-borderWidth, radius-borderWidth), clrTransparent, 1);
            //Lower Ellipse
            pixmapContent->DrawEllipse(cRect(osdWidth - contentWidthMinimum, contentHeight - radius, radius, radius), Theme.Color(clrMenuBorder),4);
            pixmapContent->DrawEllipse(cRect(osdWidth - contentWidthMinimum, contentHeight - radius, radius-borderWidth, radius-borderWidth), clrTransparent, 4);
        }
    }
}

void cNopacityDisplayMenuView::AdjustContentBackground(eMenuCategory menuCat, eMenuCategory menuCatLast, cRect & vidWin) {
    int contentWidth = GetContentWidth(menuCat);
    int contentWidthLast = GetContentWidth(menuCatLast);
    int drawportX;
    if ((contentWidth != contentWidthLast)||(menuCatLast == mcUndefined)) {
        if (contentWidth == contentWidthFull) {
            drawportX = (config.menuAdjustLeft)?0:(contentWidthMinimum - contentWidthFull);
            pixmapContent->SetDrawPortPoint(cPoint(drawportX, 0));
            if (config.scalePicture) {
                // ask output device to restore full size
                vidWin = cDevice::PrimaryDevice()->CanScaleVideo(cRect::Null);
            }
        } else {
            drawportX = (config.menuAdjustLeft)?(contentWidth - contentWidthFull):(contentWidthMinimum - contentWidth);
            pixmapContent->SetDrawPortPoint(cPoint(drawportX, 0));
            if (config.scalePicture) {
                // ask output device to scale down
                int windowX = (config.menuAdjustLeft)?(osdLeft + contentWidth + widthScrollbar + 2 * spaceMenu)
                                                     :(osdLeft + 2 * spaceMenu);
                cRect availableRect(
                    windowX,
                    osdTop + headerHeight,
                    contentWidthFull - osdLeft - contentWidth - widthScrollbar - 4 * spaceMenu,
                    contentHeight);
                vidWin = cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
            }
        }
    }
    osd->DestroyPixmap(pixmapScrollbar);
    int scrollbarX = (config.menuAdjustLeft)?(contentWidth):(osdWidth - contentWidth - widthScrollbar);
    pixmapScrollbar = osd->CreatePixmap(2, cRect(scrollbarX , headerHeight + spaceMenu, widthScrollbar, contentHeight - 2*spaceMenu));
    pixmapScrollbar->Fill(clrTransparent);
}

void cNopacityDisplayMenuView::DrawHeaderLogo(void) {
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("skinIcons/vdrlogo", config.menuHeaderLogoWidth, config.menuHeaderLogoHeight)) {
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
            icon = "menuIcons/Schedule";
            break;
        case mcChannel:
            icon = "menuIcons/Channels";
            break;
        case mcTimer:
            icon = "menuIcons/Timers";
            break;
        case mcRecording:
            icon = "menuIcons/Recordings";
            break;
        case mcSetup:
            icon = "menuIcons/Setup";
            break;
        case mcCommand:
            icon = "menuIcons/Commands";
            break;
        default:
            drawIcon = false;
    }
    
    int left = 0;
    if (drawIcon) {
        int iconX = (config.menuAdjustLeft) ? 0 : (osdWidth - config.headerIconHeight);
        pixmapHeaderIcon = osd->CreatePixmap(2, cRect(iconX, 0, config.headerIconHeight, config.headerIconHeight));
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
    int iconX = (config.menuAdjustLeft) ? 0 : (osdWidth - config.menuItemLogoWidth);
    pixmapHeaderIcon = osd->CreatePixmap(2, cRect(iconX, 0, config.menuItemLogoWidth, config.menuItemLogoHeight));
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
    int labelW = fontHeader->Width(label);
    int labelX = (config.menuAdjustLeft) ? (left) : (osdWidth - dateWidth - labelW - left - 2*spaceMenu);
    pixmapHeaderLabel->DrawText(cPoint(labelX, ((headerHeight - 10) - fontHeader->Height()) / 2), *label, Theme.Color(clrMenuFontHeader), clrTransparent, fontHeader);
}

void cNopacityDisplayMenuView::DrawDate(bool initial) {
    cString date = DayDateTime();
    if (initial || strcmp(date, lastDate)) {
        pixmapDate->Fill(clrTransparent);
        int dateW = fontDate->Width(date);
        int dateX = (config.menuAdjustLeft) ? (dateWidth - dateW - 2*spaceMenu) : (spaceMenu);
        pixmapDate->DrawText(cPoint(dateX, (headerHeight - fontDate->Height()) / 2), date, Theme.Color(clrMenuFontDate), clrTransparent, fontDate);
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
    if (imgLoader.LoadIcon("skinIcons/DiskUsage", iconWidth)) {
        cImage icon = imgLoader.GetImage();
        pixmapDiskUsageIcon->DrawImage(cPoint((diskUsageWidth - iconWidth)/2,0), icon);
    }
    if (imgLoader.LoadIcon("skinIcons/discpercent", diskUsageWidth - 4, diskUsageHeight/5, false)) {
        cImage icon = imgLoader.GetImage();
        pixmapDiskUsageIcon->DrawImage(cPoint(0,4*diskUsageHeight/5), icon);
    }
    pixmapDiskUsageLabel->Fill(clrTransparent);
    cString usage = cString::sprintf("%d%%", DiskUsage);
    cString rest;
    if (config.discUsageStyle == 0)
        rest = cString::sprintf("%02d:%02dh %s", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60, tr("free"));
    else
        rest = cString::sprintf("%d GB %s", cVideoDiskUsage::FreeMB() / 1024, tr("free"));
    pixmapDiskUsageLabel->DrawRectangle(cRect((diskUsageWidth - 4)*DiskUsage/100,4*diskUsageHeight/5, (diskUsageWidth - 4) - (diskUsageWidth - 4)*DiskUsage/100, diskUsageHeight/5), 0xDD000000);
    pixmapDiskUsageLabel->DrawText(cPoint((diskUsageWidth - fontDiskUsagePercent->Width(*usage))/2, 4*diskUsageHeight/5), *usage, Theme.Color(clrMenuFontDiscUsage), clrTransparent, fontDiskUsagePercent);
    pixmapDiskUsageLabel->DrawText(cPoint((diskUsageWidth - fontDiskUsage->Width(*rest))/2, (diskUsageHeight - fontDiskUsage->Height())/2), *rest, Theme.Color(clrMenuFontDiscUsage), clrTransparent, fontDiskUsage);

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

void cNopacityDisplayMenuView::DrawButton(const char *text, int handleImage, tColor buttonColor, tColor borderColor, int num) {
    if (num < 0)
        return;
    int top = 2*buttonsBorder;
    int left = num * buttonWidth + (2*num + 1) * buttonsBorder;
    if (config.doBlending) {
        pixmapFooter->DrawRectangle(cRect(left, top, buttonWidth, buttonHeight), borderColor);
	pixmapFooter->DrawImage(cPoint(left+2, top+2), handleImage);
    } else {
        pixmapFooter->DrawRectangle(cRect(left, top, buttonWidth, buttonHeight), buttonColor);
    }
    if (config.roundedCorners) {
        int radius = config.cornerRadius;
        if (radius > 2) {
            pixmapFooter->DrawEllipse(cRect(left,top,radius,radius), borderColor, -2);
            pixmapFooter->DrawEllipse(cRect(left-2,top-2,radius,radius), Theme.Color(clrMenuBack), -2);

            pixmapFooter->DrawEllipse(cRect(left + buttonWidth -radius, top,radius,radius), borderColor, -1);
            pixmapFooter->DrawEllipse(cRect(left + buttonWidth -radius+2,top-2,radius,radius), Theme.Color(clrMenuBack), -1);

            pixmapFooter->DrawEllipse(cRect(left,top + buttonHeight -radius,radius,radius), borderColor, -3);
            pixmapFooter->DrawEllipse(cRect(left - 2, top + buttonHeight - radius + 2,radius,radius), Theme.Color(clrMenuBack), -3);

            pixmapFooter->DrawEllipse(cRect(left + buttonWidth -radius, top + buttonHeight -radius,radius,radius), borderColor, -4);
            pixmapFooter->DrawEllipse(cRect(left + buttonWidth -radius + 2, top + buttonHeight -radius + 2,radius,radius), Theme.Color(clrMenuBack), -4);
        }
    }
    
    int textWidth = fontButtons->Width(text);
    int textHeight = fontButtons->Height();
    tColor clrFontBack = (config.doBlending)?(clrTransparent):buttonColor;
    pixmapFooter->DrawText(cPoint(left + (buttonWidth-textWidth)/2, top + (buttonHeight-textHeight)/2), text, Theme.Color(clrMenuFontButton), clrFontBack, fontButtons);
}

void cNopacityDisplayMenuView::ClearButton(int num) {
    if (num < 0)
        return;
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

cNopacityTimer *cNopacityDisplayMenuView::DrawTimerConflict(int numConflicts, int y) {
    cNopacityTimer *t = new cNopacityTimer(osd, numConflicts, fontTimers, fontTimersHead);
    t->SetGeometry(timersWidth, y);
    t->CreateConflictText();
    t->CalculateHeight(spaceMenu);
    int timerX = (config.menuAdjustLeft) ? (osdWidth - timersWidth - 10) : 10;
    t->CreatePixmaps(timerX);
    t->Render();
    return t;
}

cNopacityTimer *cNopacityDisplayMenuView::DrawTimer(const cTimer *Timer, int y) {
    cNopacityTimer *t = new cNopacityTimer(osd, Timer, fontTimers, fontTimersHead);
    t->SetGeometry(timersWidth, y);
    t->CreateDate();
    t->CreateShowName();
    t->CalculateHeight(spaceMenu);
    int timerX = (config.menuAdjustLeft) ? (osdWidth - timersWidth - 10) : 10;
    t->CreatePixmaps(timerX);
    t->Render();
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
    pixmapStatus = osd->CreatePixmap(7, cRect(0.1*osdWidth, 0.8*osdHeight, messageWidth, messageHeight));
    pixmapStatus->Fill(col);
    if (config.doBlending) {
        cImageLoader imgLoader;
        imgLoader.DrawBackground2(Theme.Color(clrMenuBack), col, messageWidth-2, messageHeight-2);
        pixmapStatus->DrawImage(cPoint(1, 1), imgLoader.GetImage());
    }
    int textWidth = fontMessage->Width(Text);
    tColor clrFontBack = (config.doBlending)?(clrTransparent):col;
    pixmapStatus->DrawText(cPoint((messageWidth - textWidth) / 2, (messageHeight - fontMessage->Height()) / 2), Text, Theme.Color(clrMenuFontMessages), clrFontBack, fontMessage);
}

void cNopacityDisplayMenuView::ClearMessage(void) {
    if (pixmapStatus) {
        osd->DestroyPixmap(pixmapStatus);
        pixmapStatus = NULL;
    }
}

void cNopacityDisplayMenuView::SetDetailViewSize(eDetailViewType detailViewType, cNopacityMenuDetailView *detailView) {
    int x = (config.menuAdjustLeft) ? 0 : osdWidth - contentWidthFull + 2*spaceMenu;
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
    detailView->SetGeometry(x, width, height, top, contentBorder, detailHeaderHeight);
}

void cNopacityDisplayMenuView::DrawRssFeed(std::string feedName) {
    pixmapRssFeedBackground->Fill(clrBlack);
    pixmapRssFeed->Fill(clrTransparent);
    feedNameLength = fontRssFeed->Width(feedName.c_str());
    int labelWidth = 2 + rssFeedHeight + 2 + feedNameLength + 6;
    pixmapRssFeed->Fill(Theme.Color(clrMenuBorder));
    cImageLoader imgLoader;
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), labelWidth, rssFeedHeight - 4);
    pixmapRssFeed->DrawImage(cPoint(2,2), imgLoader.GetImage());

    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), osdWidth - labelWidth - 2, rssFeedHeight - 4);
    pixmapRssFeed->DrawImage(cPoint(labelWidth,2), imgLoader.GetImage());
    
    pixmapRssFeed->DrawText(cPoint(rssFeedHeight + 2, (rssFeedHeight - fontRssFeed->Height()) / 2), feedName.c_str(), Theme.Color(clrMenuFontHeader), clrTransparent, fontRssFeed);
    pixmapRssFeedIcon->Fill(clrTransparent);
    if (imgLoader.LoadIcon("skinIcons/rss", rssFeedHeight-4)) {
        cImage icon = imgLoader.GetImage();
        pixmapRssFeedIcon->DrawImage(cPoint(2,2), icon);
    }
}

cPoint cNopacityDisplayMenuView::GetRssFeedPosition(void) {
    int x = rssFeedHeight + feedNameLength + 10;
    int y = osdHeight - rssFeedHeight + 2;
    return cPoint(x, y);
}

cPoint cNopacityDisplayMenuView::GetRssFeedSize(void) {
    int width = osdWidth - (rssFeedHeight + feedNameLength + 12);
    int height = rssFeedHeight - 4;
    return cPoint(width, height);
}
