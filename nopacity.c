#include <vdr/osd.h>
#include <vdr/menu.h>

#include "nopacity.h"
#include "config.h"
#include "displaychannel.h"
#include "displaymenuview.h"
#include "displaymenu.h"
#include "displayreplay.h"
#include "displayvolume.h"
#include "displaytracks.h"
#include "displaymessage.h"

cNopacity::cNopacity(void) : cSkin("nOpacity", &::Theme) {
    geoManager = new cGeometryManager();
    fontManager = NULL;
    imgCache = NULL;
}

cNopacity::~cNopacity() {
    delete imgCache;
    delete geoManager;
    delete fontManager;
}

const char *cNopacity::Description(void) {
  return "nOpacity";
}

cSkinDisplayChannel *cNopacity::DisplayChannel(bool WithInfo) {
    ReloadCaches();
    return new cNopacityDisplayChannel(WithInfo);
}

cSkinDisplayMenu *cNopacity::DisplayMenu(void) {
    ReloadCaches();
    return new cNopacityDisplayMenu();
}

cSkinDisplayReplay *cNopacity::DisplayReplay(bool ModeOnly) {
  ReloadCaches();
  return new cNopacityDisplayReplay(ModeOnly);
}

cSkinDisplayVolume *cNopacity::DisplayVolume(void) {
  ReloadCaches();
  return new cNopacityDisplayVolume();
}

cSkinDisplayTracks *cNopacity::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) {
  ReloadCaches();
  return new cNopacityDisplayTracks(Title, NumTracks, Tracks);
}

cSkinDisplayMessage *cNopacity::DisplayMessage(void) {
  ReloadCaches();
  return new cNopacityDisplayMessage();
}

void cNopacity::ReloadCaches(void) {
    if (geoManager->SetOSDSize() || !imgCache || imgCache->ThemeChanged()) {
        int start = cTimeMs::Now();
        config.Init();
        geoManager->SetGeometry();
        delete fontManager;
        fontManager = new cFontManager();
        delete imgCache;
        imgCache = new cImageCache();
        dsyslog("nopacity: Cache reloaded in %d ms", int(cTimeMs::Now()-start));
    }
}
