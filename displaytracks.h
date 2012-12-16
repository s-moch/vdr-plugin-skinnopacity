#ifndef __NOPACITY_DISPLAYTRACKS_H
#define __NOPACITY_DISPLAYTRACKS_H

class cNopacityDisplayTracks : public cSkinDisplayTracks, cThread {
private:
    cOsd *osd;
    int FrameTime;
    int FadeTime;
    int width, height;
    int menuItemWidth;
    int menuItemHeight;
    bool initial;
    int currentIndex;
    int numTracks;
    cPixmap *pixmapContainer;
    cPixmap *pixmapHeader;
    cPixmap *pixmapIcon;
    cList<cNopacityMenuItem> menuItems;
    int handleBackgrounds[2];
    cFont *font;
    cFont *fontHeader;
    virtual void Action(void);
    void SetItem(const char *Text, int Index, bool Current);
    static cBitmap bmStereo, bmDolbyDigital;
    void SetGeometry(void);
    void CreatePixmaps(void);
    void CreateFonts(void);
    void CreateBackgroundImages(void);
    void DrawHeader(const char *Title);
public:
    cNopacityDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
    virtual ~cNopacityDisplayTracks();
    virtual void SetTrack(int Index, const char * const *Tracks);
    virtual void SetAudioChannel(int AudioChannel);
    virtual void Flush(void);
};


#endif //__NOPACITY_DISPLAYTRACKS_H