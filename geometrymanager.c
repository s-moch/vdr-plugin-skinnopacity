#include "geometrymanager.h"
#include "config.h"
#include "helpers.h"
#include <vdr/osd.h>

cGeometryManager *geoManager;

cGeometryManager::cGeometryManager() {
    osdLeft = 0;
    osdTop = 0;
    osdWidth = 0;
    osdHeight = 0;
}

cGeometryManager::~cGeometryManager() {
}

bool cGeometryManager::SetOSDSize(void) {
    if ((osdWidth != cOsd::OsdWidth()) ||
        (osdHeight != cOsd::OsdHeight()) ||
        (osdLeft != cOsd::OsdLeft()) ||
        (osdTop != cOsd::OsdTop())) {
        dsyslog("nopacity: osd Size changed");
        dsyslog("nopacity: old osd size: top %d left %d size %d * %d", osdLeft, osdTop, osdWidth, osdHeight);
        osdWidth = cOsd::OsdWidth();
        osdHeight = cOsd::OsdHeight();
        osdLeft = cOsd::OsdLeft();
        osdTop = cOsd::OsdTop();
        dsyslog("nopacity: new osd size: top %d left %d size %d * %d", osdLeft, osdTop, osdWidth, osdHeight);
        return true;
    }
    return false;
}

void cGeometryManager::SetGeometry(void) {
    // DisplayMenu Sizes
    menuSpace = config.GetValue("spaceMenu");

    menuWidthScrollbar = config.GetValue("widthScrollbar");
    menuDateWidth = osdWidth * 0.3;

    menuHeaderHeight = osdHeight * config.GetValue("headerHeight") / 100;
    menuFooterHeight = osdHeight * config.GetValue("footerHeight") / 100;
    menuContentHeight = osdHeight - menuHeaderHeight - menuFooterHeight;

    menuContentWidthMain = osdWidth * config.GetValue("menuWidthMain") / 100;
    menuContentWidthSchedules = osdWidth * config.GetValue("menuWidthSchedules") / 100;
    menuContentWidthChannels = osdWidth * config.GetValue("menuWidthChannels") / 100;
    menuContentWidthTimers = osdWidth * config.GetValue("menuWidthTimers") / 100;
    menuContentWidthRecordings = osdWidth * config.GetValue("menuWidthRecordings") / 100;
    menuContentWidthSetup = osdWidth * config.GetValue("menuWidthSetup") / 100;
    menuContentWidthFull = osdWidth - config.GetValue("widthScrollbar") - config.GetValue("spaceMenu");
    menuContentWidthMinimum = Minimum(menuContentWidthMain,
                                  menuContentWidthSchedules,
                                  menuContentWidthChannels,
                                  menuContentWidthTimers,
                                  menuContentWidthRecordings,
                                  menuContentWidthSetup);

    menuItemWidthDefault = menuContentWidthFull - 4 * config.GetValue("spaceMenu");
    menuItemWidthMain = menuContentWidthMain - 4 * config.GetValue("spaceMenu");
    menuItemWidthSchedule = menuContentWidthSchedules - 4 * config.GetValue("spaceMenu");
    menuItemWidthChannel = menuContentWidthChannels - 4 * config.GetValue("spaceMenu");
    menuItemWidthTimer = menuContentWidthTimers - 4 * config.GetValue("spaceMenu");
    menuItemWidthRecording = menuContentWidthRecordings - 4 * config.GetValue("spaceMenu");
    menuItemWidthSetup = menuContentWidthSetup - 4 * config.GetValue("spaceMenu");
    menuItemWidthTracks = osdWidth * config.GetValue("tracksWidth") / 100 - 4;
    menuItemHeightMain = menuContentHeight / config.GetValue("numMainMenuItems") - config.GetValue("spaceMenu");
    menuItemHeightSchedule = menuContentHeight / config.GetValue("numSchedulesMenuItems") - config.GetValue("spaceMenu");;
    menuItemHeightDefault = menuContentHeight / config.GetValue("numDefaultMenuItems") - config.GetValue("spaceMenu");
    menuItemHeightRecordings = menuContentHeight / config.GetValue("numRecordingsMenuItems") - config.GetValue("spaceMenu");
    menuItemHeightTracks = config.GetValue("tracksItemHeight");

    menuMainMenuIconSize = menuItemHeightMain - 2 * menuSpace;
    cSize menuLogoSize = ScaleToFit(1000,
                                menuItemHeightSchedule - 2,
                                config.GetValue("logoWidthOriginal"),
                                config.GetValue("logoHeightOriginal"));
    menuLogoWidth = menuLogoSize.Width();
    menuLogoHeight = menuLogoSize.Height();
    
    cSize logoSizeVDRHeader = ScaleToFit(1000,
                                menuHeaderHeight - 4,
                                config.GetValue("menuHeaderLogoWidth"),
                                config.GetValue("menuHeaderLogoHeight"));
    menuHeaderVDRLogoWidth = logoSizeVDRHeader.Width();

    menuButtonsBorder = menuFooterHeight / 6;
    menuButtonWidth = (osdWidth / 4) - 2 * menuButtonsBorder;
    menuButtonHeight = menuFooterHeight - 3 * menuButtonsBorder;

    menuDiskUsageWidth = menuDiskUsageHeight = osdWidth  * config.GetValue("menuSizeDiskUsage") / 100;
    menuTimersWidth = osdWidth  * config.GetValue("menuWidthRightItems") / 100;
    cSize timersLogoSize = ScaleToFit(menuTimersWidth * config.GetValue("timersLogoWidth") / 100,
                                1000,
                                config.GetValue("logoWidthOriginal"),
                                config.GetValue("logoHeightOriginal"));
    menuTimersLogoWidth = timersLogoSize.Width();
    menuTimersLogoHeight = timersLogoSize.Height();

    // DisplayChannel Sizes
    channelX = config.GetValue("channelBorderVertical");
    channelWidth = osdWidth - 2 * config.GetValue("channelBorderVertical");
    channelHeight = osdHeight * config.GetValue("channelHeight") / 100;
    channelTop = osdHeight - channelHeight - config.GetValue("channelBorderBottom");

    channelHeaderHeight = 0.2 * channelHeight;
    channelFooterHeight = 0.2 * channelHeight;
    channelContentHeight = channelHeight - channelHeaderHeight - channelFooterHeight;

    int logoWidthTotalPercent = 16;
    channelLogoWidthTotal = logoWidthTotalPercent * channelWidth /100;

    int logoMaxWidth = channelLogoWidthTotal - 10;
    int logoMaxHeight;
    if (config.GetValue("displayType") == dtGraphical) {
        logoMaxHeight = channelHeight - channelHeaderHeight - 2;
    } else {
        logoMaxHeight = channelHeight - 2;
    }
    cSize channelLogoSize = ScaleToFit(logoMaxWidth,
                                logoMaxHeight,
                                config.GetValue("logoWidthOriginal"),
                                config.GetValue("logoHeightOriginal"));
    channelLogoWidth = channelLogoSize.Width();
    channelLogoHeight = channelLogoSize.Height();
    channelLogoX = (channelLogoWidthTotal - channelLogoWidth) / 2;

    switch (config.GetValue("logoVerticalAlignment")) {
        case lvTop:
            channelLogoY = channelTop + (channelHeight - channelHeaderHeight - channelLogoHeight)/2;
            break;
        case lvMiddle:
            channelLogoY = channelTop + (channelHeight - channelLogoHeight)/2;
            break;
        case lvBottom:
            channelLogoY = (channelTop + channelHeaderHeight) + (channelHeight - channelHeaderHeight - channelLogoHeight)/2;
            break;
    }

    switch (config.GetValue("logoPosition")) {
        case lpLeft:
            channelContentX = channelLogoWidthTotal;
            channelContentWidth = channelWidth - channelLogoWidthTotal;
            break;
        case lpRight:
            channelContentX = 0;
            channelContentWidth = channelWidth - channelLogoWidthTotal;
            channelLogoX = channelContentWidth;
            break;
        case lpNone:
            channelContentX = 0;
            channelContentWidth = channelWidth;
            break;
    }

    channelChannelNameWidth = channelContentWidth * 70 / 100;
    channelDateWidth = channelContentWidth - channelChannelNameWidth;
    channelProgressBarHeight = channelHeight * 0.1;
    channelEpgInfoHeight = channelContentHeight - channelProgressBarHeight;
    channelEpgInfoLineHeight = channelEpgInfoHeight / 4;
    channelFooterY = channelTop + channelHeaderHeight + channelContentHeight;

    // DisplayReplay Sizes
    replayHeight = osdHeight * config.GetValue("replayHeight") / 100;
    replayWidth = osdWidth - 2 * config.GetValue("replayBorderVertical");
    replayHeaderHeight = replayHeight * 0.2;
    if (replayHeaderHeight%2 != 0)
        replayHeaderHeight++;
    replayFooterHeight = replayHeaderHeight;
    replayResolutionSize = replayHeaderHeight - 10;
    replayResolutionX = replayWidth - replayResolutionSize*3 - replayHeaderHeight/2;
    replayResolutionY = replayHeight - replayFooterHeight;
    replayInfo2Height = replayHeaderHeight;
    replayProgressBarHeight = 0.1 * replayHeight;
    if (replayProgressBarHeight%2 != 0)
        replayProgressBarHeight++;
    replayCurrentHeight = replayProgressBarHeight + config.GetValue("fontReplay");
    replayControlsHeight = replayHeight - replayHeaderHeight - replayInfo2Height - replayFooterHeight - replayProgressBarHeight;
    if (replayControlsHeight < 11)
        replayControlsHeight = 11;
    replayInfoWidth = 0.75 * replayWidth;
    replayDateWidth = replayWidth - replayInfoWidth;

    replayJumpX = (replayWidth - 4 * replayControlsHeight)/2 + 5*replayControlsHeight;
    replayJumpY = replayHeaderHeight + replayInfo2Height + replayProgressBarHeight;
    replayJumpWidth = replayWidth - replayJumpX;
    replayJumpHeight = replayControlsHeight;

    replayIconBorder = 5;
    replayIconSize = std::min(replayControlsHeight - 2*replayIconBorder, 128);

    replayMessageY = replayHeight - replayFooterHeight;
    replayMessageWidth = replayWidth * 75 / 100;
    replayMessageHeight = replayFooterHeight;

    // DisplayMessage Sizes
    messageWidth = osdWidth * config.GetValue("messageWidth") / 100;
    messageHeight = osdHeight * config.GetValue("messageHeight") / 100;

    // DisplayTrack Sizes
    trackWidth = osdWidth * config.GetValue("tracksWidth") / 100;

    // DisplayVolume Sizes
    volumeWidth = osdWidth * config.GetValue("volumeWidth") / 100;
    volumeHeight = osdHeight * config.GetValue("volumeHeight") / 100;
    volumeLabelHeight = volumeHeight/3;
    volumeProgressBarWidth = 0.9 * volumeWidth;
    volumeProgressBarHeight = 0.3 * volumeHeight;
    if (volumeProgressBarHeight%2 != 0)
        volumeProgressBarHeight++;
}
