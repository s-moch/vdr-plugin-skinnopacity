#include "fontmanager.h"

cFontManager::cFontManager() {
}

cFontManager::~cFontManager() {
    DeleteFonts();
}

void cFontManager::SetFonts() {
    SetFontsMenu();
    SetFontsChannel();
    SetFontsReplay();
    SetFontsMessage();
    SetFontsTrack();
    SetFontsVolume();
}

void cFontManager::DeleteFonts() {
    DeleteFontsMenu();
    DeleteFontsChannel();
    DeleteFontsReplay();
    DeleteFontsMessage();
    DeleteFontsTrack();
    DeleteFontsVolume();
}

cFont *cFontManager::CreateFont(int size) {
    return cFont::CreateFont(config.fontName, size);
}

void cFontManager::SetFontsMenu(void) {
    menuHeader = CreateFont(geoManager->menuHeaderHeight / 2 + config.fontHeader);
    menuDate = CreateFont(geoManager->menuHeaderHeight / 2 + config.fontDate);
    menuItemLarge = CreateFont(geoManager->menuItemHeightMain/3 + 4 + config.fontMenuitemLarge);
    menuItemSchedule = CreateFont(geoManager->menuItemHeightSchedule / 4 + 5 + config.fontMenuitemSchedule);
    menuItemScheduleSmall = CreateFont(geoManager->menuItemHeightSchedule / 4 - 5 + config.fontMenuitemScheduleSmall);
    menuItemChannel = CreateFont(geoManager->menuItemHeightSchedule / 3 + config.fontMenuitemChannel);
    menuItemChannelSmall = CreateFont(geoManager->menuItemHeightSchedule / 5 - 2 + config.fontMenuitemChannelSmall);
    menuItemRecordings = CreateFont(geoManager->menuItemHeightRecordings / 2 - 14 + config.fontMenuitemRecordings);
    menuItemRecordingsSmall = CreateFont(geoManager->menuItemHeightRecordings / 4 - 3 + config.fontMenuitemRecordingsSmall);
    menuItemTimers = CreateFont(geoManager->menuItemHeightSchedule / 3 + config.fontMenuitemTimers);
    menuItemTimersSmall = CreateFont(geoManager->menuItemHeightSchedule / 4 - 3 + config.fontMenuitemTimersSmall);
    menuItemDefault = CreateFont(geoManager->menuItemHeightDefault * 2 / 3 + config.fontMenuitemDefault);
    menuDiskUsage = CreateFont(geoManager->menuDiskUsageHeight/6 - 2 + config.fontDiskUsage);
    menuDiskUsagePercent = CreateFont(geoManager->menuDiskUsageHeight/5 - 4 + config.fontDiskUsagePercent);
    menuTimersHead = CreateFont((geoManager->menuContentHeight - 3*geoManager->menuSpace - geoManager->menuDiskUsageHeight) / 25 + config.fontTimersHead);
    menuTimers = CreateFont((geoManager->menuContentHeight - 3*geoManager->menuSpace - geoManager->menuDiskUsageHeight) / 25 - 6 + config.fontTimers);
    menuButtons = CreateFont(geoManager->menuButtonHeight*0.8 + config.fontButtons);
    menuMessage = CreateFont(geoManager->menuMessageHeight / 3 + config.fontMessageMenu);
    menuEPGInfoWindow = CreateFont(geoManager->menuContentHeight / 30 + config.fontEPGInfoWindow);
    menuEPGInfoWindowLarge = CreateFont(geoManager->menuContentHeight / 20 + config.fontEPGInfoWindowLarge);
    menuRssFeed = CreateFont((geoManager->menuRssFeedHeight / 2) + 3 + config.fontRssFeed);
}

void cFontManager::DeleteFontsMenu(void) {
    delete menuHeader;
    delete menuDate;
    delete menuItemLarge;
    delete menuItemSchedule;
    delete menuItemScheduleSmall;
    delete menuItemChannel;
    delete menuItemChannelSmall;
    delete menuItemRecordings;
    delete menuItemRecordingsSmall;
    delete menuItemTimers;
    delete menuItemTimersSmall;
    delete menuItemDefault;
    delete menuDiskUsage;
    delete menuDiskUsagePercent;
    delete menuTimers;
    delete menuTimersHead;
    delete menuButtons;
    delete menuMessage;
    delete menuEPGInfoWindow;
    delete menuEPGInfoWindowLarge;
    delete menuRssFeed;
}

void cFontManager::SetFontsChannel(void) {
    channelHeader = CreateFont(geoManager->channelInfoHeight - 8 + config.fontChannelHeaderSize);
    channelDate = CreateFont(geoManager->channelInfoHeight/2 + config.fontChannelDateSize);
    channelEPG = CreateFont(geoManager->channelEpgInfoLineHeight + config.fontEPGSize);
    channelEPGSmall = CreateFont(geoManager->channelEpgInfoLineHeight - 6 + config.fontEPGSmallSize);
    channelChannelGroup = CreateFont(geoManager->channelEpgInfoHeight/3 + config.fontChannelGroupSize);
    channelChannelGroupSmall = CreateFont(geoManager->channelEpgInfoHeight/3 - 5 + config.fontChannelGroupSmallSize);
}

void cFontManager::DeleteFontsChannel(void) {
    delete channelHeader;
    delete channelDate;
    delete channelEPG;
    delete channelEPGSmall;
    delete channelChannelGroup;
    delete channelChannelGroupSmall;
}

void cFontManager::SetFontsReplay(void) {
    replayHeader = CreateFont(geoManager->replayHeaderHeight - 8 + config.fontReplayHeader);
    replayText = CreateFont(geoManager->replayCurrentHeight);
}

void cFontManager::DeleteFontsReplay(void) {
    delete replayHeader;
    delete replayText;
}

void cFontManager::SetFontsMessage(void) {
    messageText = CreateFont(geoManager->messageHeight / 4 + 15 + config.fontMessage);
}

void cFontManager::DeleteFontsMessage(void) {
    delete messageText;
}

void cFontManager::SetFontsTrack(void) {
    trackText = CreateFont(geoManager->menuItemHeightTracks/3 + config.fontTracks);
    trackHeader = CreateFont(geoManager->menuItemHeightTracks/2 + config.fontTracksHeader);
}

void cFontManager::DeleteFontsTrack(void) {
    delete trackText;
    delete trackHeader;
}

void cFontManager::SetFontsVolume(void) {
    volumeText = CreateFont(geoManager->volumeLabelHeight - 6 + config.fontVolume);
}

void cFontManager::DeleteFontsVolume(void) {
    delete volumeText;
}