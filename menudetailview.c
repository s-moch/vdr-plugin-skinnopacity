#include "menudetailview.h"

/********************************************************************************************
* cNopacityDetailView
********************************************************************************************/
cNopacityDetailView::cNopacityDetailView(eDetailViewType detailViewType, cOsd *osd, cPixmap *s, cPixmap *sBack) {
    type = detailViewType;
    this->osd = osd;
    this->scrollBar = s;
    this->scrollBarBack = sBack;
    ev = NULL;
    rec = NULL;
    text = NULL;
    view = NULL;
    x = (config.GetValue("menuAdjustLeft")) ? 0 : geoManager->osdWidth - geoManager->menuContentWidthFull + 2 * geoManager->menuSpace;
    width = geoManager->menuContentWidthFull - 2 * geoManager->menuSpace;
    height = geoManager->menuContentHeight;
    top = geoManager->menuHeaderHeight;
    border = 30;
    headerHeight = 0;
    switch (type) {
        case dvEvent:
            headerHeight = config.GetValue("headerDetailedEPG") * height / 100;
            border = config.GetValue("borderDetailedEPG");
            break;
        case dvRecording:
            headerHeight = config.GetValue("headerDetailedRecordings") * height / 100;
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
}

void cNopacityDetailView::SetAlpha(int Alpha) {
    if (view) view->SetAlpha(Alpha);
}

void cNopacityDetailView::InitiateViewType(void) {
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
