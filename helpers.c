static cOsd *CreateOsd(int Left, int Top, int Width, int Height) {
	cOsd *osd = cOsdProvider::NewOsd(Left, Top);
	if (osd) {
		tArea Area = { 0, 0, Width, Height,  32 };
		if (osd->SetAreas(&Area, 1) == oeOk) {	
			return osd;
		}
	}
	return NULL;
}

static void DrawBlendedBackground(cPixmap *pixmap, tColor color, tColor colorBlending, bool fromTop) {
	int width = pixmap->ViewPort().Width();
	int height = pixmap->ViewPort().Height();
	pixmap->Fill(color);
	int numSteps = 16;
	int alphaStep = 0x0F;
	int alpha = 0x00;
	int step, begin, end;
	bool cont = true;
	if (fromTop) {
		step = 1;
		begin = 0;
		end = numSteps;
	} else {
		step = -1;
		begin = height;
		end = height - numSteps;
	}
	tColor clr;
	for (int i = begin; cont; i = i + step) {
		clr = AlphaBlend(color, colorBlending, alpha);
		pixmap->DrawRectangle(cRect(0,i,width,1), clr);
		alpha += alphaStep;
		if (i == end) 
			cont = false;
	}
}