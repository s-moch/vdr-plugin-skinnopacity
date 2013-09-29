#ifndef __NOPACITY_DISPLAYREPLAY_H
#define __NOPACITY_DISPLAYREPLAY_H

class cNopacityDisplayReplay : public cSkinDisplayReplay , cThread{
private:
    cOsd *osd;
    cImageCache *imgCache;
    bool initial;
    bool modeOnly;
    cString lastDate;
    int FrameTime;
    int FadeTime;
    cPixmap *pixmapHeader;
    cPixmap *pixmapBackground;
    cPixmap *pixmapInfo;
    cPixmap *pixmapDate;
    cPixmap *pixmapInfo2;
    cPixmap *pixmapProgressBar;
    cPixmap *pixmapCurrent;
    cPixmap *pixmapTotal;
    cPixmap *pixmapScreenResolution;
    cPixmap *pixmapControls;
    cPixmap *pixmapRew;
    cPixmap *pixmapPause;
    cPixmap *pixmapPlay;
    cPixmap *pixmapFwd;
    cPixmap *pixmapJump;
    cPixmap *pixmapFooter;
    virtual void Action(void);
    void createOSD(void);
    void CreatePixmaps(void);
    void DrawBackground(void);
    void DrawDate(void);
    void LoadControlIcons(void);
    void DrawScreenResolution(void);
public:
  cNopacityDisplayReplay(cImageCache *imgCache, bool ModeOnly);
  virtual ~cNopacityDisplayReplay();
  virtual void SetRecording(const cRecording *Recording);
  virtual void SetTitle(const char *Title);
  virtual void SetMode(bool Play, bool Forward, int Speed);
  virtual void SetProgress(int Current, int Total);
  virtual void SetCurrent(const char *Current);
  virtual void SetTotal(const char *Total);
  virtual void SetJump(const char *Jump);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };
  
#endif //__NOPACITY_DISPLAYREPLAY_H