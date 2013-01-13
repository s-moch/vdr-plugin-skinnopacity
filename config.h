#ifndef __NOPACITY_CONFIG_H
#define __NOPACITY_CONFIG_H

class cNopacityConfig {
    private:
    public:
        cNopacityConfig();
        ~cNopacityConfig();
        bool SetupParse(const char *Name, const char *Value);
        void SetLogoPath(cString path);
        void SetIconPath(cString path);
        void SetEpgImagePath(cString path);
        bool logoPathSet;
        bool epgImagePathSet;
        bool iconPathSet;
        cString logoPathDefault;
        cString iconPathDefault;
        cString epgImagePathDefault;
        void setDynamicValues();
        //Common
        int fontIndex;
        const char *fontDefaultName;
        char *fontName;
        cString logoPath;
        cString logoExtension;
        cString iconPath;
        cString epgImagePath;
        bool mainMenuEntry;
        //DisplayChannel
        int channelHeight;
        int channelBorderVertical;
        int channelBorderBottom;
        int channelFadeTime;
        int channelFrameTime;
        int logoWidth;
        int logoHeight;
        int logoBorder;
        int displaySignalStrength;
        int fontChannelHeaderSize;
        int fontChannelDateSize;
        int fontEPGSize;
        int fontEPGSmallSize;
        int resolutionIconSize;
        int statusIconSize;
        //DisplayReplay
        int replayHeight;
        int replayBorderVertical;
        int replayBorderBottom;
        int replayFadeTime;
        int replayFrameTime;
        int fontReplayHeader;
        int fontReplay;
        //Display Message
        int messageWidth;
        int messageHeight;
        int messageBorderBottom;
        int fontMessage;
        int messageFadeTime;
        int messageFrameTime;
        //DisplayTracks
        int tracksFadeTime;
        int tracksFrameTime;
        int tracksWidth;
        int tracksHeight;
        int tracksPosition;
        int tracksBorderHorizontal;
        int tracksBorderVertical;
        int fontTracksHeader;
        int fontTracks;
        //DisplayVolume
        int volumeFadeTime;
        int volumeFrameTime;
        int volumeWidth;
        int volumeHeight;
        int volumeBorderBottom;
        int fontVolume;
        //DisplayMenu
        int scalePicture;
        int displayRerunsDetailEPGView;
        int numReruns;
        int useSubtitleRerun;
        int menuFadeTime;
        int menuEPGWindowFadeTime;
        int menuFrameTime;
        int menuEPGWindowFrameTime;
        int menuScrollDelay;
        int menuScrollSpeed;
        int menuScrollFrameTime;
        int menuInfoTextDelay;
        int menuInfoScrollDelay;
        int menuInfoScrollSpeed;
        int menuInfoScrollFrameTime;
        int menuWidthNarrow;
        int menuHeightInfoWindow;
        int menuWidthRightItems;
        int showDiscUsage;
        int showTimers; 
        int headerHeight;
        int footerHeight;
        int numDefaultMenuItems;
        int iconHeight;
        int headerIconHeight;
        int menuItemLogoWidth;
        int menuItemLogoHeight;
        int menuHeaderLogoWidth;
        int menuHeaderLogoHeight;
        int detailViewLogoWidth;
        int detailViewLogoHeight;
        int timersLogoWidth;
        int timersLogoHeight;
        int epgImageWidth;
        int epgImageHeight;
        int menuRecFolderSize;
        int fontHeader;
        int fontDate;
        int fontMenuitemLarge;
        int fontMenuitemSchedule;
        int fontMenuitemScheduleSmall;
        int fontMenuitemChannel;
        int fontMenuitemChannelSmall;
        int fontMenuitemRecordings;
        int fontMenuitemRecordingsSmall;
        int fontMenuitemDefault;
        int fontDiskUsage;
        int fontTimersHead;
        int fontTimers;
        int fontButtons;
        int fontMessageMenu;
        int fontDetailView;
        int fontDetailViewHeader;
        int fontDetailViewHeaderLarge;
        int fontEPGInfoWindow;
};

#endif //__NOPACITY_CONFIG_H