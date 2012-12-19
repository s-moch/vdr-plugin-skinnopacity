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
    Add(new cOsdItem(tr("VDR Menu")));
    Add(new cOsdItem(tr("Channel Switching")));
    Add(new cOsdItem(tr("Replay")));
    Add(new cOsdItem(tr("Audio Tracks")));
    Add(new cOsdItem(tr("Messages")));
    Add(new cOsdItem(tr("Volume")));
    
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
            if (strcmp(ItemText, tr("VDR Menu")) == 0)
                state = AddSubMenu(new cNopacitySetupMenuDisplay(&tmpNopacityConfig));
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
    SetupStore("logoWidth", config.logoWidth);
    SetupStore("logoHeight", config.logoHeight);
    SetupStore("logoBorder", config.logoBorder);
    SetupStore("displaySignalStrength", config.displaySignalStrength);
    SetupStore("channelFadeTime", config.channelFadeTime);
    SetupStore("fontChannelHeaderSize", config.fontChannelHeaderSize);
    SetupStore("fontChannelDateSize", config.fontChannelDateSize);
    SetupStore("fontEPGSize", config.fontEPGSize);
    SetupStore("fontEPGSmallSize", config.fontEPGSmallSize);
    SetupStore("resolutionIconSize", config.resolutionIconSize);
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
    SetupStore("tracksHeight", config.tracksHeight);
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
    SetupStore("scalePicture", config.scalePicture);
    SetupStore("displayRerunsDetailEPGView", config.displayRerunsDetailEPGView);
    SetupStore("numReruns", config.numReruns);
    SetupStore("useSubtitleRerun", config.useSubtitleRerun);
    SetupStore("menuFadeTime", config.menuFadeTime);
    SetupStore("menuScrollDelay", config.menuScrollDelay);
    SetupStore("menuScrollSpeed", config.menuScrollSpeed);
    SetupStore("menuWidthNarrow", config.menuWidthNarrow);
    SetupStore("menuWidthRightItems", config.menuWidthRightItems);
    SetupStore("showDiscUsage", config.showDiscUsage);
    SetupStore("showTimers", config.showTimers);
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
    SetupStore("epgImageWidth", config.epgImageWidth);
    SetupStore("epgImageHeight", config.epgImageHeight);
    SetupStore("fontHeader", config.fontHeader);
    SetupStore("fontDate", config.fontDate);
    SetupStore("fontMenuitemLarge", config.fontMenuitemLarge);
    SetupStore("fontMenuitemSchedule", config.fontMenuitemSchedule);
    SetupStore("fontMenuitemScheduleSmall", config.fontMenuitemScheduleSmall);
    SetupStore("fontMenuitemDefault", config.fontMenuitemDefault);
    SetupStore("fontDiskUsage", config.fontDiskUsage);
    SetupStore("fontTimersHead", config.fontTimersHead);
    SetupStore("fontTimers", config.fontTimers);
    SetupStore("fontButtons", config.fontButtons);
    SetupStore("fontMessageMenu", config.fontMessageMenu);
    SetupStore("fontDetailView", config.fontDetailView);
    SetupStore("fontDetailViewHeader", config.fontDetailViewHeader);
    SetupStore("fontDetailViewHeaderLarge", config.fontDetailViewHeaderLarge);
}

//------------------------------------------------------------------------------------------------------------------

cMenuSetupSubMenu::cMenuSetupSubMenu(const char* Title, cNopacityConfig* data) : cOsdMenu(Title, 30) {
    tmpNopacityConfig = data;
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
  return state;
}

//-----MenuDisplay-------------------------------------------------------------------------------------------------------------

cNopacitySetupMenuDisplay::cNopacitySetupMenuDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("VDR Menu"), data) {
    useSubtitleRerunTexts[0] = tr("never");
    useSubtitleRerunTexts[1] = tr("if exists");
    useSubtitleRerunTexts[2] = tr("always");
    scrollSpeed[0] = tr("off");
    scrollSpeed[1] = tr("slow");
    scrollSpeed[2] = tr("medium");
    scrollSpeed[3] = tr("fast");
    Set();
}

void cNopacitySetupMenuDisplay::Set(void) {
    int currentItem = Current();
    Clear();
    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->menuFadeTime, 0, 1000));
    Add(new cMenuEditStraItem(tr("Scrolling Speed"), &tmpNopacityConfig->menuScrollSpeed, 4, scrollSpeed));
    Add(new cMenuEditIntItem(tr("Scrolling Delay in ms"), &tmpNopacityConfig->menuScrollDelay, 0, 2000));
    Add(new cMenuEditBoolItem(tr("Scale Video size to fit into menu window"), &tmpNopacityConfig->scalePicture));
    Add(new cMenuEditBoolItem(tr("Display Reruns in detailed EPG View"), &tmpNopacityConfig->displayRerunsDetailEPGView));
    Add(new cMenuEditIntItem(tr("Number of reruns to display"), &tmpNopacityConfig->numReruns, 1, 10));
    Add(new cMenuEditStraItem(tr("Use Subtitle for reruns"), &tmpNopacityConfig->useSubtitleRerun, 3, useSubtitleRerunTexts));
    Add(new cMenuEditIntItem(tr("Width of narrow Menu Bar (Percent of OSD Width)"), &tmpNopacityConfig->menuWidthNarrow, 10, 100));
    Add(new cMenuEditBoolItem(tr("Display Disk Usage"), &tmpNopacityConfig->showDiscUsage));
    Add(new cMenuEditBoolItem(tr("Display Timers"), &tmpNopacityConfig->showTimers));
    Add(new cMenuEditIntItem(tr("Width of Disc Usage and Timers Display (Percent of OSD Width)"), &tmpNopacityConfig->menuWidthRightItems, 10, 100));
    Add(new cMenuEditIntItem(tr("Header Height (Percent of OSD Height)"), &tmpNopacityConfig->headerHeight, 0, 30));
    Add(new cMenuEditIntItem(tr("Footer Height (Percent of OSD Height)"), &tmpNopacityConfig->footerHeight, 0, 30));
    Add(new cMenuEditIntItem(tr("Number of Default Menu Entries per Page"), &tmpNopacityConfig->numDefaultMenuItems, 10, 40));
    Add(new cMenuEditIntItem(tr("Icon Size (Square Main Menu Icons)"), &tmpNopacityConfig->iconHeight, 30, 200));
    Add(new cMenuEditIntItem(tr("Header Icon Size (Square Header Menu Icons)"), &tmpNopacityConfig->headerIconHeight, 30, 200));
    Add(new cMenuEditIntItem(tr("Channel Logo Width (on the Menu Buttons)"), &tmpNopacityConfig->menuItemLogoWidth, 30, 200));
    Add(new cMenuEditIntItem(tr("Channel Logo Height (on the Menu Buttons)"), &tmpNopacityConfig->menuItemLogoHeight, 30, 200));
    Add(new cMenuEditIntItem(tr("Main Menu Header Logo Width"), &tmpNopacityConfig->menuHeaderLogoWidth, 30, 500));
    Add(new cMenuEditIntItem(tr("Main Menu Header Logo Height"), &tmpNopacityConfig->menuHeaderLogoHeight, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View Logo Width"), &tmpNopacityConfig->detailViewLogoWidth, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View Logo Height"), &tmpNopacityConfig->detailViewLogoHeight, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View EPG Image Width"), &tmpNopacityConfig->epgImageWidth, 30, 500));
    Add(new cMenuEditIntItem(tr("Detail EPG View EPG Image Height"), &tmpNopacityConfig->epgImageHeight, 30, 500));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), &tmpNopacityConfig->fontHeader, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Date"), &tmpNopacityConfig->fontDate, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Large Menu Item"), &tmpNopacityConfig->fontMenuitemLarge, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Schedule Menu Item"), &tmpNopacityConfig->fontMenuitemSchedule, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Schedule Menu Item Small"), &tmpNopacityConfig->fontMenuitemScheduleSmall, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Default Menu Item"), &tmpNopacityConfig->fontMenuitemDefault, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Disc Usage"), &tmpNopacityConfig->fontDiskUsage, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Timers Header"), &tmpNopacityConfig->fontTimersHead, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Timers Title"), &tmpNopacityConfig->fontTimers, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Color Buttons"), &tmpNopacityConfig->fontButtons, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Messages"), &tmpNopacityConfig->fontMessageMenu, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Text"), &tmpNopacityConfig->fontDetailView, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Header"), &tmpNopacityConfig->fontDetailViewHeader, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Detail View Header Large"), &tmpNopacityConfig->fontDetailViewHeaderLarge, -20, 20));

    SetCurrent(Get(currentItem));
    Display();
}

//----ChannelDisplay--------------------------------------------------------------------------------------------------------------

cNopacitySetupChannelDisplay::cNopacitySetupChannelDisplay(cNopacityConfig* data)  : cMenuSetupSubMenu(tr("Channel Switching"), data) {
    Set();
}

void cNopacitySetupChannelDisplay::Set(void) {
    int currentItem = Current();
    Clear();

    Add(new cMenuEditIntItem(tr("Fade-In Time in ms (Zero for switching off fading)"), &tmpNopacityConfig->channelFadeTime, 0, 1000));
    Add(new cMenuEditIntItem(tr("Hight of Channel Display (Percent of OSD Height)"), &tmpNopacityConfig->channelHeight, 15, 100));
    Add(new cMenuEditIntItem(tr("Left & Right Border Width"), &tmpNopacityConfig->channelBorderVertical, 0, 300));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), &tmpNopacityConfig->channelBorderBottom, 0, 300));
    Add(new cMenuEditIntItem(tr("Channel Logo Width"), &tmpNopacityConfig->logoWidth, 30, 500));
    Add(new cMenuEditIntItem(tr("Channel Logo Height"), &tmpNopacityConfig->logoHeight, 30, 500));
    Add(new cMenuEditIntItem(tr("Channel Logo Border"), &tmpNopacityConfig->logoBorder, 0, 200));
    Add(new cMenuEditBoolItem(tr("Display Signal Strength & Quality"), &tmpNopacityConfig->displaySignalStrength));
    Add(new cMenuEditIntItem(tr("Screen Resolution Icon Size"), &tmpNopacityConfig->resolutionIconSize, 30, 200));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Header"), &tmpNopacityConfig->fontChannelHeaderSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - Date"), &tmpNopacityConfig->fontChannelDateSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Text"), &tmpNopacityConfig->fontEPGSize, -20, 20));
    Add(new cMenuEditIntItem(tr("Adjust Font Size - EPG Infotext"), &tmpNopacityConfig->fontEPGSmallSize, -20, 20));
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
    Add(new cMenuEditIntItem(tr("Hight of Replay Display (Percent of OSD Height)"), &tmpNopacityConfig->replayHeight, 15, 100));
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
    Add(new cMenuEditIntItem(tr("Hight of Tracks Display (Percent of OSD Height)"), &tmpNopacityConfig->tracksHeight, 10, 100));
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
    Add(new cMenuEditIntItem(tr("Hight of Message Display (Percent of OSD Height)"), &tmpNopacityConfig->messageHeight, 5, 100));
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
    Add(new cMenuEditIntItem(tr("Hight of Volume Display (Percent of OSD Height)"), &tmpNopacityConfig->volumeHeight, 5, 100));
    Add(new cMenuEditIntItem(tr("Bottom Border Height"), &tmpNopacityConfig->volumeBorderBottom, 0, 1000));
    Add(new cMenuEditIntItem(tr("Adjust Font Size"), &tmpNopacityConfig->fontVolume, -30, 30));

    SetCurrent(Get(currentItem));
    Display();
}