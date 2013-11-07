#ifndef __NOPACITY_CONFIG_H
#define __NOPACITY_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <vdr/tools.h>
#include <vdr/skins.h>
#include <vdr/plugin.h>
#include "fontmanager.h"
#include "geometrymanager.h"

enum eDisplayType {
    dtFlat = 0,
    dtBlending,
    dtGraphical
};

class cNopacityConfig {
    private:
        std::map<std::string, int> conf;
        std::map<std::string, std::map<std::string, int> > themeConfigDefaults;
        std::map<std::string, std::map<std::string, int> > themeConfigSetup;
        void LoadThemeConfig(cString confFile, cString theme);
        std::pair<std::string, int> ReadThemeConfigLine(const char *line);
        cString checkSlashAtEnd(std::string path);
    public:
        cNopacityConfig();
        ~cNopacityConfig();
        int GetValue(std::string name);
        int *GetValueRef(std::string name);
        std::map<std::string, int>::const_iterator GetStart(void) { return conf.begin(); };
        std::map<std::string, int>::const_iterator GetEnd(void) { return conf.end(); };
        bool SetupParse(const char *Name, const char *Value);
        void SetThemeConfigSetupValue(std::string themeName, std::string key, int value);
        void SetLogoPath(cString path);
        void SetIconPath(cString path);
        void SetEpgImagePath(cString path);
        bool logoPathSet;
        bool epgImagePathSet;
        bool iconPathSet;
        cString logoPathDefault;
        cString iconPathDefault;
        cString epgImagePathDefault;
        void LoadDefaults(void);
        void LoadThemeSpecificConfigs(void);
        void SetThemeSpecificDefaults(void);
        void SetThemeSetup(void);
        void SetPathes(void);
        void DumpConfig(void);
        void DumpThemeConfig(void);
        void SetFontName();
        const char *fontDefaultName;
        char *fontName;
        cString logoPath;
        cString logoExtension;
        cString iconPath;
        cString epgImagePath;
        bool mainMenuEntry;
};
#ifdef DEFINE_CONFIG
   bool firstDisplay = true;
   cNopacityConfig config;
   cGeometryManager *geoManager;
   cFontManager *fontManager;
   cTheme Theme;
#else
   extern bool firstDisplay;
   extern cNopacityConfig config;
   extern cGeometryManager *geoManager;
   extern cFontManager *fontManager;
   extern cTheme Theme;
#endif

//COMMON
#define CLR_TRANSPARENT         0x00000000
#define CLR_BACKGROUND_BLUE     0xFF242A38
#define CLR_TRANSBLACK          0xDD000000
#define CLR_TRANSBLACK2         0xB0000000
#define CLR_BLACK               0xFF000000
#define CLR_DARKBLUE            0xDD003DF5
#define CLR_DARKBLUE2           0xB0003DF5
#define CLR_WHITE               0xFFFFFFFF
#define CLR_BRIGHTBLUE          0xFF0066FF
#define CLR_GRAY                0xFF858585

//CHANNELS
#define CLR_CHANNELPROGRESSBAR  0xDD003DF5
#define CLR_CHANNELPROGRESSBARBACK 0xDD858585
#define CLR_CHANNELPROGRESSBARBLEND 0xDD80B3FF
#define CLR_CHANNELSYMBOLOFF    0xDD858585
#define CLR_CHANNELRECACTIVE    0xDDFF0000
#define CLR_RECNEXT             0xDDFFFF00

//REPLAY
#define CLR_REPLAYCURRENTTOTAL  0xFF003DF5
#define CLR_REST                0xDD858585
#define CLR_EXPOSED             0xFF000000
#define CLR_CURRENT             0x90FFFFFF
#define CLR_MARKS               0xFF000000

//MENU
#define CLR_MENUSCROLLBARBACK   0x40003DF5
#define CLR_MENUSCROLLBARBASE   0x00000000
#define CLR_MENUITEM            0xEE444444
#define CLR_MENUITEMBLEND       0x90000000
#define CLR_MENUITEMHIGHBLEND   0xEE0033FF
#define CLR_SEPARATORBORDER     0xEE444444
#define CLR_DISKALERT           0xDDFF0000
#define CLR_MENUHEADER          0xDD000000
#define CLR_MENUHEADERBLEND     0xEE0033FF
#define CLR_AUDIOMENUHEADER     0xDD000000
#define CLR_PROGRESSBAR         0xDD003DF5
#define CLR_PROGRESSBARBACK     0xDD858585
#define CLR_PROGRESSBARBLEND    0xDD80B3FF
#define CLR_PROGRESSBARHIGH     0xDD003DF5
#define CLR_PROGRESSBARBACKHIGH 0xDD858585
#define CLR_PROGRESSBARBLENDHIGH 0xDD80B3FF
#define CLR_CHANNELLOGOBACK     0xDD858585

//BUTTONS
#define CLR_BUTTONRED           0x99BB0000
#define CLR_BUTTONREDBORDER     0xFFBB0000
#define CLR_BUTTONGREEN         0x9900BB00
#define CLR_BUTTONGREENBORDER   0xFF00BB00
#define CLR_BUTTONYELLOW        0x99BBBB00
#define CLR_BUTTONYELLOWBORDER  0xFFBBBB00
#define CLR_BUTTONBLUE          0x990000BB
#define CLR_BUTTONBLUEBORDER    0xFF0000BB

//MESSAGES
#define CLR_MESSAGESTATUS       0x900000FF
#define CLR_MESSAGEINFO         0x90009900
#define CLR_MESSAGEWARNING      0x90BBBB00
#define CLR_MESSAGEERROR        0x90BB0000

//CHANNELS
THEME_CLR(Theme, clrChannelBackground,      CLR_TRANSBLACK2);
THEME_CLR(Theme, clrChannelBackBlend,       CLR_DARKBLUE2);
THEME_CLR(Theme, clrChannelHead,            CLR_BRIGHTBLUE);
THEME_CLR(Theme, clrChannelEPG,             CLR_WHITE);
THEME_CLR(Theme, clrChannelEPGInfo,         CLR_WHITE);
THEME_CLR(Theme, clrChannelEPGNext,         CLR_GRAY);
THEME_CLR(Theme, clrChannelEPGInfoNext,     CLR_GRAY);
THEME_CLR(Theme, clrChannelProgressBar,     CLR_CHANNELPROGRESSBAR);
THEME_CLR(Theme, clrChannelProgressBarBack, CLR_CHANNELPROGRESSBARBACK);
THEME_CLR(Theme, clrChannelProgressBarBlend,CLR_CHANNELPROGRESSBARBLEND);
THEME_CLR(Theme, clrStatusIconsBack,        CLR_TRANSPARENT);
THEME_CLR(Theme, clrRecNow,                 CLR_CHANNELRECACTIVE);
THEME_CLR(Theme, clrRecNowFont,             CLR_WHITE);
THEME_CLR(Theme, clrRecNext,                CLR_RECNEXT);
THEME_CLR(Theme, clrRecNextFont,            CLR_BLACK);
//REPLAY
THEME_CLR(Theme, clrReplayBackground,       CLR_TRANSBLACK2);
THEME_CLR(Theme, clrReplayBackBlend,        CLR_DARKBLUE2);
THEME_CLR(Theme, clrReplayHead,             CLR_WHITE);
THEME_CLR(Theme, clrReplayDescription,      CLR_GRAY);
THEME_CLR(Theme, clrReplayCurrentTotal,     CLR_REPLAYCURRENTTOTAL);
THEME_CLR(Theme, clrReplayProgressSeen,     CLR_DARKBLUE);
THEME_CLR(Theme, clrReplayProgressRest,     CLR_REST);
THEME_CLR(Theme, clrReplayProgressSelected, CLR_EXPOSED);
THEME_CLR(Theme, clrReplayProgressMark,     CLR_MARKS);
THEME_CLR(Theme, clrReplayProgressCurrent,  CLR_CURRENT);
THEME_CLR(Theme, clrReplayHighlightIcon,    CLR_DARKBLUE);
//TRACKS
THEME_CLR(Theme, clrTracksFontHead,         CLR_GRAY);
THEME_CLR(Theme, clrTracksFontButtons,      CLR_WHITE);
//Volume
THEME_CLR(Theme, clrVolumeFont,             CLR_GRAY);
//MENU
THEME_CLR(Theme, clrMenuBack,               CLR_TRANSBLACK);
THEME_CLR(Theme, clrMenuBorder,             CLR_DARKBLUE);
THEME_CLR(Theme, clrMenuScrollBar,          CLR_DARKBLUE);
THEME_CLR(Theme, clrMenuScrollBarBack,      CLR_MENUSCROLLBARBACK);
THEME_CLR(Theme, clrMenuScrollBarBase,      CLR_MENUSCROLLBARBASE);
THEME_CLR(Theme, clrMenuItem,               CLR_MENUITEM);
THEME_CLR(Theme, clrMenuItemBlend,          CLR_MENUITEMBLEND);
THEME_CLR(Theme, clrMenuItemHigh,           CLR_TRANSBLACK);
THEME_CLR(Theme, clrMenuItemHighBlend,      CLR_MENUITEMHIGHBLEND);
THEME_CLR(Theme, clrSeparatorBorder,        CLR_SEPARATORBORDER);
THEME_CLR(Theme, clrDiskAlert,              CLR_DISKALERT);
THEME_CLR(Theme, clrTimersBack,             CLR_MENUITEMHIGHBLEND);
THEME_CLR(Theme, clrTimersBackBlend,        CLR_TRANSBLACK);
THEME_CLR(Theme, clrMenuFontHeader,         CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDate,           CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDiscUsage,      CLR_WHITE);
THEME_CLR(Theme, clrMenuFontTimers,         CLR_WHITE);
THEME_CLR(Theme, clrMenuFontTimersHeader,   CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDetailViewText, CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDetailViewHeader, CLR_GRAY);
THEME_CLR(Theme, clrMenuFontDetailViewHeaderTitle, CLR_BRIGHTBLUE);
THEME_CLR(Theme, clrMenuFontMenuItem,       CLR_WHITE);
THEME_CLR(Theme, clrMenuFontMenuItemHigh,   CLR_BRIGHTBLUE);
THEME_CLR(Theme, clrMenuFontMenuItemTitle,  CLR_BRIGHTBLUE);
THEME_CLR(Theme, clrMenuFontMenuItemSep,    CLR_GRAY);
THEME_CLR(Theme, clrMenuHeader,             CLR_MENUHEADER);
THEME_CLR(Theme, clrMenuHeaderBlend,        CLR_MENUHEADERBLEND);
THEME_CLR(Theme, clrAudioMenuHeader,        CLR_AUDIOMENUHEADER);
THEME_CLR(Theme, clrProgressBar,            CLR_PROGRESSBAR);
THEME_CLR(Theme, clrProgressBarBack,        CLR_PROGRESSBARBACK);
THEME_CLR(Theme, clrProgressBarBlend,       CLR_PROGRESSBARBLEND);
THEME_CLR(Theme, clrProgressBarHigh,        CLR_PROGRESSBARHIGH);
THEME_CLR(Theme, clrProgressBarBackHigh,    CLR_PROGRESSBARBACKHIGH);
THEME_CLR(Theme, clrProgressBarBlendHigh,   CLR_PROGRESSBARBLENDHIGH);
THEME_CLR(Theme, clrMenuTextWindow,         CLR_TRANSBLACK);
THEME_CLR(Theme, clrMenuChannelLogoBack,    CLR_CHANNELLOGOBACK);
//BUTTONS
THEME_CLR(Theme, clrButtonRed,              CLR_BUTTONRED);
THEME_CLR(Theme, clrButtonRedBorder,        CLR_BUTTONREDBORDER);
THEME_CLR(Theme, clrButtonRedFont,          CLR_WHITE);
THEME_CLR(Theme, clrButtonGreen,            CLR_BUTTONGREEN);
THEME_CLR(Theme, clrButtonGreenBorder,      CLR_BUTTONGREENBORDER);
THEME_CLR(Theme, clrButtonGreenFont,        CLR_WHITE);
THEME_CLR(Theme, clrButtonYellow,           CLR_BUTTONYELLOW);
THEME_CLR(Theme, clrButtonYellowBorder,     CLR_BUTTONYELLOWBORDER);
THEME_CLR(Theme, clrButtonYellowFont,       CLR_WHITE);
THEME_CLR(Theme, clrButtonBlue,             CLR_BUTTONBLUE);
THEME_CLR(Theme, clrButtonBlueBorder,       CLR_BUTTONBLUEBORDER);
THEME_CLR(Theme, clrButtonBlueFont,         CLR_WHITE);
//MESSAGES
THEME_CLR(Theme, clrMessageFontStatus,      CLR_WHITE);
THEME_CLR(Theme, clrMessageFontInfo,        CLR_WHITE);
THEME_CLR(Theme, clrMessageFontWarning,     CLR_WHITE);
THEME_CLR(Theme, clrMessageFontError,       CLR_WHITE);
THEME_CLR(Theme, clrMessageStatus,          CLR_MESSAGESTATUS);
THEME_CLR(Theme, clrMessageInfo,            CLR_MESSAGEINFO);
THEME_CLR(Theme, clrMessageWarning,         CLR_MESSAGEWARNING);
THEME_CLR(Theme, clrMessageError,           CLR_MESSAGEERROR);
THEME_CLR(Theme, clrMessageBlend,           CLR_TRANSBLACK);

#endif //__NOPACITY_CONFIG_H
