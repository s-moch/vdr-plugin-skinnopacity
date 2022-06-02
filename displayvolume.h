#ifndef __NOPACITY_DISPLAYVOLUME_H
#define __NOPACITY_DISPLAYVOLUME_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "volumebox.h"
#include "imagecache.h"

class cNopacityDisplayVolume : public cSkinDisplayVolume, cThread {
private:
  bool initial;
  bool fadeout;
  cOsd *osd;
  cNopacityVolumeBox *volumeBox;
  void Action(void);
  void SetAlpha(int Alpha = 0);
public:
  cNopacityDisplayVolume(void);
  ~cNopacityDisplayVolume(void);
  virtual void SetVolume(int Current, int Total, bool Mute);
  virtual void Flush(void);
  };

#endif //__NOPACITY_DISPLAYVOLUME_H
