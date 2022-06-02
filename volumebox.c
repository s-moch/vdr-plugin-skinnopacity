#include "symbols/mute.xpm"
#include "displayvolume.h"

#include "config.h"
#include "helpers.h"

cNopacityVolumeBox::cNopacityVolumeBox(cOsd *osd, const cRect &Rect) {

    this->osd = osd;
    
    pixmapBackgroundVolume = osd->CreatePixmap(6, cRect(Rect));
    pixmapBackgroundVolume->Fill(clrTransparent);

    if (config.GetValue("displayType") == dtGraphical) {
        cImage *imgBack = imgCache->GetSkinElement(seVolumeBackground);
        if (imgBack) {
            pixmapBackgroundVolume->DrawImage(cPoint(0, 0), *imgBack);
        }
    } else {
        pixmapBackgroundVolume->Fill(Theme.Color(clrChannelBackground));
        if (config.GetValue("displayType") == dtBlending) {
            DrawBlendedBackground(pixmapBackgroundVolume,
                                  0,
                                  geoManager->volumeWidth,
                                  Theme.Color(clrChannelBackground),
                                  Theme.Color(clrChannelBackBlend),
                                  true);
            DrawBlendedBackground(pixmapBackgroundVolume,
                                  0,
                                  geoManager->volumeWidth,
                                  Theme.Color(clrChannelBackground),
                                  Theme.Color(clrChannelBackBlend),
                                  false);
        }
        int cornerRadius = geoManager->volumeHeight/4;
        if (cornerRadius > 2) {
            DrawRoundedCorners(pixmapBackgroundVolume,
                               cornerRadius,
                               0,
                               0,
                               geoManager->volumeWidth,
                               geoManager->volumeHeight);
        }
    }
    pixmapLabelVolume = osd->CreatePixmap(7, cRect(Rect.Left(), Rect.Top() + 5, geoManager->volumeWidth, geoManager->volumeLabelHeight));
    pixmapProgressBarVolume = osd->CreatePixmap(7, cRect((Rect.Left() + (geoManager->volumeWidth - geoManager->volumeProgressBarWidth) / 2), Rect.Top() + ((geoManager->volumeHeight - geoManager->volumeProgressBarHeight) * 2 / 3), geoManager->volumeProgressBarWidth, geoManager->volumeProgressBarHeight));
}

cNopacityVolumeBox::~cNopacityVolumeBox(void) {
    osd->DestroyPixmap(pixmapProgressBarVolume);
    osd->DestroyPixmap(pixmapLabelVolume);
    osd->DestroyPixmap(pixmapBackgroundVolume);
}

void cNopacityVolumeBox::SetVolume(int Current, int Total, bool Mute) {
    pixmapLabelVolume->Fill(clrTransparent);
    cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
    pixmapLabelVolume->DrawText(cPoint((geoManager->volumeWidth - fontManager->volumeText->Width(*label)) / 2, (geoManager->volumeLabelHeight - fontManager->volumeText->Height()) / 2), *label, Theme.Color(clrVolumeFont), clrTransparent, fontManager->volumeText);
    if (Mute) {
        cBitmap bmMute(mute_xpm);
        pixmapLabelVolume->DrawBitmap(cPoint(geoManager->volumeWidth - 2 * bmMute.Width(), (geoManager->volumeLabelHeight - bmMute.Height()) / 2), bmMute, Theme.Color(clrDiskAlert), clrTransparent);
    }
        DrawProgressBar(Current, Total);
}

void cNopacityVolumeBox::DrawProgressBar(int Current, int Total) {
    pixmapProgressBarVolume->Fill(clrTransparent);
    if (geoManager->volumeProgressBarHeight < 5)
        return;
    double percent = ((double)Current) / (double)Total;
    int barWidth = geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight;
    if ((Current > 0) || (Total > 0)) {
        pixmapProgressBarVolume->DrawEllipse(cRect(0, 0, geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBarVolume->DrawEllipse(cRect(geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight, 0, geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBarVolume->DrawRectangle(cRect(geoManager->volumeProgressBarHeight / 2, 0, geoManager->volumeProgressBarWidth - geoManager->volumeProgressBarHeight, geoManager->volumeProgressBarHeight), Theme.Color(clrProgressBarBack));

        pixmapProgressBarVolume->DrawEllipse(cRect(1, 1, geoManager->volumeProgressBarHeight - 2, geoManager->volumeProgressBarHeight - 2), Theme.Color(clrProgressBarBlend));
        if (Current > 0) {
            tColor colAct = DrawProgressbarBackground(geoManager->volumeProgressBarHeight / 2 - 1, 1, barWidth * percent - 2, geoManager->volumeProgressBarHeight - 2);
            pixmapProgressBarVolume->DrawEllipse(cRect(barWidth * percent, 1, geoManager->volumeProgressBarHeight - 2, geoManager->volumeProgressBarHeight - 2), colAct);
        }
    }
}

tColor cNopacityVolumeBox::DrawProgressbarBackground(int left, int top, int width, int height) {

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
        pixmapProgressBarVolume->DrawRectangle(cRect(x, top, step + 1, height), clr);
        alpha += alphaStep;
    }
    return clr;
}

void cNopacityVolumeBox::SetAlpha(int Alpha) {
    pixmapBackgroundVolume->SetAlpha(Alpha);
    pixmapProgressBarVolume->SetAlpha(Alpha);
    pixmapLabelVolume->SetAlpha(Alpha);
}
