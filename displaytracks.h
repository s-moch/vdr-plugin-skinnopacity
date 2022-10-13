#ifndef __NOPACITY_DISPLAYTRACKS_H
#define __NOPACITY_DISPLAYTRACKS_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "menuitem.h"
#include <memory>
#include <vector>

class cNopacityDisplayTracks : public cSkinDisplayTracks, cThread {
private:
    cOsd *osd;
    bool fadeout;
    int width, height;
    int menuItemWidth;
    int menuItemHeight;
    bool initial;
    int currentIndex;
    int numTracks;
    int audioChannelLast;
    cPixmap *pixmapContainer = NULL;
    cPixmap *pixmapHeader = NULL;
    cPixmap *pixmapHeaderAudio = NULL;
    cPixmap *pixmapIcon = NULL;
    std::vector<std::unique_ptr<cNopacityMenuItem>> menuItems;
    virtual void Action(void);
    void SetItem(const char *Text, int Index, bool Current);
    void SetGeometry(void);
    void CreatePixmaps(void);
    void DrawHeader(const char *Title);
    void SetAlpha(int Alpha = 0, bool force = false);
public:
    cNopacityDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
    virtual ~cNopacityDisplayTracks();
    virtual void SetTrack(int Index, const char * const *Tracks);
    virtual void SetAudioChannel(int AudioChannel);
    virtual void Flush(void);
};


#endif //__NOPACITY_DISPLAYTRACKS_H
