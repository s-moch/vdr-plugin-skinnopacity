#include "status.h"
#include "displaymenuview.h"

cNopacityDisplayMenuView::cNopacityDisplayMenuView(cOsd *osd) {
    this->osd = osd;
    diskUsageAlert = 95;
    pixmapHeaderIcon = NULL;
    messageBox = NULL;
    volumeBox = NULL;
    lastVolume = statusMonitor->GetVolume();
    lastVolumeTime = time(NULL);
    menuAdjustLeft = config.GetValue("menuAdjustLeft");
    SetDescriptionTextWindowSize();
    CreatePixmaps();
    DrawHeaderLogo();
    DrawBorderDecoration();
}

cNopacityDisplayMenuView::~cNopacityDisplayMenuView(void) {
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapHeaderForeground);
    osd->DestroyPixmap(pixmapHeaderLogo);
    osd->DestroyPixmap(pixmapHeaderLabel);
    osd->DestroyPixmap(pixmapDate);
    osd->DestroyPixmap(pixmapFooter);
    osd->DestroyPixmap(pixmapFooterBack);
    osd->DestroyPixmap(pixmapContent);
    osd->DestroyPixmap(pixmapScrollbar);
    osd->DestroyPixmap(pixmapScrollbarBack);
    osd->DestroyPixmap(pixmapDiskUsage);
    osd->DestroyPixmap(pixmapDiskUsageIcon);
    osd->DestroyPixmap(pixmapDiskUsageLabel);
    osd->DestroyPixmap(pixmapHeaderIcon);
    delete messageBox;
    delete volumeBox;
}

void cNopacityDisplayMenuView::SetDescriptionTextWindowSize(void) {
    int xSchedules, xRecordings, xChannels, xTimers;
    int widthSchedules, widthRecordings, widthChannels, widthTimers;
    if (menuAdjustLeft) {
        xSchedules =    2 * geoManager->menuSpace
                        + geoManager->menuContentWidthSchedules
                        + geoManager->menuWidthScrollbar;
        xRecordings =   2 * geoManager->menuSpace
                        + geoManager->menuContentWidthRecordings
                        + geoManager->menuWidthScrollbar;
        xChannels =     2 * geoManager->menuSpace
                        + geoManager->menuContentWidthChannels
                        + geoManager->menuWidthScrollbar;
        xTimers =       2 * geoManager->menuSpace
                        + geoManager->menuContentWidthTimers
                        + geoManager->menuWidthScrollbar;
        widthSchedules = geoManager->osdWidth - xSchedules - geoManager->menuSpace;
        widthRecordings = geoManager->osdWidth - xRecordings - geoManager->menuSpace;
        widthChannels = geoManager->osdWidth - xChannels - geoManager->menuSpace;
        widthTimers = geoManager->osdWidth - xTimers - geoManager->menuSpace;
    } else {
        xSchedules = geoManager->menuSpace;
        xRecordings = geoManager->menuSpace;
        xChannels = geoManager->menuSpace;
        xTimers = geoManager->menuSpace;
        widthSchedules =  geoManager->osdWidth
                          - geoManager->menuContentWidthSchedules
                          - geoManager->menuWidthScrollbar
                          - 2 * geoManager->menuSpace;
        widthRecordings = geoManager->osdWidth
                          - geoManager->menuContentWidthRecordings
                          - geoManager->menuWidthScrollbar
                          - 2 * geoManager->menuSpace;
        widthChannels =   geoManager->osdWidth
                          - geoManager->menuContentWidthChannels
                          - geoManager->menuWidthScrollbar
                          - 2 * geoManager->menuSpace;
        widthTimers =     geoManager->osdWidth
                          - geoManager->menuContentWidthTimers
                          - geoManager->menuWidthScrollbar
                          - 2 * geoManager->menuSpace;
    }
    int heightFull = geoManager->menuContentHeight - 2*geoManager->menuSpace;
    int height = config.GetValue("menuHeightInfoWindow") * heightFull / 100;
    int y = geoManager->menuHeaderHeight + (geoManager->menuContentHeight - height - geoManager->menuSpace);
    int yFullScreen = geoManager->menuHeaderHeight + geoManager->menuSpace;

    if (config.GetValue("menuSchedulesWindowMode") == 0)
        textWindowSizeSchedules = cRect(xSchedules,y,widthSchedules,height);
    else
        textWindowSizeSchedules = cRect(xSchedules,yFullScreen,widthSchedules,heightFull);
    if (config.GetValue("menuRecordingsWindowMode") == 0)
        textWindowSizeRecordings = cRect(xRecordings,y,widthRecordings,height);
    else
        textWindowSizeRecordings = cRect(xRecordings,yFullScreen,widthRecordings,heightFull);
    if (config.GetValue("menuTimersWindowMode") == 0)
        textWindowSizeTimers = cRect(xTimers,y,widthTimers,height);
    else
        textWindowSizeTimers = cRect(xTimers,yFullScreen,widthTimers,heightFull);
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
        case mcTimer:
            return &textWindowSizeTimers;
        default:
            return NULL;
    }
    return NULL;
}

int cNopacityDisplayMenuView::GetContentWidth(eMenuCategory menuCat) {
    switch (menuCat) {
        case mcMain:
            if (config.GetValue("narrowMainMenu"))
                return geoManager->menuContentWidthMain;
            else
                return geoManager->menuContentWidthFull;
        case mcSetup:
            if (config.GetValue("narrowSetupMenu"))
                return geoManager->menuContentWidthSetup;
            else
                return geoManager->menuContentWidthFull;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            if (config.GetValue("narrowScheduleMenu"))
                return geoManager->menuContentWidthSchedules;
            else
                return geoManager->menuContentWidthFull;
        case mcChannel:
            if (config.GetValue("narrowChannelMenu"))
                return geoManager->menuContentWidthChannels;
            else
                return geoManager->menuContentWidthFull;
        case mcTimer:
            if (config.GetValue("narrowTimerMenu"))
                return geoManager->menuContentWidthTimers;
            else
                return geoManager->menuContentWidthFull;
        case mcRecording:
            if (config.GetValue("narrowRecordingMenu"))
                return geoManager->menuContentWidthRecordings;
            else
                return geoManager->menuContentWidthFull;
        default:
            return geoManager->menuContentWidthFull;
    }
    return geoManager->menuContentWidthFull;
}


void cNopacityDisplayMenuView::CreatePixmaps(void) {
    pixmapHeader = CreatePixmap(osd, "pixmapHeader", 1, cRect(0, 0, geoManager->osdWidth, geoManager->menuHeaderHeight));

    pixmapHeaderForeground = CreatePixmap(osd, "pixmapHeaderForeground", 3, cRect(0, 0, geoManager->osdWidth, geoManager->menuHeaderHeight));

    int dateX = (menuAdjustLeft) ? (geoManager->osdWidth - geoManager->menuDateWidth) : 0;
    pixmapDate = CreatePixmap(osd, "pixmapDate", 2, cRect(dateX, 0, geoManager->menuDateWidth, geoManager->menuHeaderHeight));

    int logoX = (menuAdjustLeft) ? 0 : (geoManager->osdWidth - geoManager->menuHeaderVDRLogoWidth);
    pixmapHeaderLogo = CreatePixmap(osd, "pixmapHeaderLogo", -1, cRect(logoX, 2, geoManager->menuHeaderVDRLogoWidth, geoManager->menuHeaderHeight - 4));

    int labelX = (menuAdjustLeft) ? 0 : geoManager->menuDateWidth;
    pixmapHeaderLabel = CreatePixmap(osd, "pixmapHeaderLabel", 2, cRect(labelX, 0, geoManager->osdWidth - geoManager->menuDateWidth, geoManager->menuHeaderHeight));

    pixmapFooter = CreatePixmap(osd, "pixmapFooter", 2, cRect(0, geoManager->osdHeight - geoManager->menuFooterHeight, geoManager->osdWidth, geoManager->menuFooterHeight));

    pixmapFooterBack = CreatePixmap(osd, "pixmapFooterBack", 1, cRect(0, geoManager->osdHeight - geoManager->menuFooterHeight, geoManager->osdWidth, geoManager->menuFooterHeight));

    int drawPortWidth = geoManager->osdWidth + geoManager->menuContentWidthFull - geoManager->menuContentWidthMinimum;
    pixmapContent = CreatePixmap(osd, "pixmapContent", 1, cRect(0, geoManager->menuHeaderHeight, geoManager->osdWidth, geoManager->menuContentHeight),
                                                          cRect(0, 0, drawPortWidth, geoManager->menuContentHeight));

    int diskUsageX = (menuAdjustLeft) ? (geoManager->osdWidth - geoManager->menuDiskUsageWidth - 10) : 10;
    pixmapDiskUsage = CreatePixmap(osd, "pixmapDiskUsage", 2, cRect(diskUsageX, geoManager->menuHeaderHeight + geoManager->menuSpace, geoManager->menuDiskUsageWidth, geoManager->menuDiskUsageHeight));

    pixmapDiskUsageIcon = CreatePixmap(osd, "pixmapDiskUsageIcon", 3, cRect(diskUsageX + 2, geoManager->menuHeaderHeight + geoManager->menuSpace + 2, geoManager->menuDiskUsageWidth - 4, geoManager->menuDiskUsageHeight - 4));

    pixmapDiskUsageLabel = CreatePixmap(osd, "pixmapDiskUsageLabel", 4, cRect(diskUsageX + 2, geoManager->menuHeaderHeight + geoManager->menuSpace + 2, geoManager->menuDiskUsageWidth - 4, geoManager->menuDiskUsageHeight - 4));

    int scrollbarX = (menuAdjustLeft) ? geoManager->menuContentWidthMain : (geoManager->osdWidth - geoManager->menuContentWidthMain);
    pixmapScrollbar = CreatePixmap(osd, "pixmapScrollbar", 3, cRect(scrollbarX, geoManager->menuHeaderHeight + geoManager->menuSpace, geoManager->menuWidthScrollbar, geoManager->menuContentHeight - 2 * geoManager->menuSpace));

    pixmapScrollbarBack = CreatePixmap(osd, "pixmapScrollbarBack", 2, cRect(scrollbarX, geoManager->menuHeaderHeight + geoManager->menuSpace, geoManager->menuWidthScrollbar, geoManager->menuContentHeight - 2 * geoManager->menuSpace));

    PixmapFill(pixmapHeaderLogo, clrTransparent);
    PixmapFill(pixmapHeaderLabel, clrTransparent);
    PixmapFill(pixmapDiskUsage, clrTransparent);
    PixmapFill(pixmapDiskUsageIcon, clrTransparent);
    PixmapFill(pixmapDiskUsageLabel, clrTransparent);
}

void cNopacityDisplayMenuView::SetAlpha(int Alpha) {
    PixmapSetAlpha(pixmapHeader, Alpha);
    PixmapSetAlpha(pixmapHeaderForeground, Alpha);
    PixmapSetAlpha(pixmapHeaderLogo, Alpha);
    PixmapSetAlpha(pixmapHeaderLabel, Alpha);
    PixmapSetAlpha(pixmapDate, Alpha);
    PixmapSetAlpha(pixmapContent, Alpha);
    PixmapSetAlpha(pixmapFooter, Alpha);
    PixmapSetAlpha(pixmapFooterBack, Alpha);
    PixmapSetAlpha(pixmapScrollbarBack, Alpha);
    PixmapSetAlpha(pixmapScrollbar, Alpha);
    PixmapSetAlpha(pixmapDiskUsage, Alpha);
    PixmapSetAlpha(pixmapDiskUsageIcon, Alpha);
    PixmapSetAlpha(pixmapDiskUsageLabel, Alpha);
    PixmapSetAlpha(pixmapHeaderIcon, Alpha);
    if (volumeBox) volumeBox->SetAlpha(Alpha);
}

void cNopacityDisplayMenuView::GetMenuItemSize(eMenuCategory menuCat, cPoint *itemSize) {
    int itemWidth = 0;
    int itemHeight = 0;
    switch (menuCat) {
        case mcUnknown:
            itemWidth = geoManager->menuItemWidthDefault;
            itemHeight = geoManager->menuItemHeightDefault;
            break;
        case mcMain:
            itemWidth = geoManager->menuItemWidthMain;
            itemHeight = geoManager->menuItemHeightMain;
            break;
        case mcSetup:
            itemWidth = geoManager->menuItemWidthSetup;
            itemHeight = geoManager->menuItemHeightMain;
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            itemWidth = geoManager->menuItemWidthSchedule;
            itemHeight = geoManager->menuItemHeightSchedule;
            break;
        case mcChannel:
            itemWidth = geoManager->menuItemWidthChannel;
            itemHeight = geoManager->menuItemHeightSchedule;
            break;
        case mcTimer:
            itemWidth = geoManager->menuItemWidthTimer;
            itemHeight = geoManager->menuItemHeightSchedule;
            break;
        case mcRecording:
            itemWidth = geoManager->menuItemWidthRecording;
            itemHeight = geoManager->menuItemHeightRecordings;
            break;
        default:
            itemWidth = geoManager->menuItemWidthDefault;
            itemHeight = geoManager->menuItemHeightDefault;
            break;
    }
    itemSize->Set(itemWidth, itemHeight);
}

int cNopacityDisplayMenuView::GetMenuTop(int numItems, int itemHeight) {
    return geoManager->menuHeaderHeight + (geoManager->menuContentHeight - numItems*(itemHeight + geoManager->menuSpace))/2;
}

int cNopacityDisplayMenuView::GetMenuItemLeft(int itemWidth) {
    int menuLeft = geoManager->menuSpace;
    if (!menuAdjustLeft)
        menuLeft = geoManager->osdWidth - itemWidth - geoManager->menuSpace;
    return menuLeft;
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
        mytabs[i] *= fontManager->menuItemDefault->Width("x") + 3;
    } else if (Tab1) {
        mytabs[1] = GetEditableWidth();
    }
}

int cNopacityDisplayMenuView::GetEditableWidth(void) {
    return geoManager->menuContentWidthFull*0.5;
}

int cNopacityDisplayMenuView::GetTextAreaWidth(void) {
    return geoManager->menuContentWidthFull - 2*geoManager->menuSpace;
}

const cFont *cNopacityDisplayMenuView::GetTextAreaFont(bool FixedFont) {
    return cFont::CreateFont(config.fontName, geoManager->menuContentHeight / 25 + config.GetValue("fontDetailView"));
}

void cNopacityDisplayMenuView::DrawBorderDecoration() {
    if (config.GetValue("displayType") == dtGraphical) {
        if (pixmapHeader) {
            cImage *headerImageBack = imgCache->GetSkinElement(seMenuHeader);
            if (headerImageBack)
                pixmapHeader->DrawImage(cPoint(0, 0), *headerImageBack);
            else
                PixmapFill(pixmapHeader, Theme.Color(clrMenuBack));
        }
        if (pixmapHeaderForeground) {
            cImage *headerImageTop = imgCache->GetSkinElement(seMenuHeaderTop);
            if (headerImageTop)
                pixmapHeaderForeground->DrawImage(cPoint(0, 0), *headerImageTop);
            else
                PixmapFill(pixmapHeaderForeground, clrTransparent);
        }
    } else if (config.GetValue("displayType") == dtBlending) {
        PixmapFill(pixmapHeaderForeground, clrTransparent);
        if (pixmapHeader) {
            cImage *headerImage = imgCache->GetSkinElement(seMenuHeader);
            if (headerImage)
                pixmapHeader->DrawImage(cPoint(0, 0), *headerImage);
            else
                PixmapFill(pixmapHeader, Theme.Color(clrMenuBack));
        }
    } else {
        PixmapFill(pixmapHeaderForeground, clrTransparent);
        PixmapFill(pixmapHeader, Theme.Color(clrMenuBack));
    }
    PixmapFill(pixmapFooter, clrTransparent);
    PixmapFill(pixmapFooterBack, Theme.Color(clrMenuBack));

    int borderWidth = 2;
    int radius = 10;

    if (!pixmapContent)
        return;

    PixmapFill(pixmapContent, clrTransparent);

    if (menuAdjustLeft) {
        //Background
        pixmapContent->DrawRectangle(cRect(0, 0, geoManager->menuContentWidthFull - radius, geoManager->menuContentHeight), Theme.Color(clrMenuBack));
        pixmapContent->DrawRectangle(cRect(geoManager->menuContentWidthFull - radius, 0, geoManager->menuWidthScrollbar + geoManager->menuSpace + radius, geoManager->menuContentHeight), Theme.Color(clrMenuScrollBarBase));
        //Upper and lower Corner Square
        pixmapContent->DrawRectangle(cRect(geoManager->menuContentWidthFull - radius, 0, radius, radius), Theme.Color(clrMenuBack));
        pixmapContent->DrawRectangle(cRect(geoManager->menuContentWidthFull - radius, geoManager->menuContentHeight - radius, radius, radius), Theme.Color(clrMenuBack));
        //Upper line
        pixmapContent->DrawRectangle(cRect(geoManager->menuContentWidthFull, 0, geoManager->osdWidth - geoManager->menuContentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Lower Line
        pixmapContent->DrawRectangle(cRect(geoManager->menuContentWidthFull, geoManager->menuContentHeight - borderWidth, geoManager->osdWidth - geoManager->menuContentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Vertical line
        pixmapContent->DrawRectangle(cRect(geoManager->menuContentWidthFull-radius, radius, borderWidth, geoManager->menuContentHeight - 2*radius), Theme.Color(clrMenuBorder));
        if (radius-borderWidth > 2) {
            //Upper Ellipse
            pixmapContent->DrawEllipse(cRect(geoManager->menuContentWidthFull - radius, 0, radius, radius), Theme.Color(clrMenuBorder),2);
            pixmapContent->DrawEllipse(cRect(geoManager->menuContentWidthFull - radius + borderWidth, borderWidth, radius-borderWidth, radius-borderWidth), Theme.Color(clrMenuScrollBarBase), 2);
            //Lower Ellipse
            pixmapContent->DrawEllipse(cRect(geoManager->menuContentWidthFull - radius, geoManager->menuContentHeight - radius, radius, radius), Theme.Color(clrMenuBorder),3);
            pixmapContent->DrawEllipse(cRect(geoManager->menuContentWidthFull - radius + borderWidth, geoManager->menuContentHeight - radius, radius-borderWidth, radius-borderWidth), Theme.Color(clrMenuScrollBarBase), 3);
        }
    } else {
        //Background
        pixmapContent->DrawRectangle(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum + radius, 0, geoManager->menuContentWidthFull - radius, geoManager->menuContentHeight), Theme.Color(clrMenuBack));
        pixmapContent->DrawRectangle(cRect(0, 0, geoManager->menuWidthScrollbar + geoManager->menuSpace + radius, geoManager->menuContentHeight), Theme.Color(clrMenuScrollBarBase));
        //Upper and lower Corner Square
        pixmapContent->DrawRectangle(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum, 0, radius, radius), Theme.Color(clrMenuBack));
        pixmapContent->DrawRectangle(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum, geoManager->menuContentHeight - radius, radius, radius), Theme.Color(clrMenuBack));
        //Upper line
        pixmapContent->DrawRectangle(cRect(0, 0, geoManager->osdWidth - geoManager->menuContentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Lower Line
        pixmapContent->DrawRectangle(cRect(0, geoManager->menuContentHeight - borderWidth, geoManager->osdWidth - geoManager->menuContentWidthMinimum, borderWidth), Theme.Color(clrMenuBorder));
        //Vertical line
        pixmapContent->DrawRectangle(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum + radius - borderWidth, radius, borderWidth, geoManager->menuContentHeight - 2*radius), Theme.Color(clrMenuBorder));
        if (radius-borderWidth > 2) {
            //Upper Ellipse
            pixmapContent->DrawEllipse(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum, 0, radius, radius), Theme.Color(clrMenuBorder),1);
            pixmapContent->DrawEllipse(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum, borderWidth, radius-borderWidth, radius-borderWidth), Theme.Color(clrMenuScrollBarBase), 1);
            //Lower Ellipse
            pixmapContent->DrawEllipse(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum, geoManager->menuContentHeight - radius, radius, radius), Theme.Color(clrMenuBorder),4);
            pixmapContent->DrawEllipse(cRect(geoManager->osdWidth - geoManager->menuContentWidthMinimum, geoManager->menuContentHeight - radius, radius-borderWidth, radius-borderWidth), Theme.Color(clrMenuScrollBarBase), 4);
        }
    }
}

void cNopacityDisplayMenuView::AdjustContentBackground(eMenuCategory menuCat, eMenuCategory menuCatLast, cRect & vidWin) {
    int contentWidth = GetContentWidth(menuCat);
    int contentWidthLast = GetContentWidth(menuCatLast);
    int drawportX;
    if (pixmapContent && (contentWidth != contentWidthLast) || (menuCatLast == mcUndefined)) {
        if (contentWidth == geoManager->menuContentWidthFull) {
            drawportX = (menuAdjustLeft) ? 0 : (geoManager->menuContentWidthMinimum - geoManager->menuContentWidthFull);
            pixmapContent->SetDrawPortPoint(cPoint(drawportX, 0));
            if (config.GetValue("scalePicture")) {
                // ask output device to restore full size
                vidWin = cDevice::PrimaryDevice()->CanScaleVideo(cRect::Null);
            }
        } else {
            drawportX = (menuAdjustLeft)?(contentWidth - geoManager->menuContentWidthFull):(geoManager->menuContentWidthMinimum - contentWidth);
            pixmapContent->SetDrawPortPoint(cPoint(drawportX, 0));
            if (config.GetValue("scalePicture")) {
                // ask output device to scale down
                int windowX = (menuAdjustLeft)?(geoManager->osdLeft + contentWidth + geoManager->menuWidthScrollbar + 2 * geoManager->menuSpace)
                                              :(geoManager->osdLeft + 2 * geoManager->menuSpace);
                cRect availableRect(
                    windowX,
                    geoManager->osdTop + geoManager->menuHeaderHeight,
                    geoManager->menuContentWidthFull - geoManager->osdLeft - contentWidth - geoManager->menuWidthScrollbar - 4 * geoManager->menuSpace,
                    geoManager->menuContentHeight);
                vidWin = cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
            }
        }
    }

    osd->DestroyPixmap(pixmapScrollbar);
    osd->DestroyPixmap(pixmapScrollbarBack);

    int scrollbarX = (menuAdjustLeft) ? (contentWidth) : (geoManager->osdWidth - contentWidth - geoManager->menuWidthScrollbar);
    pixmapScrollbar = CreatePixmap(osd, "pixmapScrollbar", 3, cRect(scrollbarX , geoManager->menuHeaderHeight + geoManager->menuSpace, geoManager->menuWidthScrollbar, geoManager->menuContentHeight - 2 * geoManager->menuSpace));

    pixmapScrollbarBack = CreatePixmap(osd, "pixmapScrollbarBack", 2, cRect(scrollbarX , geoManager->menuHeaderHeight + geoManager->menuSpace, geoManager->menuWidthScrollbar, geoManager->menuContentHeight - 2 * geoManager->menuSpace));

    PixmapFill(pixmapScrollbar, clrTransparent);
    PixmapFill(pixmapScrollbarBack, clrTransparent);
}

void cNopacityDisplayMenuView::DrawHeaderLogo(void) {
    if (!pixmapHeaderLogo)
        return;

    cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/vdrlogo");
    if (imgIcon)
        pixmapHeaderLogo->DrawImage(cPoint(0,0), *imgIcon);
}

int cNopacityDisplayMenuView::ShowHeaderLogo(bool show) {
    if (!pixmapHeaderLogo)
        return 0;

    pixmapHeaderLogo->SetLayer(show ? 2 : -1);
    return geoManager->menuHeaderVDRLogoWidth + geoManager->menuSpace;
}

int cNopacityDisplayMenuView::DrawHeaderIcon(eMenuCategory menuCat, bool initial) {
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
        int iconSize = geoManager->menuHeaderHeight - 4;
        int iconX = (menuAdjustLeft) ? 0 : (geoManager->osdWidth - geoManager->menuHeaderHeight);
        if (!(pixmapHeaderIcon = CreatePixmap(osd, "pixmapHeaderIcon", 2, cRect(iconX, 2, iconSize, iconSize)))) {
            return left;
        }
        PixmapFill(pixmapHeaderIcon, clrTransparent);

        if (initial && config.GetValue("animation") && config.GetValue("menuFadeTime")) {
            pixmapHeaderIcon->SetAlpha(0);
        }
        cImage *imgIcon = imgCache->GetSkinIcon(*icon);
        if (imgIcon) {
            pixmapHeaderIcon->DrawImage(cPoint(0,0), *imgIcon);
            left = geoManager->menuHeaderHeight + geoManager->menuSpace;
        }
    }
    return left;
}

int cNopacityDisplayMenuView::ShowHeaderIconChannelLogo(const char *Title, bool initial) {
    int left = 0;
    int logoHeight = geoManager->menuHeaderHeight - 4;
    cSize logoSize = ScaleToFit(1000,
                                logoHeight,
                                config.GetValue("logoWidth"),
                                config.GetValue("logoHeight"));
    int logoWidth = logoSize.Width();
    int iconX = (menuAdjustLeft) ? 0 : (geoManager->osdWidth - logoWidth);
    if (!(pixmapHeaderIcon = CreatePixmap(osd, "pixmapHeaderIcon", 2, cRect(iconX, 2, logoWidth, logoHeight)))) {
         return left;
    }
    PixmapFill(pixmapHeaderIcon, clrTransparent);

    if (initial && config.GetValue("animation") && config.GetValue("menuFadeTime")) {
        pixmapHeaderIcon->SetAlpha(0);
    }
    std::string channel = Title;
    if (channel.length() == 0)
        return left;
    std::string remove = trVDR("Schedule");
    try {
        remove.append(" - ");
        channel.erase(0, remove.length());
    } catch (...) {}
    cImageLoader imgLoader;
    if (imgLoader.LoadLogo(channel.c_str(), logoWidth, logoHeight)) {
        cImage logo = imgLoader.GetImage();
        pixmapHeaderIcon->DrawImage(cPoint((logoWidth - logo.Width()) / 2, (logoHeight - logo.Height()) / 2), logo);
        left =  logoWidth + geoManager->menuSpace;
    }
    return left;
}

void cNopacityDisplayMenuView::DestroyHeaderIcon(void) {
    osd->DestroyPixmap(pixmapHeaderIcon);
    pixmapHeaderIcon = NULL;
}

void cNopacityDisplayMenuView::DrawHeaderLabel(int left, cString label) {
    if (!pixmapHeaderLabel)
        return;

    PixmapFill(pixmapHeaderLabel, clrTransparent);
    int labelW = fontManager->menuHeader->Width(label);
    int labelX = (menuAdjustLeft) ? (left)
                : (geoManager->osdWidth - geoManager->menuDateWidth - labelW - left - 2*geoManager->menuSpace);
    int labelY = (geoManager->menuHeaderHeight - fontManager->menuHeader->Height())/2;
    pixmapHeaderLabel->DrawText(cPoint(labelX, labelY),
                                *label,
                                Theme.Color(clrMenuFontHeader),
                                clrTransparent,
                                fontManager->menuHeader);
}

void cNopacityDisplayMenuView::DrawDate(bool initial) {
    if (!pixmapDate)
        return;

    cString date = DayDateTime();
    if (initial || strcmp(date, lastDate)) {
        PixmapFill(pixmapDate, clrTransparent);
        int dateW = fontManager->menuDate->Width(date);
        int dateX = (menuAdjustLeft) ? (geoManager->menuDateWidth - dateW - 2*geoManager->menuSpace) : (geoManager->menuSpace);
        pixmapDate->DrawText(cPoint(dateX, (geoManager->menuHeaderHeight - fontManager->menuDate->Height()) / 2), date, Theme.Color(clrMenuFontDate), clrTransparent, fontManager->menuDate);
        lastDate = date;
    }
}

void cNopacityDisplayMenuView::DrawDiskUsage(void) {
    if (!(pixmapDiskUsage && pixmapDiskUsageIcon && pixmapDiskUsageLabel))
        return;

    int iconWidth = geoManager->menuDiskUsageWidth * 3/4;
    int DiskUsage = cVideoDiskUsage::UsedPercent();
    bool DiskAlert = DiskUsage > diskUsageAlert;
    tColor bgColor = DiskAlert ? Theme.Color(clrDiskAlert) : Theme.Color(clrMenuBack);
    PixmapFill(pixmapDiskUsage, Theme.Color(clrMenuBorder));
    pixmapDiskUsage->DrawRectangle(cRect(2,2,geoManager->menuDiskUsageWidth-4, geoManager->menuDiskUsageHeight-4), bgColor);
    cImage *imgDisc = imgCache->GetSkinIcon("skinIcons/DiskUsage");
    if (imgDisc)
        pixmapDiskUsageIcon->DrawImage(cPoint((geoManager->menuDiskUsageWidth - iconWidth)/2,0), *imgDisc);
    cImage *imgDiscPerc = imgCache->GetSkinIcon("skinIcons/discpercent");
    if (imgDiscPerc)
        pixmapDiskUsageIcon->DrawImage(cPoint(0,4*geoManager->menuDiskUsageHeight/5), *imgDiscPerc);
    PixmapFill(pixmapDiskUsageLabel, clrTransparent);
    cString usage = cString::sprintf("%d%%", DiskUsage);
    cString rest;
    if (config.GetValue("discUsageStyle") == 0)
        rest = cString::sprintf("%02d:%02dh %s", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60, tr("free"));
    else
        rest = cString::sprintf("%d GB %s", cVideoDiskUsage::FreeMB() / 1024, tr("free"));
    pixmapDiskUsageLabel->DrawRectangle(cRect((geoManager->menuDiskUsageWidth - 4)*DiskUsage/100,4*geoManager->menuDiskUsageHeight/5, (geoManager->menuDiskUsageWidth - 4) - (geoManager->menuDiskUsageWidth - 4)*DiskUsage/100, geoManager->menuDiskUsageHeight/5), 0xDD000000);
    pixmapDiskUsageLabel->DrawText(cPoint((geoManager->menuDiskUsageWidth - fontManager->menuDiskUsagePercent->Width(*usage))/2, 4*geoManager->menuDiskUsageHeight/5), *usage, Theme.Color(clrMenuFontDiscUsagePerc), clrTransparent, fontManager->menuDiskUsagePercent);
    pixmapDiskUsageLabel->DrawText(cPoint((geoManager->menuDiskUsageWidth - fontManager->menuDiskUsage->Width(*rest))/2, (geoManager->menuDiskUsageHeight - fontManager->menuDiskUsage->Height())/2), *rest, Theme.Color(clrMenuFontDiscUsage), clrTransparent, fontManager->menuDiskUsage);

    pixmapDiskUsage->SetLayer(2);
    pixmapDiskUsageIcon->SetLayer(3);
    pixmapDiskUsageLabel->SetLayer(4);
}

void cNopacityDisplayMenuView::ShowDiskUsage(bool show) {
    if (!(pixmapDiskUsage && pixmapDiskUsageIcon && pixmapDiskUsageLabel))
        return;

    pixmapDiskUsage->SetLayer(show ? 2 : -1);
    pixmapDiskUsageIcon->SetLayer(show ? 3 : -1);
    pixmapDiskUsageLabel->SetLayer(show ? 4 : -1);
}

void cNopacityDisplayMenuView::DrawButton(const char *text, eSkinElementType seButton, tColor buttonColor, tColor borderColor, tColor fontColor, int num) {
    if (!pixmapFooter || num < 0)
        return;

    int top = 2*geoManager->menuButtonsBorder;
    int left = num * geoManager->menuButtonWidth + (2*num + 1) * geoManager->menuButtonsBorder;
    if (config.GetValue("displayType") == dtBlending) {
        pixmapFooter->DrawRectangle(cRect(left, top, geoManager->menuButtonWidth, geoManager->menuButtonHeight), borderColor);
        cImage *back = imgCache->GetSkinElement(seButton);
        if (back)
            pixmapFooter->DrawImage(cPoint(left+2, top+2), *back);
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *image = imgCache->GetSkinElement(seButton);
        if (image)
            pixmapFooter->DrawImage(cPoint(left, top), *image);
    } else {
        pixmapFooter->DrawRectangle(cRect(left, top, geoManager->menuButtonWidth, geoManager->menuButtonHeight), borderColor);
        pixmapFooter->DrawRectangle(cRect(left+1, top+1, geoManager->menuButtonWidth-2, geoManager->menuButtonHeight-2), buttonColor);
    }
    if (config.GetValue("roundedCorners") && (config.GetValue("displayType") != dtGraphical)) {
        int radius = config.GetValue("cornerRadius");
        if (radius > 2) {
            pixmapFooter->DrawEllipse(cRect(left,top,radius,radius), borderColor, -2);
            pixmapFooter->DrawEllipse(cRect(left-1,top-1,radius,radius), clrTransparent, -2);

            pixmapFooter->DrawEllipse(cRect(left + geoManager->menuButtonWidth -radius, top,radius,radius), borderColor, -1);
            pixmapFooter->DrawEllipse(cRect(left + geoManager->menuButtonWidth -radius+1,top-1,radius,radius), clrTransparent, -1);

            pixmapFooter->DrawEllipse(cRect(left,top + geoManager->menuButtonHeight -radius,radius,radius), borderColor, -3);
            pixmapFooter->DrawEllipse(cRect(left - 1, top + geoManager->menuButtonHeight - radius + 1,radius,radius), clrTransparent, -3);

            pixmapFooter->DrawEllipse(cRect(left + geoManager->menuButtonWidth -radius, top + geoManager->menuButtonHeight -radius,radius,radius), borderColor, -4);
            pixmapFooter->DrawEllipse(cRect(left + geoManager->menuButtonWidth -radius + 1, top + geoManager->menuButtonHeight -radius + 1,radius,radius), clrTransparent, -4);
        }
    }
    int textWidth = fontManager->menuButtons->Width(text);
    int textHeight = fontManager->menuButtons->Height();
    pixmapFooter->DrawText(cPoint(left + (geoManager->menuButtonWidth-textWidth)/2, top + (geoManager->menuButtonHeight-textHeight)/2), text, fontColor, clrTransparent, fontManager->menuButtons);
}

void cNopacityDisplayMenuView::ClearButton(void) {
    PixmapFill(pixmapFooter, clrTransparent);
}

int cNopacityDisplayMenuView::GetTimersInitHeight(void) {
    int initHeight = geoManager->menuHeaderHeight + 2*geoManager->menuSpace;
    if (config.GetValue("showDiscUsage"))
        initHeight += geoManager->menuDiskUsageHeight;
    return initHeight;
}

int cNopacityDisplayMenuView::GetTimersMaxHeight(void) {
    return geoManager->menuHeaderHeight + geoManager->menuContentHeight;
}

cNopacityTimer *cNopacityDisplayMenuView::DrawTimerConflict(int numConflicts, int y) {
    cNopacityTimer *t = new cNopacityTimer(osd, numConflicts, fontManager->menuTimers, fontManager->menuTimersHead);
    t->SetGeometry(geoManager->menuTimersWidth, y);
    t->CreateConflictText();
    t->CalculateHeight(geoManager->menuSpace);
    int timerX = (menuAdjustLeft) ? (geoManager->osdWidth - geoManager->menuTimersWidth - 10) : 10;
    t->CreatePixmaps(timerX);
    t->Render();
    return t;
}

cNopacityTimer *cNopacityDisplayMenuView::DrawTimer(const cTimer *Timer, int y) {
    cNopacityTimer *t = new cNopacityTimer(osd, Timer, fontManager->menuTimers, fontManager->menuTimersHead);
    t->SetGeometry(geoManager->menuTimersWidth, y);
    t->CreateDate();
    t->CreateShowName();
    t->CalculateHeight(geoManager->menuSpace);
    int timerX = (menuAdjustLeft) ? (geoManager->osdWidth - geoManager->menuTimersWidth - 10) : 10;
    t->CreatePixmaps(timerX);
    t->Render();
    return t;
}

void cNopacityDisplayMenuView::DrawScrollbar(double Height, double Offset) {
    if (!(pixmapScrollbar && pixmapScrollbarBack))
        return;

    int totalHeight = pixmapScrollbar->ViewPort().Height() - 6;
    int height = Height * totalHeight;
    int offset = Offset * totalHeight;
    if (config.GetValue("displayType") == dtGraphical) {
        cImage *image = imgCache->GetSkinElement(seScrollbar);
        if (image)
            pixmapScrollbarBack->DrawImage(cPoint(0, 0), *image);
    } else {
        PixmapFill(pixmapScrollbarBack, Theme.Color(clrMenuScrollBar));
        pixmapScrollbarBack->DrawRectangle(cRect(2,2,geoManager->menuWidthScrollbar-4,totalHeight+2), Theme.Color(clrMenuScrollBarBack));
    }
    PixmapFill(pixmapScrollbar, clrTransparent);
    pixmapScrollbar->DrawRectangle(cRect(3,3 + offset,geoManager->menuWidthScrollbar-6,height), Theme.Color(clrMenuScrollBar));
}

void cNopacityDisplayMenuView::ClearScrollbar(void) {
    PixmapFill(pixmapScrollbar, clrTransparent);
    PixmapFill(pixmapScrollbarBack, clrTransparent);
}


void cNopacityDisplayMenuView::DrawMessage(eMessageType Type, const char *Text) {
    DELETENULL(volumeBox);
    DELETENULL(messageBox);
    if (!Text)
        return;
    messageBox = new cNopacityMessageBox(osd,
					 cRect((geoManager->osdWidth - geoManager->messageWidth) / 2,
					       geoManager->osdHeight * 9/10 - geoManager->messageHeight,
					       geoManager->messageWidth, geoManager->messageHeight),
					 Type, Text, true);
}

void cNopacityDisplayMenuView::DrawVolume(void) {
    int volume = statusMonitor->GetVolume();
    if (volume != lastVolume) {
        if (!volumeBox) {
            bool simple = (config.GetValue("displayMenuVolume") == vbSimple) ? true : false;
            volumeBox = new cNopacityVolumeBox(osd,
                                               cRect(geoManager->menuVolumeLeft,
                                                     geoManager->menuVolumeTop,
                                                     geoManager->menuVolumeWidth,
                                                     geoManager->menuVolumeHeight),
                                               fontManager->volumeText,
                                               simple,
                                               simple);
        }
        volumeBox->SetVolume(volume, MAXVOLUME, volume ? false : true);
        lastVolumeTime = time(NULL);
        lastVolume = volume;
    }
    else {
        if (volumeBox && (time(NULL) - lastVolumeTime > 2))
            DELETENULL(volumeBox);
    }
}
