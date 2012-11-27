#include "symbols/mute.xpm"
#include "displayvolume.h"

cNopacityDisplayVolume::cNopacityDisplayVolume(void) {
	config.setDynamicValues();
	initial = true;
	muted = false;
	FrameTime = config.volumeFrameTime; 
	FadeTime = config.volumeFadeTime;
	
	width = cOsd::OsdHeight() * config.volumeWidth / 100;
	height = cOsd::OsdHeight() * config.volumeHeight / 100;

	int top = (cOsd::OsdHeight() - height) / 2;
	int left = (cOsd::OsdWidth() - width) / 2;
	
	osd = CreateOsd(left, top, width, height);
	pixmapContent = osd->CreatePixmap(1, cRect(0, 0, width, height));
	pixmapContent->Fill(Theme.Color(clrMenuBorder));
	cImageLoader imgLoader;
	imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), width-2, height-2);
	pixmapContent->DrawImage(cPoint(1,1), imgLoader.GetImage());
	
	labelHeight = height/3;
	pixmapLabel = osd->CreatePixmap(2, cRect(0, 5, width, labelHeight));
	progressBarWidth = 0.9 * width;
	progressBarHeight = 0.3 * height;
	if (progressBarHeight%2 != 0)
		progressBarHeight++;
	pixmapProgressBar = osd->CreatePixmap(2, cRect((width - progressBarWidth) / 2, (height - progressBarHeight)*2/3, progressBarWidth, progressBarHeight));

	if (config.volumeFadeTime) {
		pixmapContent->SetAlpha(0);
		pixmapProgressBar->SetAlpha(0);
		pixmapLabel->SetAlpha(0);
	}
	font = cFont::CreateFont(config.fontName, labelHeight - 6 + config.fontVolume);
}

cNopacityDisplayVolume::~cNopacityDisplayVolume() {
	osd->DestroyPixmap(pixmapContent);
	osd->DestroyPixmap(pixmapLabel);
	osd->DestroyPixmap(pixmapProgressBar);
	delete font;
	delete osd;
}

void cNopacityDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
	pixmapLabel->Fill(clrTransparent);
	cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
	pixmapLabel->DrawText(cPoint((width - font->Width(*label)) / 2, (labelHeight - font->Height()) / 2), *label, Theme.Color(clrVolumeFont), clrTransparent, font);
	if (Mute) {
		cBitmap bmMute(mute_xpm);
		pixmapLabel->DrawBitmap(cPoint(width - 2*bmMute.Width(), (labelHeight - bmMute.Height()) / 2), bmMute, Theme.Color(clrDiskAlert), clrTransparent);
	}
	pixmapProgressBar->Fill(clrTransparent);
	double percent = ((double)Current) / (double)Total;
	int barWidth = progressBarWidth - progressBarHeight;
	pixmapProgressBar->DrawEllipse(cRect(0, 0, progressBarHeight, progressBarHeight), Theme.Color(clrProgressBarBack));
	pixmapProgressBar->DrawEllipse(cRect(progressBarWidth - progressBarHeight, 0, progressBarHeight, progressBarHeight), Theme.Color(clrProgressBarBack));
	pixmapProgressBar->DrawRectangle(cRect(progressBarHeight/2, 0, progressBarWidth - progressBarHeight, progressBarHeight), Theme.Color(clrProgressBarBack));
	if (Current > 0) {
		pixmapProgressBar->DrawEllipse(cRect(1, 1, progressBarHeight-2, progressBarHeight-2), Theme.Color(clrProgressBar));
		pixmapProgressBar->DrawEllipse(cRect(barWidth * percent - 1, 1, progressBarHeight-2, progressBarHeight-2), Theme.Color(clrProgressBar));
		pixmapProgressBar->DrawRectangle(cRect(progressBarHeight / 2 - 1, 1, barWidth * percent - 2, progressBarHeight - 2), Theme.Color(clrProgressBar));
	}
}

void cNopacityDisplayVolume::Flush(void) {
	if (initial)
		if (FadeTime)
			Start();
	initial = false;
	osd->Flush();
}

void cNopacityDisplayVolume::Action(void) {
	uint64_t Start = cTimeMs::Now();
	while (true) {
		uint64_t Now = cTimeMs::Now();
		cPixmap::Lock();
		double t = min(double(Now - Start) / FadeTime, 1.0);
	    int Alpha = t * ALPHA_OPAQUE;
		pixmapContent->SetAlpha(Alpha);
		pixmapProgressBar->SetAlpha(Alpha);
		pixmapLabel->SetAlpha(Alpha);
		osd->Flush();
		cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Delta < FrameTime)
           cCondWait::SleepMs(FrameTime - Delta);
		if ((int)(Now - Start) > FadeTime)
			break;
    }
}
