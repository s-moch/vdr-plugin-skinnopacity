#ifndef __NOPACITY_DISPLAYVOLUME_H
#define __NOPACITY_DISPLAYVOLUME_H

class cNopacityDisplayVolume : public cSkinDisplayVolume, cThread {
private:
    int FrameTime;
    int FadeTime;
    bool initial;
    bool muted;
    cOsd *osd;
    cPixmap *pixmapBackgroundTop;
    cPixmap *pixmapBackgroundBottom;
    cPixmap *pixmapProgressBar;
    cPixmap *pixmapLabel;
    virtual void Action(void);
    void DrawProgressBar(int Current, int Total);
    tColor DrawProgressbarBackground(int left, int top, int width, int height);
public:
  cNopacityDisplayVolume(void);
  virtual ~cNopacityDisplayVolume();
  virtual void SetVolume(int Current, int Total, bool Mute);
  virtual void Flush(void);
  };
  
#endif //__NOPACITY_DISPLAYVOLUME_H