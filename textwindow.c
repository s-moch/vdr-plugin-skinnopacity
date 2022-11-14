#include "textwindow.h"
#include "config.h"
#include "imagecache.h"
#include "imageloader.h"
#include "helpers.h"
#include <sstream>

cNopacityTextWindow::cNopacityTextWindow(cOsd *osd, cFont *font, cRect *vidWin) : cThread("TextWindow") {
    this->osd = osd;
    this->font = font;
    this->fontHeader = NULL;
    this->vidWin = vidWin;
    posterWidth = 0;
    posterHeight = 0;
}

cNopacityTextWindow::cNopacityTextWindow(cOsd *osd, cFont *font, cFont *fontHeader) : cThread("TextWindow") {
    this->osd = osd;
    this->font = font;
    this->fontHeader = fontHeader;
    this->vidWin = NULL;
    posterWidth = 0;
    posterHeight = 0;
}

cNopacityTextWindow::~cNopacityTextWindow(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmapBackground);
    osd->DestroyPixmap(pixmap);
    if ((config.GetValue("scalePicture") == 2) && scaledWindow) {
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
        int posterWidthOrig = config.GetValue("posterWidth");
        int posterHeightOrig = config.GetValue("posterHeight");
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

void cNopacityTextWindow::SetPoster(const cEvent *event, const cRecording *recording, bool fullscreen) {
    if (!event && !recording)
        return;

    hasPoster = false;
    posterHeight = 0;
    posterWidth = 0;
    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper && (config.GetValue("scraperInfo") == 1)) {
        posterScraper2Vdr.event = event;
        posterScraper2Vdr.recording = recording;
        if (pScraper->Service("GetPoster", &posterScraper2Vdr)) {
            int posterWidthOrig = posterScraper2Vdr.poster.width;
            int posterHeightOrig = posterScraper2Vdr.poster.height;
            if (!(posterWidthOrig == 0 || posterHeightOrig == 0)) {
                if (!fullscreen) {
                    posterHeight = geometry->Height() - 5;
                    posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
                } else {
                    posterWidth = geometry->Width() / 4;
                    posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
                }
                hasPoster = true;
            }
        }
    }
}

bool cNopacityTextWindow::SetTextScroller(int left) {
    int lineHeight = font->Height();
    bool scrolling = false;
    drawportHeight = geometry->Height();
    if (!(hasPoster || hasManualPoster)) {
        drawTextTall = false;
        drawTextFull = true;
        twTextTall.Set("", font, 5);
        twTextFull.Set(*text, font, geometry->Width());
        int pixmapTotalHeight = lineHeight * twTextFull.Lines();
        if (pixmapTotalHeight > drawportHeight) {
            drawportHeight = pixmapTotalHeight;
            scrolling = true;
        }
    } else {
        cTextWrapper test;
        int widthTall = geometry->Width() - left;
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
            twTextFull.Set(textFull.str().c_str(), font, geometry->Width());
            scrolling = true;
            drawportHeight = lineHeight * (twTextTall.Lines() + twTextFull.Lines());
        } else {
            scrolling = false;
            drawTextTall = true;
            drawTextFull = false;
            twTextTall.Set(*text, font, geometry->Width() - left);
            twTextFull.Set("", font, 5);
        }
    }
    return scrolling;
}

void cNopacityTextWindow::SetGeometry(cRect *geo, int border) {
    geometryback = geo;
    geotext.Set(geometryback->X() + border, geometryback->Y() + border, geometryback->Width() - 2 * border, geometryback->Height() - 2 * border);
    geometry = &geotext;
}

void cNopacityTextWindow::CreatePixmapWindow(void) {
    cPixmap::Lock();

    pixmapBackground = CreatePixmap(osd, "pixmapBackground", 4, cRect(geometryback->X(), geometryback->Y(), geometryback->Width(), geometryback->Height()));
    PixmapFill(pixmapBackground, Theme.Color(clrMenuBorder));
    if (pixmapBackground)
        pixmapBackground->DrawRectangle(cRect(2, 2, geometryback->Width() - 4, geometryback->Height() - 4), clrBlack);

    pixmap = CreatePixmap(osd, "pixmap", 5, cRect(geometry->X(), geometry->Y(), geometry->Width(), geometry->Height()),
                                            cRect(0, 0, geometry->Width(), drawportHeight));
    PixmapFill(pixmap, Theme.Color(clrMenuBack));
    if (config.GetValue("animation") && config.GetValue("menuEPGWindowFadeTime"))
        SetAlpha();

    cPixmap::Unlock();
}

void cNopacityTextWindow::CreatePixmapFullScreen(void) {
    pixmapBackground = CreatePixmap(osd, "pixmapBackground", 4, cRect(geometryback->X(), geometryback->Y(), geometryback->Width(), geometryback->Height()));
    PixmapFill(pixmapBackground, Theme.Color(clrMenuBorder));
    if (pixmapBackground)
        pixmapBackground->DrawRectangle(cRect(2, 2, geometryback->Width() - 4, geometryback->Height() - 4), Theme.Color(clrMenuTextWindow));

    pixmap = CreatePixmap(osd, "pixmap", 5, cRect(geometry->X(), geometry->Y(), geometry->Width(), geometry->Height()));
    PixmapFill(pixmap, clrTransparent);
}

void cNopacityTextWindow::SetAlpha(int Alpha) {
    PixmapSetAlpha(pixmapBackground, Alpha);
    PixmapSetAlpha(pixmap, Alpha);
}

void cNopacityTextWindow::DrawText(int left) {
    if (!pixmap)
        return;

    int lineHeight = font->Height();
    int y = 0;
    tColor clrFontBack = (config.GetValue("displayType") != dtFlat)?(clrTransparent):(Theme.Color(clrMenuBack));
    cPixmap::Lock();
    if (drawTextTall) {
        for (int i = 0; (i < twTextTall.Lines()) && Running(); i++) {
            pixmap->DrawText(cPoint(left, y), twTextTall.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrFontBack, font);
            y += lineHeight;
        }
    }
    if (drawTextFull) {
        for (int i = 0; (i < twTextFull.Lines()) && Running(); i++) {
            pixmap->DrawText(cPoint(0, y), twTextFull.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrFontBack, font);
            y += lineHeight;
        }
    }
    cPixmap::Unlock();
}

void cNopacityTextWindow::SetEvent(const cEvent *event) {
    if (!event)
        return;

    CreatePixmapFullScreen();
    if (!pixmap)
        return;

    int border = config.GetValue("borderDetailedEPG");
    int width = geometry->Width();
    int height = geometry->Height();
    int widthTextHeader = width;
    int widthText = widthTextHeader;
    int heightPicture = 0;
    int y = 0;
    cImageLoader imgLoader;
    bool epgImageFound = false;
    if (hasPoster) {
        int posterX = width - posterWidth;
        if (imgLoader.LoadPoster(posterScraper2Vdr.poster.path.c_str(), posterWidth, posterHeight)) {
            cImage image = imgLoader.GetImage();
            pixmap->DrawImage(cPoint(posterX, y), image);
            widthTextHeader -= image.Width() + border;
            heightPicture = image.Height();
        }
    } else if (imgLoader.LoadEPGImage(event->EventID())) {
        cImage image = imgLoader.GetImage();
        epgImageFound = true;
        pixmap->DrawImage(cPoint(width - image.Width(), y), image);
        widthTextHeader -= image.Width() + border;
        heightPicture = image.Height();
    }
    //Title
    y = DrawTextWrapper(event->Title(), widthTextHeader, y, 0, fontHeader, Theme.Color(clrMenuFontDetailViewHeaderTitle), height);
    //Short Text
    y = DrawTextWrapper(event->ShortText(), widthTextHeader, y, 0, font, Theme.Color(clrMenuFontDetailViewHeader), height);

    y += fontHeader->Height();
    //Description
    if (hasPoster && (y < heightPicture + border)) {
        int heightNarrow = heightPicture + border - y;
        DrawTextWrapperFloat(event->Description(),
                             widthTextHeader, widthText, y, heightNarrow,
                             0, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else if (epgImageFound && (y < heightPicture + border)) {
        y = heightPicture + border;
        DrawTextWrapper(event->Description(), widthText, y, 0, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else {
        DrawTextWrapper(event->Description(), widthText, y, 0, font, Theme.Color(clrMenuFontDetailViewText), height);
    }
}

void cNopacityTextWindow::SetRecording(const cRecording *recording) {
    if (!recording)
        return;

    CreatePixmapFullScreen();
    if (!pixmap)
        return;

    int border = config.GetValue("borderDetailedRecordings");
    int width = geometry->Width();
    int height = geometry->Height();
    int widthTextHeader = width;
    int widthText = widthTextHeader;
    int heightPicture = 0;
    int y = 0;
    //Image
    cImageLoader imgLoader;
    bool recImageFound = false;
    if (hasManualPoster) {
        if (imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
            cImage image = imgLoader.GetImage();
            int posterX = width - image.Width();
            pixmap->DrawImage(cPoint(posterX, y), image);
            widthTextHeader -= image.Width() + border;
            heightPicture = image.Height();
        }
    } else if (hasPoster) {
        if (imgLoader.LoadPoster(posterScraper2Vdr.poster.path.c_str(), posterWidth, posterHeight)) {
            cImage image = imgLoader.GetImage();
            int posterX = width - image.Width();
            pixmap->DrawImage(cPoint(posterX, y), image);
            widthTextHeader -= image.Width() + border;
            heightPicture = image.Height();
        }
    } else if (imgLoader.LoadRecordingImage(recording->FileName())) {
        cImage image = imgLoader.GetImage();
        int posterX = width - image.Width();
        pixmap->DrawImage(cPoint(posterX, y), image);
        widthTextHeader -= image.Width() + border;
        heightPicture = image.Height();
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
    y = DrawTextWrapper(*recTitle, widthTextHeader, y, 0, fontHeader, Theme.Color(clrMenuFontDetailViewHeaderTitle), height);
    //Short Text
    if (!isempty(info->ShortText())) {
        y = DrawTextWrapper(info->ShortText(), widthTextHeader, y, 0, font, Theme.Color(clrMenuFontDetailViewHeader), height);
    }
    y += fontHeader->Height();
    //Description
    if ((hasPoster || hasManualPoster) && (y < heightPicture + border)) {
        int heightNarrow = heightPicture + border - y;
        DrawTextWrapperFloat(recording->Info()->Description(),
                             widthTextHeader, widthText, y, heightNarrow,
                             0, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else if (recImageFound && (y < heightPicture + border)) {
        y = heightPicture + border;
        DrawTextWrapper(recording->Info()->Description(), widthText, y, 0, font, Theme.Color(clrMenuFontDetailViewText), height);
    } else {
        DrawTextWrapper(recording->Info()->Description(), widthText, y, 0, font, Theme.Color(clrMenuFontDetailViewText), height);
    }
}

int cNopacityTextWindow::DrawTextWrapper(const char *text, int width, int top, int x,
                                         const cFont *font, tColor color, int maxHeight) {
    cTextWrapper wrapper;
    int lineHeight = font->Height();
    wrapper.Set(text, font, width);
    int y = top;
    for (int i = 0; i < wrapper.Lines(); i++) {
        if (y + 2 * lineHeight > maxHeight) {
            pixmap->DrawText(cPoint(x, y), "...", color, clrTransparent, font);
            y += 2 * lineHeight;
            break;
        }
        pixmap->DrawText(cPoint(x, y), wrapper.GetLine(i), color, clrTransparent, font);
        y += lineHeight;
    }
    return y;
}

void cNopacityTextWindow::DrawTextWrapperFloat(const char *text, int widthSmall, int widthFull,
                                              int top, int heightNarrow, int x, const cFont *font,
                                              tColor color, int maxHeight) {

    if (!text)
        return;
    int lineHeight = font->Height();
    int numLinesNarrow = heightNarrow / lineHeight + 1;
    splitstring s(text);
    std::vector<std::string> flds = s.split('\n', 1);
    if (flds.size() < 1)
        return;
    int y = top;
    int linesDrawn = 0;
    bool drawNarrow = true;
    for (unsigned int i = 0; i < flds.size(); i++) {
        if (!flds[i].size()) {
            //empty line
            linesDrawn++;
            y += lineHeight;
        } else {
            cTextWrapper wrapper;
            if (drawNarrow) {
                wrapper.Set((flds[i].c_str()), font, widthSmall);
                int newLines = wrapper.Lines();
                //check if wrapper fits completely into narrow area
                if (linesDrawn + newLines < numLinesNarrow) {
                    y = DrawTextWrapper(flds[i].c_str(), widthSmall, y, x, font, color, maxHeight);
                    linesDrawn += newLines;
                } else {
                    //this wrapper has to be splitted
                    std::stringstream sstrTextTall;
                    std::stringstream sstrTextFull;
                    for (int line = 0; line < wrapper.Lines(); line++) {
                        if (line + linesDrawn < numLinesNarrow) {
                            sstrTextTall << wrapper.GetLine(line) << " ";
                        } else {
                            sstrTextFull << wrapper.GetLine(line) << " ";
                        }
                    }
                    y = DrawTextWrapper(sstrTextTall.str().c_str(), widthSmall, y, x, font, color, maxHeight);
                    y = DrawTextWrapper(sstrTextFull.str().c_str(), widthFull, y, x, font, color, maxHeight);
                    drawNarrow = false;
                }
            } else {
                if (y > maxHeight)
                    break;
                y = DrawTextWrapper(flds[i].c_str(), widthFull, y, x, font, color, maxHeight);
            }
        }
    }
}

void cNopacityTextWindow::DrawPoster(void) {
    if (!pixmap)
        return;

    bool drawposter = false;
    cImageLoader imgLoader;
    if (hasManualPoster && imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
        drawposter = true;
    }
    if (hasPoster && imgLoader.LoadPoster(posterScraper2Vdr.poster.path.c_str(), posterWidth, posterHeight)) {
        drawposter = true;
    }
    if (drawposter) {
        cImage poster = imgLoader.GetImage();
        pixmap->DrawImage(cPoint(0, 0), poster);
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

    int initialSleepTime = (initial) ? config.GetValue("menuFadeTime") : 0;
    DoSleep(initialSleepTime + config.GetValue("menuInfoTextDelay") * 1000);

    if (config.GetValue("scalePicture") == 2) {
          ScaleVideoWindow();
    }

    int left = 0;
    if (hasPoster || hasManualPoster)
        left = 10 + posterWidth;
    bool scrolling = false;
    if (Running()) {
        scrolling = SetTextScroller(left);
        CreatePixmapWindow();
    }
    if (Running()) {
        DrawText(left);
    }
    if (Running() && (hasPoster || hasManualPoster)) {
        DrawPoster();
    }

    //FadeIn
    if (config.GetValue("menuEPGWindowFadeTime")) {
        int FadeTime = config.GetValue("menuEPGWindowFadeTime");
        int FadeFrameTime = FadeTime / 10;
        uint64_t Start = cTimeMs::Now();
        while (Running()) {
            uint64_t Now = cTimeMs::Now();
            double t = std::min(double(Now - Start) / FadeTime, 1.0);
            int Alpha = t * ALPHA_OPAQUE;
            cPixmap::Lock();
            SetAlpha(Alpha);
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

    if (pixmap && scrolling && Running()) {
        int scrollDelay = config.GetValue("menuInfoScrollDelay") * 1000;
        DoSleep(scrollDelay);
        int drawPortY;
        int FrameTime = 0;
        if (config.GetValue("menuInfoScrollSpeed") == 1)
            FrameTime = 50;
        else if (config.GetValue("menuInfoScrollSpeed") == 2)
            FrameTime = 30;
        else if (config.GetValue("menuInfoScrollSpeed") == 3)
            FrameTime = 15;

        int maxY = pixmap->DrawPort().Height() - pixmap->ViewPort().Height();
        bool doSleep = false;
        while (Running()) {
            uint64_t Now = cTimeMs::Now();
            cPixmap::Lock();
            drawPortY = pixmap->DrawPort().Y();
            drawPortY -= 1;
            cPixmap::Unlock();
            if (std::abs(drawPortY) > maxY) {
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
