#include "geometrymanager.h"

cGeometryManager::cGeometryManager() {
    SetOSDSize();
}

cGeometryManager::~cGeometryManager() {
}

void cGeometryManager::SetOSDSize(void) {
    osdWidth = cOsd::OsdWidth();
    osdHeight = cOsd::OsdHeight();
    osdLeft = cOsd::OsdLeft();
    osdTop = cOsd::OsdTop();
}

void cGeometryManager::SetGeometry(void) {
    SetDisplayMenuSizes();
    SetDisplayChannelSizes();
    SetDisplayReplaySizes();
    SetDisplayMessageSizes();
    SetDisplayTrackSizes();
    SetDisplayVolumeSizes();
}

bool cGeometryManager::GeometryChanged(void) {
    if ((osdWidth != cOsd::OsdWidth()) || 
        (osdHeight != cOsd::OsdHeight()) ||
        (osdLeft != cOsd::OsdLeft()) ||
        (osdTop != cOsd::OsdTop())) {
        dsyslog("nopacity: osd Size changed");
        dsyslog("nopacity: old osd size: top %d left %d size %d * %d", osdLeft, osdTop, osdWidth, osdHeight);
        SetOSDSize();
        dsyslog("nopacity: new osd size: top %d left %d size %d * %d", osdLeft, osdTop, osdWidth, osdHeight);
        return true;
    }
    return false;
}

void cGeometryManager::SetDisplayMenuSizes() {
    menuSpace = config.spaceMenu;
    
    menuWidthScrollbar = config.widthScrollbar;
    menuDateWidth = osdWidth * 0.3;
    
    menuHeaderHeight = osdHeight * config.headerHeight / 100;
    menuFooterHeight = osdHeight * config.footerHeight / 100;
    menuRssFeedHeight = (config.displayRSSFeed)?(osdHeight * config.rssFeedHeight / 100):0;
    menuContentHeight = osdHeight - menuHeaderHeight - menuFooterHeight - menuRssFeedHeight;

    menuContentWidthMain = osdWidth * config.menuWidthMain / 100;
    menuContentWidthSchedules = osdWidth * config.menuWidthSchedules / 100;
    menuContentWidthChannels = osdWidth * config.menuWidthChannels / 100;
    menuContentWidthTimers = osdWidth * config.menuWidthTimers / 100;
    menuContentWidthRecordings = osdWidth * config.menuWidthRecordings / 100;
    menuContentWidthSetup = osdWidth * config.menuWidthSetup / 100;
    menuContentWidthFull = osdWidth - config.widthScrollbar - config.spaceMenu;
    menuContentWidthMinimum = Minimum(menuContentWidthMain, 
                                  menuContentWidthSchedules, 
                                  menuContentWidthChannels,
                                  menuContentWidthTimers,
                                  menuContentWidthRecordings,
                                  menuContentWidthSetup);

    menuItemWidthDefault = menuContentWidthFull - 4 * config.spaceMenu;
    menuItemWidthMain = menuContentWidthMain - 4 * config.spaceMenu;
    menuItemWidthSchedule = menuContentWidthSchedules - 4 * config.spaceMenu;
    menuItemWidthChannel = menuContentWidthChannels - 4 * config.spaceMenu;
    menuItemWidthTimer = menuContentWidthTimers - 4 * config.spaceMenu;
    menuItemWidthRecording = menuContentWidthRecordings - 4 * config.spaceMenu;
    menuItemWidthSetup = menuContentWidthSetup - 4 * config.spaceMenu;
    menuItemWidthTracks = osdWidth * config.tracksWidth / 100 - 4;
    menuItemHeightMain = config.iconHeight + 2;
    menuItemHeightSchedule = config.menuItemLogoHeight + 2;
    menuItemHeightDefault = menuContentHeight / config.numDefaultMenuItems - config.spaceMenu;
    menuItemHeightRecordings = config.menuRecFolderSize + 2;
    menuItemHeightTracks = config.tracksItemHeight;

    menuButtonsBorder = menuFooterHeight / 6;
    menuButtonWidth = (osdWidth / 4) - 2 * menuButtonsBorder;
    menuButtonHeight = menuFooterHeight - 3 * menuButtonsBorder;
    
    menuDiskUsageWidth = menuDiskUsageHeight = osdWidth  * config.menuSizeDiskUsage / 100;
    menuTimersWidth = osdWidth  * config.menuWidthRightItems / 100;
    menuMessageWidth = 0.8 * osdWidth;
    menuMessageHeight = 0.1 * osdHeight;
}

void cGeometryManager::SetDisplayChannelSizes(void) {
    channelHeight = osdHeight * config.channelHeight / 100;
    channelTop = osdTop + osdHeight - channelHeight - config.channelBorderBottom;
    switch (config.logoPosition) {
        case lpLeft:
            channelWidth = osdWidth - (config.logoWidth + 2 * config.channelBorderVertical + config.logoBorder);
            channelX = config.logoWidth + config.channelBorderVertical + config.logoBorder;
            break;
        case lpRight:
            channelWidth = osdWidth - (config.logoWidth + 2 * config.channelBorderVertical + config.logoBorder);
            channelX = config.channelBorderVertical;
            break;
        case lpNone:
            channelWidth = osdWidth - 2 * config.channelBorderVertical;
            channelX = config.channelBorderVertical;
            break;
    }
    channelInfoWidth = channelWidth * 0.7;
    channelDateWidth = channelWidth - channelInfoWidth;   
    channelInfoHeight = channelHeight * 0.2;
    if (channelInfoHeight%2 != 0)
        channelInfoHeight++;
    channelProgressBarHeight = channelHeight * 0.1;
    channelStreamInfoHeight = channelHeight * 0.2;
    if (channelStreamInfoHeight%2 != 0)
        channelStreamInfoHeight++;
    channelEpgInfoHeight = channelHeight - channelInfoHeight - channelStreamInfoHeight - channelProgressBarHeight;
    channelEpgInfoLineHeight = channelEpgInfoHeight / 4;
    channelStreamInfoY = channelInfoHeight + channelProgressBarHeight + channelEpgInfoHeight;
    channelIconSize = config.statusIconSize;
    channelIconsWidth = 5*channelIconSize;
}

void cGeometryManager::SetDisplayReplaySizes(void) {
    replayHeight = osdHeight * config.replayHeight / 100;
    replayWidth = osdWidth - 2 * config.replayBorderVertical;
    replayHeaderHeight = replayHeight * 0.2;
    if (replayHeaderHeight%2 != 0)
        replayHeaderHeight++;
    replayFooterHeight = replayHeaderHeight;
    replayResolutionX = 10;
    replayResolutionY = 5;
    replayInfo2Height = max(replayHeaderHeight,config.resolutionIconSize+replayResolutionY*2-replayHeaderHeight);
    replayProgressBarHeight = 0.1 * replayHeight;
    if (replayProgressBarHeight%2 != 0)
        replayProgressBarHeight++;
    replayCurrentHeight = replayProgressBarHeight + config.fontReplay;
    replayControlsHeight = replayHeight - replayHeaderHeight - replayInfo2Height - replayFooterHeight - replayProgressBarHeight;
    if (replayControlsHeight < 11)
        replayControlsHeight = 11;
    replayInfoWidth = 0.75 * replayWidth - config.resolutionIconSize - 10;
    replayDateWidth = replayWidth - replayInfoWidth;

    replayJumpX = (replayWidth - 4 * replayControlsHeight)/2 + 5*replayControlsHeight;
    replayJumpY = replayHeaderHeight + replayInfo2Height + replayProgressBarHeight;
    replayJumpWidth = replayWidth - replayJumpX;
    replayJumpHeight = replayControlsHeight;

    replayIconBorder = 5;
    replayIconSize = min(replayControlsHeight - 2*replayIconBorder, 128);    
}

void cGeometryManager::SetDisplayMessageSizes(void) {
    messageWidth = osdWidth * config.messageWidth / 100;
    messageHeight = osdHeight * config.messageHeight / 100;
}

void cGeometryManager::SetDisplayTrackSizes(void) {
    trackWidth = osdWidth * config.tracksWidth / 100;
}

void cGeometryManager::SetDisplayVolumeSizes(void) {
    volumeWidth = osdWidth * config.volumeWidth / 100;
    volumeHeight = osdHeight * config.volumeHeight / 100;
    volumeLabelHeight = volumeHeight/3;
    volumeProgressBarWidth = 0.9 * volumeWidth;
    volumeProgressBarHeight = 0.3 * volumeWidth;
    if (volumeProgressBarHeight%2 != 0)
        volumeProgressBarHeight++;
}
