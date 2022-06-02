#ifndef __NOPACITY_VOLUMEBOX_H
#define __NOPACITY_VOLUMEBOX_H

#include <vdr/skins.h>
#include "imagecache.h"

class cNopacityVolumeBox {
private:
  cOsd *osd;
  cPixmap *pixmapBackgroundVolume;
  cPixmap *pixmapProgressBarVolume;
  cPixmap *pixmapLabelVolume;
  void DrawProgressBar(int Current, int Total);
  tColor DrawProgressbarBackground(int left, int top, int width, int height);
public:
  cNopacityVolumeBox(cOsd *Osd, const cRect &Rect);
  ~cNopacityVolumeBox(void);
  void SetVolume(int Current = 0, int Total = 0, bool Mute = false);
  void SetAlpha(int Alpha = 0);
  };

#endif //__NOPACITY_VOLUMEBOX_H
