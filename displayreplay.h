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
    cPixmap *pixmapBackground = NULL;
    cPixmap *pixmapTop = NULL;
    cPixmap *pixmapInfo = NULL;
    cPixmap *pixmapDate = NULL;
    cPixmap *pixmapInfo2 = NULL;
    cPixmap *pixmapProgressBar = NULL;
    cPixmap *pixmapCurrent = NULL;
    cPixmap *pixmapTotal = NULL;
    cPixmap *pixmapScreenRes = NULL;
    cPixmap *pixmapScreenResBackground = NULL;
    cPixmap *pixmapControls = NULL;
    cPixmap *pixmapRew = NULL;
    cPixmap *pixmapPause = NULL;
    cPixmap *pixmapPlay = NULL;
    cPixmap *pixmapFwd = NULL;
    cPixmap *pixmapJump = NULL;
    cPixmap *pixmapPoster = NULL;
    cNopacityMessageBox *messageBox = NULL;
    cNopacityVolumeBox *volumeBox = NULL;
    virtual void Action(void);
    void createOSD(void);
    void CreatePixmaps(void);
    void CreatePixmaps2(int x = 0);
    void SetAlpha(int Alpha = 0);
    void DrawBackground(void);
    void DrawDate(void);
    void LoadControlIcons(void);
    void DrawScreenResolution(void);
    void DrawVolume(void);
    int DrawPoster(const cRecording *Recording);
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
