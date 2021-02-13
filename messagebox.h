#ifndef __NOPACITY_MESSAGEBOX_H
#define __NOPACITY_MESSAGEBOX_H

#include <vdr/skins.h>
#include "imagecache.h"

class cNopacityMessageBox {
private:
  cOsd *osd;
  cPixmap *pixmap;
  cPixmap *pixmapBackground;
public:
  cNopacityMessageBox(cOsd *Osd, cImageCache *imgCache, const cRect &Rect, eMessageType Type, const char *Text, bool isMenuMessage = false);
  ~cNopacityMessageBox();
  void SetAlpha(int Alpha);
  };

#endif //__NOPACITY_MESSAGEBOX_H
