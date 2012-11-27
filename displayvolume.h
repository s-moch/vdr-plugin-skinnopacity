#ifndef __NOPACITY_DISPLAYVOLUME_H
#define __NOPACITY_DISPLAYVOLUME_H

class cNopacityDisplayVolume : public cSkinDisplayVolume, cThread {
private:
	int FrameTime;
	int FadeTime;
	bool initial;
	bool muted;
	int width, height;
	int labelHeight;
	int progressBarWidth, progressBarHeight;
	cOsd *osd;
	cPixmap *pixmapContent;
	cPixmap *pixmapProgressBar;
	cPixmap *pixmapLabel;
	cFont *font;
	virtual void Action(void);
public:
  cNopacityDisplayVolume(void);
  virtual ~cNopacityDisplayVolume();
  virtual void SetVolume(int Current, int Total, bool Mute);
  virtual void Flush(void);
  };
  
#endif //__NOPACITY_DISPLAYVOLUME_H