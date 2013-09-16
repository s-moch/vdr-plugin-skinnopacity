#include <string>
#include <vector>
#include "config.h"

cNopacityConfig::cNopacityConfig() {
    logoPathSet = false;
    epgImagePathSet = false;
    iconPathSet = false;
    //Common
    mainMenuEntry = false;
    fontIndex = 0;
    fontDefaultName = "VDRSymbols Sans:Book";
    //DisplayChannel
    channelHeight = 25;
    channelBorderVertical = 15;
    channelBorderBottom = 15;
    channelFadeTime = 300; // ms
    logoPosition = 1;
    logoWidth = 260;
    logoHeight = 200;
    logoExtension = "png";
    logoBorder = 15;
    backgroundStyle = 0;
    symbolStyle = 0;
    roundedCornersChannel = 1;
    displaySignalStrength = 1;
    displaySourceInfo = 1;
    displayPrevNextChannelGroup = 1;
    fontChannelHeaderSize = 0;
    fontChannelDateSize = 0;
    fontEPGSize = 0;
    fontEPGSmallSize = 0;
    fontChannelGroupSize = 0;
    fontChannelGroupSmallSize = 0;
    resolutionIconSize = 100;
    statusIconSize = 64;
    progressCurrentSchedule = 0;
    displayPoster = 1;
    //Display Replay 
    replayHeight = 25;
    replayBorderVertical = 15;
    replayBorderBottom = 15;
    replayFadeTime = 300; // ms
    fontReplayHeader = 0;
    fontReplay = 0;
    //DisplayMessage
    messageWidth = 90;
    messageHeight = 10;
    messageBorderBottom = 10;
    fontMessage = 0;
    messageFadeTime = 300;
    //DisplayTracks
    tracksFadeTime = 300;
    tracksWidth = 25;
    tracksItemHeight = 80;
    tracksPosition = 0;
    tracksBorderHorizontal = 10;
    tracksBorderVertical = 10;
    fontTracksHeader = 0;
    fontTracks = 0;
    //DisplayVolume
    volumeFadeTime = 300;
    volumeWidth = 40;
    volumeHeight = 10;
    volumeBorderBottom = 10;
    fontVolume = 0;
    //DisplayMenu
    menuAdjustLeft = 1;
    scalePicture = 1;
    roundedCorners = 0;
    cornerRadius = 12;
    useMenuIcons = 1;
    mainMenuTitleStyle = 0;
    narrowMainMenu = 1;
    narrowScheduleMenu = 1;
    narrowChannelMenu = 1;
    narrowTimerMenu = 1;
    narrowRecordingMenu = 1;
    narrowSetupMenu = 1;
    displayRerunsDetailEPGView = 1;
    numReruns = 5;
    useSubtitleRerun = 1;
    displayAdditionalEPGPictures = 1;
    numAdditionalEPGPictures = 9;
    displayAdditionalRecEPGPictures = 1;
    numAdditionalRecEPGPictures = 9;
    menuChannelDisplayMode = 0;
    menuChannelDisplayTime = 1;
    numEPGEntriesChannelsMenu = 15;
    menuFadeTime = 0;
    menuEPGWindowFadeTime = 300;
    menuWidthMain = 30;
    menuWidthSchedules = 30;
    menuWidthChannels = 30;
    menuWidthTimers = 30;
    menuWidthRecordings = 30;
    menuWidthSetup = 30;
    menuHeightInfoWindow = 20;
    menuScrollDelay = 1;
    menuScrollSpeed = 2;
    menuInfoTextDelay = 2;
    menuInfoScrollDelay = 5;
    menuInfoScrollSpeed = 2;
    menuWidthRightItems = 12;
    menuSizeDiskUsage = 12;
    showDiscUsage = 1;
    discUsageStyle = 0;
    showTimers = 1;
    numberTimers = 10;
    checkTimerConflict = 1;
    headerHeight = 7;
    footerHeight = 7;
    numDefaultMenuItems = 16;
    iconHeight = 100;
    headerIconHeight = 80;
    menuHeaderLogoWidth = 160;
    menuHeaderLogoHeight = 70;
    menuItemLogoWidth = 130;
    menuItemLogoHeight = 100;
    detailViewLogoWidth = 260;
    detailViewLogoHeight = 200;
    timersLogoWidth = 90;
    timersLogoHeight = 70;
    epgImageWidth = 210;
    epgImageHeight = 160;
    epgImageWidthLarge = 525;
    epgImageHeightLarge = 400;
    menuRecFolderSize = 128;
    borderDetailedEPG = 30;
    borderDetailedRecordings = 30;
    menuSchedulesWindowMode = 0;
    menuRecordingsWindowMode = 0;
    fontHeader = 0;
    fontDate = 0;
    fontMenuitemLarge = 0;
    fontMenuitemSchedule = 0;
    fontMenuitemScheduleSmall = 0;
    fontMenuitemChannel = 0;
    fontMenuitemChannelSmall = 0;
    fontMenuitemRecordings = 0;
    fontMenuitemRecordingsSmall = 0;
    fontMenuitemTimers = 0;
    fontMenuitemTimersSmall = 0;
    fontMenuitemDefault = 0;
    fontDiskUsage = 0;
    fontDiskUsagePercent = 0;
    fontTimersHead = 0;
    fontTimers = 0;
    fontButtons = 0;
    fontMessageMenu = 0;
    fontDetailView = 0;
    fontDetailViewSmall = 0;
    fontDetailViewHeader = 0;
    fontDetailViewHeaderLarge = 0;
    fontEPGInfoWindow = 0;
    fontEPGInfoWindowLarge = 0;
    //RSS Feeds
    displayRSSFeed = 0;
    rssFeedHeight = 5;
    rssFeedHeightStandalone = 7;
    fontRssFeed = 0;
    fontRssFeedStandalone = 0;
    rssFeedStandalonePos = 0;
    rssScrollDelay = 2;
    rssScrollSpeed = 1;
    rssFeed[0] = 0;
    rssFeed[1] = 0;
    rssFeed[2] = 0;
    rssFeed[3] = 0;
    rssFeed[4] = 0;
}

cNopacityConfig::~cNopacityConfig() {
}

void cNopacityConfig::setDynamicValues() {
    doBlending = (Theme.Color(clrDoBlending) == CLR_BLENDING_ON)?true:false;
    if (fontIndex == 0) {
        fontName = strdup(fontDefaultName);
    } else {
        cStringList availableFonts;
        cFont::GetAvailableFontNames(&availableFonts);
        if (availableFonts[fontIndex-1]) {
            fontName = strdup(availableFonts[fontIndex-1]);
        } else 
            fontName = strdup(fontDefaultName);
    }
    channelFrameTime = channelFadeTime / 10;
    replayFrameTime = replayFadeTime / 10;
    messageFrameTime = messageFadeTime / 10;
    tracksFrameTime = tracksFadeTime / 10;
    volumeFrameTime = volumeFadeTime / 10;
    menuFrameTime = menuFadeTime / 10;
    menuEPGWindowFrameTime = menuEPGWindowFadeTime / 10;
    
    menuScrollFrameTime = 0;
    if (menuScrollSpeed == 1)
        menuScrollFrameTime = 50;
    else if (menuScrollSpeed == 2)
        menuScrollFrameTime = 30;
    else if (menuScrollSpeed == 3)
        menuScrollFrameTime = 15;
        
    menuInfoScrollFrameTime = 0;
    if (menuInfoScrollSpeed == 1)
        menuInfoScrollFrameTime = 50;
    else if (menuInfoScrollSpeed == 2)
        menuInfoScrollFrameTime = 30;
    else if (menuInfoScrollSpeed == 3)
        menuInfoScrollFrameTime = 15;

    rssScrollFrameTime = 0;
    if (rssScrollSpeed == 0)
        rssScrollFrameTime = 30;
    else if (rssScrollSpeed == 1)
        rssScrollFrameTime = 15;
    else if (rssScrollSpeed == 2)
        rssScrollFrameTime = 5;

        
    logoPathDefault = cString::sprintf("%s/logos/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    iconPathDefault = cString::sprintf("%s/icons/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    epgImagePathDefault = cString::sprintf("%s/epgimages/", cPlugin::CacheDirectory(PLUGIN_NAME_I18N));
    
    dsyslog("nopacity: using Logo Directory %s", (logoPathSet)?(*logoPath):(*logoPathDefault)); 
    dsyslog("nopacity: using Icon Directory %s", (iconPathSet)?(*iconPath):(*iconPathDefault)); 
    dsyslog("nopacity: using EPG Images Directory %s", (epgImagePathSet)?(*epgImagePath):(*epgImagePathDefault)); 
}

void cNopacityConfig::loadRssFeeds(void) {
    cString rssconf = cString::sprintf("%s/rssfeeds.conf", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
    dsyslog("nopacity: trying to load rss feeds from %s", *rssconf);
    FILE *f = fopen(*rssconf, "r");
    bool foundEntries = false;
    if (f) {
        char *line;
        cReadLine ReadLine;
        while ((line = ReadLine.Read(f)) != NULL) {
            try {
                std::string currentLine = line;
                size_t startTag = currentLine.find_first_of('[') + 1;
                size_t endTag = currentLine.find_first_of(']');
                if (endTag != std::string::npos) {
                    std::string rssName = currentLine.substr(startTag, endTag - 1);
                    std::string rssUrl = currentLine.substr(endTag + 1);
                    size_t startUrl = rssUrl.find_first_of("http");
                    if (startUrl != std::string::npos) {
                        rssUrl = rssUrl.substr(startUrl);
                        size_t whitespaces = rssUrl.find_first_of(" ");
                        if (whitespaces != std::string::npos) {
                            rssUrl = rssUrl.substr(0, whitespaces);
                        }
                        RssFeed feed;
                        feed.name = rssName;
                        feed.url = rssUrl;
                        rssFeeds.push_back(feed);
                        foundEntries = true;
                    }
                }
            } catch (...) {}
        }
        fclose(f);
    }
    if (foundEntries) {
        dsyslog("nopacity: loaded %lu rss feeds from %s", (long unsigned)rssFeeds.size(), *rssconf);
        int i = 1;
        for (std::vector<RssFeed>::iterator it = rssFeeds.begin(); it != rssFeeds.end(); it++) {
            dsyslog("nopacity: RssFeed %d: name %s, URL: %s", i, it->name.c_str(), it->url.c_str());
            i++;
        }
    } else {
        dsyslog("nopacity: no valid rss config found, using default rss feed");
        RssFeed feed;
        feed.name = "Tagesschau";
        feed.url = "http://www.tagesschau.de/xml/rss2";
        rssFeeds.push_back(feed);
    }
}


void cNopacityConfig::SetLogoPath(cString path) {
    logoPath = checkSlashAtEnd(*path);
    logoPathSet = true;
}

void cNopacityConfig::SetIconPath(cString path) {
    iconPath = checkSlashAtEnd(*path);
    iconPathSet = true;
}

void cNopacityConfig::SetEpgImagePath(cString path) {
    epgImagePath = checkSlashAtEnd(*path);
    epgImagePathSet = true;
}

cString cNopacityConfig::checkSlashAtEnd(std::string path) {
    try {
        if (!(path.at(path.size()-1) == '/'))
            return cString::sprintf("%s/", path.c_str());
    } catch (...) {return path.c_str();}
    return path.c_str();
}


bool cNopacityConfig::SetupParse(const char *Name, const char *Value) {
    if      (strcmp(Name, "fontIndex") == 0)               fontIndex = atoi(Value);
    else if (strcmp(Name, "channelFadeTime") == 0)         channelFadeTime = atoi(Value);
    else if (strcmp(Name, "channelHeight") == 0)           channelHeight = atoi(Value);
    else if (strcmp(Name, "channelBorderVertical") == 0)   channelBorderVertical = atoi(Value);
    else if (strcmp(Name, "channelBorderBottom") == 0)     channelBorderBottom = atoi(Value);
    else if (strcmp(Name, "logoPosition") == 0)            logoPosition = atoi(Value);
    else if (strcmp(Name, "logoWidth") == 0)               logoWidth = atoi(Value);
    else if (strcmp(Name, "logoHeight") == 0)              logoHeight = atoi(Value);
    else if (strcmp(Name, "logoBorder") == 0)              logoBorder = atoi(Value);
    else if (strcmp(Name, "backgroundStyle") == 0)         backgroundStyle = atoi(Value);
    else if (strcmp(Name, "symbolStyle") == 0)              symbolStyle = atoi(Value);
    else if (strcmp(Name, "roundedCornersChannel") == 0)   roundedCornersChannel = atoi(Value);
    else if (strcmp(Name, "displaySignalStrength") == 0)   displaySignalStrength = atoi(Value);
    else if (strcmp(Name, "displaySourceInfo") == 0)       displaySourceInfo = atoi(Value);
    else if (strcmp(Name, "displayPrevNextChannelGroup") == 0) displayPrevNextChannelGroup = atoi(Value);
    else if (strcmp(Name, "fontChannelHeaderSize") == 0)   fontChannelHeaderSize = atoi(Value);
    else if (strcmp(Name, "fontChannelDateSize") == 0)     fontChannelDateSize = atoi(Value);
    else if (strcmp(Name, "fontEPGSize") == 0)             fontEPGSize = atoi(Value);
    else if (strcmp(Name, "fontEPGSmallSize") == 0)        fontEPGSmallSize = atoi(Value);
    else if (strcmp(Name, "fontChannelGroupSize") == 0)    fontChannelGroupSize = atoi(Value);
    else if (strcmp(Name, "fontChannelGroupSmallSize") == 0) fontChannelGroupSmallSize = atoi(Value);
    else if (strcmp(Name, "resolutionIconSize") == 0)      resolutionIconSize = atoi(Value);
    else if (strcmp(Name, "statusIconSize") == 0)          statusIconSize = atoi(Value);
    else if (strcmp(Name, "progressCurrentSchedule") == 0) progressCurrentSchedule = atoi(Value);
    else if (strcmp(Name, "displayPoster") == 0)           displayPoster = atoi(Value);
    else if (strcmp(Name, "replayHeight") == 0)            replayHeight = atoi(Value);
    else if (strcmp(Name, "replayBorderVertical") == 0)    replayBorderVertical = atoi(Value);
    else if (strcmp(Name, "replayBorderBottom") == 0)      replayBorderBottom = atoi(Value);
    else if (strcmp(Name, "replayFadeTime") == 0)          replayFadeTime = atoi(Value);
    else if (strcmp(Name, "fontReplayHeader") == 0)        fontReplayHeader = atoi(Value);
    else if (strcmp(Name, "fontReplay") == 0)              fontReplay = atoi(Value);
    else if (strcmp(Name, "messageWidth") == 0)            messageWidth = atoi(Value);
    else if (strcmp(Name, "messageHeight") == 0)           messageHeight = atoi(Value);
    else if (strcmp(Name, "messageBorderBottom") == 0)     messageBorderBottom = atoi(Value);
    else if (strcmp(Name, "fontMessage") == 0)             fontMessage = atoi(Value);
    else if (strcmp(Name, "messageFadeTime") == 0)         messageFadeTime = atoi(Value);
    else if (strcmp(Name, "tracksFadeTime") == 0)          tracksFadeTime = atoi(Value);
    else if (strcmp(Name, "tracksWidth") == 0)             tracksWidth = atoi(Value);
    else if (strcmp(Name, "tracksItemHeight") == 0)        tracksItemHeight = atoi(Value);
    else if (strcmp(Name, "tracksPosition") == 0)          tracksPosition = atoi(Value);
    else if (strcmp(Name, "tracksBorderHorizontal") == 0)  tracksBorderHorizontal = atoi(Value);
    else if (strcmp(Name, "tracksBorderVertical") == 0)    tracksBorderVertical = atoi(Value);
    else if (strcmp(Name, "fontTracksHeader") == 0)        fontTracksHeader = atoi(Value);
    else if (strcmp(Name, "fontTracks") == 0)              fontTracks = atoi(Value);
    else if (strcmp(Name, "volumeFadeTime") == 0)          volumeFadeTime = atoi(Value);
    else if (strcmp(Name, "volumeWidth") == 0)             volumeWidth = atoi(Value);
    else if (strcmp(Name, "volumeHeight") == 0)            volumeHeight = atoi(Value);
    else if (strcmp(Name, "volumeBorderBottom") == 0)      volumeBorderBottom = atoi(Value);
    else if (strcmp(Name, "fontVolume") == 0)              fontVolume = atoi(Value);
    else if (strcmp(Name, "menuFadeTime") == 0)            menuFadeTime = atoi(Value);
    else if (strcmp(Name, "menuEPGWindowFadeTime") == 0)   menuEPGWindowFadeTime = atoi(Value);
    else if (strcmp(Name, "menuScrollDelay") == 0)         menuScrollDelay = atoi(Value);
    else if (strcmp(Name, "menuScrollSpeed") == 0)         menuScrollSpeed = atoi(Value);
    else if (strcmp(Name, "menuInfoTextDelay") == 0)       menuInfoTextDelay = atoi(Value);
    else if (strcmp(Name, "menuInfoScrollDelay") == 0)     menuInfoScrollDelay = atoi(Value);
    else if (strcmp(Name, "menuInfoScrollSpeed") == 0)     menuInfoScrollSpeed = atoi(Value);
    else if (strcmp(Name, "menuAdjustLeft") == 0)          menuAdjustLeft = atoi(Value);
    else if (strcmp(Name, "scalePicture") == 0)            scalePicture = atoi(Value);
    else if (strcmp(Name, "roundedCorners") == 0)          roundedCorners = atoi(Value);
    else if (strcmp(Name, "cornerRadius") == 0)            cornerRadius = atoi(Value);
    else if (strcmp(Name, "useMenuIcons") == 0)            useMenuIcons = atoi(Value);
    else if (strcmp(Name, "mainMenuTitleStyle") == 0)      mainMenuTitleStyle = atoi(Value);
    else if (strcmp(Name, "narrowMainMenu") == 0)          narrowMainMenu = atoi(Value);
    else if (strcmp(Name, "narrowScheduleMenu") == 0)      narrowScheduleMenu = atoi(Value);
    else if (strcmp(Name, "narrowChannelMenu") == 0)       narrowChannelMenu = atoi(Value);
    else if (strcmp(Name, "narrowTimerMenu") == 0)         narrowTimerMenu = atoi(Value);
    else if (strcmp(Name, "narrowRecordingMenu") == 0)     narrowRecordingMenu = atoi(Value);
    else if (strcmp(Name, "narrowSetupMenu") == 0)         narrowSetupMenu = atoi(Value);
    else if (strcmp(Name, "displayRerunsDetailEPGView") == 0) displayRerunsDetailEPGView = atoi(Value);
    else if (strcmp(Name, "numReruns") == 0)               numReruns = atoi(Value);
    else if (strcmp(Name, "useSubtitleRerun") == 0)        useSubtitleRerun = atoi(Value);
    else if (strcmp(Name, "displayAdditionalEPGPictures") == 0) displayAdditionalEPGPictures = atoi(Value);
    else if (strcmp(Name, "numAdditionalEPGPictures") == 0) numAdditionalEPGPictures = atoi(Value);
    else if (strcmp(Name, "displayAdditionalRecEPGPictures") == 0) displayAdditionalRecEPGPictures = atoi(Value);
    else if (strcmp(Name, "numAdditionalRecEPGPictures") == 0) numAdditionalRecEPGPictures = atoi(Value);
    else if (strcmp(Name, "menuChannelDisplayMode") == 0)  menuChannelDisplayMode = atoi(Value);
    else if (strcmp(Name, "menuChannelDisplayTime") == 0)  menuChannelDisplayTime= atoi(Value);
    else if (strcmp(Name, "numEPGEntriesChannelsMenu") == 0) numEPGEntriesChannelsMenu = atoi(Value);
    else if (strcmp(Name, "menuWidthMain") == 0)           menuWidthMain = atoi(Value);
    else if (strcmp(Name, "menuWidthSchedules") == 0)      menuWidthSchedules = atoi(Value);
    else if (strcmp(Name, "menuWidthChannels") == 0)       menuWidthChannels = atoi(Value);
    else if (strcmp(Name, "menuWidthTimers") == 0)         menuWidthTimers = atoi(Value);
    else if (strcmp(Name, "menuWidthRecordings") == 0)     menuWidthRecordings = atoi(Value);
    else if (strcmp(Name, "menuWidthSetup") == 0)          menuWidthSetup = atoi(Value);
    else if (strcmp(Name, "menuWidthRightItems") == 0)     menuWidthRightItems = atoi(Value);
    else if (strcmp(Name, "menuSizeDiskUsage") == 0)       menuSizeDiskUsage = atoi(Value);
    else if (strcmp(Name, "menuHeightInfoWindow") == 0)    menuHeightInfoWindow = atoi(Value);
    else if (strcmp(Name, "showDiscUsage") == 0)           showDiscUsage = atoi(Value);
    else if (strcmp(Name, "discUsageStyle") == 0)          discUsageStyle = atoi(Value);
    else if (strcmp(Name, "showTimers") == 0)              showTimers = atoi(Value);
    else if (strcmp(Name, "numberTimers") == 0)            numberTimers = atoi(Value);
    else if (strcmp(Name, "checkTimerConflict") == 0)      checkTimerConflict = atoi(Value);
    else if (strcmp(Name, "headerHeight") == 0)            headerHeight = atoi(Value);
    else if (strcmp(Name, "footerHeight") == 0)            footerHeight = atoi(Value);
    else if (strcmp(Name, "numDefaultMenuItems") == 0)     numDefaultMenuItems = atoi(Value);
    else if (strcmp(Name, "iconHeight") == 0)              iconHeight = atoi(Value);
    else if (strcmp(Name, "headerIconHeight") == 0)        headerIconHeight = atoi(Value);
    else if (strcmp(Name, "menuItemLogoWidth") == 0)       menuItemLogoWidth = atoi(Value);
    else if (strcmp(Name, "menuItemLogoHeight") == 0)      menuItemLogoHeight = atoi(Value);
    else if (strcmp(Name, "menuHeaderLogoWidth") == 0)     menuHeaderLogoWidth = atoi(Value);
    else if (strcmp(Name, "menuHeaderLogoHeight") == 0)    menuHeaderLogoHeight = atoi(Value);
    else if (strcmp(Name, "detailViewLogoWidth") == 0)     detailViewLogoWidth = atoi(Value);
    else if (strcmp(Name, "detailViewLogoHeight") == 0)    detailViewLogoHeight = atoi(Value);
    else if (strcmp(Name, "timersLogoWidth") == 0)         timersLogoWidth = atoi(Value);
    else if (strcmp(Name, "timersLogoHeight") == 0)        timersLogoHeight = atoi(Value);
    else if (strcmp(Name, "epgImageWidth") == 0)           epgImageWidth = atoi(Value);
    else if (strcmp(Name, "epgImageHeight") == 0)          epgImageHeight = atoi(Value);
    else if (strcmp(Name, "epgImageWidthLarge") == 0)      epgImageWidthLarge = atoi(Value);
    else if (strcmp(Name, "epgImageHeightLarge") == 0)     epgImageHeightLarge = atoi(Value);
    else if (strcmp(Name, "menuRecFolderSize") == 0)       menuRecFolderSize = atoi(Value);
    else if (strcmp(Name, "borderDetailedEPG") == 0)       borderDetailedEPG = atoi(Value);
    else if (strcmp(Name, "borderDetailedRecordings") == 0) borderDetailedRecordings = atoi(Value);
    else if (strcmp(Name, "menuSchedulesWindowMode") == 0) menuSchedulesWindowMode = atoi(Value);
    else if (strcmp(Name, "menuRecordingsWindowMode") == 0) menuRecordingsWindowMode = atoi(Value);
    else if (strcmp(Name, "fontHeader") == 0)              fontHeader = atoi(Value);
    else if (strcmp(Name, "fontDate") == 0)                fontDate = atoi(Value);
    else if (strcmp(Name, "fontMenuitemLarge") == 0)       fontMenuitemLarge = atoi(Value);
    else if (strcmp(Name, "fontMenuitemSchedule") == 0)    fontMenuitemSchedule = atoi(Value);
    else if (strcmp(Name, "fontMenuitemScheduleSmall") == 0) fontMenuitemScheduleSmall = atoi(Value);
    else if (strcmp(Name, "fontMenuitemChannel") == 0)     fontMenuitemChannel = atoi(Value);
    else if (strcmp(Name, "fontMenuitemChannelSmall") == 0) fontMenuitemChannelSmall = atoi(Value);
    else if (strcmp(Name, "fontMenuitemRecordings") == 0)    fontMenuitemRecordings = atoi(Value);
    else if (strcmp(Name, "fontMenuitemRecordingsSmall") == 0) fontMenuitemRecordingsSmall = atoi(Value);
    else if (strcmp(Name, "fontMenuitemTimers") == 0)      fontMenuitemTimers = atoi(Value);
    else if (strcmp(Name, "fontMenuitemTimersSmall") == 0) fontMenuitemTimersSmall = atoi(Value);
    else if (strcmp(Name, "fontMenuitemDefault") == 0)     fontMenuitemDefault = atoi(Value);
    else if (strcmp(Name, "fontDiskUsage") == 0)           fontDiskUsage = atoi(Value);
    else if (strcmp(Name, "fontDiskUsagePercent") == 0)    fontDiskUsagePercent = atoi(Value);
    else if (strcmp(Name, "fontTimersHead") == 0)          fontTimersHead = atoi(Value);
    else if (strcmp(Name, "fontTimers") == 0)              fontTimers = atoi(Value);
    else if (strcmp(Name, "fontButtons") == 0)             fontButtons = atoi(Value);
    else if (strcmp(Name, "fontMessageMenu") == 0)         fontMessageMenu = atoi(Value);
    else if (strcmp(Name, "fontDetailView") == 0)          fontDetailView = atoi(Value);
    else if (strcmp(Name, "fontDetailViewSmall") == 0)     fontDetailViewSmall = atoi(Value);
    else if (strcmp(Name, "fontDetailViewHeader") == 0)    fontDetailViewHeader = atoi(Value);
    else if (strcmp(Name, "fontDetailViewHeaderLarge") == 0) fontDetailViewHeaderLarge = atoi(Value);
    else if (strcmp(Name, "fontEPGInfoWindow") == 0)       fontEPGInfoWindow = atoi(Value);
    else if (strcmp(Name, "fontEPGInfoWindowLarge") == 0)  fontEPGInfoWindowLarge = atoi(Value);
    else if (strcmp(Name, "displayRSSFeed") == 0)          displayRSSFeed = atoi(Value);
    else if (strcmp(Name, "rssFeedHeight") == 0)           rssFeedHeight = atoi(Value);
    else if (strcmp(Name, "fontRssFeed") == 0)             fontRssFeed = atoi(Value);
    else if (strcmp(Name, "rssScrollDelay") == 0)          rssScrollDelay = atoi(Value);
    else if (strcmp(Name, "rssScrollSpeed") == 0)          rssScrollSpeed = atoi(Value);
    else if (strcmp(Name, "rssFeed[0]") == 0)              rssFeed[0] = atoi(Value);
    else if (strcmp(Name, "rssFeed[1]") == 0)              rssFeed[1] = atoi(Value);
    else if (strcmp(Name, "rssFeed[2]") == 0)              rssFeed[2] = atoi(Value);
    else if (strcmp(Name, "rssFeed[3]") == 0)              rssFeed[3] = atoi(Value);
    else if (strcmp(Name, "rssFeed[4]") == 0)              rssFeed[4] = atoi(Value);
    else if (strcmp(Name, "rssFeedHeightStandalone") == 0) rssFeedHeightStandalone = atoi(Value);
    else if (strcmp(Name, "fontRssFeedStandalone") == 0)   fontRssFeedStandalone = atoi(Value);
    else if (strcmp(Name, "rssFeedStandalonePos") == 0)    rssFeedStandalonePos = atoi(Value);
    else return false;
    return true;
    
}
