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
    init = true;
    config.LoadThemeSpecificConfigs();
    config.SetThemeSpecificDefaults();
    config.SetThemeSetup();
    config.SetPathes();
    config.SetFontName();
    geoManager = new cGeometryManager();
    geoManager->SetGeometry();
    fontManager = new cFontManager();
    fontManager->SetFonts();
    imgCache = new cImageCache();
    imgCache->CreateCache();
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
    if (init) {
        imgCache->CreateCacheDelayed();
        init = false;
    }
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
    if (geoManager->GeometryChanged() || imgCache->ThemeChanged()) {
        int start = cTimeMs::Now();
        config.LoadDefaults();
        config.SetThemeSpecificDefaults();
        config.SetThemeSetup();
        config.SetFontName();
        geoManager->SetGeometry();
        fontManager->DeleteFonts();
        fontManager->SetFonts();
        imgCache->Reload();
        dsyslog("nopacity: Cache reloaded in %d ms", int(cTimeMs::Now()-start));
    }
}
