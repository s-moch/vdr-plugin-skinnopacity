#ifndef __NOPACITY_CONFIG_H
#define __NOPACITY_CONFIG_H

struct RssFeed {
    std::string name;
    std::string url;
};

class cNopacityConfig {
    private:
        cString checkSlashAtEnd(std::string path);
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
        void loadRssFeeds(void);
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
        int logoPosition;
        int logoWidth;
        int logoHeight;
        int logoBorder;
        int backgroundStyle;
        int displaySignalStrength;
        int fontChannelHeaderSize;
        int fontChannelDateSize;
        int fontEPGSize;
        int fontEPGSmallSize;
        int resolutionIconSize;
        int statusIconSize;
        int progressCurrentSchedule;
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
        int tracksItemHeight;
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
        int menuAdjustLeft;
        int scalePicture;
        int roundedCorners;
        int cornerRadius;
        int useMenuIcons;
        int mainMenuTitleStyle;
        int narrowMainMenu;
        int narrowScheduleMenu;
        int narrowChannelMenu;
        int narrowTimerMenu;
        int narrowRecordingMenu;
        int narrowSetupMenu;
        int displayRerunsDetailEPGView;
        int numReruns;
        int useSubtitleRerun;
        int displayAdditionalEPGPictures;
        int numAdditionalEPGPictures;
        int displayAdditionalRecEPGPictures;
        int numAdditionalRecEPGPictures;
        int numEPGEntriesChannelsMenu;
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
        int menuWidthMain;
        int menuWidthSchedules;
        int menuWidthChannels;
        int menuWidthTimers;
        int menuWidthRecordings;
        int menuWidthSetup;
        int menuHeightInfoWindow;
        int menuWidthRightItems;
        int menuSizeDiskUsage;
        int showDiscUsage;
        int showTimers;
        int numberTimers;
        int checkTimerConflict;
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
        int epgImageWidthLarge;
        int epgImageHeightLarge;
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
        int fontMenuitemTimers;
        int fontMenuitemTimersSmall;
        int fontMenuitemDefault;
        int fontDiskUsage;
        int fontDiskUsagePercent;
        int fontTimersHead;
        int fontTimers;
        int fontButtons;
        int fontMessageMenu;
        int fontDetailView;
        int fontDetailViewHeader;
        int fontDetailViewHeaderLarge;
        int fontEPGInfoWindow;
        //RSS Feeds
        std::vector<RssFeed> rssFeeds;
        int displayRSSFeed;
        int rssFeedHeight;
        int rssFeedHeightStandalone;
        int fontRssFeed;
        int fontRssFeedStandalone;
        int rssFeedStandalonePos;
        int rssScrollDelay;
        int rssScrollSpeed;
        int rssScrollFrameTime;
        int rssFeed[5];
};

#endif //__NOPACITY_CONFIG_H