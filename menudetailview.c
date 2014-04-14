#include "menudetailview.h"
#include "services/epgsearch.h"
#include <sstream>
#include <iostream>
#include <dirent.h>
#include <vector>
#include "config.h"
#include "helpers.h"
#include "imageloader.h"


cNopacityMenuDetailView::cNopacityMenuDetailView(cOsd *osd, cImageCache *imgCache) {
    this->osd = osd;
    this->imgCache = imgCache;
    hasScrollbar = false;
    hasManualPoster = false;
    manualPosterPath = "";
    hasAdditionalMedia = false;
    isMovie = false;
    isSeries = false;
    pixmapPoster = NULL;
}

cNopacityMenuDetailView::~cNopacityMenuDetailView(void) {
    delete font;
    if (fontSmall)
        delete fontSmall;
    if (fontHeader)
        delete fontHeader;
    if (fontHeaderLarge)
        delete fontHeaderLarge;
}

void cNopacityMenuDetailView::SetGeometry(int x, int width, int height, int top, int contentBorder, int headerHeight) {
    this->x = x;
    this->width = width;
    this->height = height;
    this->top = top;
    this->border = contentBorder;
    this->headerHeight = headerHeight;
    contentHeight = height - headerHeight;
    widthPoster = 30 * width / 100;
}

void cNopacityMenuDetailView::DrawTextWrapper(cTextWrapper *wrapper, int top) {
    int linesText = wrapper->Lines();
    int textHeight = font->Height();
    int currentHeight = top;
    for (int i=0; i < linesText; i++) {
        pixmapContent->DrawText(cPoint(border, currentHeight), wrapper->GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        currentHeight += textHeight;
    }
}

int cNopacityMenuDetailView::HeightActorPics(void) {
    int numActors = 0;
    if (isMovie)
        numActors = movie.actors.size();
    else if (isSeries)
        numActors = series.actors.size();
    else
        numActors = mediaInfo.actors.size();
    if (numActors < 1)
        return 0;
    if (isMovie) {
        actorThumbWidth = movie.actors[0].actorThumb.width/2;
        actorThumbHeight = movie.actors[0].actorThumb.height/2;        
    } else if (isSeries) {
        actorThumbWidth = series.actors[0].actorThumb.width/2;
        actorThumbHeight = series.actors[0].actorThumb.height/2;        
    } else if (mediaInfo.type == typeMovie) {
        actorThumbWidth = mediaInfo.actors[0].thumb.width/2;
        actorThumbHeight = mediaInfo.actors[0].thumb.height/2;
    } else if (mediaInfo.type == typeSeries) {
        actorThumbWidth = mediaInfo.actors[0].thumb.width/2;
        actorThumbHeight = mediaInfo.actors[0].thumb.height/2;
    }
    int picsPerLine = contentWidth / (actorThumbWidth + 2*border);
    int picLines = numActors / picsPerLine;
    if (numActors%picsPerLine != 0)
        picLines++;
    int actorsHeight = picLines * (actorThumbHeight + 2*fontSmall->Height()) + font->Height() + fontHeader->Height();
    return actorsHeight;
}

int cNopacityMenuDetailView::HeightScraperInfo(void) {
    int heightScraperInfo = 0;
    std::stringstream info;
    if (isSeries) {
        info << tr("TheTVDB Information") << ":\n\n";
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
    } else if (isMovie) {
        info << tr("TheMovieDB Information") << ":\n\n";
        if (movie.originalTitle.size() > 0) {
            info << tr("Original Title") << ": " << movie.originalTitle << "\n";
        }
        if (movie.tagline.size() > 0) {
            info << tr("Tagline") << ": " << movie.tagline << "\n";
        }
        if (movie.overview.size() > 0) {
            info << tr("Overview") << ": " << movie.overview << "\n";
        }
        std::string strAdult = (movie.adult)?(tr("yes")):(tr("no"));
        info << tr("Adult") << ": " << strAdult << "\n";
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
    }
    scrapInfo.Set(info.str().c_str(), font, contentWidth - 2 * border);
    int lineHeight = font->Height();
    int scrapInfoHeight = (scrapInfo.Lines() + 1) * lineHeight;
    if (isSeries) {
        if (series.banners.size() == 2)
            scrapInfoHeight += series.banners[1].height + lineHeight;
        else if (series.banners.size() == 3)
            scrapInfoHeight += series.banners[1].height + series.banners[2].height + 2*lineHeight;
    } 
    return scrapInfoHeight;
}

int cNopacityMenuDetailView::HeightFanart(void) {
    int retVal = 0;
    int fanartWidthOrig = 0;
    int fanartHeightOrig = 0;
    
    if (isMovie) {
        fanartWidthOrig = movie.fanart.width;
        fanartHeightOrig = movie.fanart.height;
    } else if (isSeries && series.fanarts.size() > 0) {
        fanartWidthOrig = series.fanarts[0].width;
        fanartHeightOrig = series.fanarts[0].height;
    } else if (mediaInfo.fanart.size() >= 1) {
        fanartWidthOrig = mediaInfo.fanart[0].width;
        fanartHeightOrig = mediaInfo.fanart[0].height;
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

void cNopacityMenuDetailView::DrawPoster(void) {
    int posterWidthOrig;
    int posterHeightOrig;
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
    } else {
        if (mediaInfo.posters.size() < 1)
            return;
        posterWidthOrig = mediaInfo.posters[0].width;
        posterHeightOrig = mediaInfo.posters[0].height;
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
    if (!Running())
        return;
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
    } else if (imgLoader.LoadPoster(mediaInfo.posters[0].path.c_str(), posterWidth, posterHeight)) {
        drawPoster = true;
    }
    if (drawPoster) {
        if (Running() && pixmapPoster)
            pixmapPoster->DrawImage(cPoint(posterX, posterY), imgLoader.GetImage());        
    }
}

void cNopacityMenuDetailView::DrawBanner(int height) {
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
    } else if (hasAdditionalMedia) {
        bannerWidthOrig = mediaInfo.banner.width;
        bannerHeightOrig = mediaInfo.banner.height;
        bannerPath = mediaInfo.banner.path;
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
    if (!Running())
        return;
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(bannerPath.c_str(), bannerWidth, bannerHeight)) {
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(bannerX, height), imgLoader.GetImage());
    }
    if (imgLoader.LoadPoster(seasonPoster.c_str(), seasonPosterWidth, seasonPosterHeight)) {
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(contentWidth - seasonPosterWidth - border, height), imgLoader.GetImage());
    }    
}

void cNopacityMenuDetailView::DrawAdditionalBanners(int top, int bottom) {
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
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(bannerX, top), imgLoader.GetImage());
    }

    if (series.banners.size() < 3)
        return;
    if (imgLoader.LoadPoster(series.banners[2].path.c_str(), bannerWidth, bannerHeight)) {
        if (Running() && pixmapContent)
            pixmapContent->DrawImage(cPoint(bannerX, bottom - bannerHeight - font->Height()), imgLoader.GetImage());
    }
}

void cNopacityMenuDetailView::DrawActors(int height) {
    int numActors = 0;
    if (isMovie)
        numActors = movie.actors.size();
    else if (isSeries)
        numActors = series.actors.size();
    else
        numActors = mediaInfo.actors.size();
    if (numActors < 1)
        return;

    cString header = cString::sprintf("%s:", tr("Actors"));
    pixmapContent->DrawText(cPoint(border, height), *header, Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontHeader);

    int picsPerLine = contentWidth / (actorThumbWidth + 2*border);
    int picLines = numActors / picsPerLine;
    if (numActors%picsPerLine != 0)
        picLines++;
    int x = 0;
    int y = height + fontHeader->Height();
    if (!Running())
        return;
    cImageLoader imgLoader;
    int actor = 0;
    for (int row = 0; row < picLines; row++) {
        for (int col = 0; col < picsPerLine; col++) {
            if (!Running())
                return;
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
            } else {
                path = mediaInfo.actors[actor].thumb.path;
                name = mediaInfo.actors[actor].name;
                sstrRole << "\"" << mediaInfo.actors[actor].role << "\"";
            }
            std::string role = sstrRole.str();
            if (imgLoader.LoadPoster(path.c_str(), actorThumbWidth, actorThumbHeight)) {
                if (Running() && pixmapContent)
                    pixmapContent->DrawImage(cPoint(x + border, y), imgLoader.GetImage());
            }

            if (fontSmall->Width(name.c_str()) > actorThumbWidth + 2*border)
                name = CutText(name, actorThumbWidth + 2*border, fontSmall);
            if (fontSmall->Width(role.c_str()) > actorThumbWidth + 2*border)
                role = CutText(role, actorThumbWidth + 2*border, fontSmall);
            int xName = x + ((actorThumbWidth+2*border) - fontSmall->Width(name.c_str()))/2;
            int xRole = x + ((actorThumbWidth+2*border) - fontSmall->Width(role.c_str()))/2;
            if (Running() && pixmapContent) {
                pixmapContent->DrawText(cPoint(xName, y + actorThumbHeight), name.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                pixmapContent->DrawText(cPoint(xRole, y + actorThumbHeight + fontSmall->Height()), role.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                x += actorThumbWidth + 2*border;
            }
            actor++;
        }
        x = 0;
        y += actorThumbHeight + 2 * fontSmall->Height();
    }
}

void cNopacityMenuDetailView::DrawFanart(int height) {
    if (isSeries && series.fanarts.size() < 1)
        return;
    else if (hasAdditionalMedia && mediaInfo.fanart.size() < 1)
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
    } else {
        fanartWidthOrig = mediaInfo.fanart[0].width;
        fanartHeightOrig = mediaInfo.fanart[0].height;
        fanartPath = mediaInfo.fanart[0].path;
    }

    if (fanartWidthOrig == 0)
        return;

    int fanartWidth = fanartWidthOrig;
    int fanartHeight = fanartHeightOrig;

    if (fanartWidthOrig > contentWidth - 2*border) {
        fanartWidth = contentWidth - 2*border;
        fanartHeight = fanartHeightOrig * ((double)fanartWidth / (double)fanartWidthOrig);
    }
    if (!Running())
        return;
    cImageLoader imgLoader;
    if (isMovie) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        if (imgLoader.LoadPoster(fanartPath.c_str(), fanartWidth, fanartHeight)) {
            if (Running() && pixmapContent)
                pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
        }
        if (movie.collectionFanart.path.size() > 0) {
            if (imgLoader.LoadPoster(movie.collectionFanart.path.c_str(), fanartWidth, fanartHeight)) {
                if (Running() && pixmapContent)
                    pixmapContent->DrawImage(cPoint(fanartX, height + fanartHeight + font->Size()), imgLoader.GetImage());
            }            
        }
    } else if (isSeries) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        for (std::vector<cTvMedia>::iterator f = series.fanarts.begin(); f != series.fanarts.end(); f++) {
            cTvMedia m = *f;
            if (imgLoader.LoadPoster(m.path.c_str(), fanartWidth, fanartHeight)) {
                if (Running() && pixmapContent)
                    pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
                height += fanartHeight + font->Height();
            }
        }        
    } else {
        if (imgLoader.LoadPoster(fanartPath.c_str(), fanartWidth, fanartHeight)) {
            int fanartX = (contentWidth - fanartWidth) / 2;
            if (Running() && pixmapContent)
                pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
        }        
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

cNopacityMenuDetailEventView::cNopacityMenuDetailEventView(cOsd *osd, cImageCache *imgCache, const cEvent *Event) : cNopacityMenuDetailView(osd, imgCache) {
    event = Event;
    numEPGPics = 0;
}

cNopacityMenuDetailEventView::~cNopacityMenuDetailEventView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmapHeader);
    pixmapHeader = NULL;
    osd->DestroyPixmap(pixmapContent);
    pixmapContent = NULL;
    osd->DestroyPixmap(pixmapLogo);
    pixmapLogo = NULL;
    if (pixmapPoster) {
        osd->DestroyPixmap(pixmapPoster);
        pixmapLogo = NULL;
    }
}

void cNopacityMenuDetailEventView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + 3 + config.GetValue("fontDetailView"));
    fontSmall = cFont::CreateFont(config.fontName, contentHeight / 30 + config.GetValue("fontDetailViewSmall"));
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + config.GetValue("fontDetailViewHeaderLarge"));
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.GetValue("fontDetailViewHeader"));
}

void cNopacityMenuDetailEventView::SetContent(void) {
    if (event) {
        static cPlugin *pScraper2Vdr = cPluginManager::GetPlugin("scraper2vdr");
        if (pScraper2Vdr) {
            ScraperGetEventType call;
            call.event = event;
            int seriesId = 0;
            int episodeId = 0;
            int movieId = 0;
            if (pScraper2Vdr->Service("GetEventType", &call)) {
                esyslog("nopacity: event: %d, %s", event->EventID(), event->Title());
                esyslog("nopacity: Type detected: %d, seriesId %d, episodeId %d, movieId %d", call.type, call.seriesId, call.episodeId, call.movieId);
                seriesId = call.seriesId;
                episodeId = call.episodeId;
                movieId = call.movieId;
            }
            if (seriesId > 0) {
                series.seriesId = seriesId;
                series.episodeId = episodeId;
                if (pScraper2Vdr->Service("GetSeries", &series)) {
                    isSeries = true;
                }
            } else if (movieId > 0) {
                movie.movieId = movieId;
                if (pScraper2Vdr->Service("GetMovie", &movie)) {
                    isMovie = true;
                }
            }
        } else {
            static cPlugin *pTVScraper = cPluginManager::GetPlugin("tvscraper");
            if (pTVScraper) {
                mediaInfo.event = event;
                mediaInfo.isRecording = false;
                if (pTVScraper->Service("TVScraperGetFullInformation", &mediaInfo)) {
                    hasAdditionalMedia = true;
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
        } else if (hasAdditionalMedia) {
            if (mediaInfo.posters.size() >= 1) {
                displayPoster = true;
            }
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

void cNopacityMenuDetailEventView::SetContentHeight(void) {
    int lineHeight = font->Height();
    //Height of banner (only for series)
    int heightBanner = 0;
    if ((hasAdditionalMedia && (mediaInfo.type == typeSeries)) || isSeries) {
        if (isSeries && series.banners.size() > 0) {
            heightBanner = series.banners[0].height + lineHeight;
        } else
            heightBanner = mediaInfo.banner.height + lineHeight;
    }
    //Height of EPG Text
    int heightEPG = (epgText.Lines()+1) * lineHeight;
    //Height of rerun information
    int heightReruns = 0;
    if (config.GetValue("displayRerunsDetailEPGView")) {
        heightReruns = reruns.Lines() * lineHeight;
    }
    //Height of actor pictures
    int heightActors = 0;
    if (hasAdditionalMedia || isMovie || isSeries) {
        heightActors = HeightActorPics();
    }

    //Height of additional scraper info
    int heightScraperInfo = 0;
    if (isMovie || isSeries) {
        heightScraperInfo = HeightScraperInfo();
    }

    //Height of fanart
    int heightFanart = 0;
    if (hasAdditionalMedia || isMovie || isSeries) {
        heightFanart = HeightFanart() + lineHeight;
    }
    //Height of EPG Pictures
    int heightEPGPics = 0;
    if ((config.GetValue("displayAdditionalEPGPictures") == 1) || ((config.GetValue("displayAdditionalEPGPictures") == 2) && !hasAdditionalMedia && !isMovie &&  !isSeries)) {
        heightEPGPics = HeightEPGPics();
    }

    yBanner    = border;
    yEPGText   = yBanner     + heightBanner;
    yAddInf    = yEPGText    + heightEPG;
    yActors    = yAddInf     + heightReruns;
    yScrapInfo = yActors     + heightActors;
    yFanart    = yScrapInfo  + heightScraperInfo;
    yEPGPics   = yFanart     + heightFanart;

    int totalHeight = 2 * border + heightBanner + heightEPG + heightActors + heightScraperInfo + heightFanart + heightReruns + heightEPGPics;
    //check if pixmap content has to be scrollable
    if (totalHeight > contentHeight) {
        contentDrawPortHeight = totalHeight;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;
    }
}

void cNopacityMenuDetailEventView::CreatePixmaps(void) {
    pixmapHeader =  osd->CreatePixmap(3, cRect(x, top, width, headerHeight));
    pixmapContent = osd->CreatePixmap(3, cRect(x + contentX, top + headerHeight, contentWidth, contentHeight),
                                      cRect(0, 0, contentWidth, contentDrawPortHeight));
    pixmapLogo =    osd->CreatePixmap(4, cRect(x + border, top + max((headerHeight-config.GetValue("logoHeightOriginal"))/2,1), config.GetValue("logoWidthOriginal"), config.GetValue("logoHeightOriginal")));

    pixmapHeader->Fill(clrTransparent);
    pixmapHeader->DrawRectangle(cRect(0, headerHeight - 2, width, 2), Theme.Color(clrMenuBorder));
    pixmapContent->Fill(clrTransparent);
    pixmapLogo->Fill(clrTransparent);

    if (hasAdditionalMedia || isSeries || isMovie) {
        pixmapPoster = osd->CreatePixmap(4, cRect(x, top + headerHeight, widthPoster, contentHeight));
        pixmapPoster->Fill(clrTransparent);
    }
}

void cNopacityMenuDetailEventView::Render(void) {
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
}

void cNopacityMenuDetailEventView::Action(void) {
    if ((hasAdditionalMedia || isSeries || isMovie) && Running()) {
        DrawPoster();
        osd->Flush();
    }
    //draw banner only for series
    if (((hasAdditionalMedia && (mediaInfo.type == typeSeries)) || isSeries)  && Running()) {
        DrawBanner(yBanner);
        osd->Flush();
    }
    //draw actors
    if ((hasAdditionalMedia || isSeries || isMovie) && Running()) {
       DrawActors(yActors);
        osd->Flush();
    }
    //draw additional banners
    if (isSeries  && Running()) {
       DrawAdditionalBanners(yScrapInfo, yFanart);
        osd->Flush();
    }
    //draw fanart
    if ((hasAdditionalMedia || isSeries || isMovie) && Running()) {
       DrawFanart(yFanart);
        osd->Flush();
    }
    //draw additional EPG Pictures
    if (((config.GetValue("displayAdditionalEPGPictures") == 1) || ((config.GetValue("displayAdditionalEPGPictures") == 2) && !hasAdditionalMedia && !isMovie && !isSeries)) && Running()) {
        DrawEPGPictures(yEPGPics);
        osd->Flush();
    }
}

int cNopacityMenuDetailEventView::HeightEPGPics(void) {
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
    if (numPicsAvailable%picsPerLine != 0)
        picLines++;
    return picLines * (config.GetValue("epgImageHeightLarge") + border) + 2*border;
}

void cNopacityMenuDetailEventView::DrawHeader(void) {
    int logoWidth = config.GetValue("logoWidthOriginal");
    cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true);
    if (channel) {
        cImage *logo = imgCache->GetLogo(ctLogo, channel);
        if (logo) {
            pixmapLogo->DrawImage(cPoint(0, max((headerHeight - config.GetValue("logoHeightOriginal") - border)/2, 0)), *logo);
        }
    }
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

void cNopacityMenuDetailEventView::LoadReruns(void) {
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
                    cChannel *channel = Channels.GetByChannelID(r->event->ChannelID(), true, true);
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

void cNopacityMenuDetailEventView::DrawEPGPictures(int height) {
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

//------------------cNopacityMenuDetailRecordingView------------------

cNopacityMenuDetailRecordingView::cNopacityMenuDetailRecordingView(cOsd *osd, const cRecording *Recording) : cNopacityMenuDetailView(osd, NULL) {
    recording = Recording;
    info = Recording->Info();
}

cNopacityMenuDetailRecordingView::~cNopacityMenuDetailRecordingView(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmapHeader);
    pixmapHeader = NULL;
    osd->DestroyPixmap(pixmapContent);
    pixmapContent = NULL;
    if (pixmapPoster) {
        osd->DestroyPixmap(pixmapPoster);
        pixmapLogo = NULL;
    }
}

void cNopacityMenuDetailRecordingView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + config.GetValue("fontDetailView"));
    fontSmall = cFont::CreateFont(config.fontName, contentHeight / 30 + config.GetValue("fontDetailViewSmall"));
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + config.GetValue("fontDetailViewHeaderLarge"));
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.GetValue("fontDetailViewHeader"));
}

void cNopacityMenuDetailRecordingView::SetContent(void) {
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
            static cPlugin *pScraper2Vdr = cPluginManager::GetPlugin("scraper2vdr");
            if (pScraper2Vdr) {
                ScraperGetEventType call;
                call.recording = recording;
                int seriesId = 0;
                int episodeId = 0;
                int movieId = 0;
                if (pScraper2Vdr->Service("GetEventType", &call)) {
                    esyslog("nopacity: Type detected: %d, seriesId %d, episodeId %d, movieId %d", call.type, call.seriesId, call.episodeId, call.movieId);
                    seriesId = call.seriesId;
                    episodeId = call.episodeId;
                    movieId = call.movieId;
                }
                if (seriesId > 0) {
                    series.seriesId = seriesId;
                    series.episodeId = episodeId;
                    if (pScraper2Vdr->Service("GetSeries", &series)) {
                        isSeries = true;
                    }
                } else if (movieId > 0) {
                    movie.movieId = movieId;
                    if (pScraper2Vdr->Service("GetMovie", &movie)) {
                        isMovie = true;
                    }
                }
            } else {
                const cEvent *event = info->GetEvent();
                if (event && !hasManualPoster) {
                    static cPlugin *pTVScraper = cPluginManager::GetPlugin("tvscraper");
                    if (pTVScraper) {
                        mediaInfo.event = event;
                        mediaInfo.isRecording = true;
                        if (pTVScraper->Service("TVScraperGetFullInformation", &mediaInfo)) {
                            hasAdditionalMedia = true;
                        }
                    }
                }
            }
            bool displayPoster = false;
            if (isSeries || isMovie) {
                displayPoster = true;
            } else if (hasAdditionalMedia) {
                if (mediaInfo.posters.size() >= 1) {
                    displayPoster = true;
                }
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

void cNopacityMenuDetailRecordingView::SetContentHeight(void) {
    int lineHeight = font->Height();
    //Height of banner (only for series)
    int heightBanner = 0;
    if (!hasManualPoster && ((hasAdditionalMedia && (mediaInfo.type == typeSeries)) || isSeries)) {
        if (isSeries && series.banners.size() > 0) {
            heightBanner = series.banners[0].height + lineHeight;
        } else
            heightBanner = mediaInfo.banner.height + lineHeight;
    }
    //Height of Recording EPG Info
    int heightEPG = (recInfo.Lines()+1) * lineHeight;
    //Height of actor pictures
    int heightActors = 0;
    if (!hasManualPoster && (hasAdditionalMedia || isMovie || isSeries)) {
        heightActors = HeightActorPics();
    }
    //Height of additional scraper info
    int heightScraperInfo = 0;
    if (isMovie || isSeries) {
        heightScraperInfo = HeightScraperInfo();
    }
    //Height of fanart
    int heightFanart = 0;
    if (!hasManualPoster && (hasAdditionalMedia || isMovie || isSeries)) {
        heightFanart = HeightFanart() + lineHeight;
    }
    //Height of EPG Pictures
    int heightEPGPics = 0;
    if ((config.GetValue("displayAdditionalRecEPGPictures") == 1) || ((config.GetValue("displayAdditionalRecEPGPictures") == 2) && !hasAdditionalMedia)) {
        if (LoadEPGPics())
            heightEPGPics = HeightEPGPics();
    }
    //additional recording Info
    int heightAdditionalInfo = (additionalInfo.Lines() + 1) * lineHeight;

    yBanner  = border;
    yEPGText = yBanner + heightBanner;
    yActors  = yEPGText + heightEPG;
    yScrapInfo = yActors + heightActors;
    yFanart  = yScrapInfo + heightScraperInfo;
    yEPGPics = yFanart + heightFanart;
    yAddInf  = yEPGPics + heightEPGPics;

    int totalHeight = 2*border + heightBanner + heightEPG + heightActors + heightScraperInfo + heightFanart + heightAdditionalInfo + heightEPGPics;
    //check if pixmap content has to be scrollable
    if (totalHeight > contentHeight) {
        contentDrawPortHeight = totalHeight;
        hasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;
    }

}

void cNopacityMenuDetailRecordingView::CreatePixmaps(void) {
    pixmapHeader =  osd->CreatePixmap(3, cRect(x, top, width, headerHeight));
    pixmapContent = osd->CreatePixmap(3, cRect(x + contentX, top + headerHeight, contentWidth, contentHeight),
                                         cRect(0, 0, contentWidth, contentDrawPortHeight));

    pixmapHeader->Fill(clrTransparent);
    pixmapHeader->DrawRectangle(cRect(0, headerHeight - 2, width, 2), Theme.Color(clrMenuBorder));
    pixmapContent->Fill(clrTransparent);
    if (hasManualPoster || hasAdditionalMedia || isMovie || isSeries) {
        pixmapPoster = osd->CreatePixmap(4, cRect(x, top + headerHeight, widthPoster, contentHeight));
        pixmapPoster->Fill(clrTransparent);
    }
}

void cNopacityMenuDetailRecordingView::Render(void) {
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
}

void cNopacityMenuDetailRecordingView::Action(void) {
    if ((hasManualPoster || hasAdditionalMedia || isSeries || isMovie) && Running()) {
        DrawPoster();
        osd->Flush();
    }
    //draw banner only for series
    if (!hasManualPoster && (hasAdditionalMedia && (mediaInfo.type == typeSeries)  || isSeries) && Running()) {
        DrawBanner(yBanner);
        osd->Flush();
    }
    //draw actors
    if (!hasManualPoster && (hasAdditionalMedia || isSeries || isMovie) && Running()) {
        DrawActors(yActors);
        osd->Flush();
    }
    //draw fanart
    if (!hasManualPoster && (hasAdditionalMedia  || isSeries || isMovie) && Running()) {
       DrawFanart(yFanart);
        osd->Flush();
    }
    //draw additional EPG Pictures
    if (((config.GetValue("displayAdditionalRecEPGPictures") == 1) || ((config.GetValue("displayAdditionalRecEPGPictures") == 2) && !(hasAdditionalMedia && !isMovie && !isSeries))) && Running()) {
        DrawEPGPictures(yEPGPics);
        osd->Flush();
    }
}

bool cNopacityMenuDetailRecordingView::LoadEPGPics(void) {
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

int cNopacityMenuDetailRecordingView::HeightEPGPics(void) {
    int numPicsAvailable = epgpics.size();
    int picsPerLine = contentWidth / (config.GetValue("epgImageWidthLarge") + border);
    int picLines = numPicsAvailable / picsPerLine;
    if (numPicsAvailable%picsPerLine != 0)
        picLines++;
    return picLines * (config.GetValue("epgImageHeightLarge") + border) + 2*border;
}

void cNopacityMenuDetailRecordingView::DrawEPGPictures(int height) {
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

void cNopacityMenuDetailRecordingView::DrawHeader(void) {
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

    if (Info) {
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

std::string cNopacityMenuDetailRecordingView::StripXmlTag(std::string &Line, const char *Tag) {
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

cNopacityMenuDetailTextView::cNopacityMenuDetailTextView(cOsd *osd, const char *text) : cNopacityMenuDetailView(osd, NULL) {
    this->text = text;
}

cNopacityMenuDetailTextView::~cNopacityMenuDetailTextView(void) {
    osd->DestroyPixmap(pixmapContent);
}

void cNopacityMenuDetailTextView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + config.GetValue("fontDetailView"));
    fontSmall = NULL;
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
    pixmapContent = osd->CreatePixmap(3, cRect(x, top + headerHeight, width, contentHeight),
                                         cRect(0, 0, width, contentDrawPortHeight));

    pixmapContent->Fill(clrTransparent);
}

void cNopacityMenuDetailTextView::Render(void) {
    DrawTextWrapper(&content, 0);
}
