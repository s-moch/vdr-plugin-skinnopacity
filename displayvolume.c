#include "symbols/mute.xpm"
#include "displayvolume.h"

#include "config.h"
#include "helpers.h"

cNopacityDisplayVolume::cNopacityDisplayVolume(void) : cThread("DisplayVolume") {
    initial = true;
    fadeout = false;
    muted = false;

    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->volumeHeight - config.GetValue("volumeBorderBottom");
    int left = geoManager->osdLeft + (geoManager->osdWidth - geoManager->volumeWidth) / 2;
    osd = CreateOsd(left, top, geoManager->volumeWidth, geoManager->volumeHeight);

    pixmapBackground = osd->CreatePixmap(1, cRect(0, 0, geoManager->volumeWidth, geoManager->volumeHeight));

    if (config.GetValue("displayType") == dtGraphical) {
        cImage *imgBack = imgCache->GetSkinElement(seVolumeBackground);
        if (imgBack) {
            pixmapBackground->DrawImage(cPoint(0, 0), *imgBack);
        }
    } else {
        pixmapBackground->Fill(Theme.Color(clrChannelBackground));
        if (config.GetValue("displayType") == dtBlending) {
            DrawBlendedBackground(pixmapBackground,
                                  0,
                                  geoManager->volumeWidth,
                                  Theme.Color(clrChannelBackground),
                                  Theme.Color(clrChannelBackBlend),
                                  true);
            DrawBlendedBackground(pixmapBackground,
                                  0,
                                  geoManager->volumeWidth,
                                  Theme.Color(clrChannelBackground),
                                  Theme.Color(clrChannelBackBlend),
                                  false);
        }
        int cornerRadius = geoManager->volumeHeight/4;
        if (cornerRadius > 2) {
            DrawRoundedCorners(pixmapBackground,
                               cornerRadius,
                               0,
                               0,
                               geoManager->volumeWidth,
                               geoManager->volumeHeight);
        }
    }
    pixmapLabel = osd->CreatePixmap(2, cRect(0, 5, geoManager->volumeWidth, geoManager->volumeLabelHeight));
    pixmapProgressBar = osd->CreatePixmap(2, cRect((geoManager->volumeWidth - geoManager->volumeProgressBarWidth) / 2, (geoManager->volumeHeight - geoManager->volumeProgressBarHeight)*2/3, geoManager->volumeProgressBarWidth, geoManager->volumeProgressBarHeight));
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
    osd->DestroyPixmap(pixmapBackground);
    osd->DestroyPixmap(pixmapLabel);
    osd->DestroyPixmap(pixmapProgressBar);
    delete osd;
}

void cNopacityDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
    pixmapLabel->Fill(clrTransparent);
    cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
    pixmapLabel->DrawText(cPoint((geoManager->volumeWidth - fontManager->volumeText->Width(*label)) / 2, (geoManager->volumeLabelHeight - fontManager->volumeText->Height()) / 2), *label, Theme.Color(clrVolumeFont), clrTransparent, fontManager->volumeText);
    if (Mute) {
        cBitmap bmMute(mute_xpm);
        pixmapLabel->DrawBitmap(cPoint(geoManager->volumeWidth - 2*bmMute.Width(), (geoManager->volumeLabelHeight - bmMute.Height()) / 2), bmMute, Theme.Color(clrDiskAlert), clrTransparent);
    }
        DrawProgressBar(Current, Total);
}

void cNopacityDisplayVolume::DrawProgressBar(int Current, int Total) {
    pixmapProgressBar->Fill(clrTransparent);
    if (geoManager->volumeProgressBarHeight < 5)
        return;
    double percent = ((double)Current) / (double)Total;
    int barWidth = geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight;
    if ((Current > 0) || (Total > 0)) {
        pixmapProgressBar->DrawEllipse(cRect(0, 0, geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawEllipse(cRect(geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight, 0, geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawRectangle(cRect(geoManager->volumeProgressBarHeight/2, 0, geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));

        pixmapProgressBar->DrawEllipse(cRect(1, 1, geoManager->volumeProgressBarHeight-2, geoManager->volumeProgressBarHeight-2), Theme.Color(clrProgressBarBlend));
        if (Current > 0) {
            tColor colAct = DrawProgressbarBackground(geoManager->volumeProgressBarHeight / 2 - 1, 1, barWidth * percent - 2, geoManager->volumeProgressBarHeight - 2);
            pixmapProgressBar->DrawEllipse(cRect(barWidth * percent, 1, geoManager->volumeProgressBarHeight-2, geoManager->volumeProgressBarHeight-2), colAct);
        }
    }
}

tColor cNopacityDisplayVolume::DrawProgressbarBackground(int left, int top, int width, int height) {

    tColor clr1 = Theme.Color(clrProgressBar);
    tColor clr2 = Theme.Color(clrProgressBarBlend);
    tColor clr = 0x00000000;

    int alpha = 0x0;                                    // 0...255
    int alphaStep = 0x1;
    int maximumsteps = 256;                             // alphaStep * maximumsteps <= 256
    int factor = 2;                                     // max. 128 steps

    double step = width / maximumsteps;
    if (width < 128) {                                  // width < 128
        factor = 4 * factor;                            // 32 steps
    } else if (width < 256) {                           // width < 256
        factor = 2 * factor;                            // 64 steps
    }

    step = step * factor;
    alphaStep = alphaStep * factor;
    maximumsteps = maximumsteps / factor;

    int x = left + height / 2;
    for (int i = 0; i < maximumsteps; i++) {
        x = left + height / 2 + i * step;
        clr = AlphaBlend(clr1, clr2, alpha);
        pixmapProgressBar->DrawRectangle(cRect(x, top, step + 1, height), clr);
        alpha += alphaStep;
    }
    return clr;
}

void cNopacityDisplayVolume::SetAlpha(int Alpha) {
    pixmapBackground->SetAlpha(Alpha);
    pixmapProgressBar->SetAlpha(Alpha);
    pixmapLabel->SetAlpha(Alpha);
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
    dsyslog ("skinnopacity: First Lock(): %lims \n", Start - First);
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
