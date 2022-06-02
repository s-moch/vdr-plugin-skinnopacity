/*
 * status.h: Keeping track of several VDR status settings
 */

#ifndef __STATUS_H_
#define __STATUS_H_

#include <vdr/status.h>

class cNopacityStatusMonitor : public cStatus
{
private:
   int Volume = 0;
protected:
   virtual void SetVolume(int Volume, bool Absolute);
public:
   cNopacityStatusMonitor(void) {};
   ~cNopacityStatusMonitor(void) {};
   int GetVolume(void) { return Volume; };
};

extern cNopacityStatusMonitor *statusMonitor;

#endif
