#include <vdr/osd.h>
#include <vdr/menu.h>

static cTheme Theme;

//COMMON
#define CLR_TRANSBLACK          0xDD000000
#define CLR_TRANSBLACK2         0xB0000000
#define CLR_DARKBLUE            0xDD003DF5
#define CLR_DARKBLUE2           0xB0003DF5
#define CLR_WHITE               0xFFFFFFFF
#define CLR_BRIGHTBLUE          0xFF0066FF
#define CLR_GRAY                0xFF858585

//CHANNELS
#define CLR_PROGRESSBARBACK     0xDD858585
#define CLR_PROGRESSBARBLEND    0xDD80B3FF
#define CLR_CHANNELSYMBOLOFF    0xDD858585
#define CLR_CHANNELRECACTIVE    0xDDFF0000

//REPLAY
#define CLR_REPLAYCURRENTTOTAL  0xFF003DF5
#define CLR_REST                0xDD858585
#define CLR_EXPOSED             0xFF000000
#define CLR_CURRENT             0x90FFFFFF
#define CLR_MARKS               0xFF000000

//MENU
#define CLR_MENUSCROLLBARBACK   0x40003DF5
#define CLR_MENUITEM            0xEE444444
#define CLR_MENUITEMBLEND       0x90000000
#define CLR_MENUITEMHIGHBLEND   0xEE0033FF
#define CLR_DISKALERT           0xDDFF0000

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
THEME_CLR(Theme, clrChannelEPGInfo,         CLR_GRAY);
THEME_CLR(Theme, clrProgressBar,            CLR_DARKBLUE);
THEME_CLR(Theme, clrProgressBarBack,        CLR_PROGRESSBARBACK);
THEME_CLR(Theme, clrProgressBarBlend,       CLR_PROGRESSBARBLEND);
THEME_CLR(Theme, clrChannelSymbolOn,        CLR_DARKBLUE);
THEME_CLR(Theme, clrChannelSymbolOff,       CLR_CHANNELSYMBOLOFF);
THEME_CLR(Theme, clrChannelRecActive,       CLR_CHANNELRECACTIVE);
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
THEME_CLR(Theme, clrMenuItem,               CLR_MENUITEM);
THEME_CLR(Theme, clrMenuItemBlend,          CLR_MENUITEMBLEND);
THEME_CLR(Theme, clrMenuItemHigh,           CLR_TRANSBLACK);
THEME_CLR(Theme, clrMenuItemHighBlend,      CLR_MENUITEMHIGHBLEND);
THEME_CLR(Theme, clrDiskAlert,              CLR_DISKALERT);
THEME_CLR(Theme, clrMenuFontHeader,         CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDate,           CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDiscUsage,      CLR_WHITE);
THEME_CLR(Theme, clrMenuFontButton,         CLR_WHITE);
THEME_CLR(Theme, clrMenuFontTimers,         CLR_WHITE);
THEME_CLR(Theme, clrMenuFontTimersHeader,   CLR_WHITE);
THEME_CLR(Theme, clrMenuFontMessages,       CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDetailViewText, CLR_WHITE);
THEME_CLR(Theme, clrMenuFontDetailViewHeader, CLR_GRAY);
THEME_CLR(Theme, clrMenuFontDetailViewHeaderTitle, CLR_BRIGHTBLUE);
THEME_CLR(Theme, clrMenuFontMenuItem,       CLR_WHITE);
THEME_CLR(Theme, clrMenuFontMenuItemHigh,   CLR_BRIGHTBLUE);
THEME_CLR(Theme, clrMenuFontMenuItemTitle,  CLR_BRIGHTBLUE);
THEME_CLR(Theme, clrMenuFontMenuItemSep,    CLR_GRAY);
//BUTTONS
THEME_CLR(Theme, clrButtonRed,              CLR_BUTTONRED);
THEME_CLR(Theme, clrButtonRedBorder,        CLR_BUTTONREDBORDER);
THEME_CLR(Theme, clrButtonGreen,            CLR_BUTTONGREEN);
THEME_CLR(Theme, clrButtonGreenBorder,      CLR_BUTTONGREENBORDER);
THEME_CLR(Theme, clrButtonYellow,           CLR_BUTTONYELLOW);
THEME_CLR(Theme, clrButtonYellowBorder,     CLR_BUTTONYELLOWBORDER);
THEME_CLR(Theme, clrButtonBlue,             CLR_BUTTONBLUE);
THEME_CLR(Theme, clrButtonBlueBorder,       CLR_BUTTONBLUEBORDER);
//MESSAGES
THEME_CLR(Theme, clrMessageFont,            CLR_WHITE);
THEME_CLR(Theme, clrMessageStatus,          CLR_MESSAGESTATUS);
THEME_CLR(Theme, clrMessageInfo,            CLR_MESSAGEINFO);
THEME_CLR(Theme, clrMessageWarning,         CLR_MESSAGEWARNING);
THEME_CLR(Theme, clrMessageError,           CLR_MESSAGEERROR);
THEME_CLR(Theme, clrMessageBlend,           CLR_TRANSBLACK);

#include "epgsearchconf.c"
#include "config.c"
cNopacityConfig config;
#include "setup.c"
#include "imageloader.c"
#include "nopacity.h"
#include "helpers.c"
#include "displaychannel.c"
#include "menuitem.c"
#include "menudetailview.c"
#include "displaymenuview.c"
#include "displaymenu.c"
#include "displayreplay.c"
#include "displayvolume.c"
#include "displaytracks.c"
#include "displaymessage.c"

cNopacity::cNopacity(void) : cSkin("nOpacity", &::Theme) {
    config.setDynamicValues();
    config.loadEPGSearchSettings();
}

const char *cNopacity::Description(void) {
  return "nOpacity";
}

cSkinDisplayChannel *cNopacity::DisplayChannel(bool WithInfo) {
  return new cNopacityDisplayChannel(WithInfo);
}

cSkinDisplayMenu *cNopacity::DisplayMenu(void) {
  return new cNopacityDisplayMenu;
}

cSkinDisplayReplay *cNopacity::DisplayReplay(bool ModeOnly) {
  return new cNopacityDisplayReplay(ModeOnly);
}

cSkinDisplayVolume *cNopacity::DisplayVolume(void) {
  return new cNopacityDisplayVolume;
}

cSkinDisplayTracks *cNopacity::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) {
  return new cNopacityDisplayTracks(Title, NumTracks, Tracks);
}

cSkinDisplayMessage *cNopacity::DisplayMessage(void) {
  return new cNopacityDisplayMessage;
}

