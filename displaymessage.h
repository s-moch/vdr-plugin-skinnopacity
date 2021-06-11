#ifndef __NOPACITY_DISPLAYMESSAGE_H
#define __NOPACITY_DISPLAYMESSAGE_H

#include <vdr/thread.h>
#include <vdr/skins.h>
#include "imagecache.h"
#include "messagebox.h"

class cNopacityDisplayMessage : public cSkinDisplayMessage , cThread {
private:
    cOsd *osd;
    cNopacityMessageBox *messageBox;
    bool fadeout;
    virtual void Action(void);
public:
  cNopacityDisplayMessage(void);
  virtual ~cNopacityDisplayMessage(void);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

#endif //__NOPACITY_DISPLAYMESSAGE_H
