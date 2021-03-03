#ifndef __NOPACITY_FONTMANAGER_H
#define __NOPACITY_FONTMANAGER_H

#include <vdr/skins.h>

class cFontManager {
    private:
        void SetFontsMenu(void);
        void DeleteFontsMenu(void);
        void SetFontsChannel(void);
        void DeleteFontsChannel(void);
        void SetFontsReplay(void);
        void DeleteFontsReplay(void);
        void SetFontsMessage(void);
        void DeleteFontsMessage(void);
        void SetFontsTrack(void);
        void DeleteFontsTrack(void);
        void SetFontsVolume(void);
        void DeleteFontsVolume(void);
    public:
        cFontManager();
        ~cFontManager();
        cFont *CreateFont(int size);
        void SetFonts(void);
        void DeleteFonts(void);
        //Fonts DisplayMenu
        cFont *menuHeader = NULL;
        cFont *menuDate = NULL;
        cFont *menuItemLarge = NULL;
        cFont *menuItemSchedule = NULL;
        cFont *menuItemScheduleSmall = NULL;
        cFont *menuItemChannel = NULL;
        cFont *menuItemChannelSmall = NULL;
        cFont *menuItemRecordings = NULL;
        cFont *menuItemRecordingsSmall = NULL;
        cFont *menuItemTimers = NULL;
        cFont *menuItemTimersSmall = NULL;
        cFont *menuItemDefault = NULL;
        cFont *menuDiskUsage = NULL;
        cFont *menuDiskUsagePercent = NULL;
        cFont *menuTimers = NULL;
        cFont *menuTimersHead = NULL;
        cFont *menuButtons = NULL;
        cFont *menuMessage = NULL;
        cFont *menuEPGInfoWindow = NULL;
        cFont *menuEPGInfoWindowLarge = NULL;
        //Fonts DisplayChannel
        cFont *channelHeader = NULL;
        cFont *channelDate = NULL;
        cFont *channelEPG = NULL;
        cFont *channelEPGSmall = NULL;
        cFont *channelSourceInfo = NULL;
        cFont *channelChannelGroup = NULL;
        cFont *channelChannelGroupSmall = NULL;
        //Fonts DisplayReplay
        cFont *replayHeader = NULL;
        cFont *replayText = NULL;
        //Fonts DisplayMessage
        cFont *messageText = NULL;
        //Fonts DisplayTracks
        cFont *trackText = NULL;
        cFont *trackHeader = NULL;
        //Fonts DisplayVolume
        cFont *volumeText = NULL;
};

extern cFontManager *fontManager;

#endif //__NOPACITY_FONTMANAGER_H
