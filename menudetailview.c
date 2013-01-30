#include "menudetailview.h"
#include "services/epgsearch.h"
#include <sstream>

cNopacityMenuDetailView::cNopacityMenuDetailView(cOsd *osd) {
    this->osd = osd;
    hasScrollbar = false;
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

int cNopacityMenuDetailView::DrawTextWrapper(cTextWrapper *wrapper, int top) {
    int linesText = wrapper->Lines();
    int textHeight = font->Height();
    int currentHeight = 0;
    for (int i=0; i < linesText; i++) {
        currentHeight = (i+1)*textHeight + top;
        pixmapContent->DrawText(cPoint(2*border, currentHeight), wrapper->GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
    }
    return currentHeight + textHeight;
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
    numEPGPics = 0;
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

void cNopacityMenuDetailEventView::SetContent(void) {
    if (event) {
        content.Set(event->Description(), font, width - 4 * border);
        if (config.displayRerunsDetailEPGView) {
            LoadReruns();
        }
    }
}

void cNopacityMenuDetailEventView::SetContentHeight(void) {
    int lineHeight = font->Height();
    int linesContent = content.Lines() + 1;
    if (config.displayRerunsDetailEPGView) {
        linesContent+= reruns.Lines() + 2;
    }
    int heightContentText = linesContent * lineHeight;
    
    if (config.displayAdditionalEPGPictures) {
        heightContentText += HeightEPGPics();
    }

    if (heightContentText > contentHeight) {
        contentDrawPortHeight = heightContentText;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;  
    }
}

int cNopacityMenuDetailEventView::HeightEPGPics(void) {
    int numPicsAvailable = 0;
    for (int i=1; i <= config.numAdditionalEPGPictures; i++) {
        cString epgimage;
        if (config.epgImagePathSet) {
            epgimage = cString::sprintf("%s%d_%d.jpg", *config.epgImagePath, event->EventID(), i);
        } else {
            epgimage = cString::sprintf("%s%d_%d.jpg", *config.epgImagePathDefault, event->EventID(), i);
        }
        FILE *fp = fopen(*epgimage, "r");
        if (fp) {
            numPicsAvailable = i;
            fclose(fp);
        } else {
            break;
        }
    }
    numEPGPics = numPicsAvailable;
    int picsPerLine = width / (config.epgImageWidthLarge + border);
    int picLines = numPicsAvailable / picsPerLine;
    if (numPicsAvailable%picsPerLine != 0)
        picLines++;
    return picLines * (config.epgImageHeightLarge + border) + 2*border;
}

void cNopacityMenuDetailEventView::Render(void) {
    DrawHeader();
    int currentHight = DrawTextWrapper(&content, 0);
    if (config.displayRerunsDetailEPGView) {
        currentHight = DrawTextWrapper(&reruns, currentHight);
    }
    if (config.displayAdditionalEPGPictures) {
        DrawEPGPictures(currentHight);
    }
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

void cNopacityMenuDetailEventView::LoadReruns(void) {
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
                sstrReruns << tr("RERUNS OF THIS SHOW") << ':' << std::endl;
                int i = 0;
                for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < config.numReruns; r = list->Next(r)) {
                    if ((event->ChannelID() == r->event->ChannelID()) && (event->StartTime() == r->event->StartTime()))
                        continue;
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
        reruns.Set(sstrReruns.str().c_str(), font, width - 4 * border);
    }
}

void cNopacityMenuDetailEventView::DrawEPGPictures(int height) {
    int picsPerLine = width / (config.epgImageWidthLarge + border);
    int currentX = border;
    int currentY = height + border;
    int currentPicsPerLine = 1;
    cImageLoader imgLoader;
    for (int i=1; i <= numEPGPics; i++) {
        cString epgimage = cString::sprintf("%d_%d", event->EventID(), i);
        if (imgLoader.LoadAdditionalEPGImage(epgimage)) {
            pixmapContent->DrawImage(cPoint(currentX, currentY), imgLoader.GetImage());
            if (currentPicsPerLine < picsPerLine) {
                currentX += config.epgImageWidthLarge + border;
                currentPicsPerLine++;
            } else {
                currentX = border;
                currentY += config.epgImageHeightLarge + border;
                currentPicsPerLine = 1;
            }
        } else {
            break;
        }
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

void cNopacityMenuDetailRecordingView::SetContent(void) {
    if (recording) {
        content.Set(recording->Info()->Description(), font, width - 4 * border);
        LoadRecordingInformation();
    }   
}

void cNopacityMenuDetailRecordingView::SetContentHeight(void) {
    int lineHeight = font->Height();
    int linesContent = content.Lines() + 1;
    linesContent+= additionalInfo.Lines() + 1;
    int heightContentText = linesContent * lineHeight;

    if (heightContentText > contentHeight) {
        contentDrawPortHeight = heightContentText;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;  
    }
}

void cNopacityMenuDetailRecordingView::Render(void) {
    DrawHeader();
    int currentHight = DrawTextWrapper(&content, 0);
    DrawTextWrapper(&additionalInfo, currentHight);
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

void cNopacityMenuDetailRecordingView::LoadRecordingInformation(void) {
    const cRecordingInfo *Info = recording->Info();
    unsigned long long nRecSize = -1;
    unsigned long long nFileSize[1000];
    nFileSize[0] = 0;
    int i = 0;
    struct stat filebuf;
    cString filename;
    int rc = 0;
    do {
        if (recording->IsPesRecording())
            filename = cString::sprintf("%s/%03d.vdr", recording->FileName(), ++i);
        else
            filename = cString::sprintf("%s/%05d.ts", recording->FileName(), ++i);
        rc = stat(filename, &filebuf);
        if (rc == 0)
            nFileSize[i] = nFileSize[i-1] + filebuf.st_size;
        else
            if (ENOENT != errno) {
                nRecSize = -1;
            }
    } while (i <= 999 && !rc);
    nRecSize = nFileSize[i-1];

    cMarks marks;
    bool fHasMarks = marks.Load(recording->FileName(), recording->FramesPerSecond(), recording->IsPesRecording()) && marks.Count();
    cIndexFile *index = new cIndexFile(recording->FileName(), false, recording->IsPesRecording());

    int nCutLength = 0;
    long nCutInFrame = 0;
    unsigned long long nRecSizeCut = nRecSize < 0 ? -1 : 0;
    unsigned long long nCutInOffset = 0;

    if (fHasMarks && index) {
        uint16_t FileNumber;
        off_t FileOffset;

        bool fCutIn = true;
        cMark *mark = marks.First();
        while (mark) {
            int pos = mark->Position();
            index->Get(pos, &FileNumber, &FileOffset); //TODO: will disc spin up?
            if (fCutIn) {
                nCutInFrame = pos;
                fCutIn = false;
                if (nRecSize >= 0)
                    nCutInOffset = nFileSize[FileNumber-1] + FileOffset;
            } else {
                nCutLength += pos - nCutInFrame;
                fCutIn = true;
                if (nRecSize >= 0)
                    nRecSizeCut += nFileSize[FileNumber-1] + FileOffset - nCutInOffset;
            }
            cMark *nextmark = marks.Next(mark);
            mark = nextmark;
        }
        if (!fCutIn) {
            nCutLength += index->Last() - nCutInFrame;
            index->Get(index->Last() - 1, &FileNumber, &FileOffset);
            if (nRecSize >= 0)
                nRecSizeCut += nFileSize[FileNumber-1] + FileOffset - nCutInOffset;
        }
    }

    std::stringstream sstrInfo;

    cChannel *channel = Channels.GetByChannelID(Info->ChannelID());
    if (channel)
        sstrInfo << trVDR("Channel") << ": " << channel->Number() << " - " << channel->Name() << std::endl;
    if (nRecSize < 0) {
        if ((nRecSize = ReadSizeVdr(recording->FileName())) < 0) {
            nRecSize = DirSizeMB(recording->FileName());
        }
    }
    if (nRecSize >= 0) {
        cString strRecSize = "";
        if (fHasMarks) {
            if (nRecSize > MEGABYTE(1023))
                strRecSize = cString::sprintf("%s: %.2f GB (%s: %.2f GB)", tr("Size"), (float)nRecSize / MEGABYTE(1024), tr("cut"), (float)nRecSizeCut / MEGABYTE(1024));
            else
                strRecSize = cString::sprintf("%s: %lld MB (%s: %lld MB)", tr("Size"), nRecSize / MEGABYTE(1), tr("cut"), nRecSizeCut / MEGABYTE(1));
        } else {
            if (nRecSize > MEGABYTE(1023))
                strRecSize = cString::sprintf("%s: %.2f GB", tr("Size"), (float)nRecSize / MEGABYTE(1024));
            else
                strRecSize = cString::sprintf("%s: %lld MB", tr("Size"), nRecSize / MEGABYTE(1));
        }
        sstrInfo << (const char*)strRecSize << std::endl;
    }
    
    if (index) {
        int nLastIndex = index->Last();
        if (nLastIndex) {
            cString strLength;
            if (fHasMarks) {
                strLength = cString::sprintf("%s: %s (%s %s)", tr("Length"), *IndexToHMSF(nLastIndex, false, recording->FramesPerSecond()), tr("cut"), *IndexToHMSF(nCutLength, false, recording->FramesPerSecond()));
            } else {
                strLength = cString::sprintf("%s: %s", tr("Length"), *IndexToHMSF(nLastIndex, false, recording->FramesPerSecond()));
            }
            sstrInfo << (const char*)strLength << std::endl;
            cString strBitrate = cString::sprintf("%s: %s\n%s: %.2f MBit/s (Video+Audio)", tr("Format"), recording->IsPesRecording() ? "PES" : "TS", tr("Est. bitrate"), (float)nRecSize / nLastIndex * recording->FramesPerSecond() * 8 / MEGABYTE(1));
            sstrInfo << (const char*)strBitrate << std::endl;
        }
    }
    delete index;
    
    additionalInfo.Set(sstrInfo.str().c_str(), font, width - 4 * border);
}

int cNopacityMenuDetailRecordingView::ReadSizeVdr(const char *strPath) {
    int dirSize = -1;
    char buffer[20];
    char *strFilename = NULL;
    if (-1 != asprintf(&strFilename, "%s/size.vdr", strPath)) {
        struct stat st;
        if (stat(strFilename, &st) == 0) {
                int fd = open(strFilename, O_RDONLY);
            if (fd >= 0) {
                if (safe_read(fd, &buffer, sizeof(buffer)) >= 0) {
                    dirSize = atoi(buffer);
                }
                close(fd);
            }
        }
        free(strFilename);
    }
    return dirSize;
}

//---------------cNopacityMenuDetailTextView---------------------

cNopacityMenuDetailTextView::cNopacityMenuDetailTextView(cOsd *osd, const char *text) : cNopacityMenuDetailView(osd) {
    this->text = text;
}

cNopacityMenuDetailTextView::~cNopacityMenuDetailTextView(void) {
    osd->DestroyPixmap(pixmapContent);
}

void cNopacityMenuDetailTextView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + config.fontDetailView);
    fontHeaderLarge = NULL;
    fontHeader = NULL;
}

void cNopacityMenuDetailTextView::SetContent(void) {
    content.Set(text, font, width - 4 * border);
}

void cNopacityMenuDetailTextView::SetContentHeight(void) {
    int lineHeight = font->Height();
    int linesContent = content.Lines() + 1;

    int heightContentText = linesContent * lineHeight;
    if (heightContentText > contentHeight) {
        contentDrawPortHeight = heightContentText;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;  
    }
}

void cNopacityMenuDetailTextView::CreatePixmaps(void) {
    pixmapContent = osd->CreatePixmap(3, cRect(0, top + headerHeight, width, contentHeight),
                                         cRect(0, 0, width, contentDrawPortHeight));

    pixmapContent->Fill(clrTransparent);
}

void cNopacityMenuDetailTextView::Render(void) {
    DrawTextWrapper(&content, 0);
}