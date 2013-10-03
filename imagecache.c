#include <string>
#include <sstream>
#include <map>
#include "imagecache.h"

using namespace Magick;

cImageCache::cImageCache() : cImageMagickWrapper() {
    initComplete = false;
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
        LoadIcon(ctMenuIcon, iconName, iconSize.X(), iconSize.X());
    }
    //Skin Icons
    std::vector<std::pair<std::string, sImgProperties> > skinIcons = GetSkinIcons();
    for (int i=0; i < skinIcons.size(); i++) {
        std::string iconName = skinIcons[i].first;
        sImgProperties iconProps = skinIcons[i].second;
        LoadIcon(ctSkinIcon, iconName, iconProps.width, iconProps.height, iconProps.preserveAspect);
    }
    //Channel Logos
    if (config.numLogosInitial > 0) {
        int channelsCached = 0;
        for (const cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
            if (channelsCached >= config.numLogosInitial)
                break;
            if (!channel->GroupSep()) {
                bool success = LoadLogo(channel);
                if (success) {
                    channelsCached++;
                    InsertIntoLogoCache(ctLogo, channel->Number());
                }
                success = LoadLogo(channel);
                if (success) {
                    InsertIntoLogoCache(ctLogoMenuItem, channel->Number());
                }
            }
            
        }
    }
}

void cImageCache::CreateCacheDelayed(void) {
    CreateBackgroundImages();
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
        int iconWidth = config.iconHeight;
        int iconHeight = config.iconHeight;
        bool success = LoadIcon(ctMenuIcon, name, iconWidth, iconHeight);
        if (success) {
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
        bool success = LoadIcon(ctSkinIcon, name, width, height, preserveAspect);
        if (success) {
            hit = skinIconCache.find(name);
            if (hit != skinIconCache.end()) {
                return (cImage*)hit->second;
            }
        }
    }
    return NULL;
}

cImage *cImageCache::GetLogo(eCacheType type, const cChannel *channel) {
    if (!channel)
        return NULL;
        
    std::map<int, cImage*> *cache;
    if (type == ctLogo)
        cache = &logoCache;
    else if (type == ctLogoMenuItem)
        cache = &logoMenuItemCache;
    else if (type == ctLogoTimer)
        cache = &logoTimerCache;
        
    std::map<int, cImage*>::iterator hit = cache->find(channel->Number());
    
    if (hit != cache->end()) {
        return (cImage*)hit->second;
    } else {
        bool success = LoadLogo(channel);
        if (success) {
            if (config.limitLogoCache && (cache->size() >= config.numLogosMax)) {
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
                InsertIntoLogoCache(type, channel->Number());
                hit = cache->find(channel->Number());
                if (hit != cache->end()) {
                    return (cImage*)hit->second;
                }
            }
        }
    }
    return NULL;
}

cImage *cImageCache::GetBackground(eBackgroundType type) {
    if (!initComplete)
        return NULL;
    try {
        return backgroundImages[(int)type];
    } catch (...) {
        esyslog("nopacity: trying to get undefined Background Image %d", (int)type);
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
        std::map<int, cImage*> *cache;
        if (type == ctLogo)
            cache = &logoCache;
        else if (type == ctLogoMenuItem)
            cache = &logoMenuItemCache;
        else if (type == ctLogoTimer)
            cache = &logoTimerCache;

        for(std::map<int, cImage*>::const_iterator it = cache->begin(); it != cache->end(); it++) {
            cImage *img = (cImage*)it->second;
            sizeByte += img->Width() * img->Height() * sizeof(tColor);
        }
        numImages = cache->size();
    } else if (type == ctBackground) {
        for(std::vector<cImage*>::const_iterator it = backgroundImages.begin(); it != backgroundImages.end(); it++) {
            cImage *img = (cImage*)*it;
            sizeByte += img->Width() * img->Height() * sizeof(tColor);
        }
        numImages = backgroundImages.size();
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
    int mainMenuIconSize = config.iconHeight;
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
    props.width = config.menuHeaderLogoWidth;
    props.height = config.menuHeaderLogoWidth;
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
    //RSS Feed
    int rssFeedHeight = geoManager->osdHeight * config.rssFeedHeight / 100;
    props.width = props.height = rssFeedHeight - 4;
    props.preserveAspect = true;
    skinIcons.push_back(std::pair<std::string, sImgProperties>("skinIcons/rss", props));
    //menu header icons
    std::vector<std::string> menuHeaderIcons;
    menuHeaderIcons.push_back("menuIcons/Schedule");
    menuHeaderIcons.push_back("menuIcons/Channels");
    menuHeaderIcons.push_back("menuIcons/Timers");
    menuHeaderIcons.push_back("menuIcons/Recordings");
    menuHeaderIcons.push_back("menuIcons/Setup");
    menuHeaderIcons.push_back("menuIcons/Commands");
    props.width = config.headerIconHeight;
    props.height = config.headerIconHeight;
    props.preserveAspect = true;
    for (int i=0; i<menuHeaderIcons.size(); i++) {
        skinIcons.push_back(std::pair<std::string, sImgProperties>(menuHeaderIcons[i], props));
    }
    
    //displaychannel Icons
    int iconSize = config.statusIconSize;
    props.width = iconSize*5;
    props.height = iconSize;
    props.preserveAspect = false;
    skinIcons.push_back(std::pair<std::string, sImgProperties>("skinIcons/channelsymbols", props));
    
    return skinIcons;
}

bool cImageCache::LoadIcon(eCacheType type, std::string name, int width, int height, bool preserveAspect) {
    bool success = false;
    if (config.iconPathSet) {
        cString iconPathTheme = cString::sprintf("%s%s/", *config.iconPath, Setup.OSDTheme);
        success = LoadImage(name, *iconPathTheme, "png");
        if (success) {
            InsertIntoIconCache(type, name, width, height, preserveAspect);
        } else {
            success = LoadImage(name, *config.iconPath, "png");
            if (success) {
                InsertIntoIconCache(type, name, width, height, preserveAspect);
            }
        }
    } 
    if (!success) {
        cString iconPathTheme = cString::sprintf("%s%s/", *config.iconPathDefault, Setup.OSDTheme);
        success = LoadImage(name, *iconPathTheme, "png");
        if (success) {
            InsertIntoIconCache(type, name, width, height, preserveAspect);
        } else {
            success = LoadImage(name, *config.iconPathDefault, "png");
            if (success) {
                InsertIntoIconCache(type, name, width, height, preserveAspect);
            }
        }
    }
    return success;
}

void cImageCache::InsertIntoIconCache(eCacheType type, std::string name, int width, int height, bool preserveAspect) {
    if (preserveAspect) {
        buffer.sample(Geometry(width, height));
    } else {
        cString geometry = cString::sprintf("%dx%d!", width, height);
        buffer.scale(Geometry(*geometry));
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

void cImageCache::InsertIntoLogoCache(eCacheType type, int channelNumber) {
    cPoint logoSize = LogoSize(type);
    int width = logoSize.X();
    int height = logoSize.Y();
    buffer.sample(Geometry(width, height));
    cImage *image = CreateImage();
    if (type == ctLogo)
        logoCache.insert(std::pair<int, cImage*>(channelNumber, image));
    else if (type == ctLogoMenuItem)
        logoMenuItemCache.insert(std::pair<int, cImage*>(channelNumber, image));
    else if (type == ctLogoTimer)
        logoTimerCache.insert(std::pair<int, cImage*>(channelNumber, image));
}

cPoint cImageCache::LogoSize(eCacheType type) {
    int width, height;
    switch (type) {
        case ctLogo:
            width = config.logoWidth;
            height = config.logoHeight;
            break;
        case ctLogoMenuItem:
            width = config.menuItemLogoWidth;
            height = config.menuItemLogoHeight;
            break;
        case ctLogoTimer:
            width = config.timersLogoWidth;
            height = config.timersLogoHeight;
            break;
        default:
            width = 1;
            height = 1;
    }
    return cPoint(width, height);
}

void cImageCache::CreateBackgroundImages(void) {
    //Default Menus
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthDefault-2, geoManager->menuItemHeightDefault-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthDefault-2, geoManager->menuItemHeightDefault-2);
    backgroundImages.push_back(CreateImage());
    
    //Main Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthMain-2, geoManager->menuItemHeightMain-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthMain-2, geoManager->menuItemHeightMain-2);
    backgroundImages.push_back(CreateImage());
    
    //Schedules Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthSchedule-2, geoManager->menuItemHeightSchedule-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthSchedule-2, geoManager->menuItemHeightSchedule-2);
    backgroundImages.push_back(CreateImage());
    
    //Channels Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthChannel-2, geoManager->menuItemHeightSchedule-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthChannel-2, geoManager->menuItemHeightSchedule-2);
    backgroundImages.push_back(CreateImage());
    
    //Recordings Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthRecording-2, geoManager->menuItemHeightRecordings-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthRecording-2, geoManager->menuItemHeightRecordings-2);
    backgroundImages.push_back(CreateImage());
    
    //Timers Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthTimer-2, geoManager->menuItemHeightSchedule-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthTimer-2, geoManager->menuItemHeightSchedule-2);
    backgroundImages.push_back(CreateImage());
    
    //Setup Menu
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthSetup-2, geoManager->menuItemHeightMain-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthSetup-2, geoManager->menuItemHeightMain-2);
    backgroundImages.push_back(CreateImage());
    
    //DisplayTracks
    CreateBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), geoManager->menuItemWidthTracks-2, geoManager->menuItemHeightTracks-2);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), geoManager->menuItemWidthTracks-2, geoManager->menuItemHeightTracks-2);
    backgroundImages.push_back(CreateImage());
    
    //Color Buttons
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonRed), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonGreen), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonYellow), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    backgroundImages.push_back(CreateImage());
    CreateBackground(Theme.Color(clrMenuBack), Theme.Color(clrButtonBlue), geoManager->menuButtonWidth-4, geoManager->menuButtonHeight-4);
    backgroundImages.push_back(CreateImage());
    
    //Menu Header
    bool mirrorHeader = (config.menuAdjustLeft) ? false : true;
    CreateBackground(Theme.Color(clrMenuHeaderBlend), Theme.Color(clrMenuHeader), geoManager->osdWidth, geoManager->menuHeaderHeight, mirrorHeader);
    backgroundImages.push_back(CreateImage());
    
    initComplete = true;
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
    for(std::vector<cImage*>::const_iterator it = backgroundImages.begin(); it != backgroundImages.end(); it++) {
        cImage *img = (cImage*)*it;
        delete img;
    }
    backgroundImages.clear();
    for(std::map<int, cImage*>::const_iterator it = logoCache.begin(); it != logoCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    logoCache.clear();
    for(std::map<int, cImage*>::const_iterator it = logoMenuItemCache.begin(); it != logoMenuItemCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    logoMenuItemCache.clear();
    for(std::map<int, cImage*>::const_iterator it = logoTimerCache.begin(); it != logoTimerCache.end(); it++) {
        cImage *img = (cImage*)it->second;
        delete img;
    }
    logoTimerCache.clear();
}