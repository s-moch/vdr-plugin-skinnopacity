#ifndef __NOPACITY_DISPLAYREPLAY_H
#define __NOPACITY_DISPLAYREPLAY_H

class cNopacityDisplayReplay : public cSkinDisplayReplay , cThread{
private:
    cOsd *osd;
    bool initial;
    bool modeOnly;
    cString lastDate;
    int width;
    int height;
    int headerHeight;
    int info2Height;
    int progressBarHeight;
    int currentHeight;
    int controlsHeight;
    int footerHeight;
    int infoWidth;
    int dateWidth;
    int iconSize, iconBorder;
    int jumpX, jumpY; 
    int jumpWidth, jumpHeight;
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
    cPixmap *pixmapControls;
    cPixmap *pixmapRew;
    cPixmap *pixmapRewSpeed;
    cPixmap *pixmapPause;
    cPixmap *pixmapPlay;
    cPixmap *pixmapFwd;
    cPixmap *pixmapFwdSpeed;
    cPixmap *pixmapJump;
    cPixmap *pixmapFooter;
    cFont *fontReplayHeader;
    cFont *fontReplay;
    virtual void Action(void);
    void SetGeometry(void);
    void CreatePixmaps(void);
    void CreateFonts(void);
    void DrawBackground(void);
    void DrawDate(void);
    void LoadControlIcons(void);
public:
  cNopacityDisplayReplay(bool ModeOnly);
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