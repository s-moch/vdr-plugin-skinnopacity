#include "setup.h"

cNopacitySetup::cNopacitySetup() {
    tmpNopacityConfig = config;
    cFont::GetAvailableFontNames(&fontNames);
    fontNames.Insert(strdup(config.fontDefaultName));
    Setup();
}

cNopacitySetup::~cNopacitySetup() {
}


void cNopacitySetup::Setup(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditStraItem(tr("Font"), &tmpNopacityConfig.fontIndex, fontNames.Size(), &fontNames[0]));
    Add(new cOsdItem(tr("VDR Menu: Common Settings")));
    Add(new cOsdItem(tr("VDR Menu: Main and Setup Menu")));
    Add(new cOsdItem(tr("VDR Menu: Schedules Menu")));
    Add(new cOsdItem(tr("VDR Menu: Channels Menu")));
    Add(new cOsdItem(tr("VDR Menu: Timers Menu")));
    Add(new cOsdItem(tr("VDR Menu: Recordings Menu")));
    Add(new cOsdItem(tr("Channel Switching")));
    Add(new cOsdItem(tr("Replay")));
    Add(new cOsdItem(tr("Audio Tracks")));
    Add(new cOsdItem(tr("Messages")));
    Add(new cOsdItem(tr("Volume")));
    Add(new cOsdItem(tr("RSS Feeds")));
    
    SetCurrent(Get(currentItem));
    Display();
}

eOSState cNopacitySetup::ProcessKey(eKeys Key) {
    bool hadSubMenu = HasSubMenu();   
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    if (hadSubMenu && Key == kOk)
        Store();
    if (!hadSubMenu && (state == osUnknown || Key == kOk)) {
        if ((Key == kOk && !hadSubMenu)) {
            const char* ItemText = Get(Current())->Text();
            if (strcmp(ItemText, tr("VDR Menu: Common Settings")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplay(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("VDR Menu: Main and Setup Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayMain(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("VDR Menu: Schedules Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplaySchedules(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("VDR Menu: Channels Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayChannels(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("VDR Menu: Timers Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayTimers(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("VDR Menu: Recordings Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplayRecordings(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("Channel Switching")) == 0)
                state = AddSubMenu(new cNopacitySetupChannelDisplay(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("Replay")) == 0)
                state = AddSubMenu(new cNopacitySetupReplayDisplay(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("Audio Tracks")) == 0)
                state = AddSubMenu(new cNopacitySetupTrackDisplay(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("Messages")) == 0)
                state = AddSubMenu(new cNopacitySetupMessageDisplay(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("Volume")) == 0)
                state = AddSubMenu(new cNopacitySetupVolumeDisplay(&tmpNopacityConfig));
            if (strcmp(ItemText, tr("RSS Feeds")) == 0)
                state = AddSubMenu(new cNopacitySetupRssFeed(&tmpNopacityConfig));
        }
    }   
    return state;
}

void cNopacitySetup::Store(void) {
    config = tmpNopacityConfig;
    SetupStore("fontIndex", config.fontIndex);
    SetupStore("channelHeight", config.channelHeight);
    SetupStore("channelBorderVertical", config.channelBorderVertical);
    SetupStore("channelBorderBottom", config.channelBorderBottom);
    SetupStore("logoPosition", config.logoPosition);
    SetupStore("logoWidth", config.logoWidth);
    SetupStore("logoHeight", config.logoHeight);
    SetupStore("logoBorder", config.logoBorder);
    SetupStore("backgroundStyle", config.backgroundStyle);
    SetupStore("symbolStyle", config.symbolStyle);
    SetupStore("roundedCornersChannel", config.roundedCornersChannel);
    SetupStore("displaySignalStrength", config.displaySignalStrength);
    SetupStore("displaySourceInfo", config.displaySourceInfo);
    SetupStore("displayPrevNextChannelGroup", config.displayPrevNextChannelGroup);
    SetupStore("channelFadeTime", config.channelFadeTime);
    SetupStore("fontChannelHeaderSize", config.fontChannelHeaderSize);
    SetupStore("fontChannelDateSize", config.fontChannelDateSize);
    SetupStore("fontEPGSize", config.fontEPGSize);
    SetupStore("fontEPGSmallSize", config.fontEPGSmallSize);
    SetupStore("fontChannelGroupSize", config.fontChannelGroupSize);
    SetupStore("fontChannelGroupSmallSize", config.fontChannelGroupSmallSize);
    SetupStore("resolutionIconSize", config.resolutionIconSize);
    SetupStore("statusIconSize", config.statusIconSize);
    SetupStore("progressCurrentSchedule", config.progressCurrentSchedule);
    SetupStore("displayPoster", config.displayPoster);
    SetupStore("replayHeight", config.replayHeight);
    SetupStore("replayBorderVertical", config.replayBorderVertical);
    SetupStore("replayBorderBottom", config.replayBorderBottom);
    SetupStore("replayFadeTime", config.replayFadeTime);
    SetupStore("fontReplayHeader", config.fontReplayHeader);
    SetupStore("fontReplay", config.fontReplay);
    SetupStore("messageWidth", config.messageWidth);
    SetupStore("messageHeight", config.messageHeight);
    SetupStore("messageBorderBottom", config.messageBorderBottom);
    SetupStore("fontMessage", config.fontMessage);
    SetupStore("messageFadeTime", config.messageFadeTime);
    SetupStore("tracksFadeTime", config.tracksFadeTime);
    SetupStore("tracksWidth", config.tracksWidth);
    SetupStore("tracksItemHeight", config.tracksItemHeight);
    SetupStore("tracksPosition", config.tracksPosition);
    SetupStore("tracksBorderHorizontal", config.tracksBorderHorizontal);
    SetupStore("tracksBorderVertical", config.tracksBorderVertical);
    SetupStore("fontTracksHeader", config.fontTracksHeader);
    SetupStore("fontTracks", config.fontTracks);
    SetupStore("volumeFadeTime", config.volumeFadeTime);
    SetupStore("volumeWidth", config.volumeWidth);
    SetupStore("volumeHeight", config.volumeHeight);
    SetupStore("volumeBorderBottom", config.volumeBorderBottom);
    SetupStore("fontVolume", config.fontVolume);
    SetupStore("menuAdjustLeft", config.menuAdjustLeft);
    SetupStore("scalePicture", config.scalePicture);
    SetupStore("roundedCorners", config.roundedCorners);
    SetupStore("cornerRadius", config.cornerRadius);
    SetupStore("useMenuIcons", config.useMenuIcons);
    SetupStore("mainMenuTitleStyle", config.mainMenuTitleStyle);
    SetupStore("narrowMainMenu", config.narrowMainMenu);
    SetupStore("narrowScheduleMenu", config.narrowScheduleMenu);
    SetupStore("narrowChannelMenu", config.narrowChannelMenu);
    SetupStore("narrowTimerMenu", config.narrowTimerMenu);
    SetupStore("narrowRecordingMenu", config.narrowRecordingMenu);
    SetupStore("narrowSetupMenu", config.narrowSetupMenu);
    SetupStore("displayRerunsDetailEPGView", config.displayRerunsDetailEPGView);
    SetupStore("numReruns", config.numReruns);
    SetupStore("useSubtitleRerun", config.useSubtitleRerun);
    SetupStore("displayAdditionalEPGPictures", config.displayAdditionalEPGPictures);
    SetupStore("numAdditionalEPGPictures", config.numAdditionalEPGPictures);
    SetupStore("displayAdditionalRecEPGPictures", config.displayAdditionalRecEPGPictures);
    SetupStore("numAdditionalRecEPGPictures", config.numAdditionalRecEPGPictures);
    SetupStore("menuChannelDisplayMode", config.menuChannelDisplayMode);
    SetupStore("menuChannelDisplayTime", config.menuChannelDisplayTime);
    SetupStore("numEPGEntriesChannelsMenu", config.numEPGEntriesChannelsMenu);
    SetupStore("menuFadeTime", config.menuFadeTime);
    SetupStore("menuScrollDelay", config.menuScrollDelay);
    SetupStore("menuScrollSpeed", config.menuScrollSpeed);
    SetupStore("menuWidthMain", config.menuWidthMain);
    SetupStore("menuWidthSchedules", config.menuWidthSchedules);
    SetupStore("menuWidthChannels", config.menuWidthChannels);
    SetupStore("menuWidthTimers", config.menuWidthTimers);
    SetupStore("menuWidthRecordings", config.menuWidthRecordings);
    SetupStore("menuWidthSetup", config.menuWidthSetup);
    SetupStore("menuWidthRightItems", config.menuWidthRightItems);
    SetupStore("menuSizeDiskUsage", config.menuSizeDiskUsage);
    SetupStore("menuHeightInfoWindow", config.menuHeightInfoWindow);
    SetupStore("menuEPGWindowFadeTime", config.menuEPGWindowFadeTime);
    SetupStore("menuInfoTextDelay", config.menuInfoTextDelay);
    SetupStore("menuInfoScrollDelay", config.menuInfoScrollDelay);
    SetupStore("menuInfoScrollSpeed", config.menuInfoScrollSpeed);
    SetupStore("showDiscUsage", config.showDiscUsage);
    SetupStore("discUsageStyle", config.discUsageStyle);
    SetupStore("showTimers", config.showTimers);
    SetupStore("numberTimers", config.numberTimers);
    SetupStore("checkTimerConflict", config.checkTimerConflict);
    SetupStore("headerHeight", config.headerHeight);
    SetupStore("footerHeight", config.footerHeight);
    SetupStore("numDefaultMenuItems", config.numDefaultMenuItems);
    SetupStore("iconHeight", config.iconHeight);
    SetupStore("headerIconHeight", config.headerIconHeight);
    SetupStore("menuItemLogoWidth", config.menuItemLogoWidth);
    SetupStore("menuItemLogoHeight", config.menuItemLogoHeight);
    SetupStore("menuHeaderLogoWidth", config.menuHeaderLogoWidth);
    SetupStore("menuHeaderLogoHeight", config.menuHeaderLogoHeight);
    SetupStore("detailViewLogoWidth", config.detailViewLogoWidth);
    SetupStore("detailViewLogoHeight", config.detailViewLogoHeight);
    SetupStore("timersLogoWidth", config.timersLogoWidth);
    SetupStore("timersLogoHeight", config.timersLogoHeight);
    SetupStore("epgImageWidth", config.epgImageWidth);
    SetupStore("epgImageHeight", config.epgImageHeight);
    SetupStore("epgImageWidthLarge", config.epgImageWidthLarge);
    SetupStore("epgImageHeightLarge", config.epgImageHeightLarge);
    SetupStore("menuRecFolderSize", config.menuRecFolderSize);
    SetupStore("borderDetailedEPG", config.borderDetailedEPG);
    SetupStore("borderDetailedRecordings", config.borderDetailedRecordings);
    SetupStore("menuSchedulesWindowMode", config.menuSchedulesWindowMode);
    SetupStore("menuRecordingsWindowMode", config.menuRecordingsWindowMode);
    SetupStore("fontHeader", config.fontHeader);
    SetupStore("fontDate", config.fontDate);
    SetupStore("fontMenuitemLarge", config.fontMenuitemLarge);
    SetupStore("fontMenuitemSchedule", config.fontMenuitemSchedule);
    SetupStore("fontMenuitemScheduleSmall", config.fontMenuitemScheduleSmall);
    SetupStore("fontMenuitemChannel", config.fontMenuitemChannel);
    SetupStore("fontMenuitemChannelSmall", config.fontMenuitemChannelSmall);
    SetupStore("fontMenuitemRecordings", config.fontMenuitemRecordings);
    SetupStore("fontMenuitemRecordingsSmall", config.fontMenuitemRecordingsSmall);
    SetupStore("fontMenuitemTimers", config.fontMenuitemTimers);
    SetupStore("fontMenuitemTimersSmall", config.fontMenuitemTimersSmall);
    SetupStore("fontMenuitemDefault", config.fontMenuitemDefault);
    SetupStore("fontDiskUsage", config.fontDiskUsage);
    SetupStore("fontDiskUsagePercent", config.fontDiskUsagePercent);
    SetupStore("fontTimersHead", config.fontTimersHead);
    SetupStore("fontTimers", config.fontTimers);
    SetupStore("fontButtons", config.fontButtons);
    SetupStore("fontMessageMenu", config.fontMessageMenu);
    SetupStore("fontDetailView", config.fontDetailView);
    SetupStore("fontDetailViewSmall", config.fontDetailViewSmall);
    SetupStore("fontDetailViewHeader", config.fontDetailViewHeader);
    SetupStore("fontDetailViewHeaderLarge", config.fontDetailViewHeaderLarge);
    SetupStore("fontEPGInfoWindow", config.fontEPGInfoWindow);
    SetupStore("fontEPGInfoWindowLarge", config.fontEPGInfoWindowLarge);
    SetupStore("displayRSSFeed", config.displayRSSFeed);
    SetupStore("rssFeedHeight", config.rssFeedHeight);
    SetupStore("rssFeed[0]", config.rssFeed[0]);
    SetupStore("rssFeed[1]", config.rssFeed[1]);
    SetupStore("rssFeed[2]", config.rssFeed[2]);
    SetupStore("rssFeed[3]", config.rssFeed[3]);
    SetupStore("rssFeed[4]", config.rssFeed[4]);
    SetupStore("fontRssFeed", config.fontRssFeed);
    SetupStore("rssScrollDelay", config.rssScrollDelay);
    SetupStore("rssScrollSpeed", config.rssScrollSpeed);
    SetupStore("rssFeedHeightStandalone", config.rssFeedHeightStandalone);
    SetupStore("fontRssFeedStandalone", config.fontRssFeedStandalone);
    SetupStore("rssFeedStandalonePos", config.rssFeedStandalonePos);
}

//------------------------------------------------------------------------------------------------------------------

cMenuSetupSubMenu::cMenuSetupSubMenu(const char* Title, cNopacityConfig* data) : cOsdMenu(Title, 30) {
    tmpNopacityConfig = data;
    spacer = "   ";
}

cOsdItem *cMenuSetupSubMenu::InfoItem(const char *label, const char *value) {
    cOsdItem *item;
    item = new cOsdItem(cString::sprintf("%s: %s", label, value));
    item->SetSelectable(false);
    return item;
}

eOSState cMenuSetupSubMenu::ProcessKey(eKeys Key) {
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
    switch (Key) {
      case kOk:
        return osBack;
      default:
        break;
    }
  }
  if ((Key == kLeft)||(Key == kRight))
    Set();
  return state;
}

//-----MenuDisplay Common Settings -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplay::cNopacitySetupMenuDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Common Settings"), data) {
    adjustLeft[0] = tr("right");
    adjustLeft[1] = tr("left");
    scrollSpeed[0] = tr("off");
    scrollSpeed[1] = tr("slow");
    scrollSpeed[2] = tr("medium");
    scrollSpeed[3] = tr("fast");
    scalePic[0] = tr("no");
    scalePic[1] = tr("yes");
    scalePic[2] = tr("auto");
    Set();
}

void cNopacitySetupMenuDisplay::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditIntItem(tr("Number of Default Menu Entries per Page"), &tmpNopacityConfig->numDefaultMenuItems, 10, 40));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Default Menu Item"), &tmpNopacityConfig->fontMenuitemDefault, -20, 20));
    Add(new cMenuEditStraItem(tr("Adjustment of narrow menus"), &tmpNopacityConfig->menuAdjustLeft, 2, adjustLeft));
    Add(new cMenuEditStraItem(tr("Scale Video size to fit into menu window"), &tmpNopacityConfig->scalePicture, 3, scalePic));
    Add(new cMenuEditIntItem(tr("Header Height (Percent of OSD Height)"), &tmpNopacityConfig->headerHeight, 5, 30));
    Add(new cMenuEditIntItem(tr("Header Icon Size (Square Header Menu Icons)"), &tmpNopacityConfig->headerIconHeight, 30, 200));
    Add(new cMenuEditIntItem(tr("Footer Height (Percent of OSD Height)"), &tmpNopacityConfig->footerHeight, 5, 30));
    Add(new cMenuEditBoolItem(tr("Rounded Corners for menu items and buttons"), &tmpNopacityConfig->roundedCorners));
    if (tmpNopacityConfig->roundedCorners)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Radius of rounded corners")), &tmpNopacityConfig->cornerRadius, 5, 30));
    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->menuFadeTime, 0, 1000));
    Add(new cMenuEditStraItem(tr("Menu Items Scrolling Speed"), &tmpNopacityConfig->menuScrollSpeed, 4, scrollSpeed));
    Add(new cMenuEditIntItem(tr("Menu Items Scrolling Delay in s"), &tmpNopacityConfig->menuScrollDelay, 0, 3));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), &tmpNopacityConfig->fontHeader, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Date"), &tmpNopacityConfig->fontDate, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Color Buttons"), &tmpNopacityConfig->fontButtons, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Messages"), &tmpNopacityConfig->fontMessageMenu, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Text"), &tmpNopacityConfig->fontDetailView, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Text Small"), &tmpNopacityConfig->fontDetailViewSmall, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Header"), &tmpNopacityConfig->fontDetailViewHeader, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Header Large"), &tmpNopacityConfig->fontDetailViewHeaderLarge, -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Main and Setup Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayMain::cNopacitySetupMenuDisplayMain(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Main and Setup Menu"), data) {
    titleStyle[0] = tr("\"VDR\" plus VDR version");
    titleStyle[1] = tr("only VDR version");
    titleStyle[2] = tr("no title");
    discUsageStyle[0] = tr("free time in hours");
    discUsageStyle[1] = tr("free space in GB");
    Set();
}

void cNopacitySetupMenuDisplayMain::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditBoolItem(tr("Use narrow main menu"), &tmpNopacityConfig->narrowMainMenu));
    if (tmpNopacityConfig->narrowMainMenu)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), &tmpNopacityConfig->menuWidthMain, 10, 97));
    Add(new cMenuEditBoolItem(tr("Use narrow setup menu"), &tmpNopacityConfig->narrowSetupMenu));
    if (tmpNopacityConfig->narrowSetupMenu)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), &tmpNopacityConfig->menuWidthSetup, 10, 97));

    Add(new cMenuEditBoolItem(tr("Use menu icons"), &tmpNopacityConfig->useMenuIcons));
    if (tmpNopacityConfig->useMenuIcons)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Icon Size (Square)")), &tmpNopacityConfig->iconHeight, 30, 200));
    Add(new cMenuEditStraItem(tr("Main menu title style"), &tmpNopacityConfig->mainMenuTitleStyle, 3, titleStyle));
    Add(new cMenuEditBoolItem(tr("Display Disk Usage"), &tmpNopacityConfig->showDiscUsage));
    if (tmpNopacityConfig->showDiscUsage) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Size (square, Percent of OSD Width)")), &tmpNopacityConfig->menuSizeDiskUsage, 5, 30));
        Add(new cMenuEditStraItem(cString::sprintf("%s%s", *spacer, tr("Free Disc Display")), &tmpNopacityConfig->discUsageStyle, 2, discUsageStyle));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - free")), &tmpNopacityConfig->fontDiskUsage, -20, 20));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - percent")), &tmpNopacityConfig->fontDiskUsagePercent, -20, 20));
    }
    Add(new cMenuEditBoolItem(tr("Display Timers"), &tmpNopacityConfig->showTimers));
    if (tmpNopacityConfig->showTimers) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Maximum number of Timers")), &tmpNopacityConfig->numberTimers, 1, 10));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width of Timers (Percent of OSD Width)")), &tmpNopacityConfig->menuWidthRightItems, 5, 30));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Channel Logo Width")), &tmpNopacityConfig->timersLogoWidth, 30, 300));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Channel Logo Height")), &tmpNopacityConfig->timersLogoHeight, 30, 300));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - Header")), &tmpNopacityConfig->fontTimersHead, -20, 20));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size - Title")), &tmpNopacityConfig->fontTimers, -20, 20));
    }
    Add(new cMenuEditBoolItem(tr("Show Timer Conflicts"), &tmpNopacityConfig->checkTimerConflict));
    Add(new cMenuEditIntItem(tr("Header Logo Width"), &tmpNopacityConfig->menuHeaderLogoWidth, 30, 500));
    Add(new cMenuEditIntItem(tr("Header Logo Height"), &tmpNopacityConfig->menuHeaderLogoHeight, 30, 500));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Items"), &tmpNopacityConfig->fontMenuitemLarge, -20, 20));
    
    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Schedules Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplaySchedules::cNopacitySetupMenuDisplaySchedules(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Schedules Menu"), data) {
    useSubtitleRerunTexts[0] = tr("never");
    useSubtitleRerunTexts[1] = tr("if exists");
    useSubtitleRerunTexts[2] = tr("always");
    scrollSpeed[0] = tr("off");
    scrollSpeed[1] = tr("slow");
    scrollSpeed[2] = tr("medium");
    scrollSpeed[3] = tr("fast");
    displayEPGPictures[0] = tr("never");
    displayEPGPictures[1] = tr("always");
    displayEPGPictures[2] = tr("only if no tvscraper media available");
    windowMode[0] = tr("window");
    windowMode[1] = tr("full screen");
    Set();
}

void cNopacitySetupMenuDisplaySchedules::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Use narrow menu"), &tmpNopacityConfig->narrowScheduleMenu));
    if (tmpNopacityConfig->narrowScheduleMenu)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), &tmpNopacityConfig->menuWidthSchedules, 10, 97));
    Add(new cMenuEditIntItem(tr("Channel Logo Width"), &tmpNopacityConfig->menuItemLogoWidth, 30, 200));
    Add(new cMenuEditIntItem(tr("Channel Logo Height"), &tmpNopacityConfig->menuItemLogoHeight, 30, 200));
    Add(new cMenuEditStraItem(tr("Mode of EPG Window"), &tmpNopacityConfig->menuSchedulesWindowMode, 2, windowMode));
    Add(new cMenuEditIntItem(tr("EPG Window Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->menuEPGWindowFadeTime, 0, 1000));
    Add(new cMenuEditIntItem(tr("EPG Window Display Delay in s"), &tmpNopacityConfig->menuInfoTextDelay, 0, 10));
    Add(new cMenuEditIntItem(tr("EPG Window Scroll Delay in s"), &tmpNopacityConfig->menuInfoScrollDelay, 0, 10));
    Add(new cMenuEditStraItem(tr("EPG Window Text Scrolling Speed"), &tmpNopacityConfig->menuInfoScrollSpeed, 4, scrollSpeed));
    Add(new cMenuEditIntItem(tr("Height of EPG Info Window (Percent of OSD Height)"), &tmpNopacityConfig->menuHeightInfoWindow, 10, 100));
    Add(new cMenuEditIntItem(tr("Border around detailed EPG view"), &tmpNopacityConfig->borderDetailedEPG, 1, 300));
    Add(new cMenuEditBoolItem(tr("Display Reruns in detailed EPG View"), &tmpNopacityConfig->displayRerunsDetailEPGView));
    if (tmpNopacityConfig->displayRerunsDetailEPGView) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Number of reruns to display")), &tmpNopacityConfig->numReruns, 1, 10));
        Add(new cMenuEditStraItem(cString::sprintf("%s%s", *spacer, tr("Use Subtitle for reruns")), &tmpNopacityConfig->useSubtitleRerun, 3, useSubtitleRerunTexts));
    }
    Add(new cMenuEditStraItem(tr("Display additional EPG Pictures in detailed EPG View"), &tmpNopacityConfig->displayAdditionalEPGPictures, 3, displayEPGPictures));
    if (tmpNopacityConfig->displayAdditionalEPGPictures)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Number of EPG pictures to display")), &tmpNopacityConfig->numAdditionalEPGPictures, 1, 9));
    Add(new cMenuEditIntItem(tr("Detail EPG View Logo Width"), &tmpNopacityConfig->detailViewLogoWidth, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View Logo Height"), &tmpNopacityConfig->detailViewLogoHeight, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View EPG Image Width"), &tmpNopacityConfig->epgImageWidth, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View EPG Image Height"), &tmpNopacityConfig->epgImageHeight, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View additional EPG Image Width"), &tmpNopacityConfig->epgImageWidthLarge, 100, 800));
    Add(new cMenuEditIntItem(tr("Detail EPG View additional EPG Image Height"), &tmpNopacityConfig->epgImageHeightLarge, 100, 800));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), &tmpNopacityConfig->fontMenuitemSchedule, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), &tmpNopacityConfig->fontMenuitemScheduleSmall, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Info Window"), &tmpNopacityConfig->fontEPGInfoWindow, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Info Window Header"), &tmpNopacityConfig->fontEPGInfoWindowLarge, -20, 20));
    
    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Channels Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayChannels::cNopacitySetupMenuDisplayChannels(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Channels Menu"), data) {
    displayModes[0] = tr("Transponder Information");
    displayModes[1] = tr("Current Schedule");
    displayModes[2] = tr("Plain Channels");
    Set();
}

void cNopacitySetupMenuDisplayChannels::Set(void) {
    int currentItem = Current();
    Clear();
    
    Add(new cMenuEditBoolItem(tr("Use narrow menu"), &tmpNopacityConfig->narrowChannelMenu));
    if (tmpNopacityConfig->narrowChannelMenu)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), &tmpNopacityConfig->menuWidthChannels, 10, 97));
    Add(new cMenuEditStraItem(tr("Menu Items display mode"), &tmpNopacityConfig->menuChannelDisplayMode, 3, displayModes));
    if (tmpNopacityConfig->menuChannelDisplayMode == 1) {
        Add(new cMenuEditBoolItem(cString::sprintf("%s%s", *spacer, tr("Display schedules with time info")), &tmpNopacityConfig->menuChannelDisplayTime));
    }
    Add(new cMenuEditIntItem(tr("Number of EPG Entries in Schedules Info Window"), &tmpNopacityConfig->numEPGEntriesChannelsMenu, 1, 100));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), &tmpNopacityConfig->fontMenuitemChannel, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), &tmpNopacityConfig->fontMenuitemChannelSmall, -20, 20));
   
    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Timers Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayTimers::cNopacitySetupMenuDisplayTimers(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Timers Menu"), data) {
    Set();
}

void cNopacitySetupMenuDisplayTimers::Set(void) {
    int currentItem = Current();
    Clear();
    
    Add(new cMenuEditBoolItem(tr("Use narrow menu"), &tmpNopacityConfig->narrowTimerMenu));
    if (tmpNopacityConfig->narrowTimerMenu)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), &tmpNopacityConfig->menuWidthTimers, 10, 97));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), &tmpNopacityConfig->fontMenuitemTimers, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), &tmpNopacityConfig->fontMenuitemTimersSmall, -20, 20));
   
    SetCurrent(Get(currentItem));
    Display();
}

//-----MenuDisplay Recordings Menu -------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplayRecordings::cNopacitySetupMenuDisplayRecordings(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu: Recordings Menu"), data) {
    displayEPGPictures[0] = tr("never");
    displayEPGPictures[1] = tr("always");
    displayEPGPictures[2] = tr("only if no tvscraper media available");
    windowMode[0] = tr("window");
    windowMode[1] = tr("full screen");
    Set();
}

void cNopacitySetupMenuDisplayRecordings::Set(void) {
    int currentItem = Current();
    Clear();
    
    Add(new cMenuEditBoolItem(tr("Use narrow menu"), &tmpNopacityConfig->narrowRecordingMenu));
    if (tmpNopacityConfig->narrowRecordingMenu)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Width (Percent of OSD Width)")), &tmpNopacityConfig->menuWidthRecordings, 10, 97));
    Add(new cMenuEditStraItem(tr("Mode of recording Window"), &tmpNopacityConfig->menuRecordingsWindowMode, 2, windowMode));
    Add(new cMenuEditIntItem(tr("Border around detailed recording view"), &tmpNopacityConfig->borderDetailedRecordings, 1, 300));
    Add(new cMenuEditStraItem(tr("Display additional EPG Pictures in detailed recording View"), &tmpNopacityConfig->displayAdditionalRecEPGPictures, 3, displayEPGPictures));
    if (tmpNopacityConfig->displayAdditionalRecEPGPictures)
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Number of EPG pictures to display")), &tmpNopacityConfig->numAdditionalRecEPGPictures, 1, 9));
    Add(new cMenuEditIntItem(tr("Folder Icon Size"), &tmpNopacityConfig->menuRecFolderSize, 30, 300));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item"), &tmpNopacityConfig->fontMenuitemRecordings, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Menu Item Small"), &tmpNopacityConfig->fontMenuitemRecordingsSmall, -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//----ChannelDisplay--------------------------------------------------------------------------------------------------------------

cNopacitySetupChannelDisplay::cNopacitySetupChannelDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Channel Switching"), data) {
    symStyle[0] = tr("simple, one common image");
    symStyle[1] = tr("complex, dedicated images");
    bgStyle[0] = tr("transparent channel logo");
    bgStyle[1] = tr("full osd width");
    logoPos[0] = tr("do not display");
    logoPos[1] = tr("left");
    logoPos[2] = tr("right");
    progressStyleCurrentSchedule[0] = tr("show elapsed time");
    progressStyleCurrentSchedule[1] = tr("show remaining time");
    Set();
}

void cNopacitySetupChannelDisplay::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->channelFadeTime, 0, 1000));
    Add(new cMenuEditIntItem(tr("Height of Channel Display (Percent of OSD Height)"), &tmpNopacityConfig->channelHeight, 15, 80));
    Add(new cMenuEditIntItem(tr("Left & Right Border Width"), &tmpNopacityConfig->channelBorderVertical, 0, 300));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), &tmpNopacityConfig->channelBorderBottom, 0, 300));
    Add(new cMenuEditStraItem(tr("Background Style"), &tmpNopacityConfig->backgroundStyle, 2, bgStyle));
    Add(new cMenuEditBoolItem(tr("Rounded Corners"), &tmpNopacityConfig->roundedCornersChannel));
    Add(new cMenuEditStraItem(tr("Channel Logo Position"), &tmpNopacityConfig->logoPosition, 3, logoPos));
    if (tmpNopacityConfig->logoPosition) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Channel Logo Width")), &tmpNopacityConfig->logoWidth, 30, 500));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Channel Logo Height")), &tmpNopacityConfig->logoHeight, 30, 500));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Channel Logo Border")), &tmpNopacityConfig->logoBorder, 0, 200));
    }
    Add(new cMenuEditStraItem(tr("Kind of time display for current schedule"), &tmpNopacityConfig->progressCurrentSchedule, 2, progressStyleCurrentSchedule));
    Add(new cMenuEditBoolItem(tr("Display Signal Strength & Quality"), &tmpNopacityConfig->displaySignalStrength));
    Add(new cMenuEditBoolItem(tr("Display Channel Source information"), &tmpNopacityConfig->displaySourceInfo));
    Add(new cMenuEditBoolItem(tr("Display Poster or Fanart from TVScraper"), &tmpNopacityConfig->displayPoster));
    Add(new cMenuEditBoolItem(tr("Display previous and next Channel Group"), &tmpNopacityConfig->displayPrevNextChannelGroup));
    Add(new cMenuEditIntItem(tr("Screen Resolution Icon Size"), &tmpNopacityConfig->resolutionIconSize, 30, 200));
    Add(new cMenuEditIntItem(tr("Status Icons Size"), &tmpNopacityConfig->statusIconSize, 30, 150));
    Add(new cMenuEditStraItem(tr("Status Icon Style"), &tmpNopacityConfig->symbolStyle, 2, symStyle));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), &tmpNopacityConfig->fontChannelHeaderSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Date"), &tmpNopacityConfig->fontChannelDateSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Text"), &tmpNopacityConfig->fontEPGSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Infotext"), &tmpNopacityConfig->fontEPGSmallSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Channel Group"), &tmpNopacityConfig->fontChannelGroupSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Next/Prev Channel Group"), &tmpNopacityConfig->fontChannelGroupSmallSize, -20, 20));
    SetCurrent(Get(currentItem));
    Display();
}

//-----ReplayDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupReplayDisplay::cNopacitySetupReplayDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Replay"), data) {
    Set();
}

void cNopacitySetupReplayDisplay::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->replayFadeTime, 0, 1000));
    Add(new cMenuEditIntItem(tr("Height of Replay Display (Percent of OSD Height)"), &tmpNopacityConfig->replayHeight, 15, 80));
    Add(new cMenuEditIntItem(tr("Left & Right Border Width"), &tmpNopacityConfig->replayBorderVertical, 0, 300));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), &tmpNopacityConfig->replayBorderBottom, 0, 300));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), &tmpNopacityConfig->fontReplayHeader, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Text"), &tmpNopacityConfig->fontReplay, -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----TrackDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupTrackDisplay::cNopacitySetupTrackDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Audio Tracks"), data) {
    Set();
}

void cNopacitySetupTrackDisplay::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->tracksFadeTime, 0, 1000));
    Add(new cMenuEditIntItem(tr("Width of Tracks Display (Percent of OSD Width)"), &tmpNopacityConfig->tracksWidth, 10, 100));
    Add(new cMenuEditIntItem(tr("Height of Track Items (in pixels)"), &tmpNopacityConfig->tracksItemHeight, 30, 200));
    Add(new cMenuEditIntItem(tr("Position (0: bot. center, 1: bot. left, ... , 7: bot. right)"), &tmpNopacityConfig->tracksPosition, 0, 7));
    Add(new cMenuEditIntItem(tr("Border Top / Bottom"), &tmpNopacityConfig->tracksBorderHorizontal, 0, 100));
    Add(new cMenuEditIntItem(tr("Border Left / Right"), &tmpNopacityConfig->tracksBorderVertical, 0, 100));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), &tmpNopacityConfig->fontTracksHeader, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Buttons"), &tmpNopacityConfig->fontTracks, -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//-----MessageDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupMessageDisplay::cNopacitySetupMessageDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Messages"), data) {
    Set();
}

void cNopacitySetupMessageDisplay::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->messageFadeTime, 0, 1000));
    Add(new cMenuEditIntItem(tr("Width of Message Display (Percent of OSD Height)"), &tmpNopacityConfig->messageWidth, 30, 100));
    Add(new cMenuEditIntItem(tr("Height of Message Display (Percent of OSD Height)"), &tmpNopacityConfig->messageHeight, 5, 100));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), &tmpNopacityConfig->messageBorderBottom, 0, 1000));
    Add(new cMenuEditIntItem(tr("Adjust Font Size"), &tmpNopacityConfig->fontMessage, -30, 30));

    SetCurrent(Get(currentItem));
    Display();
}

//-----VolumeDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupVolumeDisplay::cNopacitySetupVolumeDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Volume Display"), data) {
    Set();
}

void cNopacitySetupVolumeDisplay::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->volumeFadeTime, 0, 1000));
    Add(new cMenuEditIntItem(tr("Width of Volume Display (Percent of OSD Height)"), &tmpNopacityConfig->volumeWidth, 10, 100));
    Add(new cMenuEditIntItem(tr("Height of Volume Display (Percent of OSD Height)"), &tmpNopacityConfig->volumeHeight, 5, 100));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), &tmpNopacityConfig->volumeBorderBottom, 0, 1000));
    Add(new cMenuEditIntItem(tr("Adjust Font Size"), &tmpNopacityConfig->fontVolume, -30, 30));

    SetCurrent(Get(currentItem));
    Display();
}

//-----RSS Feeds-------------------------------------------------------------------------------------------------------------

cNopacitySetupRssFeed::cNopacitySetupRssFeed(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("RSS Feeds"), data) {
    scrollSpeed[0] = tr("slow");
    scrollSpeed[1] = tr("medium");
    scrollSpeed[2] = tr("fast");
    feedsWithNone[0] = tr("none");
    int i = 0;
    for (std::vector<RssFeed>::iterator it = config.rssFeeds.begin(); it!=config.rssFeeds.end(); ++it) {
        feeds[i] = it->name.c_str();
        feedsWithNone[i+1] = it->name.c_str();
        i++;
        if (i==20)
            break;
    }
    standalonePos[0] = tr("bottom");
    standalonePos[1] = tr("top");
    Set();
}

void cNopacitySetupRssFeed::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Display RSS Feed in Skin"), &tmpNopacityConfig->displayRSSFeed));
    if (tmpNopacityConfig->displayRSSFeed) {
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Height of RSS Feed Line (Percent of OSD Height)")), &tmpNopacityConfig->rssFeedHeight, 3, 10));
        Add(new cMenuEditIntItem(cString::sprintf("%s%s", *spacer, tr("Adjust Font Size")), &tmpNopacityConfig->fontRssFeed, -30, 30));
    }
    Add(new cMenuEditIntItem(tr("Height of standalone RSS Feed (Percent of OSD Height)"), &tmpNopacityConfig->rssFeedHeightStandalone, 3, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size of standalone Feed"), &tmpNopacityConfig->fontRssFeedStandalone, -30, 30));
    Add(new cMenuEditStraItem(tr("Standalone RSS Feed Position"), &tmpNopacityConfig->rssFeedStandalonePos, 2, standalonePos));
    Add(new cMenuEditStraItem(tr("RSS Feed 1"), &tmpNopacityConfig->rssFeed[0], config.rssFeeds.size(), feeds));
    Add(new cMenuEditStraItem(tr("RSS Feed 2"), &tmpNopacityConfig->rssFeed[1], config.rssFeeds.size()+1, feedsWithNone));
    Add(new cMenuEditStraItem(tr("RSS Feed 3"), &tmpNopacityConfig->rssFeed[2], config.rssFeeds.size()+1, feedsWithNone));
    Add(new cMenuEditStraItem(tr("RSS Feed 4"), &tmpNopacityConfig->rssFeed[3], config.rssFeeds.size()+1, feedsWithNone));
    Add(new cMenuEditStraItem(tr("RSS Feed 5"), &tmpNopacityConfig->rssFeed[4], config.rssFeeds.size()+1, feedsWithNone));
    Add(new cMenuEditStraItem(tr("Scrolling Speed"), &tmpNopacityConfig->rssScrollSpeed, 3, scrollSpeed));
    Add(new cMenuEditIntItem(tr("Scrolling Delay in s"), &tmpNopacityConfig->rssScrollDelay, 0, 3));
    SetCurrent(Get(currentItem));
    Display();
}