#ifndef __NOPACITY_GEOMETRYMANAGER_H
#define __NOPACITY_GEOMETRYMANAGER_H

enum eLogoPosition {lpNone = 0, lpLeft, lpRight};

class cGeometryManager {
    private:
        void SetOSDSize(void);
        void SetDisplayMenuSizes(void);
        void SetDisplayChannelSizes(void);
        void SetDisplayReplaySizes(void);
        void SetDisplayMessageSizes(void);
        void SetDisplayTrackSizes(void);
        void SetDisplayVolumeSizes(void);
    public:
        cGeometryManager();
        ~cGeometryManager();
        void SetGeometry(void);
        bool GeometryChanged(void);
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
        int menuRssFeedHeight;
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
        int menuButtonsBorder;
        int menuButtonWidth;
        int menuButtonHeight;
        int menuDiskUsageWidth;
        int menuDiskUsageHeight;
        int menuTimersWidth;
        int menuMessageWidth;
        int menuMessageHeight;
        //DisplayChannel Sizes
        int channelTop, channelHeight;
        int channelWidth;
        int channelX;
        int channelInfoWidth;
        int channelInfoHeight;
        int channelDateWidth;
        int channelProgressBarHeight;
        int channelEpgInfoHeight;
        int channelEpgInfoLineHeight;
        int channelStreamInfoHeight;
        int channelStreamInfoY;
        int channelIconSize;
        int channelIconsWidth;
        int channelSignalWidth, channelSignalHeight, channelSignalX;
        //DisplayReplay Sizes
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
        int replayResolutionX, replayResolutionY; 
        int replayJumpX, replayJumpY; 
        int replayJumpWidth, replayJumpHeight;
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

#endif //__NOPACITY_GEOMETRYMANAGER_H