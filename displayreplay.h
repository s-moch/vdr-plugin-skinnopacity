#ifndef __NOPACITY_DISPLAYREPLAY_H
#define __NOPACITY_DISPLAYREPLAY_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "imagecache.h"
#include "messagebox.h"
#include "volumebox.h"

class cNopacityDisplayReplay : public cSkinDisplayReplay , cThread{
private:
    cOsd *osd;
    bool initial;
    bool modeOnly;
    cString lastDate;
    bool fadeout;
    int lastVolume;
    time_t lastVolumeTime;
    cPixmap *pixmapBackground;
    cPixmap *pixmapTop;
    cPixmap *pixmapInfo;
    cPixmap *pixmapDate;
    cPixmap *pixmapInfo2;
    cPixmap *pixmapProgressBar;
    cPixmap *pixmapCurrent;
    cPixmap *pixmapTotal;
    cPixmap *pixmapScreenRes;
    cPixmap *pixmapScreenResBackground;
    cPixmap *pixmapControls;
    cPixmap *pixmapRew;
    cPixmap *pixmapPause;
    cPixmap *pixmapPlay;
    cPixmap *pixmapFwd;
    cPixmap *pixmapJump;
    cNopacityMessageBox *messageBox;
    cNopacityVolumeBox *volumeBox;
    virtual void Action(void);
    void createOSD(void);
    void CreatePixmaps(void);
    void SetAlpha(int Alpha = 0);
    void DrawBackground(void);
    void DrawDate(void);
    void LoadControlIcons(void);
    void DrawScreenResolution(void);
    void DrawVolume(void);
public:
  cNopacityDisplayReplay(bool ModeOnly);
  virtual ~cNopacityDisplayReplay(void);
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
