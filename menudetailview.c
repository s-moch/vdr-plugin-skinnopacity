#include "menudetailview.h"

/********************************************************************************************
* cNopacityDetailView
********************************************************************************************/
cNopacityDetailView::cNopacityDetailView(cOsd *osd, const cEvent *ev, cPixmap *s, cPixmap *sBack) {
    type = dvEvent;
    this->osd = osd;
    this->scrollBar = s;
    this->scrollBarBack = sBack;
    this->ev = ev;
    this->rec = NULL;
    this->text = NULL;
    view = NULL;
    viewLight = NULL;
    Init();
}

cNopacityDetailView::cNopacityDetailView(cOsd *osd, const cRecording *rec, cPixmap *s, cPixmap *sBack) {
    type = dvRecording;
    this->osd = osd;
    this->scrollBar = s;
    this->scrollBarBack = sBack;
    this->ev = NULL;
    this->rec = rec;
    this->text = NULL;
    view = NULL;
    viewLight = NULL;
    Init();
}

cNopacityDetailView::cNopacityDetailView(cOsd *osd, const char *text, cPixmap *s, cPixmap *sBack) {
    type = dvText;
    this->osd = osd;
    this->scrollBar = s;
    this->scrollBarBack = sBack;
    this->ev = NULL;
    this->rec = NULL;
    this->text = text;
    view = NULL;
    viewLight = NULL;
    Init();
}

void cNopacityDetailView::Init(void) {
    x = (config.GetValue("menuAdjustLeft")) ? 0 : geoManager->osdWidth - geoManager->menuContentWidthFull + 2 * geoManager->menuSpace;
    width = geoManager->menuContentWidthFull - 2 * geoManager->menuSpace;
    height = geoManager->menuContentHeight;
    top = geoManager->menuHeaderHeight;
    border = 30;
    headerHeight = 0;
    switch (type) {
        case dvEvent:
            headerHeight = (config.GetValue("tabsInDetailView")) ? config.GetValue("headerDetailedEPG") * height / 100
                                                                : std::max(config.GetValue("logoHeightOriginal"), config.GetValue("epgImageHeight")) + 4;
            border = config.GetValue("borderDetailedEPG");
            break;
        case dvRecording:
            headerHeight = (config.GetValue("tabsInDetailView")) ? config.GetValue("headerDetailedRecordings") * height / 100
                                                                : config.GetValue("epgImageHeight") + 4;
            border = config.GetValue("borderDetailedRecordings");
            break;
        case dvText:
            headerHeight = 0;
            break;
        default:
            break;
    }
}

cNopacityDetailView::~cNopacityDetailView(void) {
    if (view)
        delete view;
    if (viewLight)
        delete viewLight;
}

void cNopacityDetailView::SetAlpha(int Alpha) {
    if (view) view->SetAlpha(Alpha);
    if (viewLight)
        viewLight->SetAlpha(Alpha);
}

void cNopacityDetailView::InitiateViewType(void) {
    if ((config.GetValue("tabsInDetailView") == 0) && !(type == dvText)) {
        switch (type) {
            case dvEvent:
                viewLight = new cNopacityMenuDetailEventViewLight(osd, ev, scrollBar, scrollBarBack);
                break;
            case dvRecording:
                viewLight = new cNopacityMenuDetailRecordingViewLight(osd, rec, scrollBar, scrollBarBack);
                break;
            default:
                break;
        }
        viewLight->SetGeometry(x, top, width, height, border, headerHeight);
        viewLight->Render();
        return;
    }
    static cPlugin *pScraper = GetScraperPlugin();
    ScraperGetEventType call;
    switch (type) {
        case dvEvent: {
            if (!ev)
                break;
            call.event = ev;
            if (!pScraper || (config.GetValue("scraperInfo") == 0)) {
                view = new cNopacityEPGView(osd);
            } else if (pScraper->Service("GetEventType", &call)) {
                if (call.type == tMovie) {
                    view = new cNopacityMovieView(osd, call.movieId);
                } else if (call.type == tSeries) {
                    view = new cNopacitySeriesView(osd, call.seriesId, call.episodeId);
                } else {
                    view = new cNopacityEPGView(osd);
                }
            } else {
                view = new cNopacityEPGView(osd);
            }
            view->SetTitle(ev->Title());
            view->SetSubTitle(ev->ShortText());
            view->SetInfoText(ev->Description());
            cString dateTime;
            time_t vps = ev->Vps();
            if (vps) {
                dateTime = cString::sprintf("%s  %s - %s (%d %s) VPS: %s", *ev->GetDateString(), *ev->GetTimeString(), *ev->GetEndTimeString(), ev->Duration()/60, tr("min"), *TimeString(vps));
            } else {
                dateTime = cString::sprintf("%s  %s - %s (%d %s)", *ev->GetDateString(), *ev->GetTimeString(), *ev->GetEndTimeString(), ev->Duration()/60, tr("min"));
            }
            view->SetDateTime(*dateTime);
            LOCK_CHANNELS_READ;
            view->SetChannel(Channels->GetByChannelID(ev->ChannelID(), true));
            view->SetEventID(ev->EventID());
            break; }
        case dvRecording: {
            if (!rec)
                break;
            call.recording = rec;
            if (!pScraper || (config.GetValue("scraperInfo") == 0)) {
                view = new cNopacityEPGView(osd);
            } else if (pScraper->Service("GetEventType", &call)) {
                if (call.type == tMovie) {
                    view = new cNopacityMovieView(osd, call.movieId);
                } else if (call.type == tSeries) {
                    view = new cNopacitySeriesView(osd, call.seriesId, call.episodeId);
                } else {
                    view = new cNopacityEPGView(osd);
                }		    
            } else {
                view = new cNopacityEPGView(osd);
            }
            const cRecordingInfo *info = rec->Info();
            if (info) {
                view->SetTitle(info->Title());
                view->SetSubTitle(info->ShortText());
                view->SetInfoText(info->Description());
                LOCK_CHANNELS_READ;
                view->SetChannel(Channels->GetByChannelID(info->ChannelID(), true));
            } else {
                view->SetTitle(rec->Name());
            }
            int recDuration = rec->LengthInSeconds();
            recDuration = (recDuration>0)?(recDuration / 60):0;
            cString dateTime = cString::sprintf("%s  %s (%d %s)", *DateString(rec->Start()), *TimeString(rec->Start()), recDuration, tr("min"));
            view->SetDateTime(*dateTime);
            view->SetRecFileName(rec->FileName());
            break; }
        case dvText:
            view = new cNopacityTextView(osd);
            view->SetInfoText(text);
            break;
        default:
            break;
    }
}
void cNopacityDetailView::KeyInput(bool Up, bool Page) {
    if (viewLight) {
        viewLight->KeyInput(Up, Page);
        return;
    }
    if (!view)
        return;
    if (Up && Page) {
        view->KeyLeft();
        view->Render();
        osd->Flush();
    } else if (!Up && Page) {
        view->KeyRight();
        view->Render();
        osd->Flush();
    } else if (Up && !Page) {
        bool scrolled = view->KeyUp();
        if (scrolled) {
            view->SetScrollbarPixmaps(scrollBar, scrollBarBack);
            view->DrawScrollbar();
            osd->Flush();
        }
    }else if (!Up && !Page) {
        bool scrolled = view->KeyDown();
        if (scrolled) {
            view->SetScrollbarPixmaps(scrollBar, scrollBarBack);
            view->DrawScrollbar();
            osd->Flush();
        }
    }
}

std::string cNopacityDetailView::LoadReruns(void) {
    if (!ev)
        return "";
    
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (!epgSearchPlugin)
        return "";

    if (isempty(ev->Title()))
        return "";
    
    std::stringstream sstrReruns;
    sstrReruns << tr("Reruns of ") << "\"" << ev->Title() << "\":" << std::endl << std::endl;

    Epgsearch_searchresults_v1_0 data;
    std::string strQuery = ev->Title();

    if (config.GetValue("useSubtitleRerun") > 0) {
        if (config.GetValue("useSubtitleRerun") == 2 && !isempty(ev->ShortText())) {
            strQuery += "~";
            strQuery += ev->ShortText();
        }
        data.useSubTitle = true;
    } else {
        data.useSubTitle = false;
    }
    data.query = (char *)strQuery.c_str();
    data.mode = 0;
    data.channelNr = 0;
    data.useTitle = true;
    data.useDescription = false;

    bool foundRerun = false;
    if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
        cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
        if (list && (list->Count() > 1)) {
            foundRerun = true;
            int i = 0;
            for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < config.GetValue("numReruns"); r = list->Next(r)) {
                if ((ev->ChannelID() == r->event->ChannelID()) && (ev->StartTime() == r->event->StartTime()))
                    continue;
                i++;
                sstrReruns  << *DayDateTime(r->event->StartTime());
                LOCK_CHANNELS_READ;
                const cChannel *channel = Channels->GetByChannelID(r->event->ChannelID(), true, true);
                if (channel) {
                    sstrReruns << ", " << trVDR("Channel") << " " << channel->Number() << ":";
                    sstrReruns << " " << channel->ShortName(true);
                }
                sstrReruns << "\n" << r->event->Title();
                if (!isempty(r->event->ShortText()))
                    sstrReruns << "~" << r->event->ShortText();
                sstrReruns << std::endl << std::endl;
            }
            delete list;
        }
    }

    if (!foundRerun) {
        sstrReruns << std::endl << tr("No reruns found");        
    }
    return sstrReruns.str();
}

std::string cNopacityDetailView::LoadRecordingInformation(void) {
    const cRecordingInfo *Info = rec->Info();
    unsigned long long nRecSize = -1;
    unsigned long long nFileSize[1000];
    nFileSize[0] = 0;
    int i = 0;
    struct stat filebuf;
    cString filename;
    int rc = 0;
    do {
        if (rec->IsPesRecording())
            filename = cString::sprintf("%s/%03d.vdr", rec->FileName(), ++i);
        else
            filename = cString::sprintf("%s/%05d.ts", rec->FileName(), ++i);
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
    bool fHasMarks = marks.Load(rec->FileName(), rec->FramesPerSecond(), rec->IsPesRecording()) && marks.Count();
    cIndexFile *index = new cIndexFile(rec->FileName(), false, rec->IsPesRecording());

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

    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(Info->ChannelID());
    if (channel)
        sstrInfo << trVDR("Channel") << ": " << channel->Number() << " - " << channel->Name() << std::endl;
    if (nRecSize < 0) {
        if ((nRecSize = ReadSizeVdr(rec->FileName())) < 0) {
            nRecSize = DirSizeMB(rec->FileName());
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
                strLength = cString::sprintf("%s: %s (%s %s)", tr("Length"), *IndexToHMSF(nLastIndex, false, rec->FramesPerSecond()), tr("cut"), *IndexToHMSF(nCutLength, false, rec->FramesPerSecond()));
            } else {
                strLength = cString::sprintf("%s: %s", tr("Length"), *IndexToHMSF(nLastIndex, false, rec->FramesPerSecond()));
            }
            sstrInfo << (const char*)strLength << std::endl;
            cString strBitrate = cString::sprintf("%s: %s\n%s: %.2f MBit/s (Video+Audio)", tr("Format"), rec->IsPesRecording() ? "PES" : "TS", tr("Est. bitrate"), (float)nRecSize / nLastIndex * rec->FramesPerSecond() * 8 / MEGABYTE(1));
            sstrInfo << (const char*)strBitrate << std::endl;
        }
    }
    delete index;

    if (Info) {
#if (APIVERSNUM >= 20505)
        if (Info->Errors() >= 0) {
            cString errors = cString::sprintf("%s: %i ", tr("TS Errors"), Info->Errors());
            sstrInfo << (const char*)errors << std::endl;
        }
#endif
        const char *aux = NULL;
        aux = Info->Aux();
        if (aux) {
            std::string strAux = aux;
            std::string auxEpgsearch = StripXmlTag(strAux, "epgsearch");
            if (!auxEpgsearch.empty()) {
                std::string searchTimer = StripXmlTag(auxEpgsearch, "searchtimer");
                if (!searchTimer.empty()) {
                    sstrInfo << tr("Search timer") << ": " << searchTimer << std::endl;
                }
            }
        }
    }

    return sstrInfo.str();
}

std::string cNopacityDetailView::StripXmlTag(std::string &Line, const char *Tag) {
        // set the search strings
        std::stringstream strStart, strStop;
        strStart << "<" << Tag << ">";
        strStop << "</" << Tag << ">";
        // find the strings
        std::string::size_type locStart = Line.find(strStart.str());
        std::string::size_type locStop = Line.find(strStop.str());
        if (locStart == std::string::npos || locStop == std::string::npos)
                return "";
        // extract relevant text
        int pos = locStart + strStart.str().size();
        int len = locStop - pos;
        return len < 0 ? "" : Line.substr(pos, len);
}


int cNopacityDetailView::ReadSizeVdr(const char *strPath) {
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

void cNopacityDetailView::Render(void) {
    InitiateViewType();
    if (!view)
        return;
    view->SetGeometry(x, top, width, height, border, headerHeight);
    view->SetScrollbarPixmaps(scrollBar, scrollBarBack);
    view->LoadMedia();
    if (ev)
        view->SetAdditionalInfoText(LoadReruns());
    else if (rec)
        view->SetAdditionalInfoText(LoadRecordingInformation());
    view->Render();
}

/********************************************************************************************
* cNopacityDetailViewLight
********************************************************************************************/
cNopacityMenuDetailViewLight::cNopacityMenuDetailViewLight(cOsd *osd, cPixmap *s, cPixmap *sBack) {
    this->osd = osd;
    this->scrollBar = s;
    this->scrollBarBack = sBack;
    hasScrollbar = false;
    hasManualPoster = false;
    manualPosterPath = "";
    pixmapContent = NULL;
    pixmapHeader = NULL;
    pixmapPoster = NULL;
    pixmapLogo = NULL;
    isMovie = false;
    isSeries = false;
    contentHeight = height - headerHeight;
    widthPoster = 30 * width / 100;
}

cNopacityMenuDetailViewLight::~cNopacityMenuDetailViewLight(void) {
    delete font;
    if (fontSmall)
        delete fontSmall;
    if (fontHeader)
        delete fontHeader;
    if (fontHeaderLarge)
        delete fontHeaderLarge;
}

void cNopacityMenuDetailViewLight::SetGeometry(int x, int top, int width, int height, int border, int headerHeight) {
    this->x = x;
    this->top = top;
    this->width = width;
    this->height = height;
    this->border = border;
    this->headerHeight = headerHeight;
    contentHeight = height - headerHeight;
    widthPoster = 30 * width / 100;
    SetFonts();
    SetContent();
    SetContentHeight();
    CreatePixmaps();
}

void cNopacityMenuDetailViewLight::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + 3 + config.GetValue("fontDetailView"));
    fontSmall = cFont::CreateFont(config.fontName, contentHeight / 30 + config.GetValue("fontDetailViewSmall"));
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + config.GetValue("fontDetailViewHeaderLarge"));
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.GetValue("fontDetailViewHeader"));
}

void cNopacityMenuDetailViewLight::DrawTextWrapper(cTextWrapper *wrapper, int top) {
    if (!pixmapContent || top > contentDrawPortHeight)
        return;

    int linesText = wrapper->Lines();
    int textHeight = font->Height();
    int currentHeight = top;
    for (int i=0; i < linesText; i++) {
        pixmapContent->DrawText(cPoint(border, currentHeight), wrapper->GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        currentHeight += textHeight;
    }
}

int cNopacityMenuDetailViewLight::HeightActorPics(void) {
    int numActors = 0;
    if (isMovie)
        numActors = movie.actors.size();
    else if (isSeries)
        numActors = series.actors.size();
    if (numActors < 1)
        return 0;
    if (isMovie) {
        actorThumbWidth = movie.actors[0].actorThumb.width / 2;
        actorThumbHeight = movie.actors[0].actorThumb.height / 2;        
    } else if (isSeries) {
        actorThumbWidth = series.actors[0].actorThumb.width / 2;
        actorThumbHeight = series.actors[0].actorThumb.height / 2;        
    }
    int picsPerLine = contentWidth / (actorThumbWidth + 2 * border);
    if (picsPerLine < 1)
        return 0;

// Count actors with picture
    cImageLoader imgLoader;
    int actorsToView = 0;
    for (int a = 0; a < numActors; a++) {
        if (a == numActors)
            break;
        std::string path = "";
        if (isMovie) {
            path = movie.actors[a].actorThumb.path;
        } else if (isSeries) {
            path = series.actors[a].actorThumb.path;
        }

        if (imgLoader.LoadPoster(path.c_str(), actorThumbWidth, actorThumbHeight, false)) {
            actorsToView++;
        }
    }

    int picLines = actorsToView / picsPerLine;
    if (actorsToView % picsPerLine != 0)
        picLines++;
    int actorsHeight = picLines * (actorThumbHeight + 2 * fontSmall->Height()) + fontHeader->Height();
    return actorsHeight;
}

int cNopacityMenuDetailViewLight::HeightScraperInfo(void) {
    int heightScraperInfo = 0;
    std::stringstream info, info1;
    if (isSeries) {
        if (series.overview.size() > 0) {
            info << tr("Series Overview") << ": " << series.overview << "\n";
        }
        if (series.firstAired.size() > 0) {
            info << tr("First aired") << ": " << series.firstAired << "\n";
        }
        if (series.genre.size() > 0) {
            info << tr("Genre") << ": " << series.genre << "\n";
        }
        if (series.network.size() > 0) {
            info << tr("Network") << ": " << series.network << "\n";
        }
        if (series.rating > 0) {
            info << tr("TheMovieDB Rating") << ": " << series.rating << "\n";
        }
        if (series.status.size() > 0) {
            info << tr("Status") << ": " << series.status << "\n";
        }
        if (series.episode.name.size() > 0) {
            info << "\n" << tr("Episode") << ": " << series.episode.name << " (" << tr("Season") << " " << series.episode.season << ", " << tr("Episode") << " " << series.episode.number << ")\n";
        }
        if (series.episode.overview.size() > 0) {
            info << tr("Episode Overview") << ": " << series.episode.overview << "\n";
        }
        if (series.episode.firstAired.size() > 0) {
            info << tr("First aired") << ": " << series.episode.firstAired << "\n";
        }
        if (series.episode.guestStars.size() > 0) {
            info << tr("Guest Stars") << ": " << series.episode.guestStars << "\n";
        }
        if (series.episode.rating > 0) {
            info << tr("TheMovieDB Rating") << ": " << series.episode.rating << "\n";
        }
	if (!(info.tellp() == 0)) {
            info1 << tr("TheTVDB Information") << ":\n" << info.str();
        }
    } else if (isMovie) {
        if (movie.originalTitle.size() > 0) {
            info << tr("Original Title") << ": " << movie.originalTitle << "\n";
        }
        if (movie.tagline.size() > 0) {
            info << tr("Tagline") << ": " << movie.tagline << "\n";
        }
        if (movie.overview.size() > 0) {
            info << tr("Overview") << ": " << movie.overview << "\n";
        }
        if (movie.adult) {
            info << tr("Adult") << ": " << tr("yes") << "\n";
        } else if (info.gcount() > 0) {
            info << tr("Adult") << ": " << tr("no") << "\n";
        }
        if (movie.collectionName.size() > 0) {
            info << tr("Collection") << ": " << movie.collectionName << "\n";
        }
        if (movie.budget > 0) {
            info << tr("Budget") << ": " << movie.budget << "$\n";
        }
        if (movie.revenue > 0) {
            info << tr("Revenue") << ": " << movie.revenue << "$\n";
        }
        if (movie.genres.size() > 0) {
            info << tr("Genre") << ": " << movie.genres << "\n";
        }
        if (movie.homepage.size() > 0) {
            info << tr("Homepage") << ": " << movie.homepage << "\n";
        }
        if (movie.releaseDate.size() > 0) {
            info << tr("Release Date") << ": " << movie.releaseDate << "\n";
        }
        if (movie.runtime > 0) {
            info << tr("Runtime") << ": " << movie.runtime << " " << tr("minutes") << "\n";
        }
        if (movie.popularity > 0) {
            info << tr("TheMovieDB Popularity") << ": " << movie.popularity << "\n";
        }
        if (movie.voteAverage > 0) {
            info << tr("TheMovieDB Vote Average") << ": " << movie.voteAverage << "\n";
        }
        if (!(info.tellp() == 0)) {
            info1 << tr("TheMovieDB Information") << ":\n" << info.str();
        }
    }
    scrapInfo.Set(info1.str().c_str(), font, contentWidth - 2 * border);
    int lineHeight = font->Height();
    heightScraperInfo = (scrapInfo.Lines() + 1) * lineHeight;
    if (isSeries) {
        if (series.banners.size() == 2)
            heightScraperInfo += (series.banners[1].height + lineHeight);
        else if (series.banners.size() == 3)
            heightScraperInfo += (series.banners[1].height + series.banners[2].height + 2*lineHeight);
    } 
    return heightScraperInfo;
}

int cNopacityMenuDetailViewLight::HeightFanart(void) {
    int retVal = 0;
    int fanartWidthOrig = 0;
    int fanartHeightOrig = 0;
    
    if (isMovie) {
        fanartWidthOrig = movie.fanart.width;
        fanartHeightOrig = movie.fanart.height;
    } else if (isSeries && series.fanarts.size() > 0) {
        fanartWidthOrig = series.fanarts[0].width;
        fanartHeightOrig = series.fanarts[0].height;
    }

    if (fanartWidthOrig == 0)
        return retVal;

    int fanartWidth = fanartWidthOrig;
    int fanartHeight = fanartHeightOrig;
    retVal = fanartHeight;
    if (fanartWidthOrig > 0 && fanartWidthOrig > (contentWidth - 2*border)) {
        fanartWidth = contentWidth - 2*border;
        fanartHeight = fanartHeightOrig * ((double)fanartWidth / (double)fanartWidthOrig);
        retVal = fanartHeight;
    }
    if (isSeries) {
        retVal = (retVal + font->Height()) * series.fanarts.size();
    } else if (isMovie) {
        if (movie.collectionFanart.path.size() > 0) {
            retVal = (retVal + font->Height()) * 2;
        }
    }
    return retVal;
}

void cNopacityMenuDetailViewLight::DrawPoster(void) {
    int posterWidthOrig = 0;
    int posterHeightOrig = 0;
    if (hasManualPoster) {
        posterWidthOrig = config.GetValue("posterWidth");
        posterHeightOrig = config.GetValue("posterHeight");
    } else if (isMovie) {
        if ((movie.poster.width == 0) || (movie.poster.height == 0) || (movie.poster.path.size() < 1))
            return;
        posterWidthOrig = movie.poster.width;
        posterHeightOrig = movie.poster.height;
    } else if (isSeries) {
        if (series.posters.size() < 1)
            return;
        posterWidthOrig = series.posters[0].width;
        posterHeightOrig = series.posters[0].height;
    }

    if (posterWidthOrig == 0)
        return;

    int posterWidth = posterWidthOrig;
    int posterHeight = posterHeightOrig;

    if ((posterWidthOrig > widthPoster) && (posterHeightOrig < contentHeight)) {
        posterWidth = widthPoster - 2*border;
        posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
    } else if ((posterWidthOrig < widthPoster) && (posterHeightOrig > contentHeight)) {
        posterHeight = contentHeight - 2*border;
        posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
    } else if ((posterWidthOrig > widthPoster) && (posterHeightOrig > contentHeight)) {
        int overlapWidth = posterWidthOrig - widthPoster;
        int overlapHeight = posterHeightOrig - contentHeight;
        if (overlapWidth >= overlapHeight) {
            posterWidth = widthPoster - 2*border;
            posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
        } else {
            posterHeight = contentHeight - 2*border;
            posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
        }
    }
    int posterX = (widthPoster - posterWidth) / 2;
    int posterY = (contentHeight - posterHeight) / 2;
    cImageLoader imgLoader;
    bool drawPoster = false;
    if (hasManualPoster && imgLoader.LoadPoster(*manualPosterPath, posterWidth, posterHeight)) {
        drawPoster = true;
    } else if (isSeries && imgLoader.LoadPoster(series.posters[0].path.c_str(), posterWidth, posterHeight)) {
        drawPoster = true;
    } else if (isMovie && imgLoader.LoadPoster(movie.poster.path.c_str(), posterWidth, posterHeight)) {
        drawPoster = true;
    }
    if (drawPoster) {
        if (pixmapPoster)
            pixmapPoster->DrawImage(cPoint(posterX, posterY), imgLoader.GetImage());        
    }
}

void cNopacityMenuDetailViewLight::DrawBanner(int height) {
    int bannerWidthOrig = 0;
    int bannerHeightOrig = 0;
    std::string bannerPath = "";
    int seasonPosterWidth = 0;
    int seasonPosterHeight = 0;
    std::string seasonPoster = "";
    if (isSeries && series.banners.size() > 0) {
        bannerWidthOrig = series.banners[0].width;
        bannerHeightOrig = series.banners[0].height;
        bannerPath = series.banners[0].path;
        seasonPoster = series.seasonPoster.path;
        seasonPosterWidth = series.seasonPoster.width / 2;
        seasonPosterHeight = series.seasonPoster.height / 2;
    }

    if (bannerWidthOrig == 0)
        return;

    int bannerWidth = bannerWidthOrig;
    int bannerHeight = bannerHeightOrig;
    int bannerX = (contentWidth - bannerWidth) / 2;

    if (isSeries && seasonPoster.size() > 0) {
        int spaceBanner = contentWidth - 3 * border - seasonPosterWidth;
        if (spaceBanner < bannerWidthOrig) {
            bannerWidth = spaceBanner;
            bannerHeight = bannerHeightOrig * ((double)bannerWidth / (double)bannerWidthOrig);
            bannerX = ((contentWidth - seasonPosterWidth) - bannerWidth)/2;
        }
    } else if (bannerWidthOrig > contentWidth - 2*border) {
        bannerWidth = contentWidth - 2*border;
        bannerHeight = bannerHeightOrig * ((double)bannerWidth / (double)bannerWidthOrig);
        bannerX = (contentWidth - bannerWidth) / 2;
    }
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(bannerPath.c_str(), bannerWidth, bannerHeight)) {
        if (pixmapContent)
            pixmapContent->DrawImage(cPoint(bannerX, height), imgLoader.GetImage());
    }
    if (imgLoader.LoadPoster(seasonPoster.c_str(), seasonPosterWidth, seasonPosterHeight)) {
        if (pixmapContent)
            pixmapContent->DrawImage(cPoint(contentWidth - seasonPosterWidth - border, height), imgLoader.GetImage());
    }    
}

void cNopacityMenuDetailViewLight::DrawAdditionalBanners(int top, int bottom) {
    if (series.banners.size() < 2)
        return;
    int bannerWidthOrig = series.banners[1].width;
    int bannerHeightOrig = series.banners[1].height;
    int bannerWidth = bannerWidthOrig;
    int bannerHeight = bannerHeightOrig;
    
    if (bannerWidthOrig == 0)
        return;

    if (bannerWidthOrig > contentWidth - 2*border) {
        bannerWidth = contentWidth - 2*border;
        bannerHeight = bannerHeightOrig * ((double)bannerWidth / (double)bannerWidthOrig);
    }
    int bannerX = (contentWidth - bannerWidth) / 2;
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(series.banners[1].path.c_str(), bannerWidth, bannerHeight)) {
        if (pixmapContent)
            pixmapContent->DrawImage(cPoint(bannerX, top), imgLoader.GetImage());
    }

    if (series.banners.size() < 3)
        return;
    if (imgLoader.LoadPoster(series.banners[2].path.c_str(), bannerWidth, bannerHeight)) {
        if (pixmapContent)
            pixmapContent->DrawImage(cPoint(bannerX, bottom - bannerHeight - font->Height()), imgLoader.GetImage());
    }
}

void cNopacityMenuDetailViewLight::DrawActors(int height) {
    if (!pixmapContent)
        return;

    int numActors = 0;
    if (isMovie)
        numActors = movie.actors.size();
    else if (isSeries)
        numActors = series.actors.size();
    if (numActors < 1)
        return;

    cString header = cString::sprintf("%s:", tr("Actors"));
    pixmapContent->DrawText(cPoint(border, height), *header, Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);

    int picsPerLine = contentWidth / (actorThumbWidth + 2 * border);
    int picLines = numActors / picsPerLine;
    if (numActors % picsPerLine != 0)
        picLines++;
    int x = 0;
    int y = height + font->Height();
    cImageLoader imgLoader;
    int actor = 0;
    for (int row = 0; row < picLines; row++) {
        for (int col = 0; col < picsPerLine;) {
            if (actor == numActors)
                break;
            std::string path = "";
            std::string name = "";
            std::stringstream sstrRole;
            if (isMovie) {
                path = movie.actors[actor].actorThumb.path;
                name = movie.actors[actor].name;
                sstrRole << "\"" << movie.actors[actor].role << "\"";
            } else if (isSeries) {
                path = series.actors[actor].actorThumb.path;
                name = series.actors[actor].name;
                sstrRole << "\"" << series.actors[actor].role << "\"";
            }
            std::string role = sstrRole.str();
            if (imgLoader.LoadPoster(path.c_str(), actorThumbWidth, actorThumbHeight)) {
                pixmapContent->DrawImage(cPoint(x + border, y), imgLoader.GetImage());
                if (fontSmall->Width(name.c_str()) > actorThumbWidth + 2 * border)
                    name = CutText(name, actorThumbWidth + 2 * border, fontSmall);
                if (fontSmall->Width(role.c_str()) > actorThumbWidth + 2 * border)
                    role = CutText(role, actorThumbWidth + 2 * border, fontSmall);
                int xName = x + ((actorThumbWidth + 2 * border) - fontSmall->Width(name.c_str())) / 2;
                int xRole = x + ((actorThumbWidth + 2 * border) - fontSmall->Width(role.c_str())) / 2;
                pixmapContent->DrawText(cPoint(xName, y + actorThumbHeight), name.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                pixmapContent->DrawText(cPoint(xRole, y + actorThumbHeight + fontSmall->Height()), role.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                x += actorThumbWidth + 2 * border;
                col++;
            }
            actor++;
        }
        x = 0;
        y += actorThumbHeight + 2 * fontSmall->Height();
    }
}

void cNopacityMenuDetailViewLight::DrawFanart(int height) {
    if (isSeries && series.fanarts.size() < 1)
        return;
    
    int fanartWidthOrig = 0;
    int fanartHeightOrig = 0;
    std::string fanartPath = "";
    if (isMovie) {
        fanartWidthOrig = movie.fanart.width;
        fanartHeightOrig = movie.fanart.height;
        fanartPath = movie.fanart.path;
    } else if (isSeries) {
        fanartWidthOrig = series.fanarts[0].width;
        fanartHeightOrig = series.fanarts[0].height;
        fanartPath = series.fanarts[0].path;
    }

    if (fanartWidthOrig == 0)
        return;

    int fanartWidth = fanartWidthOrig;
    int fanartHeight = fanartHeightOrig;

    if (fanartWidthOrig > contentWidth - 2*border) {
        fanartWidth = contentWidth - 2*border;
        fanartHeight = fanartHeightOrig * ((double)fanartWidth / (double)fanartWidthOrig);
    }
    cImageLoader imgLoader;
    if (isMovie) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        if (imgLoader.LoadPoster(fanartPath.c_str(), fanartWidth, fanartHeight)) {
            if (pixmapContent)
                pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
        }
        if (movie.collectionFanart.path.size() > 0) {
            if (imgLoader.LoadPoster(movie.collectionFanart.path.c_str(), fanartWidth, fanartHeight)) {
                if (pixmapContent)
                    pixmapContent->DrawImage(cPoint(fanartX, height + fanartHeight + font->Size()), imgLoader.GetImage());
            }            
        }
    } else if (isSeries) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        for (std::vector<cTvMedia>::iterator f = series.fanarts.begin(); f != series.fanarts.end(); f++) {
            cTvMedia m = *f;
            if (imgLoader.LoadPoster(m.path.c_str(), fanartWidth, fanartHeight)) {
                if (pixmapContent)
                    pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
                height += fanartHeight + font->Height();
            }
        }        
    } else {
        if (imgLoader.LoadPoster(fanartPath.c_str(), fanartWidth, fanartHeight)) {
            int fanartX = (contentWidth - fanartWidth) / 2;
            if (pixmapContent)
                pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
        }        
    }
}

void cNopacityMenuDetailViewLight::ClearScrollbar(void) {
    scrollBar->Fill(clrTransparent);
    scrollBarBack->Fill(clrTransparent);
}

void cNopacityMenuDetailViewLight::DrawScrollbar(void) {
    ClearScrollbar();
    if (!hasScrollbar || !pixmapContent)
        return;

    int totalBarHeight = scrollBar->ViewPort().Height() - 6;

    int aktHeight = (-1)*pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();

    int barHeight = (double)(screenHeight * totalBarHeight) / (double)totalHeight ;
    int barTop = (double)(aktHeight * totalBarHeight) / (double)totalHeight ;

    if (config.GetValue("displayType") == dtGraphical) {
        cImage *image = imgCache->GetSkinElement(seScrollbar);
        if (image)
            scrollBarBack->DrawImage(cPoint(0, 0), *image);
        } else {
            scrollBarBack->Fill(Theme.Color(clrMenuScrollBar));
            scrollBarBack->DrawRectangle(cRect(2,2,geoManager->menuWidthScrollbar-4, scrollBarBack->ViewPort().Height() - 4), Theme.Color(clrMenuScrollBarBack));
        }

    scrollBar->DrawRectangle(cRect(3,3 + barTop,geoManager->menuWidthScrollbar-6,barHeight), Theme.Color(clrMenuScrollBar));
}

void cNopacityMenuDetailViewLight::KeyInput(bool Up, bool Page) {
    if (!hasScrollbar || !pixmapContent)
        return;

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
            if (totalHeight - ((-1) * aktHeight) > screenHeight) {
                pixmapContent->SetDrawPortPoint(cPoint(0, aktHeight - lineHeight));
                scrolled = true;
            }
        }
    }
    if (scrolled)
        DrawScrollbar();
}

void cNopacityMenuDetailViewLight::SetAlpha(int Alpha) {
    PixmapSetAlpha(pixmapContent, Alpha);
    PixmapSetAlpha(pixmapHeader, Alpha);
    PixmapSetAlpha(pixmapPoster, Alpha);
    PixmapSetAlpha(pixmapLogo, Alpha);
}

//---------------cNopacityMenuDetailEventViewLight---------------------

cNopacityMenuDetailEventViewLight::cNopacityMenuDetailEventViewLight(cOsd *osd, const cEvent *Event, cPixmap *s, cPixmap *sBack) : cNopacityMenuDetailViewLight(osd, s, sBack) {
    event = Event;
    numEPGPics = 0;
}

cNopacityMenuDetailEventViewLight::~cNopacityMenuDetailEventViewLight(void) {
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapContent);
    osd->DestroyPixmap(pixmapLogo);
    osd->DestroyPixmap(pixmapPoster);
}

void cNopacityMenuDetailEventViewLight::SetContent(void) {
    if (event) {
        static cPlugin *pScraper = GetScraperPlugin();
        if (pScraper && (config.GetValue("scraperInfo") == 1)) {
            ScraperGetEventType call;
            call.event = event;
            int seriesId = 0;
            int episodeId = 0;
            int movieId = 0;
            if (pScraper->Service("GetEventType", &call)) {
                esyslog("nopacity: event: %d, %s", event->EventID(), event->Title());
                esyslog("nopacity: Type detected: %d, seriesId %d, episodeId %d, movieId %d", call.type, call.seriesId, call.episodeId, call.movieId);
                seriesId = call.seriesId;
                episodeId = call.episodeId;
                movieId = call.movieId;
            }
            if (seriesId > 0) {
                series.seriesId = seriesId;
                series.episodeId = episodeId;
                if (pScraper->Service("GetSeries", &series)) {
                    isSeries = true;
                }
            } else if (movieId > 0) {
                movie.movieId = movieId;
                if (pScraper->Service("GetMovie", &movie)) {
                    isMovie = true;
                }
            }
        }
        contentWidth = width;
        contentX = 0;
        bool displayPoster = false;
        if (isSeries && series.posters.size() > 0) {
            displayPoster = true;
        } else if (isMovie && (movie.poster.width > 0) && (movie.poster.height > 0) && (movie.poster.path.size() > 0)) {
            displayPoster = true;
        }
        if (displayPoster) {
            contentWidth -=  widthPoster;
            contentX = widthPoster;
        }
        epgText.Set(event->Description(), font, contentWidth - 2 * border);
        if (config.GetValue("displayRerunsDetailEPGView")) {
            LoadReruns();
        }
    }
}

void cNopacityMenuDetailEventViewLight::SetContentHeight(void) {
    int lineHeight = font->Height();
    //Height of banner (only for series)
    int heightBanner = 0;
    if (isSeries) {
        if (isSeries && series.banners.size() > 0) {
            heightBanner = series.banners[0].height;
        }
    }
    //Height of EPG Text
    int heightEPG = epgText.Lines() * lineHeight;
    //Height of rerun information
    int heightReruns = 0;
    if (config.GetValue("displayRerunsDetailEPGView")) {
        heightReruns = reruns.Lines() * lineHeight;
    }
    //Height of actor pictures
    int heightActors = 0;
    if (isMovie || isSeries) {
        heightActors = HeightActorPics();
    }

    //Height of additional scraper info
    int heightScraperInfo = 0;
    if (isMovie || isSeries) {
        heightScraperInfo = HeightScraperInfo();
    }

    //Height of fanart
    int heightFanart = 0;
    if (isMovie || isSeries) {
        heightFanart = HeightFanart();
    }
    //Height of EPG Pictures
    int heightEPGPics = 0;
    if ((config.GetValue("displayAdditionalEPGPictures") == 1) || ((config.GetValue("displayAdditionalEPGPictures") == 2) && !isMovie &&  !isSeries)) {
        heightEPGPics = HeightEPGPics();
    }

    yBanner    = 0;
    yEPGText   = yBanner     + heightBanner + ((heightBanner) ? lineHeight : 0);
    yAddInf    = yEPGText    + heightEPG + ((heightEPG) ? lineHeight : 0);
    yActors    = yAddInf     + heightReruns + ((heightReruns) ? lineHeight : 0);
    yScrapInfo = yActors     + heightActors + ((heightActors) ? lineHeight : 0);
    yFanart    = yScrapInfo  + heightScraperInfo + ((heightScraperInfo) ? lineHeight : 0);
    yEPGPics   = yFanart     + heightFanart + ((heightFanart) ? lineHeight : 0);

    int totalHeight = yEPGPics + heightEPGPics + border;

    //check if pixmap content has to be scrollable
    if (totalHeight > contentHeight) {
        contentDrawPortHeight = totalHeight;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;
    }
}

void cNopacityMenuDetailEventViewLight::CreatePixmaps(void) {
    contentHeight = contentHeight - border;
    pixmapHeader = CreatePixmap(osd, "pixmapHeader", 3, cRect(x, top, width, headerHeight));

    pixmapContent = CreatePixmap(osd, "pixmapContent", 3, cRect(x + contentX, top + headerHeight + border, contentWidth, contentHeight),
                                                          cRect(0, 0, contentWidth, contentDrawPortHeight));

    pixmapLogo = CreatePixmap(osd, "pixmapLogo", 4, cRect(x + border,
                                                          top + max((headerHeight - config.GetValue("logoHeightOriginal")) / 2, 1),
                                                          config.GetValue("logoWidthOriginal"),
                                                          config.GetValue("logoHeightOriginal")));

    if (isSeries || isMovie) {
        pixmapPoster = CreatePixmap(osd, "pixmapPoster", 4, cRect(x, top + headerHeight, widthPoster, contentHeight));
    }

    PixmapFill(pixmapHeader, clrTransparent);
    PixmapFill(pixmapContent, clrTransparent);
    PixmapFill(pixmapLogo, clrTransparent);
    PixmapFill(pixmapPoster, clrTransparent);
    if (pixmapHeader)
        pixmapHeader->DrawRectangle(cRect(0, headerHeight - 2, width, 2), Theme.Color(clrMenuBorder));
}

void cNopacityMenuDetailEventViewLight::Render(void) {
    DrawHeader();
    //draw EPG text
    DrawTextWrapper(&epgText, yEPGText);
    //draw reruns
    if (config.GetValue("displayRerunsDetailEPGView")) {
        DrawTextWrapper(&reruns, yAddInf);
    }
    //draw additional scraper info
    if (isMovie) {
        DrawTextWrapper(&scrapInfo, yScrapInfo);
    } else if (isSeries) {
        int yInfo = yScrapInfo + font->Height();
        if (series.banners.size() > 1)
            yInfo += series.banners[1].height;
        DrawTextWrapper(&scrapInfo, yInfo);
    } 
    if ((isSeries || isMovie)) {
        DrawPoster();
    }
    //draw banner only for series
    if (isSeries) {
        DrawBanner(yBanner);
    }
    //draw actors
    if ((isSeries || isMovie)) {
       DrawActors(yActors);
    }
    //draw additional banners
    if (isSeries) {
       DrawAdditionalBanners(yScrapInfo, yFanart);
    }
    //draw fanart
    if ((isSeries || isMovie)) {
       DrawFanart(yFanart);
    }
    //draw additional EPG Pictures
    if (((config.GetValue("displayAdditionalEPGPictures") == 1) || ((config.GetValue("displayAdditionalEPGPictures") == 2) && !isMovie && !isSeries))) {
        DrawEPGPictures(yEPGPics);
    }
    DrawScrollbar();
}

int cNopacityMenuDetailEventViewLight::HeightEPGPics(void) {
    int numPicsAvailable = 0;
    for (int i=1; i <= config.GetValue("numAdditionalEPGPictures"); i++) {
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
    int picsPerLine = contentWidth / (config.GetValue("epgImageWidthLarge") + border);
    int picLines = numPicsAvailable / picsPerLine;
    if (numPicsAvailable % picsPerLine != 0)
        picLines++;
    return picLines * (config.GetValue("epgImageHeightLarge") + border) - border;
}

void cNopacityMenuDetailEventViewLight::DrawHeader(void) {
    int logoWidth = config.GetValue("logoWidthOriginal");
    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(event->ChannelID(), true);
    if (channel) {
        cImage *logo = imgCache->GetLogo(ctLogo, channel);
        if (pixmapLogo && logo) {
            pixmapLogo->DrawImage(cPoint(0, max((headerHeight - config.GetValue("logoHeightOriginal") - border)/2, 0)), *logo);
        }
    }
    if (!pixmapHeader)
        return;

    int widthTextHeader = width - 4 * border - logoWidth;
    cImageLoader imgLoader;
    if (isSeries && series.episode.episodeImage.path.size() > 0) {
        int imgWidth = series.episode.episodeImage.width;
        int imgHeight = series.episode.episodeImage.height;
        if (imgHeight > headerHeight) {
            imgHeight = headerHeight - 6;
            imgWidth = imgWidth * ((double)imgHeight / (double)series.episode.episodeImage.height);
        }
        if (imgLoader.LoadPoster(series.episode.episodeImage.path.c_str(), imgWidth, imgHeight)) {
            pixmapHeader->DrawImage(cPoint(width - imgWidth - border, (headerHeight - imgHeight)/2), imgLoader.GetImage());
            widthTextHeader -= imgWidth;
        }
    } else if (imgLoader.LoadEPGImage(event->EventID())) {
        pixmapHeader->DrawImage(cPoint(width - config.GetValue("epgImageWidth") - border, (headerHeight-config.GetValue("epgImageHeight"))/2), imgLoader.GetImage());
        if (config.GetValue("roundedCorners")) {
            int radius = config.GetValue("cornerRadius");
            int x = width - config.GetValue("epgImageWidth") - border;
            int y = (headerHeight-config.GetValue("epgImageHeight"))/2;
            DrawRoundedCorners(pixmapHeader, radius, x, y, config.GetValue("epgImageWidth"), config.GetValue("epgImageHeight"));
        }
        widthTextHeader -= config.GetValue("epgImageWidth");
    }
    int lineHeight = fontHeaderLarge->Height();

    cString dateTime;
    time_t vps = event->Vps();
    if (vps) {
        dateTime = cString::sprintf("%s  %s - %s (%d %s) VPS: %s", *event->GetDateString(), *event->GetTimeString(), *event->GetEndTimeString(), event->Duration()/60, tr("min"), *TimeString(vps));
    } else {
        dateTime = cString::sprintf("%s  %s - %s (%d %s)", *event->GetDateString(), *event->GetTimeString(), *event->GetEndTimeString(), event->Duration()/60, tr("min"));
    }
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

void cNopacityMenuDetailEventViewLight::LoadReruns(void) {
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (epgSearchPlugin && !isempty(event->Title())) {
        std::stringstream sstrReruns;
        Epgsearch_searchresults_v1_0 data;
        std::string strQuery = event->Title();
        if (config.GetValue("useSubtitleRerun") > 0) {
            if (config.GetValue("useSubtitleRerun") == 2 || !isempty(event->ShortText()))
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
                for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < config.GetValue("numReruns"); r = list->Next(r)) {
                    if ((event->ChannelID() == r->event->ChannelID()) && (event->StartTime() == r->event->StartTime()))
                        continue;
                    i++;
                    sstrReruns  << "- "
                                << *DayDateTime(r->event->StartTime());
                    LOCK_CHANNELS_READ;
                    const cChannel *channel = Channels->GetByChannelID(r->event->ChannelID(), true, true);
                    if (channel) {
                        sstrReruns << ", " << channel->Number() << ".";
                        sstrReruns << " " << channel->ShortName(true);
                    }
                    sstrReruns << ":  " << r->event->Title();
                    if (!isempty(r->event->ShortText()))
                        sstrReruns << "~" << r->event->ShortText();
                    sstrReruns << std::endl;
                }
                delete list;
            }
        }
        reruns.Set(sstrReruns.str().c_str(), font, contentWidth - 4 * border);
    } else
        reruns.Set("", font, contentWidth);
}

void cNopacityMenuDetailEventViewLight::DrawEPGPictures(int height) {
    if (!pixmapContent)
        return;

    int picsPerLine = contentWidth / (config.GetValue("epgImageWidthLarge") + border);
    int currentX = border;
    int currentY = height + border;
    int currentPicsPerLine = 1;
    cImageLoader imgLoader;
    for (int i=1; i <= numEPGPics; i++) {
        cString epgimage = cString::sprintf("%d_%d", event->EventID(), i);
        if (imgLoader.LoadAdditionalEPGImage(epgimage)) {
            pixmapContent->DrawImage(cPoint(currentX, currentY), imgLoader.GetImage());
            if (config.GetValue("roundedCorners")) {
                int radius = config.GetValue("cornerRadius");
                DrawRoundedCorners(pixmapContent, radius, currentX, currentY, config.GetValue("epgImageWidthLarge"), config.GetValue("epgImageHeightLarge"));
            }
            if (currentPicsPerLine < picsPerLine) {
                currentX += config.GetValue("epgImageWidthLarge") + border;
                currentPicsPerLine++;
            } else {
                currentX = border;
                currentY += config.GetValue("epgImageHeightLarge") + border;
                currentPicsPerLine = 1;
            }
        } else {
            break;
        }
    }
}

//------------------cNopacityMenuDetailRecordingViewLight------------------

cNopacityMenuDetailRecordingViewLight::cNopacityMenuDetailRecordingViewLight(cOsd *osd, const cRecording *Recording, cPixmap *s, cPixmap *sBack) : cNopacityMenuDetailViewLight(osd, s, sBack) {
    recording = Recording;
    info = Recording->Info();
}

cNopacityMenuDetailRecordingViewLight::~cNopacityMenuDetailRecordingViewLight(void) {
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapContent);
    osd->DestroyPixmap(pixmapPoster);
}

void cNopacityMenuDetailRecordingViewLight::SetContent(void) {
    contentWidth = width;
    contentX = 0;
    if (recording) {
        //check first if manually set poster exists
        cString posterFound;
        cImageLoader imgLoader;
        hasManualPoster = imgLoader.SearchRecordingPoster(recording->FileName(), posterFound);
        if (hasManualPoster) {
            manualPosterPath = posterFound;
            contentWidth -=  widthPoster;
            contentX = widthPoster;
        } else {
            static cPlugin *pScraper = GetScraperPlugin();
            if (pScraper && (config.GetValue("scraperInfo") == 1)) {
                ScraperGetEventType call;
                call.recording = recording;
                int seriesId = 0;
                int episodeId = 0;
                int movieId = 0;
                if (pScraper->Service("GetEventType", &call)) {
                    esyslog("nopacity: Type detected: %d, seriesId %d, episodeId %d, movieId %d", call.type, call.seriesId, call.episodeId, call.movieId);
                    seriesId = call.seriesId;
                    episodeId = call.episodeId;
                    movieId = call.movieId;
                }
                if (seriesId > 0) {
                    series.seriesId = seriesId;
                    series.episodeId = episodeId;
                    if (pScraper->Service("GetSeries", &series)) {
                        isSeries = true;
                    }
                } else if (movieId > 0) {
                    movie.movieId = movieId;
                    if (pScraper->Service("GetMovie", &movie)) {
                        isMovie = true;
                    }
                }
            }
            bool displayPoster = false;
            if (isSeries && series.posters.size() > 0) {
               displayPoster = true;
            } else if (isMovie && (movie.poster.width > 0) && (movie.poster.height > 0) && (movie.poster.path.size() > 0)) {
                displayPoster = true;
            }
            if (displayPoster) {
                contentWidth -=  widthPoster;
                contentX = widthPoster;
            }
        }
        recInfo.Set(recording->Info()->Description(), font, contentWidth - 2 * border);
        LoadRecordingInformation();
    }
}

void cNopacityMenuDetailRecordingViewLight::SetContentHeight(void) {
    int lineHeight = font->Height();
    //Height of banner (only for series)
    int heightBanner = 0;
    if (!hasManualPoster && isSeries) {
        if (isSeries && series.banners.size() > 0) {
            heightBanner = series.banners[0].height;
        }
    }
    //Height of Recording EPG Info
    int heightEPG = recInfo.Lines() * lineHeight;
    //Height of actor pictures
    int heightActors = 0;
    if (!hasManualPoster && (isMovie || isSeries)) {
        heightActors = HeightActorPics();
    }
    //Height of additional scraper info
    int heightScraperInfo = 0;
    if (isMovie || isSeries) {
        heightScraperInfo = HeightScraperInfo();
    }
    //Height of fanart
    int heightFanart = 0;
    if (!hasManualPoster && (isMovie || isSeries)) {
        heightFanart = HeightFanart();
    }
    //Height of EPG Pictures
    int heightEPGPics = 0;
    if ((config.GetValue("displayAdditionalRecEPGPictures") == 1)) {
        if (LoadEPGPics())
            heightEPGPics = HeightEPGPics();
    }
    //additional recording Info
    int heightAdditionalInfo = additionalInfo.Lines() * lineHeight;

    yBanner  = 0;
    yEPGText = yBanner + heightBanner + ((heightBanner) ? lineHeight : 0);
    yActors  = yEPGText + heightEPG + ((heightEPG) ? lineHeight : 0);
    yScrapInfo = yActors + heightActors + ((heightActors) ? lineHeight : 0);
    yFanart  = yScrapInfo + heightScraperInfo + ((heightScraperInfo) ? lineHeight : 0);
    yEPGPics = yFanart + heightFanart + ((heightFanart) ? lineHeight : 0);
    yAddInf  = yEPGPics + heightEPGPics + ((heightEPGPics) ? lineHeight : 0);

    int totalHeight = yAddInf + heightAdditionalInfo + border;

    //check if pixmap content has to be scrollable
    if (totalHeight > contentHeight) {
        contentDrawPortHeight = totalHeight;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;
    }

}

void cNopacityMenuDetailRecordingViewLight::CreatePixmaps(void) {
    contentHeight = contentHeight - border;
    pixmapHeader = CreatePixmap(osd, "pixmapHeader", 3, cRect(x, top, width, headerHeight));

    pixmapContent = CreatePixmap(osd, "pixmapContent", 3, cRect(x + contentX, top + headerHeight + border, contentWidth, contentHeight),
                                                          cRect(0, 0, contentWidth, contentDrawPortHeight));

    if (hasManualPoster || isMovie || isSeries) {
        pixmapPoster = CreatePixmap(osd, "pixmapPoster", 4, cRect(x, top + headerHeight, widthPoster, contentHeight));
    }

    PixmapFill(pixmapHeader, clrTransparent);
    PixmapFill(pixmapContent, clrTransparent);
    PixmapFill(pixmapPoster, clrTransparent);
    if (pixmapHeader)
        pixmapHeader->DrawRectangle(cRect(0, headerHeight - 2, width, 2), Theme.Color(clrMenuBorder));
}

void cNopacityMenuDetailRecordingViewLight::Render(void) {
    DrawHeader();
    //draw Recording EPG text
    DrawTextWrapper(&recInfo, yEPGText);
    //draw additional Info
    if (config.GetValue("displayRerunsDetailEPGView")) {
        DrawTextWrapper(&additionalInfo, yAddInf);
    }
    //draw additional scraper info
    if (isMovie) {
        DrawTextWrapper(&scrapInfo, yScrapInfo);
    } else if (isSeries) {
        int yInfo = yScrapInfo + font->Height();
        if (series.banners.size() > 1)
            yInfo += series.banners[1].height;
        DrawTextWrapper(&scrapInfo, yInfo);
    } 
    if ((hasManualPoster || isSeries || isMovie)) {
        DrawPoster();
    }
    //draw banner only for series
    if (!hasManualPoster && isSeries) {
        DrawBanner(yBanner);
    }
    //draw actors
    if (!hasManualPoster && (isSeries || isMovie)) {
        DrawActors(yActors);
    }
    //draw fanart
    if (!hasManualPoster && (isSeries || isMovie)) {
       DrawFanart(yFanart);
    }
    //draw additional EPG Pictures
    if (((config.GetValue("displayAdditionalRecEPGPictures") == 1) || ((config.GetValue("displayAdditionalRecEPGPictures") == 2) && !(!isMovie && !isSeries)))) {
        DrawEPGPictures(yEPGPics);
    }
    DrawScrollbar();
}

bool cNopacityMenuDetailRecordingViewLight::LoadEPGPics(void) {
    DIR *dirHandle;
    struct dirent *dirEntry;
    dirHandle = opendir(recording->FileName());
    int picsFound = 0;
    if (dirHandle != NULL) {
        while ( 0 != (dirEntry = readdir(dirHandle))) {
            if (endswith(dirEntry->d_name, "jpg")) {
                std::string fileName = dirEntry->d_name;
                if (!fileName.compare("cover_vdr.jpg"))
                    continue;
                if (fileName.length() > 4) {
                    fileName = fileName.substr(0, fileName.length() - 4);
                    epgpics.push_back(fileName);
                    picsFound++;
                }
            }
            if (picsFound >= config.GetValue("numAdditionalRecEPGPictures"))
                break;
        }
        closedir(dirHandle);
    }
    if (picsFound > 0)
        return true;
    return false;
}

int cNopacityMenuDetailRecordingViewLight::HeightEPGPics(void) {
    int numPicsAvailable = epgpics.size();
    int picsPerLine = contentWidth / (config.GetValue("epgImageWidthLarge") + border);
    int picLines = numPicsAvailable / picsPerLine;
    if (numPicsAvailable%picsPerLine != 0)
        picLines++;
    return picLines * (config.GetValue("epgImageHeightLarge") + border) + 2*border;
}

void cNopacityMenuDetailRecordingViewLight::DrawEPGPictures(int height) {
    if (pixmapContent)
        return;

    int picsPerLine = contentWidth / (config.GetValue("epgImageWidthLarge") + border);
    int currentX = border;
    int currentY = height + border;
    int currentPicsPerLine = 1;
    cImageLoader imgLoader;
    for (unsigned i=0; i < epgpics.size(); i++) {
        cString path = cString::sprintf("%s/", recording->FileName());
        cString epgimage = epgpics.at(i).c_str();
        if (imgLoader.LoadAdditionalRecordingImage(path, epgimage)) {
            pixmapContent->DrawImage(cPoint(currentX, currentY), imgLoader.GetImage());
            if (config.GetValue("roundedCorners")) {
                int radius = config.GetValue("cornerRadius");
                DrawRoundedCorners(pixmapContent, radius, currentX, currentY, config.GetValue("epgImageWidthLarge"), config.GetValue("epgImageHeightLarge"));
            }
            if (currentPicsPerLine < picsPerLine) {
                currentX += config.GetValue("epgImageWidthLarge") + border;
                currentPicsPerLine++;
            } else {
                currentX = border;
                currentY += config.GetValue("epgImageHeightLarge") + border;
                currentPicsPerLine = 1;
            }
        } else {
            break;
        }
    }
}

void cNopacityMenuDetailRecordingViewLight::DrawHeader(void) {
    if (!pixmapHeader)
        return;

    cImageLoader imgLoader;
    int widthTextHeader = width - 2 * border;
    if (isSeries && series.episode.episodeImage.path.size() > 0) {
        int imgWidth = series.episode.episodeImage.width;
        int imgHeight = series.episode.episodeImage.height;
        if (imgHeight > headerHeight) {
            imgHeight = headerHeight - 6;
            imgWidth = imgWidth * ((double)imgHeight / (double)series.episode.episodeImage.height);
        }
        if (imgLoader.LoadPoster(series.episode.episodeImage.path.c_str(), imgWidth, imgHeight)) {
            pixmapHeader->DrawImage(cPoint(width - imgWidth - border, (headerHeight - imgHeight)/2), imgLoader.GetImage());
            widthTextHeader -= imgWidth;
        }
    } else if ((config.GetValue("displayAdditionalRecEPGPictures") == 1) && imgLoader.LoadRecordingImage(recording->FileName())) {
        pixmapHeader->DrawImage(cPoint(width - config.GetValue("epgImageWidth") - border, (headerHeight-config.GetValue("epgImageHeight"))/2), imgLoader.GetImage());
        if (config.GetValue("roundedCorners")) {
            int radius = config.GetValue("cornerRadius");
            int x = width - config.GetValue("epgImageWidth") - border;
            int y = (headerHeight-config.GetValue("epgImageHeight"))/2;
            DrawRoundedCorners(pixmapHeader, radius, x, y, config.GetValue("epgImageWidth"), config.GetValue("epgImageHeight"));
        }
        widthTextHeader -= config.GetValue("epgImageWidth");
    }
    int lineHeight = fontHeaderLarge->Height();
    int recDuration = recording->LengthInSeconds();
    recDuration = (recDuration>0)?(recDuration / 60):0;
    cString dateTime = cString::sprintf("%s  %s (%d %s)", *DateString(recording->Start()), *TimeString(recording->Start()), recDuration, tr("min"));
    pixmapHeader->DrawText(cPoint(border, (lineHeight - fontHeader->Height())/2), *dateTime, Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);

    const char *Title = info->Title();
    if (isempty(Title))
        Title = recording->Name();
    cTextWrapper title;
    title.Set(Title, fontHeaderLarge, widthTextHeader);
    int currentLineHeight = lineHeight;
	for (int i=0; i < title.Lines(); i++) {
        pixmapHeader->DrawText(cPoint(border, currentLineHeight), title.GetLine(i), Theme.Color(clrMenuFontDetailViewHeaderTitle), clrTransparent, fontHeaderLarge);
        currentLineHeight += lineHeight;
    }
	
    if (!isempty(info->ShortText())) {
        cTextWrapper shortText;
        shortText.Set(info->ShortText(), fontHeader, widthTextHeader);
        for (int i=0; i < shortText.Lines(); i++) {
            if ((currentLineHeight + fontHeader->Height()) < headerHeight) {
                pixmapHeader->DrawText(cPoint(border, currentLineHeight), shortText.GetLine(i), Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);
                currentLineHeight += fontHeader->Height();
        } else
            break;
        }
    }
}

void cNopacityMenuDetailRecordingViewLight::LoadRecordingInformation(void) {
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

    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(Info->ChannelID());
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

    if (Info) {
#if (APIVERSNUM >= 20505)
        if (Info->Errors() >= 0) {
            cString errors = cString::sprintf("%s: %i ", tr("TS Errors"), Info->Errors());
            sstrInfo << (const char*)errors << std::endl;
        }
#endif
        const char *aux = NULL;
        aux = Info->Aux();
        if (aux) {
            std::string strAux = aux;
            std::string auxEpgsearch = StripXmlTag(strAux, "epgsearch");
            if (!auxEpgsearch.empty()) {
                std::string searchTimer = StripXmlTag(auxEpgsearch, "searchtimer");
                if (!searchTimer.empty()) {
                    sstrInfo << tr("Search timer") << ": " << searchTimer << std::endl;
                }
            }
        }
    }

    additionalInfo.Set(sstrInfo.str().c_str(), font, width - 4 * border);
}

std::string cNopacityMenuDetailRecordingViewLight::StripXmlTag(std::string &Line, const char *Tag) {
        // set the search strings
        std::stringstream strStart, strStop;
        strStart << "<" << Tag << ">";
        strStop << "</" << Tag << ">";
        // find the strings
        std::string::size_type locStart = Line.find(strStart.str());
        std::string::size_type locStop = Line.find(strStop.str());
        if (locStart == std::string::npos || locStop == std::string::npos)
                return "";
        // extract relevant text
        int pos = locStart + strStart.str().size();
        int len = locStop - pos;
        return len < 0 ? "" : Line.substr(pos, len);
}


int cNopacityMenuDetailRecordingViewLight::ReadSizeVdr(const char *strPath) {
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
