#include "symbols/mute.xpm"
#include "displayvolume.h"

cNopacityDisplayVolume::cNopacityDisplayVolume(void) {
	config.setDynamicValues();
	initial = true;
	muted = false;
	FrameTime = config.volumeFrameTime; 
	FadeTime = config.volumeFadeTime;

	width = cOsd::OsdWidth() * config.volumeWidth / 100;
	height = cOsd::OsdHeight() * config.volumeHeight / 100;

	int top = (cOsd::OsdHeight() - height) - config.volumeBorderBottom;
	int left = (cOsd::OsdWidth() - width) / 2;
	
	osd = CreateOsd(left, top, width, height);

	pixmapBackgroundTop = osd->CreatePixmap(1, cRect(0, 0, width, height/2));
	pixmapBackgroundBottom = osd->CreatePixmap(1, cRect(0, height/2, width, height/2));

	DrawBlendedBackground(pixmapBackgroundTop, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), true);
	DrawBlendedBackground(pixmapBackgroundBottom, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), false);

	pixmapBackgroundTop->DrawEllipse(cRect(0, 0, height/4, height/4), clrTransparent, -2);
	pixmapBackgroundTop->DrawEllipse(cRect(width - height/4, 0, height/4, height/4), clrTransparent, -1);
	pixmapBackgroundBottom->DrawEllipse(cRect(0, height/4, height/4, height/4), clrTransparent, -3);
	pixmapBackgroundBottom->DrawEllipse(cRect(width - height/4, height/4, height/4, height/4), clrTransparent, -4);

	labelHeight = height/3;
	pixmapLabel = osd->CreatePixmap(2, cRect(0, 5, width, labelHeight));
	progressBarWidth = 0.9 * width;
	progressBarHeight = 0.3 * height;
	if (progressBarHeight%2 != 0)
		progressBarHeight++;
	pixmapProgressBar = osd->CreatePixmap(2, cRect((width - progressBarWidth) / 2, (height - progressBarHeight)*2/3, progressBarWidth, progressBarHeight));

	if (config.volumeFadeTime) {
		pixmapBackgroundTop->SetAlpha(0);
		pixmapBackgroundBottom->SetAlpha(0);
		pixmapProgressBar->SetAlpha(0);
		pixmapLabel->SetAlpha(0);
	}
	font = cFont::CreateFont(config.fontName, labelHeight - 6 + config.fontVolume);
}

cNopacityDisplayVolume::~cNopacityDisplayVolume() {
	Cancel(-1);
	while (Active())
		cCondWait::SleepMs(10);
	osd->DestroyPixmap(pixmapBackgroundTop);
	osd->DestroyPixmap(pixmapBackgroundBottom);
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
        DrawProgressBar(Current, Total);
}

void cNopacityDisplayVolume::DrawProgressBar(int Current, int Total) {
	pixmapProgressBar->Fill(clrTransparent);
	double percent = ((double)Current) / (double)Total;
	int barWidth = progressBarWidth - progressBarHeight;
	if ((Current > 0) || (Total > 0)) {
		pixmapProgressBar->DrawEllipse(cRect(0, 0, progressBarHeight, progressBarHeight), Theme.Color(clrProgressBarBack));
		pixmapProgressBar->DrawEllipse(cRect(progressBarWidth - progressBarHeight, 0, progressBarHeight, progressBarHeight), Theme.Color(clrProgressBarBack));
		pixmapProgressBar->DrawRectangle(cRect(progressBarHeight/2, 0, progressBarWidth - progressBarHeight, progressBarHeight), Theme.Color(clrProgressBarBack));

		pixmapProgressBar->DrawEllipse(cRect(1, 1, progressBarHeight-2, progressBarHeight-2), Theme.Color(clrProgressBarBlend));
		if (Current > 0) {
			tColor colAct = DrawProgressbarBackground(progressBarHeight / 2 - 1, 1, barWidth * percent - 2, progressBarHeight - 2);
			pixmapProgressBar->DrawEllipse(cRect(barWidth * percent, 1, progressBarHeight-2, progressBarHeight-2), colAct);
		}
	}
}

tColor cNopacityDisplayVolume::DrawProgressbarBackground(int left, int top, int width, int height) {

	tColor clr1 = Theme.Color(clrProgressBar);
	tColor clr2 = Theme.Color(clrProgressBarBlend);
	tColor clr = 0x00000000;
	int step = width / 256;
	int alpha = 0x0;
	int alphaStep;
	int maximum = 0;
	if (step == 0) {    //width < 256
		step = 1;
        alphaStep = 256 / width;
		maximum = width;
    } else { 			//width > 256
        alphaStep = 0x1;
		maximum = 256;
    }
	int x = 0;
	for (int i = 0; i < maximum; i++) {
		x = left + i*step;
		clr = AlphaBlend(clr1, clr2, alpha);
		pixmapProgressBar->DrawRectangle(cRect(x,top,step,height), clr);
        alpha += alphaStep;
	}
	if (step > 0) {
		int rest = width - step*256;
        pixmapProgressBar->DrawRectangle(cRect(left+step*256, top, rest, height), clr);
	}
	return clr;
}

void cNopacityDisplayVolume::Flush(void) {
	if (initial)
		if (config.volumeFadeTime)
			Start();
	initial = false;
	osd->Flush();
}

void cNopacityDisplayVolume::Action(void) {
	uint64_t Start = cTimeMs::Now();
	while (Running()) {
		uint64_t Now = cTimeMs::Now();
		cPixmap::Lock();
		double t = min(double(Now - Start) / FadeTime, 1.0);
		int Alpha = t * ALPHA_OPAQUE;
		pixmapBackgroundTop->SetAlpha(Alpha);
		pixmapBackgroundBottom->SetAlpha(Alpha);
		pixmapProgressBar->SetAlpha(Alpha);
		pixmapLabel->SetAlpha(Alpha);
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
