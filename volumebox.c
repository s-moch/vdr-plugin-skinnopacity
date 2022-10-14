#include "symbols/mute.xpm"
#include "displayvolume.h"

#include "config.h"
#include "helpers.h"

cNopacityVolumeBox::cNopacityVolumeBox(cOsd *osd, const cRect &Rect, cFont *Font, bool simple, bool altcolor) {

    this->osd = osd;
    font = Font;
    this->simple = simple;
    barWidth = 0;
    barHeight = 0;
    textLeft = 0;
    textTop = 0;

    cString testlabel = cString::sprintf("%s: %s", tr("Volume"), "999");
    if (simple) {
        if (altcolor) {
            tColor altBackgroundColor = Theme.Color(clrChannelBackground);
            pixmapBackgroundVolume = CreatePixmap(osd, "pixmapBackgroundVolume", 6, cRect(Rect.Left() - Rect.Height() / 4, Rect.Top(), Rect.Width() + Rect.Height() / 2, Rect.Height()));
            PixmapFill(pixmapBackgroundVolume, clrTransparent);
            if (pixmapBackgroundVolume) {
                pixmapBackgroundVolume->DrawEllipse(cRect(0, 0, Rect.Height() / 2, Rect.Height()), altBackgroundColor, 7);
                pixmapBackgroundVolume->DrawEllipse(cRect(Rect.Width(), 0, Rect.Height() / 2, Rect.Height()), altBackgroundColor, 5);
                pixmapBackgroundVolume->DrawRectangle(cRect(Rect.Height() / 2, 0, Rect.Width() - Rect.Height() / 2, Rect.Height()), altBackgroundColor);
            }
        } else {
            pixmapBackgroundVolume = CreatePixmap(osd, "pixmapBackgroundVolume", 6, cRect(Rect));
            PixmapFill(pixmapBackgroundVolume, clrTransparent);
        }
	int textHeight = font->Height();
        int textWidth = font->Width(*testlabel);
        pixmapLabelVolume = CreatePixmap(osd, "pixmapLabelVolume", 7, cRect(Rect.Left(), Rect.Top() + (Rect.Height() - textHeight) / 2, textWidth, textHeight));
        PixmapFill(pixmapLabelVolume, clrTransparent);

        barWidth = Rect.Width() - textWidth - 10;
        barHeight = Rect.Height() * 2 / 3;
        if (barHeight % 2 != 0)
            barHeight++;
        int barTop = (Rect.Height() - barHeight) / 2 + 1;
        pixmapProgressBarVolume = CreatePixmap(osd, "pixmapProgressBarVolume", 7, cRect(Rect.Left() + textWidth + 10, Rect.Top() + barTop, barWidth, barHeight));
        PixmapFill(pixmapProgressBarVolume, clrTransparent);
    } else {
        pixmapBackgroundVolume = CreatePixmap(osd, "pixmapBackgroundVolume", 6, cRect(Rect));
        PixmapFill(pixmapBackgroundVolume, clrTransparent);
        if (config.GetValue("displayType") == dtGraphical) {
            cImage *imgBack = imgCache->GetSkinElement(seVolumeBackground);
            if (pixmapBackgroundVolume && imgBack) {
                pixmapBackgroundVolume->DrawImage(cPoint(0, 0), *imgBack);
            }
        } else {
            PixmapFill(pixmapBackgroundVolume, Theme.Color(clrChannelBackground));
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

        textLeft = (geoManager->volumeWidth - font->Width(*testlabel)) / 2;
        textTop = (geoManager->volumeLabelHeight - font->Height()) / 2;
        barWidth = geoManager->volumeProgressBarWidth;
        barHeight = geoManager->volumeProgressBarHeight;

        pixmapLabelVolume = CreatePixmap(osd, "pixmapLabelVolume", 7, cRect(Rect.Left(), Rect.Top() + 5, geoManager->volumeWidth, geoManager->volumeLabelHeight));
        PixmapFill(pixmapLabelVolume, clrTransparent);
        pixmapProgressBarVolume = CreatePixmap(osd, "pixmapProgressBarVolume", 7, cRect((Rect.Left() + (geoManager->volumeWidth - barWidth) / 2), Rect.Top() + ((geoManager->volumeHeight - barHeight) * 2 / 3), barWidth, barHeight));
        PixmapFill(pixmapProgressBarVolume, clrTransparent);
    }
}

cNopacityVolumeBox::~cNopacityVolumeBox(void) {
    osd->DestroyPixmap(pixmapProgressBarVolume);
    osd->DestroyPixmap(pixmapLabelVolume);
    osd->DestroyPixmap(pixmapBackgroundVolume);
}

void cNopacityVolumeBox::SetVolume(int Current, int Total, bool Mute) {
    PixmapFill(pixmapProgressBarVolume, clrTransparent);
    PixmapFill(pixmapLabelVolume, clrTransparent);

    int barHeight = geoManager->volumeProgressBarHeight;
    if ((Current == 0) && (Total == 0) || (barHeight < 5)) {
        return;
    }

    if (pixmapProgressBarVolume) {
        pixmapProgressBarVolume->DrawEllipse(cRect(0, 0, barHeight, barHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBarVolume->DrawEllipse(cRect(barWidth - barHeight, 0, barHeight, barHeight), Theme.Color(clrProgressBarBack));
        pixmapProgressBarVolume->DrawRectangle(cRect(barHeight / 2, 0, barWidth - barHeight, barHeight), Theme.Color(clrProgressBarBack));

        DrawProgressbar(pixmapProgressBarVolume, 1, 1, barWidth - 2, barHeight - 2, Current, Total, Theme.Color(clrProgressBar), Theme.Color(clrProgressBarBlend), true);
    }
    if (!pixmapLabelVolume)
        return;

    cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
    pixmapLabelVolume->DrawText(cPoint(textLeft, textTop), *label, Theme.Color(clrChannelHead), clrTransparent, font);

    if (!simple && Mute) {
        cBitmap bmMute(mute_xpm);
        pixmapLabelVolume->DrawBitmap(cPoint(geoManager->volumeWidth - 2 * bmMute.Width(), (geoManager->volumeLabelHeight - bmMute.Height()) / 2), bmMute, Theme.Color(clrDiskAlert), clrTransparent);
    }
}

void cNopacityVolumeBox::SetAlpha(int Alpha) {
    PixmapSetAlpha(pixmapBackgroundVolume, Alpha);
    PixmapSetAlpha(pixmapProgressBarVolume, Alpha);
    PixmapSetAlpha(pixmapLabelVolume, Alpha);
}
