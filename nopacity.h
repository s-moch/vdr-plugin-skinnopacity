/*
 * skinlcars.h: A VDR skin with Star Trek's "LCARS" layout
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: skinlcars.h 2.1 2012/04/15 13:17:35 kls Exp $
 */

#ifndef __NOPACITY_H
#define __NOPACITY_H

#include <vdr/skins.h>
#include <vdr/videodir.h>

class cNopacity : public cSkin {
public:
  cNopacity(void);
  virtual const char *Description(void);
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual cSkinDisplayMessage *DisplayMessage(void);
  };
#endif //__NOPACITY_H
