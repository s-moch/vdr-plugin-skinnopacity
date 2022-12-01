#include "symbols/mute.xpm"
#include "volumebox.h"
#include "displayvolume.h"

#include "config.h"
#include "helpers.h"

cNopacityDisplayVolume::cNopacityDisplayVolume(void) : cThread("DisplayVolume") {
    initial = true;
    fadeout = false;

    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->volumeHeight - config.GetValue("volumeBorderBottom");
    int left = geoManager->osdLeft + ((geoManager->osdWidth - geoManager->volumeWidth) / 2);
    osd = CreateOsd(left, top, geoManager->volumeWidth, geoManager->volumeHeight);
    volumeBox = new cNopacityVolumeBox(osd, cRect(0, 0, geoManager->volumeWidth, geoManager->volumeHeight), fontManager->volumeText);
}

cNopacityDisplayVolume::~cNopacityDisplayVolume(void) {
    if (config.GetValue("animation") && config.GetValue("volumeFadeOutTime")) {
        fadeout = true;
        Start();
    }
    int count = 0;
    while (Active()) {
        cCondWait::SleepMs(10);
        count++;
        if (count > 150)
           Cancel(1);
    }
    delete volumeBox;
    delete osd;
}

void cNopacityDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
    volumeBox->SetVolume(!Mute ? Current : 0, Total, Mute);
}

void cNopacityDisplayVolume::SetAlpha(int Alpha) {
    volumeBox->SetAlpha(Alpha);
}

void cNopacityDisplayVolume::Flush(void) {
    if (Running())
        return;
    if (initial && config.GetValue("animation") && config.GetValue("volumeFadeTime")) {
        SetAlpha(0);
        Start();
    }
    initial = false;
    osd->Flush();
}

void cNopacityDisplayVolume::Action(void) {
    int x = (fadeout) ? 255 : 0;
    int FadeTime = (fadeout) ? config.GetValue("volumeFadeOutTime") : config.GetValue("volumeFadeTime");
    int FrameTime = FadeTime / 10;
    uint64_t First = cTimeMs::Now();
    cPixmap::Lock();
    cPixmap::Unlock();
    uint64_t Start = cTimeMs::Now();
    dsyslog ("skinnopacity: First Lock(): %llums \n", (long long)(Start - First));
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        double t = std::min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = std::abs(x - (int(t * ALPHA_OPAQUE)));
        cPixmap::Lock();
        SetAlpha(Alpha);
        if (Running())
            osd->Flush();
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
