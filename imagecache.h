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
    void CreateCache2(void);
    void Reload(void);
    bool ThemeChanged(void);
    cImage *GetMenuIcon(std::string name);
    cImage *GetSkinIcon(std::string name, int width=0, int height=0, bool preserveAspect = true);
    cImage *GetBackground(eBackgroundType type);
    cImage GetBackground(tColor back, tColor blend, int width, int height, bool flip = false);
private:
    std::string osdTheme;
    std::map<std::string, cImage*> menuIconCache;
    std::map<std::string, cImage*> skinIconCache;
    std::vector<cImage*> backgroundImages;
    std::vector<std::pair<std::string, cPoint> > GetMenuIcons(void);
    std::vector<std::pair<std::string, sImgProperties> > GetSkinIcons(void);
    bool LoadIcon(eCacheType type, std::string name, int width, int height, bool preserveAspect = true);
    void InsertIntoCache(eCacheType type, std::string name, int width, int height, bool preserveAspect = true);
    void CreateBackgroundImages(void);
    void Clear(void);
};

#endif //__NOPACITY_IMAGECACHE_H
