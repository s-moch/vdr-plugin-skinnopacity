#ifndef __NOPACITY_IMAGECACHE_H
#define __NOPACITY_IMAGECACHE_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>

using namespace Magick;

enum eCacheType {   
    ctMenuIcon = 0, 
    ctSkinIcon,
    ctLogo,
    ctLogoMenuItem,
    ctLogoTimer,
    ctBackground,
};
enum eBackgroundType {
    btNone = -1,
    btDefault = 0, 
    btDefaultHigh,
    btMain,
    btMainHigh,
    btSchedules,
    btSchedulesHigh,
    btChannels,
    btChannelsHigh,
    btRecordings,
    btRecordingsHigh,
    btTimers,
    btTimersHigh,
    btSetup,
    btSetupHigh,
    btTracks,
    btTracksHigh,
    btButtonRed,
    btButtonGreen,
    btButtonYellow,
    btButtonBlue,
    btMenuHeader,
};

struct sImgProperties {
    int width;
    int height;
    bool preserveAspect;
};

class cImageCache : public cImageMagickWrapper {
public:
    cImageCache();
    ~cImageCache();
    void CreateCache(void);
    void CreateCacheDelayed(void);
    void Reload(void);
    bool ThemeChanged(void);
    cImage *GetMenuIcon(std::string name);
    cImage *GetSkinIcon(std::string name, int width=0, int height=0, bool preserveAspect = true);
    cImage *GetLogo(eCacheType type, const cChannel *channel);
    cImage *GetBackground(eBackgroundType type);
    cImage GetBackground(tColor back, tColor blend, int width, int height, bool flip = false);
    std::string GetCacheSize(eCacheType type);
private:
    bool initComplete;
    cImage *tempStaticLogo;
    std::string osdTheme;
    std::map<std::string, cImage*> menuIconCache;
    std::map<std::string, cImage*> skinIconCache;
    std::map<int, cImage*> logoCache;
    std::map<int, cImage*> logoMenuItemCache;
    std::map<int, cImage*> logoTimerCache;
    std::vector<cImage*> backgroundImages;
    std::vector<std::pair<std::string, cPoint> > GetMenuIcons(void);
    std::vector<std::pair<std::string, sImgProperties> > GetSkinIcons(void);
    bool LoadIcon(eCacheType type, std::string name, int width, int height, bool preserveAspect = true);
    void InsertIntoIconCache(eCacheType type, std::string name, int width, int height, bool preserveAspect = true);
    bool LoadLogo(const cChannel *channel);
    void InsertIntoLogoCache(eCacheType type, int channelNumber);
    cPoint LogoSize(eCacheType type);
    void CreateBackgroundImages(void);
    void Clear(void);
};

#endif //__NOPACITY_IMAGECACHE_H
