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

    barWidth = geoManager->volumeProgressBarWidth;
    barHeight = geoManager->volumeProgressBarHeight;
	    
    pixmapLabelVolume = osd->CreatePixmap(7, cRect(Rect.Left(), Rect.Top() + 5, geoManager->volumeWidth, geoManager->volumeLabelHeight));
    pixmapLabelVolume->Fill(clrTransparent);
    pixmapProgressBarVolume = osd->CreatePixmap(7, cRect((Rect.Left() + (geoManager->volumeWidth - barWidth) / 2), Rect.Top() + ((geoManager->volumeHeight - barHeight) * 2 / 3), barWidth, barHeight));
    pixmapProgressBarVolume->Fill(clrTransparent);
}

cNopacityVolumeBox::~cNopacityVolumeBox(void) {
    osd->DestroyPixmap(pixmapProgressBarVolume);
    osd->DestroyPixmap(pixmapLabelVolume);
    osd->DestroyPixmap(pixmapBackgroundVolume);
}

void cNopacityVolumeBox::SetVolume(int Current, int Total, bool Mute) {
    pixmapProgressBarVolume->Fill(clrTransparent);
    pixmapLabelVolume->Fill(clrTransparent);

    int barHeight = geoManager->volumeProgressBarHeight;
    if ((Current == 0) && (Total == 0) || (barHeight < 5)) {
        return;
    }

    cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
    pixmapLabelVolume->DrawText(cPoint((geoManager->volumeWidth - fontManager->volumeText->Width(*label)) / 2,
                                       (geoManager->volumeLabelHeight - fontManager->volumeText->Height()) / 2),
                                *label, Theme.Color(clrVolumeFont), clrTransparent, fontManager->volumeText);

    pixmapProgressBarVolume->DrawEllipse(cRect(0, 0, barHeight, barHeight), Theme.Color(clrProgressBarBack));
    pixmapProgressBarVolume->DrawEllipse(cRect(barWidth - barHeight, 0, barHeight, barHeight), Theme.Color(clrProgressBarBack));
    pixmapProgressBarVolume->DrawRectangle(cRect(barHeight / 2, 0, barWidth - barHeight, barHeight), Theme.Color(clrProgressBarBack));

    DrawProgressbar(pixmapProgressBarVolume, 1, 1, barWidth - 2, barHeight - 2, Current, Total, Theme.Color(clrProgressBar), Theme.Color(clrProgressBarBlend), true);

    if (Mute) { 
        cBitmap bmMute(mute_xpm);
        pixmapLabelVolume->DrawBitmap(cPoint(geoManager->volumeWidth - 2 * bmMute.Width(), (geoManager->volumeLabelHeight - bmMute.Height()) / 2), bmMute, Theme.Color(clrDiskAlert), clrTransparent);
    }
}

void cNopacityVolumeBox::SetAlpha(int Alpha) {
    pixmapBackgroundVolume->SetAlpha(Alpha);
    pixmapProgressBarVolume->SetAlpha(Alpha);
    pixmapLabelVolume->SetAlpha(Alpha);
}
