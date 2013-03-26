#ifndef __NOPACITY_DISPLAYMESSAGE_H
#define __NOPACITY_DISPLAYMESSAGE_H
class cNopacityDisplayMessage : public cSkinDisplayMessage , cThread {
private:
    cOsd *osd;
    int width;
    int height;
    cPixmap *pixmap;
    cPixmap *pixmapBackground;
    cFont *font;
    int FrameTime;
    int FadeTime;
    virtual void Action(void);
public:
  cNopacityDisplayMessage(void);
  virtual ~cNopacityDisplayMessage();
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

#endif //__NOPACITY_DISPLAYMESSAGE_H