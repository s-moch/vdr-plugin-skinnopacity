#include "textwindow.h"

cNopacityTextWindow::cNopacityTextWindow(cOsd *osd, cFont *font, cRect *vidWin) {
    this->osd = osd;
    this->font = font;
    this->fontHeader = NULL;
    this->vidWin = vidWin;
    pixmapBackground = NULL;
    pixmap = NULL;
    scaledWindow = false;
    posterWidth = 0;
    posterHeight = 0;
    hasManualPoster = false;
    manualPosterPath = "";
    hasPoster = false;
}

cNopacityTextWindow::cNopacityTextWindow(cOsd *osd, cFont *font, cFont *fontHeader) {
    this->osd = osd;
    this->font = font;
    this->fontHeader = fontHeader;
    this->vidWin = NULL;
    pixmapBackground = NULL;
    pixmap = NULL;
    scaledWindow = false;
    posterWidth = 0;
    posterHeight = 0;
    hasManualPoster = false;
    manualPosterPath = "";
    hasPoster = false;
}

cNopacityTextWindow::~cNopacityTextWindow(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    if (pixmapBackground) {
        osd->DestroyPixmap(pixmapBackground);
        pixmapBackground = NULL;
    }
    if (pixmap) {
        osd->DestroyPixmap(pixmap);
        pixmap = NULL;
    }
    if ((config.scalePicture == 2) && scaledWindow) {
        cRect vidWinNew = cDevice::PrimaryDevice()->CanScaleVideo(oldVidWin);
        if (vidWinNew != cRect::Null) {
            vidWin->SetX(vidWinNew.X());
            vidWin->SetY(vidWinNew.Y());
            vidWin->SetWidth(vidWinNew.Width());
            vidWin->SetHeight(vidWinNew.Height());
        }
    }
}

bool cNopacityTextWindow::SetManualPoster(const cRecording *recording, bool fullscreen) {
    cString posterFound;
    cImageLoader imgLoader;
    hasManualPoster = imgLoader.SearchRecordingPoster(recording->FileName(), posterFound);
    if (hasManualPoster) {
        manualPosterPath = posterFound;
        int posterWidthOrig = config.posterWidth;
        int posterHeightOrig = config.posterHeight;
        if (!fullscreen) {
            posterHeight = geometry->Height() - 5;
            posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
        } else {
            posterWidth = geometry->Width() / 4;
            posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
        }
        return true;
    }
    return false;
}

void cNopacityTextWindow::SetPoster(const cEvent *event, bool isRecording, bool fullscreen) {
    static cPlugin *pTVScraper = cPluginManager::GetPlugin("tvscraper");
    if (pTVScraper && event) {
        poster.event = event;
        poster.isRecording = isRecording;
        if (pTVScraper->Service("TVScraperGetPoster", &poster)) {
            hasPoster = true;
            int posterWidthOrig = poster.media.width;
            int posterHeightOrig = poster.media.height;
            if (!fullscreen) {
                posterHeight = geometry->Height() - 5;
                posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
            } else {
                posterWidth = geometry->Width() / 4;
                posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
            }
        } else {
            hasPoster = false;
            posterHeight = 0;
            posterWidth = 0;
        }
    }
}

bool cNopacityTextWindow::SetTextScroller(int border, int left) {
    int lineHeight = font->Height();
    bool scrolling = false;
    drawportHeight = geometry->Height();
    if (!(hasPoster || hasManualPoster)) {
        drawTextTall = false;
        drawTextFull = true;
        twTextTall.Set("", font, 5);
        twTextFull.Set(*text, font, geometry->Width() - 2*border);
        int pixmapTotalHeight = lineHeight * (twTextFull.Lines()+1);
        if ((pixmapTotalHeight - (lineHeight/2)) > drawportHeight) {
            drawportHeight = pixmapTotalHeight;
            scrolling = true;
        }
    } else {
        cTextWrapper test;
        int widthTall = geometry->Width() - 2*border - left;
        test.Set(*text, font, widthTall);
        int linesTotal = test.Lines();
        int textHeight =  linesTotal * lineHeight;
        if (textHeight > posterHeight) {
            drawTextTall = true;
            drawTextFull = true;
            int lineSplit = posterHeight / lineHeight + 1;
            std::stringstream textTall;
            std::stringstream textFull;
            for (int line = 0; line < linesTotal; line++) {
                bool lineWrap = false;
                if (font->Width(test.GetLine(line)) < (widthTall - 100))
                    lineWrap = true;
                if (line < lineSplit) {
                    textTall << test.GetLine(line);
                    if (lineWrap)
                        textTall << "\n";
                    else
                        textTall << " ";
                } else {
                    textFull << test.GetLine(line);
                    if (lineWrap)
                        textFull << "\n";
                    else
                        textFull << " ";
                }
            }
            twTextTall.Set(textTall.str().c_str(), font, widthTall);
            twTextFull.Set(textFull.str().c_str(), font, geometry->Width() - 2*border);
            scrolling = true;
            drawportHeight = lineHeight * (twTextTall.Lines() + twTextFull.Lines() + 1);
        } else {
            scrolling = false;
            drawTextTall = true;
            drawTextFull = false;
            twTextTall.Set(*text, font, geometry->Width() - 2*border - left);
            twTextFull.Set("", font, 5);
        }
    }
    return scrolling;
}

void cNopacityTextWindow::CreatePixmap(void) {
    cPixmap::Lock();
    pixmapBackground = osd->CreatePixmap(4, cRect(geometry->X()-1, geometry->Y()-1, geometry->Width()+2, geometry->Height()+2));
    pixmap = osd->CreatePixmap(5, cRect(geometry->X(), geometry->Y(), geometry->Width(), geometry->Height()),
                                  cRect(0, 0, geometry->Width(), drawportHeight));
    pixmapBackground->Fill(Theme.Color(clrMenuBorder));
    pixmapBackground->DrawRectangle(cRect(1, 1, geometry->Width(), geometry->Height()), clrBlack);
    pixmap->Fill(Theme.Color(clrMenuBack));
    if (config.menuEPGWindowFadeTime) {
        pixmap->SetAlpha(0);
        pixmapBackground->SetAlpha(0);
    }
    cPixmap::Unlock();
}

void cNopacityTextWindow::CreatePixmapFullScreen(void) {
    pixmapBackground = osd->CreatePixmap(4, cRect(geometry->X()-1, geometry->Y()-1, geometry->Width()+2, geometry->Height()+2));
    pixmap = osd->CreatePixmap(5, cRect(geometry->X(), geometry->Y(), geometry->Width(), geometry->Height()));
    pixmapBackground->Fill(Theme.Color(clrMenuBorder));
    pixmapBackground->DrawRectangle(cRect(1, 1, geometry->Width(), geometry->Height()), Theme.Color(clrMenuBack));
    pixmap->Fill(clrTransparent);
}

void cNopacityTextWindow::DrawText(int border, int left) {
    int lineHeight = font->Height();
    int currentLineHeight = lineHeight/2;
    tColor clrFontBack = (config.doBlending)?(clrTransparent):(Theme.Color(clrMenuBack));
    cPixmap::Lock();
    if (drawTextTall) {
        for (int i=0; (i < twTextTall.Lines()) && Running(); i++) {
            pixmap->DrawText(cPoint(border + left, currentLineHeight), twTextTall.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrFontBack, font);
            currentLineHeight += lineHeight;
        }
    }
    if (drawTextFull) {
        for (int i=0; (i < twTextFull.Lines()) && Running(); i++) {
            pixmap->DrawText(cPoint(border, currentLineHeight), twTextFull.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrFontBack, font);
            currentLineHeight += lineHeight;
        }
    }
    cPixmap::Unlock();        
}

void cNopacityTextWindow::SetEvent(const cEvent *event) {
    if (!event)
        return;
    CreatePixmapFullScreen();
    int border = config.borderDetailedEPG;
    int width = geometry->Width();
    int height = geometry->Height();
    int widthTextHeader = width - 2 * border;
    int widthText = widthTextHeader;
    int y = border;
    cImageLoader imgLoader;
    bool epgImageFound = false;
    if (hasPoster) {
        if (imgLoader.LoadPoster(poster.media.path.c_str(), posterWidth, posterHeight)) {
            int posterX = width - posterWidth - border;
            pixmap->DrawImage(cPoint(posterX, border), imgLoader.GetImage());
            widthTextHeader -= posterWidth;
        }
    } else if (imgLoader.LoadEPGImage(event->EventID())) {
        epgImageFound = true;
        pixmap->DrawImage(cPoint(width - config.epgImageWidth - border, y), imgLoader.GetImage());
        widthTextHeader -= config.epgImageWidth;
    }
    //Title
    y = DrawTextWrapper(event->Title(), widthTextHeader, y, border, fontHeader, Theme.Color(clrMenuFontDetailViewHeaderTitle));
    //Short Text
    y = DrawTextWrapper(event->ShortText(), widthTextHeader, y, border, font, Theme.Color(clrMenuFontDetailViewHeader));

    y += fontHeader->Height();
    //Description
    int maxHeight = height - y;
    if (hasPoster && (y < (border + posterHeight))) {
        int heightNarrow = border + posterHeight - y;
        DrawTextWrapperFloat(event->Description(), 
                             widthTextHeader, widthText, y, heightNarrow,
                             border, font, Theme.Color(clrMenuFontDetailViewText), maxHeight);
    } else if (epgImageFound && (y < (border + config.epgImageHeight))) {
        y = border + config.epgImageHeight;
        DrawTextWrapper(event->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), maxHeight);
    } else {
        DrawTextWrapper(event->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), maxHeight);
    }
}

void cNopacityTextWindow::SetRecording(const cRecording *recording) {
    if (!recording)
        return;
    CreatePixmapFullScreen();
    int border = config.borderDetailedRecordings;
    int width = geometry->Width();
    int height = geometry->Height();
    int widthTextHeader = width - 2 * border;
    int widthText = widthTextHeader;
    int y = border;
    cImageLoader imgLoader;
    bool recImageFound = false;
    if (hasManualPoster) {
        if (imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
            int posterX = width - posterWidth - border;
            pixmap->DrawImage(cPoint(posterX, border), imgLoader.GetImage());
            widthTextHeader -= posterWidth;
        }
    } else if (hasPoster) {
        if (imgLoader.LoadPoster(poster.media.path.c_str(), posterWidth, posterHeight)) {
            int posterX = width - posterWidth - border;
            pixmap->DrawImage(cPoint(posterX, border), imgLoader.GetImage());
            widthTextHeader -= posterWidth;
        }
    } else if (imgLoader.LoadRecordingImage(recording->FileName())) {
        pixmap->DrawImage(cPoint(width - config.epgImageWidth - border, y), imgLoader.GetImage());
        widthTextHeader -= config.epgImageWidth;
        recImageFound = true;
    }
    const cRecordingInfo *info = recording->Info();
    if (!info)
        return;
    cString recTitle;
    if (info->Title())
        recTitle = info->Title();
    else    
        recTitle = recording->Name();
    //Title
    y = DrawTextWrapper(*recTitle, widthTextHeader, y, border, fontHeader, Theme.Color(clrMenuFontDetailViewHeaderTitle));
    //Short Text
    if (!isempty(info->ShortText())) {
        y = DrawTextWrapper(info->ShortText(), widthTextHeader, y, border, font, Theme.Color(clrMenuFontDetailViewHeader));
    }
    y += fontHeader->Height();
    //Description
    int maxHeight = height - y;
    if ((hasPoster || hasManualPoster) && (y < (border + posterHeight))) {
        int heightNarrow = border + posterHeight - y;
        DrawTextWrapperFloat(recording->Info()->Description(), 
                             widthTextHeader, widthText, y, heightNarrow,
                             border, font, Theme.Color(clrMenuFontDetailViewText), maxHeight);
    } else if (recImageFound && (y < (border + config.epgImageHeight))) {
        y = border + config.epgImageHeight;
        DrawTextWrapper(recording->Info()->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), maxHeight);
    } else {
        DrawTextWrapper(recording->Info()->Description(), widthText, y, border, font, Theme.Color(clrMenuFontDetailViewText), maxHeight);
    }
}

int cNopacityTextWindow::DrawTextWrapper(const char *text, int width, int top, int x, 
                                         const cFont *font, tColor color, int maxHeight) {
    cTextWrapper wrapper;
    wrapper.Set(text, font, width);
    int height = 2*font->Height();
    int y = top;
    for (int i=0; i < wrapper.Lines(); i++) {
        if (maxHeight && (height > maxHeight)) {
            pixmap->DrawText(cPoint(x, y), "...", color, clrTransparent, font);
            break;
        }
        pixmap->DrawText(cPoint(x, y), wrapper.GetLine(i), color, clrTransparent, font);
        y += font->Height();
        height += font->Height();
    }
    return y;
}

int cNopacityTextWindow::DrawTextWrapperFloat(const char *text, int widthSmall, int widthFull, 
                                              int top, int heightNarrow, int x, const cFont *font, 
                                              tColor color, int maxHeight) {
    int lineHeight = font->Height();
    int numLinesNarrow = heightNarrow / lineHeight + 1;
    
    cTextWrapper test;
    test.Set(text, font, widthSmall);
    std::stringstream sstrTextTall;
    std::stringstream sstrTextFull;
    bool drawFull = false;
    for (int line = 0; line < test.Lines(); line++) {
        bool lineWrap = false;
        if (font->Width(test.GetLine(line)) < (widthSmall - 100))
            lineWrap = true;
        if (line < numLinesNarrow) {
            sstrTextTall << test.GetLine(line);
            if (lineWrap)
                sstrTextTall << "\n";
            else
                sstrTextTall << " ";
        } else {
            drawFull = true;
            sstrTextFull << test.GetLine(line);
            if (lineWrap)
                sstrTextFull << "\n";
            else
                sstrTextFull << " ";
        }
    }
    
    cTextWrapper wrapperNarrow;
    wrapperNarrow.Set(sstrTextTall.str().c_str(), font, widthSmall);
    int height = 2*font->Height();
    int y = top;
    for (int i=0; i < wrapperNarrow.Lines(); i++) {
        pixmap->DrawText(cPoint(x, y), wrapperNarrow.GetLine(i), color, clrTransparent, font);
        y += lineHeight;
        height += lineHeight;
    }
    if (drawFull) {
        cTextWrapper wrapper;
        wrapper.Set(sstrTextFull.str().c_str(), font, widthFull);
        for (int i=0; i < wrapper.Lines(); i++) {
            if (maxHeight && (height > maxHeight)) {
                pixmap->DrawText(cPoint(x, y), "...", color, clrTransparent, font);
                break;
            }
            pixmap->DrawText(cPoint(x, y), wrapper.GetLine(i), color, clrTransparent, font);
            y += lineHeight;
            height += lineHeight;
        }
    }
    return y;
}

void cNopacityTextWindow::DrawPoster(int border) {
    cImageLoader imgLoader;
    if (hasManualPoster) {
        if (imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
            pixmap->DrawImage(cPoint(border, font->Height()), imgLoader.GetImage());
        }
    } else if (imgLoader.LoadPoster(poster.media.path.c_str(), posterWidth, posterHeight)) {
        pixmap->DrawImage(cPoint(border, font->Height()), imgLoader.GetImage());
    }
}

void cNopacityTextWindow::DoSleep(int duration) {
    int sleepSlice = 10;
    for (int i = 0; Running() && (i*sleepSlice < duration); i++)
        cCondWait::SleepMs(sleepSlice);
}

void cNopacityTextWindow::ScaleVideoWindow(void) {
    oldVidWin.SetX(vidWin->X());
    oldVidWin.SetY(vidWin->Y());
    oldVidWin.SetWidth(vidWin->Width());
    oldVidWin.SetHeight(vidWin->Height());
    cRect availableRect(vidWin->X(), vidWin->Y(), vidWin->Width(), vidWin->Height() - geometry->Height());
    cRect vidWinNew = cDevice::PrimaryDevice()->CanScaleVideo(availableRect);
    if (vidWinNew != cRect::Null) {
        vidWin->SetX(vidWinNew.X());
        vidWin->SetY(vidWinNew.Y());
        vidWin->SetWidth(vidWinNew.Width());
        vidWin->SetHeight(vidWinNew.Height());
        scaledWindow = true;
    }
}

void cNopacityTextWindow::Action(void) {
    if (! *text)
        return;
        
    DoSleep(config.menuInfoTextDelay*1000);

    if (config.scalePicture == 2) {
          ScaleVideoWindow();      
    }
    
    int border = 5;
    int left = 0;
    if (hasPoster || hasManualPoster)
        left = 10 + posterWidth;
    bool scrolling = false;
    if (Running()) {
        scrolling = SetTextScroller(border, left);
        CreatePixmap();
    }
    if (Running()) {
        DrawText(border, left);
    }
    if (Running() && (hasPoster || hasManualPoster)) {
        DrawPoster(border);
    }
    //FadeIn
    if (config.menuEPGWindowFadeTime) {
        uint64_t Start = cTimeMs::Now();
        int FadeTime = config.menuEPGWindowFadeTime;
        int FadeFrameTime = config.menuEPGWindowFrameTime;
        while (Running()) {
            uint64_t Now = cTimeMs::Now();
            cPixmap::Lock();
            double t = min(double(Now - Start) / FadeTime, 1.0);
            int Alpha = t * ALPHA_OPAQUE;
            pixmapBackground->SetAlpha(Alpha);
            pixmap->SetAlpha(Alpha);
            cPixmap::Unlock();
            if (Running())
                osd->Flush();
            int Delta = cTimeMs::Now() - Now;
            if (Running() && (Delta < FadeFrameTime))
                cCondWait::SleepMs(FadeFrameTime - Delta);
            if ((int)(Now - Start) > FadeTime)
                break;
        }
    }
    
    if (scrolling && Running()) {
        int scrollDelay = config.menuInfoScrollDelay * 1000;
        DoSleep(scrollDelay);
        int drawPortY;
        int FrameTime = config.menuInfoScrollFrameTime;
        int maxY = pixmap->DrawPort().Height() - pixmap->ViewPort().Height();
        bool doSleep = false;
        while (Running()) {
            uint64_t Now = cTimeMs::Now();
            cPixmap::Lock();
            drawPortY = pixmap->DrawPort().Y();
            drawPortY -= 1;
            cPixmap::Unlock();
            if (abs(drawPortY) > maxY) {
                doSleep = true;
                DoSleep(scrollDelay);
                drawPortY = 0;
            }
            cPixmap::Lock();
            if (Running())
                pixmap->SetDrawPortPoint(cPoint(0, drawPortY));
            cPixmap::Unlock();
            if (doSleep) {
                doSleep = false;
                DoSleep(scrollDelay);
            }
            int Delta = cTimeMs::Now() - Now;
            if (Running())
                osd->Flush();
            if (Running() && (Delta < FrameTime))
                cCondWait::SleepMs(FrameTime - Delta);
        }
    }
}
