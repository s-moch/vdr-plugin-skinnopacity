#include "menudetailview.h"
#include "services/epgsearch.h"
#include <sstream>

cNopacityMenuDetailView::cNopacityMenuDetailView(cOsd *osd) {
    this->osd = osd;
    hasScrollbar = false;
    additionalContent = NULL;
    additionalContentSet = false;
}

cNopacityMenuDetailView::~cNopacityMenuDetailView(void) {
    delete font;
    if (fontHeader)
        delete fontHeader;
    if (fontHeaderLarge)
        delete fontHeaderLarge;
}

void cNopacityMenuDetailView::SetGeometry(int width, int height, int top, int contentBorder, int headerHeight) {
    this->width = width;
    this->height = height;
    this->top = top;
    this->border = contentBorder;
    this->headerHeight = headerHeight;
    contentHeight = height - headerHeight;
}

void cNopacityMenuDetailView::SetContent(const char *textContent) {
    if (textContent) {
        cString sContent = textContent;
        if (additionalContentSet) {
            std::stringstream sstrContent;
            sstrContent << textContent;
            sstrContent << std::endl;
            sstrContent << std::endl;
            sstrContent << *additionalContent;
            sContent = sstrContent.str().c_str();
        }
        content.Set(*sContent, font, width - 4 * border);
    } else
        content.Set("", font, width - 4 * border);
        
    int textHeight = font->Height();
    int linesContent = content.Lines() + 2;
    int heightContentText = linesContent * textHeight;
    if (heightContentText > contentHeight) {
        contentDrawPortHeight = heightContentText;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;  
    }
}

void cNopacityMenuDetailView::DrawContent(void) {
    int linesContent = content.Lines();
    int textHeight = font->Height();
    for (int i=0; i<linesContent; i++) {
        pixmapContent->DrawText(cPoint(2*border, (i+1)*textHeight), content.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
    }
}

void cNopacityMenuDetailView::LoadReruns(const cEvent *event) {
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (epgSearchPlugin && !isempty(event->Title())) {
        std::stringstream sstrReruns;
        Epgsearch_searchresults_v1_0 data;
        std::string strQuery = event->Title();
        if (config.useSubtitleRerun > 0) {
            if (config.useSubtitleRerun == 2 || !isempty(event->ShortText()))
                strQuery += "~";
            if (!isempty(event->ShortText()))
                strQuery += event->ShortText();
                data.useSubTitle = true;
        } else {
            data.useSubTitle = false;
        }
        data.query = (char *)strQuery.c_str();
        data.mode = 0;
        data.channelNr = 0;
        data.useTitle = true;
        data.useDescription = false;
        
        if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
            cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
            if (list && (list->Count() > 1)) {
                //TODO: current event is shown as rerun 
                sstrReruns << tr("RERUNS OF THIS SHOW") << ':' << std::endl;
                int i = 0;
                for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < config.numReruns; r = list->Next(r)) {
                    i++;
                    sstrReruns  << "- "
                                << *DayDateTime(r->event->StartTime());
                    cChannel *channel = Channels.GetByChannelID(r->event->ChannelID(), true, true);
                    if (channel)
                        sstrReruns << " " << channel->ShortName(true);
                    sstrReruns << ":  " << r->event->Title();
                    if (!isempty(r->event->ShortText()))
                        sstrReruns << "~" << r->event->ShortText();
                    sstrReruns << std::endl;
                }
                delete list;
            }
        }
        additionalContent = sstrReruns.str().c_str();
        additionalContentSet = true;
    }
}

double cNopacityMenuDetailView::ScrollbarSize(void) {
    double barSize = (double)contentHeight / (double)contentDrawPortHeight;
    return barSize;
}

double cNopacityMenuDetailView::Offset(void) {
    double offset;
    if (((-1)*pixmapContent->DrawPort().Point().Y() + contentHeight + font->Height()) > contentDrawPortHeight)
        offset = (double)1 - ScrollbarSize();
    else
        offset = (double)((-1)*pixmapContent->DrawPort().Point().Y())/(double)((-1)*pixmapContent->DrawPort().Point().Y() + contentHeight);
    return offset;

}
bool cNopacityMenuDetailView::Scroll(bool Up, bool Page) {

    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();
    int lineHeight = font->Height();
    bool scrolled = false;
    if (Up) {
        if (Page) {
            int newY = aktHeight + screenHeight;
            if (newY > 0)
                newY = 0;
            pixmapContent->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (aktHeight < 0) {
                pixmapContent->SetDrawPortPoint(cPoint(0, aktHeight + lineHeight));
                scrolled = true;
            }
        }
    } else {
        if (Page) {
            int newY = aktHeight - screenHeight;
            if ((-1)*newY > totalHeight - screenHeight)
                newY = (-1)*(totalHeight - screenHeight);
            pixmapContent->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (totalHeight - ((-1)*aktHeight + lineHeight) > screenHeight) {
                pixmapContent->SetDrawPortPoint(cPoint(0, aktHeight - lineHeight));
                scrolled = true;
            }
        }
    }
    return scrolled;
}

//---------------cNopacityMenuDetailEventView---------------------

cNopacityMenuDetailEventView::cNopacityMenuDetailEventView(cOsd *osd, const cEvent *Event) : cNopacityMenuDetailView(osd) {
    event = Event;
}

cNopacityMenuDetailEventView::~cNopacityMenuDetailEventView(void) {
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapContent);
    osd->DestroyPixmap(pixmapLogo);
}

void cNopacityMenuDetailEventView::CreatePixmaps(void) {
    pixmapHeader =  osd->CreatePixmap(3, cRect(0, top, width, headerHeight));
    pixmapContent = osd->CreatePixmap(3, cRect(0, top + headerHeight, width, contentHeight),
                                         cRect(0, 0, width, contentDrawPortHeight));
    pixmapLogo =    osd->CreatePixmap(4, cRect(0 + border, top + max((headerHeight-config.logoHeight)/2,1), config.detailViewLogoWidth, config.detailViewLogoHeight));

    pixmapHeader->Fill(clrTransparent);
    pixmapHeader->DrawRectangle(cRect(0, headerHeight - 2, width, 2), Theme.Color(clrMenuBorder));
    pixmapContent->Fill(clrTransparent);
    pixmapLogo->Fill(clrTransparent);
    
}

void cNopacityMenuDetailEventView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + 3 + config.fontDetailView);
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + config.fontDetailViewHeaderLarge);
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.fontDetailViewHeader);
}


void cNopacityMenuDetailEventView::Render(void) {
    DrawHeader();
    DrawContent();
}

void cNopacityMenuDetailEventView::DrawHeader(void) {
    cImageLoader imgLoader;
    int logoWidth = config.detailViewLogoWidth;
    cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true);
    if (channel && channel->Name() && imgLoader.LoadLogo(channel->Name(), logoWidth, config.detailViewLogoHeight)) {        
        pixmapLogo->DrawImage(cPoint(0, max((headerHeight - config.detailViewLogoHeight - border)/2, 0)), imgLoader.GetImage());
    }
    int widthTextHeader = width - 4 * border - logoWidth;
    if (imgLoader.LoadEPGImage(event->EventID())) {
        pixmapHeader->DrawImage(cPoint(width - config.epgImageWidth - border, (headerHeight-config.epgImageHeight)/2), imgLoader.GetImage());
        widthTextHeader -= config.epgImageWidth;
    }

    int lineHeight = fontHeaderLarge->Height();

    cString dateTime = cString::sprintf("%s  %s - %s", *event->GetDateString(), *event->GetTimeString(), *event->GetEndTimeString());
    pixmapHeader->DrawText(cPoint(logoWidth + 2*border, (lineHeight - fontHeader->Height())/2), *dateTime, Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);

    cTextWrapper title;
    title.Set(event->Title(), fontHeaderLarge, widthTextHeader);
    int currentLineHeight = lineHeight;
    for (int i=0; i < title.Lines(); i++) {
        pixmapHeader->DrawText(cPoint(logoWidth + 2*border, currentLineHeight), title.GetLine(i), Theme.Color(clrMenuFontDetailViewHeaderTitle), clrTransparent, fontHeaderLarge);
        currentLineHeight += lineHeight;
    }

    cTextWrapper shortText;
    shortText.Set(event->ShortText(), fontHeader, widthTextHeader);
    currentLineHeight += (lineHeight - fontHeader->Height())/2;
    for (int i=0; i < shortText.Lines(); i++) {
        if ((currentLineHeight + fontHeader->Height()) < headerHeight) {
            pixmapHeader->DrawText(cPoint(logoWidth + 2*border, currentLineHeight), shortText.GetLine(i), Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);
            currentLineHeight += fontHeader->Height();
        } else
            break;
    }

}

//------------------cNopacityMenuDetailRecordingView------------------

cNopacityMenuDetailRecordingView::cNopacityMenuDetailRecordingView(cOsd *osd, const cRecording *Recording) : cNopacityMenuDetailView(osd) {
    recording = Recording;
    info = Recording->Info();
}

cNopacityMenuDetailRecordingView::~cNopacityMenuDetailRecordingView(void) {
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapContent);
}

void cNopacityMenuDetailRecordingView::CreatePixmaps(void) {
    pixmapHeader =  osd->CreatePixmap(3, cRect(0, top, width, headerHeight));
    pixmapContent = osd->CreatePixmap(3, cRect(0, top + headerHeight, width, contentHeight),
                                         cRect(0, 0, width, contentDrawPortHeight));

    pixmapHeader->Fill(clrTransparent);
    pixmapHeader->DrawRectangle(cRect(0, headerHeight - 2, width, 2), Theme.Color(clrMenuBorder));
    pixmapContent->Fill(clrTransparent);
}

void cNopacityMenuDetailRecordingView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + config.fontDetailView);
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + config.fontDetailViewHeaderLarge);
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.fontDetailViewHeader);
}

void cNopacityMenuDetailRecordingView::Render(void) {
    DrawHeader();
    DrawContent();
}

void cNopacityMenuDetailRecordingView::DrawHeader(void) {
    cImageLoader imgLoader;
	int widthTextHeader = width - 4 * border;
    if (imgLoader.LoadRecordingImage(recording->FileName())) {
        pixmapHeader->DrawImage(cPoint(width - config.epgImageWidth - border, (headerHeight-config.epgImageHeight)/2), imgLoader.GetImage());
        widthTextHeader -= config.epgImageWidth;
    }
    int lineHeight = fontHeaderLarge->Height();
    cString dateTime = cString::sprintf("%s  %s", *DateString(recording->Start()), *TimeString(recording->Start()));
	pixmapHeader->DrawText(cPoint(2*border, (lineHeight - fontHeader->Height())/2), *dateTime, Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);
    
    const char *Title = info->Title();
    if (isempty(Title))
        Title = recording->Name();
	cTextWrapper title;
	title.Set(Title, fontHeaderLarge, widthTextHeader);
    int currentLineHeight = lineHeight;
	for (int i=0; i < title.Lines(); i++) {
        pixmapHeader->DrawText(cPoint(2*border, currentLineHeight), title.GetLine(i), Theme.Color(clrMenuFontDetailViewHeaderTitle), clrTransparent, fontHeaderLarge);
        currentLineHeight += lineHeight;
    }
	
    if (!isempty(info->ShortText())) {
        cTextWrapper shortText;
        shortText.Set(info->ShortText(), fontHeader, widthTextHeader);
        for (int i=0; i < shortText.Lines(); i++) {
            if ((currentLineHeight + fontHeader->Height()) < headerHeight) {
                pixmapHeader->DrawText(cPoint(2*border, currentLineHeight), shortText.GetLine(i), Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);
                currentLineHeight += fontHeader->Height();
        } else
            break;
        }
    }
}

//---------------cNopacityMenuDetailTextView---------------------

cNopacityMenuDetailTextView::cNopacityMenuDetailTextView(cOsd *osd) : cNopacityMenuDetailView(osd) {
}

cNopacityMenuDetailTextView::~cNopacityMenuDetailTextView(void) {
    osd->DestroyPixmap(pixmapContent);
}

void cNopacityMenuDetailTextView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + config.fontDetailView);
    fontHeaderLarge = NULL;
    fontHeader = NULL;
}

void cNopacityMenuDetailTextView::CreatePixmaps(void) {
    pixmapContent = osd->CreatePixmap(3, cRect(0, top + headerHeight, width, contentHeight),
                                         cRect(0, 0, width, contentDrawPortHeight));

    pixmapContent->Fill(clrTransparent);
}

void cNopacityMenuDetailTextView::Render(void) {
    DrawContent();
}