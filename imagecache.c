#include <string>
#include <sstream>
#include <map>
#include "imagecache.h"

using namespace Magick;

cImageCache::cImageCache() : cImageMagickWrapper() {
    tempStaticLogo = NULL;
    osdTheme = Setup.OSDTheme;
}

cImageCache::~cImageCache() {
    Clear();
    if (tempStaticLogo) {
        delete tempStaticLogo;
        tempStaticLogo = NULL;
    }    
}

void cImageCache::CreateCache(void) {
    //Menu Icons
    std::vector<std::pair<std::string, cPoint> > menuIcons = GetMenuIcons();
    for (int i=0; i < menuIcons.size(); i++) {
        std::string iconName = menuIcons[i].first;
        cPoint iconSize = menuIcons[i].second;
        bool success = LoadIcon(ctMenuIcon, iconName);
        if (success)
            InsertIntoIconCache(ctMenuIcon, iconName, iconSize.X(), iconSize.X());
    }
    //Skin Icons
    std::vector<std::pair<std::string, sImgProperties> > skinIcons = GetSkinIcons();
    for (int i=0; i < skinIcons.size(); i++) {
        std::string iconName = skinIcons[i].first;
        sImgProperties iconProps = skinIcons[i].second;
        bool success = LoadIcon(ctSkinIcon, iconName);
        if (success)
            InsertIntoIconCache(ctSkinIcon, iconName, iconProps.width, iconProps.height, iconProps.preserveAspect);

    }
    //Channel Logos
    if (config.GetValue("numLogosInitial") > 0) {
        int channelsCached = 0;
        for (const cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
            if (channelsCached >= config.GetValue("numLogosInitial"))
                break;
            if (!channel->GroupSep()) {
                bool success = LoadLogo(channel);
                if (success) {
                    channelsCached++;
                    InsertIntoLogoCache(ctLogo, *channel->GetChannelID().ToString());
                }
                success = LoadLogo(channel);
                if (success) {
                    InsertIntoLogoCache(ctLogoMenuItem, *channel->GetChannelID().ToString());
                }
            }
            
        }
    }
}

void cImageCache::CreateCacheDelayed(void) {
    if (config.GetValue("displayType") == dtBlending)
        CreateSkinElementsBlended();
    else if (config.GetValue("displayType") == dtGraphical)
        CreateSkinElementsGraphics();
}

bool cImageCache::ThemeChanged(void) {
    if (osdTheme.compare(Setup.OSDTheme) != 0) {
        osdTheme = Setup.OSDTheme;
        return true;
    }
    return false;
}

void cImageCache::Reload(void) {
    Clear();
    CreateCache();
    CreateCacheDelayed();
}

cImage *cImageCache::GetMenuIcon(std::string name) {
    std::map<std::string, cImage*>::iterator hit = menuIconCache.find(name);
    if (hit != menuIconCache.end()) {
        return (cImage*)hit->second;
    } else {
        int iconWidth = config.GetValue("iconHeight");
        int iconHeight = config.GetValue("iconHeight");
        bool success = LoadIcon(ctMenuIcon, name);
        if (success) {
            InsertIntoIconCache(ctMenuIcon, name, iconWidth, iconHeight);
            hit = menuIconCache.find(name);
            if (hit != menuIconCache.end()) {
                return (cImage*)hit->second;
            }
        }
    }
    return NULL;
}

cImage *cImageCache::GetSkinIcon(std::string name, int width, int height, bool preserveAspect) {
    std::map<std::string, cImage*>::iterator hit = skinIconCache.find(name);
    if (hit != skinIconCache.end()) {
        return (cImage*)hit->second;
    } else {
        bool success = LoadIcon(ctSkinIcon, name);
        if (success) {
            InsertIntoIconCache(ctSkinIcon, name, width, height, preserveAspect);
            hit = skinIconCache.find(name);
            if (hit != skinIconCache.end()) {
                return (cImage*)hit->second;
            }
        }
    }
    return NULL;
}

cImage *cImageCache::GetSkinElement(eSkinElementType type) {
    std::map<eSkinElementType, cImage*>::iterator hit = skinElementCache.find(type);
    if (hit != skinElementCache.end()) {
        return (cImage*)hit->second;
    }
    return NULL;
}

cImage *cImageCache::GetLogo(eCacheType type, const cChannel *channel) {
    if (!channel)
        return NULL;
        
    std::map<std::string, cImage*> *cache;
    if (type == ctLogo)
        cache = &logoCache;
    else if (type == ctLogoMenuItem)
        cache = &logoMenuItemCache;
    else if (type == ctLogoTimer)
        cache = &logoTimerCache;
        
    std::map<std::string, cImage*>::iterator hit = cache->find(*channel->GetChannelID().ToString());
    
    if (hit != cache->end()) {
        return (cImage*)hit->second;
    } else {
        bool success = LoadLogo(channel);
        if (success) {
            if (config.GetValue("limitLogoCache") && (cache->size() >= config.GetValue("numLogosMax"))) {
            //logo cache is full, don't cache anymore
                cPoint logoSize = LogoSize(type);
                int width = logoSize.X();
                int height = logoSize.Y();
                buffer.sample(Geometry(width, height));
                if (tempStaticLogo) {
                    delete tempStaticLogo;
                    tempStaticLogo = NULL;
                }
                tempStaticLogo = CreateImage();
                return tempStaticLogo;
            } else {
            //add requested logo to cache
                InsertIntoLogoCache(type, *channel->GetChannelID().ToString());
                hit = cache->find(*channel->GetChannelID().ToString());
                if (hit != cache->end()) {
                    return (cImage*)hit->second;
                }
            }
        }
    }
    return NULL;
}

cImage cImageCache::GetBackground(tColor back, tColor blend, int width, int height, bool flip) {
    if (!flip)
        CreateBackground(back, blend, width, height);
    else
        CreateBackgroundReverse(back, blend, width, height);
    return CreateImageCopy();
}

std::string cImageCache::GetCacheSize(eCacheType type) {
    std::stringstream result;
    int sizeByte = 0;
    int numImages = 0;
    if ((type == ctMenuIcon) || (type == ctSkinIcon)) {
        std::map<std::string, cImage*> *cache;
        if (type == ctMenuIcon)
            cache = &menuIconCache;
        else if (type == ctSkinIcon)
            cache = &skinIconCache;
        
        for(std::map<std::string, cImage*>::const_iterator it = cache->begin(); it != cache->end(); it++) {
            cImage *img = (cImage*)it->second;
            sizeByte += img->Width() * img->Height() * sizeof(tColor);
        }
        numImages = cache->size();
    } else if ((type == ctLogo) || (type == ctLogoMenuItem) || (type == ctLogoTimer)) {
        std::map<std::string, cImage*> *cache;
        if (type == ctLogo)
            cache = &logoCache;
        else if (type == ctLogoMenuItem)
            cache = &logoMenuItemCache;
        else if (type == ctLogoTimer)
            cache = &logoTimerCache;

        for(std::map<std::string, cImage*>::const_iterator it = cache->begin(); it != cache->end(); it++) {
            cImage *img = (cImage*)it->second;
            sizeByte += img->Width() * img->Height() * sizeof(tColor);
        }
        numImages = cache->size();
    } else if (type == ctSkinElement) {
        for(std::map<eSkinElementType, cImage*>::const_iterator it = skinElementCache.begin(); it != skinElementCache.end(); it++) {
            cImage *img = (cImage*)it->second;
            sizeByte += img->Width() * img->Height() * sizeof(tColor);
        }
        numImages = skinElementCache.size();
    }

    result << numImages << " " << tr("images") << " / " << sizeByte/1024 << " KByte";
    return result.str();
}
    
std::vector<std::pair<std::string, cPoint> > cImageCache::GetMenuIcons(void) {
    std::vector<std::pair<std::string, cPoint> > menuIcons;
    //MainMenuIcons
    std::vector<std::string> mainMenuIcons;
    mainMenuIcons.push_back("menuIcons/Schedule");
    mainMenuIcons.push_back("menuIcons/Channels");
    mainMenuIcons.push_back("menuIcons/Timers");
    mainMenuIcons.push_back("menuIcons/Recordings");
    mainMenuIcons.push_back("menuIcons/Setup");
    mainMenuIcons.push_back("menuIcons/Commands");
    mainMenuIcons.push_back("menuIcons/OSD");
    mainMenuIcons.push_back("menuIcons/EPG");
    mainMenuIcons.push_back("menuIcons/DVB");
    mainMenuIcons.push_back("menuIcons/LNB");
    mainMenuIcons.push_back("menuIcons/CAM");
    mainMenuIcons.push_back("menuIcons/Recording");
    mainMenuIcons.push_back("menuIcons/Replay");
    mainMenuIcons.push_back("menuIcons/Miscellaneous");
    mainMenuIcons.push_back("menuIcons/Plugins");
    mainMenuIcons.push_back("menuIcons/Restart");
    int mainMenuIconSize = config.GetValue("iconHeight") - 10;
    for (int i=0; i<mainMenuIcons.size(); i++) {
        menuIcons.push_back(std::pair<std::string, cPoint>(mainMenuIcons[i], cPoint(mainMenuIconSize, mainMenuIconSize)));
    }
    //Plugin Icons
    for (int i = 0; ; i++) {
        cPlugin *p = cPluginManager::GetPlugin(i);
        if (p) {
            std::stringstream iconPlugin;
            iconPlugin << "pluginIcons/" << p->Name();
            menuIcons.push_back(std::pair<std::string, cPoint>(iconPlugin.str(), cPoint(mainMenuIconSize, mainMenuIconSize)));
        } else
            break;
    }
    return menuIcons;
}

std::vector<std::pair<std::string, sImgProperties> > cImageCache::GetSkinIcons(void) {
    std::vector<std::pair<std::string, sImgProperties> > skinIcons;
    //VDR Logo
    sImgProperties props;
    props.width = config.GetValue("menuHeaderLogoWidth");
    props.height = config.GetValue("menuHeaderLogoWidth");
    props.preserveAspect = true;
    skinIcons.push_back(std::pair<std::string, sImgProperties>("skinIcons/vdrlogo", props));
    //DiskUsage
    int discUsageSize = geoManager->menuDiskUsageWidth;
    props.width = props.height = discUsageSize*3/4;
    props.preserveAspect = true;
    skinIcons.push_back(std::pair<std::string, sImgProperties>("skinIcons/DiskUsage", props));
    props.width = discUsageSize - 4;
    props.height = discUsageSize/5;
    props.preserveAspect = false;
    skinIcons.push_back(std::pair<std::string, sImgProperties>("skinIcons/discpercent", props));
    //menu header icons
    std::vector<std::string> menuHeaderIcons;
    menuHeaderIcons.push_back("menuIcons/Schedule");
    menuHeaderIcons.push_back("menuIcons/Channels");
    menuHeaderIcons.push_back("menuIcons/Timers");
    menuHeaderIcons.push_back("menuIcons/Recordings");
    menuHeaderIcons.push_back("menuIcons/Setup");
    menuHeaderIcons.push_back("menuIcons/Commands");
    props.width = config.GetValue("headerIconHeight");
    props.height = config.GetValue("headerIconHeight");
    props.preserveAspect = true;
    for (int i=0; i<menuHeaderIcons.size(); i++) {
        skinIcons.push_back(std::pair<std::string, sImgProperties>(menuHeaderIcons[i], props));
    }

    return skinIcons;
}

bool cImageCache::LoadIcon(eCacheType type, std::string name) {
    bool success = false;
    if (config.iconPathSet) {
        cString iconPathTheme = cString::sprintf("%s%s/", *config.iconPath, Setup.OSDTheme);
        success = LoadImage(name, *iconPathTheme, "png");
        if (success) {
            return true;
        } else {
            success = LoadImage(name, *config.iconPath, "png");
            if (success) {
                return true;
            }
        }
    } 
    if (!success) {
        cString iconPathTheme = cString::sprintf("%s%s/", *config.iconPathDefault, Setup.OSDTheme);
        success = LoadImage(name, *iconPathTheme, "png");
        if (success) {
            return true;
        } else {
            success = LoadImage(name, *config.iconPathDefault, "png");
            if (success) {
                return true;
            }
        }
    }
    return false;
}

void cImageCache::InsertIntoIconCache(eCacheType type, std::string name, int width, int height, bool preserveAspect) {
    if (preserveAspect) {
        buffer.sample(Geometry(width, height));
    } else {
        cString geometry = cString::sprintf("%dx%d!", width, height);
        buffer.resize(Geometry(*geometry));
    }
    cImage *image = CreateImage();
    if (type == ctMenuIcon)
        menuIconCache.insert(std::pair<std::string, cImage*>(name, image));
    else if (type == ctSkinIcon)
        skinIconCache.insert(std::pair<std::string, cImage*>(name, image));
}

bool cImageCache::LoadLogo(const cChannel *channel) {
    if (!channel)
        return false;
    std::string channelID = StrToLowerCase(*(channel->GetChannelID().ToString()));
    std::string logoLower = StrToLowerCase(channel->Name());
    bool success = false;
    if (config.logoPathSet) {
        cString logoThemePath = cString::sprintf("%s%s/", *config.logoPath, Setup.OSDTheme);
        success = LoadImage(channelID.c_str(), *logoThemePath, *config.logoExtension);
        if (success)
            return true;
        success = LoadImage(logoLower.c_str(), *logoThemePath, *config.logoExtension);
        if (success)
            return true;
        success = LoadImage(channelID.c_str(), *config.logoPath, *config.logoExtension);
        if (success)
            return true;
        success = LoadImage(logoLower.c_str(), *config.logoPath, *config.logoExtension);
        if (success)
            return true;
    }
    cString logoThemePath = cString::sprintf("%s%s/", *config.logoPathDefault, Setup.OSDTheme);
    success = LoadImage(channelID.c_str(), *logoThemePath, *config.logoExtension);
    if (success)
        return true;
    success = LoadImage(logoLower.c_str(), *logoThemePath, *config.logoExtension);
    if (success)
        return true;
    success = LoadImage(channelID.c_str(), *config.logoPathDefault, *config.logoExtension);
    if (success)
        return true;
    success = LoadImage(logoLower.c_str(), *config.logoPathDefault, *config.logoExtension);
    if (success)
        return true;
    
    return false;
}

void cImageCache::InsertIntoLogoCache(eCacheType type, std::string channelID) {
    cPoint logoSize = LogoSize(type);
    int width = logoSize.X();
    int height = logoSize.Y();
    buffer.sample(Geometry(width, height));
    cImage *image = CreateImage();
    if (type == ctLogo)
        logoCache.insert(std::pair<std::string, cImage*>(channelID, image));
    else if (type == ctLogoMenuItem)
        logoMenuItemCache.insert(std::pair<std::string, cImage*>(channelID, image));
    else if (type == ctLogoTimer)
        logoTimerCache.insert(std::pair<std::string, cImage*>(channelID, image));
}

cPoint cImageCache::LogoSize(eCacheType type) {
    int width, height;
    switch (type) {
        case ctLogo:
            width = geoManager->channelLogoWidth;
            height = geoManager->channelLogoHeight;
            break;
        case ctLogoMenuItem:
            width = config.GetValue("menuItemLogoWidth");
            height = config.GetValue("menuItemLogoHeight");
            break;
        case ctLogoTimer:
            width = config.GetValue("timersLogoWidth");
            height = config.GetValue("timersLogoHeight");
            break;
        default:
            width = 1;
            height = 1;
    }
    return cPoint(width, height);
}

void cImageCache::CreateSkinElementsBlended(void) {
    //Default Menus
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthDefault-2, geoManager->menuItemHeightDefault-2);
    InsertIntoSkinElementCache(seDefault);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthDefault-2, geoManager->menuItemHeightDefault-2);
    InsertIntoSkinElementCache(seDefaultHigh);
    
    //Main Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthMain-2, geoManager->menuItemHeightMain-2);
    InsertIntoSkinElementCache(seMain);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthMain-2, geoManager->menuItemHeightMain-2);
    InsertIntoSkinElementCache(seMainHigh);
    
    //Schedules Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthSchedule-2, geoManager->menuItemHeightSchedule-2);
    InsertIntoSkinElementCache(seSchedules);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthSchedule-2, geoManager->menuItemHeightSchedule-2);
    InsertIntoSkinElementCache(seSchedulesHigh);
    
    //Channels Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthChannel-2, geoManager->menuItemHeightSchedule-2);
    InsertIntoSkinElementCache(seChannels);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthChannel-2, geoManager->menuItemHeightSchedule-2);
    InsertIntoSkinElementCache(seChannelsHigh);
    
    //Recordings Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthRecording-2, geoManager->menuItemHeightRecordings-2);
    InsertIntoSkinElementCache(seRecordings);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthRecording-2, geoManager->menuItemHeightRecordings-2);
    InsertIntoSkinElementCache(seRecordingsHigh);
    
    //Timers Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthTimer-2, geoManager->menuItemHeightSchedule-2);
    InsertIntoSkinElementCache(seTimers);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthTimer-2, geoManager->menuItemHeightSchedule-2);
    InsertIntoSkinElementCache(seTimersHigh);
    
    //Setup Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthSetup-2, geoManager->menuItemHeightMain-2);
    InsertIntoSkinElementCache(seSetup);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthSetup-2, geoManager->menuItemHeightMain-2);
    InsertIntoSkinElementCache(seSetupHigh);
    
    //DisplayTracks
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthTracks-2, geoManager->menuItemHeightTracks-2);
    InsertIntoSkinElementCache(seTracks);
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthTracks-2, geoManager->menuItemHeightTracks-2);
    InsertIntoSkinElementCache(seTracksHigh);
    
    //Color Buttons
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonRed), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    InsertIntoSkinElementCache(seButtonRed);
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonGreen), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    InsertIntoSkinElementCache(seButtonGreen);
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonYellow), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    InsertIntoSkinElementCache(seButtonYellow);
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonBlue), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    InsertIntoSkinElementCache(seButtonBlue);
    
    //Menu Header
    bool mirrorHeader = (config.GetValue("menuAdjustLeft")) ? false : true;
    CreateBackground(Theme.Color(clrMenuHeaderBlend), Theme.Color(clrMenuHeader), geoManager->osdWidth, geoManager->menuHeaderHeight, mirrorHeader);
    InsertIntoSkinElementCache(seMenuHeader);
}

void cImageCache::CreateSkinElementsGraphics(void) {
    bool success = false;
    
    //Default Menus
    std::string imgDefaultMenuItem = "skinElements/menubuttondefault";
    std::string imgDefaultMenuItemActive = "skinElements/menubuttondefaultactive";
    success = LoadIcon(ctSkinElement, imgDefaultMenuItem);
    if (success)
        InsertIntoSkinElementCache(seDefault, geoManager->menuItemWidthDefault, geoManager->menuItemHeightDefault);
    success = LoadIcon(ctSkinElement, imgDefaultMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seDefaultHigh, geoManager->menuItemWidthDefault, geoManager->menuItemHeightDefault);

    std::string imgMenuItem = "skinElements/menubutton";
    std::string imgMenuItemActive = "skinElements/menubuttonactive";
    std::string imgMenuItemTop = "skinElements/menubuttontop";
    //Main Menu
    success = LoadIcon(ctSkinElement, imgMenuItem);
    if (success)
        InsertIntoSkinElementCache(seMain, geoManager->menuItemWidthMain, geoManager->menuItemHeightMain);
    success = LoadIcon(ctSkinElement, imgMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seMainHigh, geoManager->menuItemWidthMain, geoManager->menuItemHeightMain);
    success = LoadIcon(ctSkinElement, imgMenuItemTop);
    if (success)
        InsertIntoSkinElementCache(seMainTop, geoManager->menuItemWidthMain, geoManager->menuItemHeightMain);

    //Setup Menu
    success = LoadIcon(ctSkinElement, imgMenuItem);
    if (success)
        InsertIntoSkinElementCache(seSetup, geoManager->menuItemWidthSetup, geoManager->menuItemHeightMain);
    success = LoadIcon(ctSkinElement, imgMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seSetupHigh, geoManager->menuItemWidthSetup, geoManager->menuItemHeightMain);
    success = LoadIcon(ctSkinElement, imgMenuItemTop);
    if (success)
        InsertIntoSkinElementCache(seSetupTop, geoManager->menuItemWidthSetup, geoManager->menuItemHeightMain);
    
    //Schedules Menu
    success = LoadIcon(ctSkinElement, imgMenuItem);
    if (success)
        InsertIntoSkinElementCache(seSchedules, geoManager->menuItemWidthSchedule, geoManager->menuItemHeightSchedule);
    success = LoadIcon(ctSkinElement, imgMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seSchedulesHigh, geoManager->menuItemWidthSchedule, geoManager->menuItemHeightSchedule);
    success = LoadIcon(ctSkinElement, imgMenuItemTop);
    if (success)
        InsertIntoSkinElementCache(seSchedulesTop, geoManager->menuItemWidthSchedule, geoManager->menuItemHeightSchedule);

    //Channels Menu
    success = LoadIcon(ctSkinElement, imgMenuItem);
    if (success)
        InsertIntoSkinElementCache(seChannels, geoManager->menuItemWidthChannel, geoManager->menuItemHeightSchedule);
    success = LoadIcon(ctSkinElement, imgMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seChannelsHigh, geoManager->menuItemWidthChannel, geoManager->menuItemHeightSchedule);
    success = LoadIcon(ctSkinElement, imgMenuItemTop);
    if (success)
        InsertIntoSkinElementCache(seChannelsTop, geoManager->menuItemWidthChannel, geoManager->menuItemHeightSchedule);
    
    //Recordings Menu
    success = LoadIcon(ctSkinElement, imgMenuItem);
    if (success)
        InsertIntoSkinElementCache(seRecordings, geoManager->menuItemWidthRecording, geoManager->menuItemHeightRecordings);
    success = LoadIcon(ctSkinElement, imgMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seRecordingsHigh, geoManager->menuItemWidthRecording, geoManager->menuItemHeightRecordings);
    success = LoadIcon(ctSkinElement, imgMenuItemTop);
    if (success)
        InsertIntoSkinElementCache(seRecordingsTop, geoManager->menuItemWidthRecording, geoManager->menuItemHeightRecordings);
    
    //Timers Menu
    success = LoadIcon(ctSkinElement, imgMenuItem);
    if (success)
        InsertIntoSkinElementCache(seTimers, geoManager->menuItemWidthTimer, geoManager->menuItemHeightSchedule);
    success = LoadIcon(ctSkinElement, imgMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seTimersHigh, geoManager->menuItemWidthTimer, geoManager->menuItemHeightSchedule);
    success = LoadIcon(ctSkinElement, imgMenuItemTop);
    if (success)
        InsertIntoSkinElementCache(seTimersTop, geoManager->menuItemWidthTimer, geoManager->menuItemHeightSchedule);
     
    //Tracks Menu
    success = LoadIcon(ctSkinElement, imgMenuItem);
    if (success)
        InsertIntoSkinElementCache(seTracks, geoManager->menuItemWidthTracks, geoManager->menuItemHeightTracks);
    success = LoadIcon(ctSkinElement, imgMenuItemActive);
    if (success)
        InsertIntoSkinElementCache(seTracksHigh, geoManager->menuItemWidthTracks, geoManager->menuItemHeightTracks);
    success = LoadIcon(ctSkinElement, imgMenuItemTop);
    if (success)
        InsertIntoSkinElementCache(seTracksTop, geoManager->menuItemWidthTracks, geoManager->menuItemHeightTracks);
    
    //Color Buttons
    std::string imgButtonRed = "skinElements/buttonred";
    std::string imgButtonGreen = "skinElements/buttongreen";
    std::string imgButtonYellow = "skinElements/buttonyellow";
    std::string imgButtonBlue = "skinElements/buttonblue";
    success = LoadIcon(ctSkinElement, imgButtonRed);
    if (success)
        InsertIntoSkinElementCache(seButtonRed, geoManager->menuButtonWidth, geoManager->menuButtonHeight);
    success = LoadIcon(ctSkinElement, imgButtonGreen);
    if (success)
        InsertIntoSkinElementCache(seButtonGreen, geoManager->menuButtonWidth, geoManager->menuButtonHeight);
    success = LoadIcon(ctSkinElement, imgButtonYellow);
    if (success)
        InsertIntoSkinElementCache(seButtonYellow, geoManager->menuButtonWidth, geoManager->menuButtonHeight);
    success = LoadIcon(ctSkinElement, imgButtonBlue);
    if (success)
        InsertIntoSkinElementCache(seButtonBlue, geoManager->menuButtonWidth, geoManager->menuButtonHeight);
    
    //Menu Header
    std::string imgHeader = "skinElements/header";
    success = LoadIcon(ctSkinElement, imgHeader);
    if (success)
        InsertIntoSkinElementCache(seMenuHeader, geoManager->osdWidth, geoManager->menuHeaderHeight);

    //Menu Messages Background
    std::string msgStatus = "skinElements/messageStatus";
    std::string msgInfo = "skinElements/messageInfo";
    std::string msgWarning = "skinElements/messageWarning";
    std::string msgError = "skinElements/messageError";
    success = LoadIcon(ctSkinElement, msgStatus);
    if (success)
        InsertIntoSkinElementCache(seMessageMenuStatus, geoManager->menuMessageWidth, geoManager->menuMessageHeight);
    success = LoadIcon(ctSkinElement, msgInfo);
    if (success)
        InsertIntoSkinElementCache(seMessageMenuInfo, geoManager->menuMessageWidth, geoManager->menuMessageHeight);
    success = LoadIcon(ctSkinElement, msgWarning);
    if (success)
        InsertIntoSkinElementCache(seMessageMenuWarning, geoManager->menuMessageWidth, geoManager->menuMessageHeight);
    success = LoadIcon(ctSkinElement, msgError);
    if (success)
        InsertIntoSkinElementCache(seMessageMenuError, geoManager->menuMessageWidth, geoManager->menuMessageHeight);
    //Messages Background
    success = LoadIcon(ctSkinElement, msgStatus);
    if (success)
        InsertIntoSkinElementCache(seMessageStatus, geoManager->messageWidth, geoManager->messageHeight);
    success = LoadIcon(ctSkinElement, msgInfo);
    if (success)
        InsertIntoSkinElementCache(seMessageInfo, geoManager->messageWidth, geoManager->messageHeight);
    success = LoadIcon(ctSkinElement, msgWarning);
    if (success)
        InsertIntoSkinElementCache(seMessageWarning, geoManager->messageWidth, geoManager->messageHeight);
    success = LoadIcon(ctSkinElement, msgError);
    if (success)
        InsertIntoSkinElementCache(seMessageError, geoManager->messageWidth, geoManager->messageHeight);


    //DisplayChannel Background and Foreground
    std::string imgChannelBackground;
    std::string imgChannelTop;
    switch (config.GetValue("logoPosition")) {
        case lpLeft:
            imgChannelBackground = "skinElements/displaychannelback";
            imgChannelTop = "skinElements/displaychanneltop";
            break;
        case lpRight:
            imgChannelBackground = "skinElements/displaychannelback_right";
            imgChannelTop = "skinElements/displaychanneltop_right";
            break;
        case lpNone:
            imgChannelBackground = "skinElements/displayreplayback";
            imgChannelTop = "skinElements/displayreplaytop";
            break;
    }
    success = LoadIcon(ctSkinElement, imgChannelBackground);
    if (success)
        InsertIntoSkinElementCache(seChannelBackground, geoManager->channelWidth, geoManager->channelHeight);
    
    success = LoadIcon(ctSkinElement, imgChannelTop);
    if (success)
        InsertIntoSkinElementCache(seChannelTop, geoManager->channelWidth, geoManager->channelHeight);

    //ChannelLogo Background
    std::string imgChannelLogoBack = "skinElements/channellogoback";
    success = LoadIcon(ctSkinElement, imgChannelLogoBack);
    if (success)
        InsertIntoSkinElementCache(seChannelLogoBack, geoManager->channelLogoWidth, geoManager->channelLogoHeight);

    //DisplayReplay Background and Foreground
    std::string imgReplayBackground = "skinElements/displayreplayback";
    success = LoadIcon(ctSkinElement, imgReplayBackground);
    if (success)
        InsertIntoSkinElementCache(seReplayBackground, geoManager->replayWidth, geoManager->replayHeight);
    std::string imgReplayTop = "skinElements/displayreplaytop";
    success = LoadIcon(ctSkinElement, imgReplayTop);
    if (success)
        InsertIntoSkinElementCache(seReplayTop, geoManager->replayWidth, geoManager->replayHeight);
    
    //DisplayVolume Background
    std::string imgVolumeBackground = "skinElements/displayvolume";
    success = LoadIcon(ctSkinElement, imgVolumeBackground);
    if (success)
        InsertIntoSkinElementCache(seVolumeBackground, geoManager->volumeWidth, geoManager->volumeHeight);
        
    //Scrolllbar
    std::string imgScrollbar = "skinElements/scrollbar";
    success = LoadIcon(ctSkinElement, imgScrollbar);
    if (success)
        InsertIntoSkinElementCache(seScrollbar, geoManager->menuWidthScrollbar, geoManager->menuContentHeight - 2 * geoManager->menuSpace);
}

void cImageCache::InsertIntoSkinElementCache(eSkinElementType type, int width, int height) {
    if (width>0 && height>0) {
        cString geometry = cString::sprintf("%dx%d!", width, height);
        buffer.resize(Geometry(*geometry));
    }
    cImage *image = CreateImage();
    skinElementCache.insert(std::pair<eSkinElementType, cImage*>(type, image));
}

void cImageCache::Clear(void) {
    for(std::map<std::string, cImage*>::const_iterator it = menuIconCache.begin(); it != menuIconCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    menuIconCache.clear();
    
    for(std::map<std::string, cImage*>::const_iterator it = skinIconCache.begin(); it != skinIconCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    skinIconCache.clear();
    
    for(std::map<eSkinElementType, cImage*>::const_iterator it = skinElementCache.begin(); it != skinElementCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    skinElementCache.clear();

    for(std::map<std::string, cImage*>::const_iterator it = logoCache.begin(); it != logoCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    logoCache.clear();
    
    for(std::map<std::string, cImage*>::const_iterator it = logoMenuItemCache.begin(); it != logoMenuItemCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    logoMenuItemCache.clear();
    
    for(std::map<std::string, cImage*>::const_iterator it = logoTimerCache.begin(); it != logoTimerCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    logoTimerCache.clear();
}