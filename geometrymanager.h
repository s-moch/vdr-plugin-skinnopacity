#ifndef __NOPACITY_GEOMETRYMANAGER_H
#define __NOPACITY_GEOMETRYMANAGER_H

enum eLogoPosition {lpNone = 0, lpLeft, lpRight};
enum eLogoVerticalPosition {lvTop = 0, lvMiddle, lvBottom};
enum eBackgroundStyle {bsTrans = 0, bsFull};

class cGeometryManager {
    private:
    public:
        cGeometryManager();
        ~cGeometryManager();
        bool SetOSDSize(void);
        void SetGeometry(void);
        int osdWidth;
        int osdHeight;
        int osdLeft;
        int osdTop;
        //DisplayMenu Sizes
        int menuSpace;
        int menuWidthScrollbar;
        int menuDateWidth;
        int menuHeaderHeight;
        int menuFooterHeight;
        int menuContentHeight;
        int menuContentWidthMain;
        int menuContentWidthSchedules;
        int menuContentWidthChannels;
        int menuContentWidthTimers;
        int menuContentWidthRecordings;
        int menuContentWidthSetup;
        int menuContentWidthFull;
        int menuContentWidthMinimum;
        int menuItemWidthDefault;
        int menuItemWidthMain;
        int menuItemWidthSchedule;
        int menuItemWidthChannel;
        int menuItemWidthTimer;
        int menuItemWidthRecording;
        int menuItemWidthSetup;
        int menuItemWidthTracks;
        int menuItemHeightMain;
        int menuItemHeightSchedule;
        int menuItemHeightDefault;
        int menuItemHeightRecordings;
        int menuItemHeightTracks;
        int menuMainMenuIconSize;
        int menuLogoWidth;
        int menuLogoHeight;
        int menuTimersLogoWidth;
        int menuTimersLogoHeight;
        int menuHeaderVDRLogoWidth;
        int menuButtonsBorder;
        int menuButtonWidth;
        int menuButtonHeight;
        int menuDiskUsageWidth;
        int menuDiskUsageHeight;
        int menuTimersWidth;
        //DisplayChannel Sizes
        int channelOsdLeft, channelOsdTop;
        int channelOsdWidth, channelOsdHeight;
        int channelX, channelTop;
        int channelWidth, channelHeight;
        int channelHeaderHeight;
        int channelFooterHeight;
        int channelContentHeight;
        int channelContentX, channelContentWidth;
        int channelLogoWidthTotal;
        int channelLogoX, channelLogoY;
        int channelLogoWidth, channelLogoHeight;
        int channelLogoBgX, channelLogoBgY;
        int channelLogoBgWidth, channelLogoBgHeight;
        int channelChannelNameWidth;
        int channelDateWidth;
        int channelFooterY;
        int channelProgressBarHeight;
        int channelEpgInfoHeight;
        int channelEpgInfoLineHeight;
        int channelStatusIconBorder;
        int channelStatusIconSize;
        int channelStatusIconsWidth;
        int channelStatusIconX;
        int channelSourceInfoX;
        int channelVolumeLeft, channelVolumeTop;
        int channelVolumeWidth, channelVolumeHeight;
        //DisplayReplay Sizes
        int replayOsdLeft, replayOsdTop;
        int replayOsdWidth, replayOsdHeight;
        int replayWidth;
        int replayHeight;
        int replayHeaderHeight;
        int replayInfo2Height;
        int replayProgressBarHeight;
        int replayCurrentHeight;
        int replayControlsHeight;
        int replayFooterHeight;
        int replayInfoWidth;
        int replayDateWidth;
        int replayIconSize, replayIconBorder;
        int replayResolutionSize;
        int replayResolutionX, replayResolutionY;
        int replayJumpX, replayJumpY;
        int replayJumpWidth, replayJumpHeight;
        int replayMessageY;
        int replayMessageWidth, replayMessageHeight;
        int replayVolumeLeft, replayVolumeTop;
        int replayVolumeWidth, replayVolumeHeight;
        //DisplayMessage Sizes
        int messageWidth;
        int messageHeight;
        //DisplayTracks Sizes
        int trackWidth;
        //DisplayVolume Sizes
        int volumeWidth;
        int volumeHeight;
        int volumeLabelHeight;
        int volumeProgressBarWidth;
        int volumeProgressBarHeight;
};

extern cGeometryManager *geoManager;

#endif //__NOPACITY_GEOMETRYMANAGER_H
