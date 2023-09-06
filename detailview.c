#include "detailview.h"

/********************************************************************************************
* cNopacityView
********************************************************************************************/

cNopacityView::cNopacityView(cOsd *osd) {
    this->osd = osd;
    activeView = 0;
    scrollable = false;
    tabbed = false;
    font = NULL;
    fontSmall = NULL;
    fontHeader = NULL;
    fontHeaderLarge = NULL;
    title = "";
    subTitle = "";
    dateTime = "";
    infoText = "";
    channel = NULL;
    eventID = 0;
    recFileName = "";
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    border = 0;
    headerHeight = 0;
    contentHeight = 0;
    contentWidth = 0;
    tabHeight = 0;
    actorThumbWidth = 0;
    actorThumbHeight = 0;
    contentDrawPortHeight = 0;
    headerDrawn = false;
}

cNopacityView::~cNopacityView(void) {
    if (font)
        delete font;
    if (fontSmall)
        delete fontSmall;
    if (fontHeader)
        delete fontHeader;
    if (fontHeaderLarge)
        delete fontHeaderLarge;
    if (fontFixed)
        delete fontFixed;
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapHeaderBack);
    osd->DestroyPixmap(pixmapLogo);
    osd->DestroyPixmap(pixmapContent);
    osd->DestroyPixmap(pixmapContentBack);
    osd->DestroyPixmap(pixmapTabs);
    osd->DestroyPixmap(pixmapHeaderEPGImage);
    osd->DestroyPixmap(pixmapHeaderBanner);
    osd->DestroyPixmap(pixmapHeaderPoster);
    osd->DestroyPixmap(pixmapHeaderImage);
    osd->DestroyPixmap(pixmapPoster);
}

void cNopacityView::SetGeometry(int x, int y, int width, int height, int border, int headerHeight) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->border = border;
    this->headerHeight = headerHeight;
    widthPoster = 30 * width / 100;
    tabHeight = 0;
    if (config.GetValue("tabsInDetailView") && tabbed)
        tabHeight = 2 * border;
    contentHeight = height - headerHeight - tabHeight;
    contentWidth = width;
    SetFonts();
}

void cNopacityView::SetAlpha(int Alpha) {
    PixmapSetAlpha(pixmapHeader, Alpha);
    PixmapSetAlpha(pixmapHeaderBack, Alpha);
    PixmapSetAlpha(pixmapLogo, Alpha);
    PixmapSetAlpha(pixmapTabs, Alpha);
    PixmapSetAlpha(pixmapContentBack, Alpha);
    PixmapSetAlpha(pixmapContent, Alpha);
    PixmapSetAlpha(pixmapScrollbar, Alpha);
    PixmapSetAlpha(pixmapScrollbarBack, Alpha);
    PixmapSetAlpha(pixmapHeaderEPGImage, Alpha);
    PixmapSetAlpha(pixmapHeaderBanner, Alpha);
    PixmapSetAlpha(pixmapHeaderPoster, Alpha);
    PixmapSetAlpha(pixmapHeaderImage, Alpha);
    PixmapSetAlpha(pixmapPoster, Alpha);
}

void cNopacityView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, (textView) ? contentHeight / 30 + config.GetValue("fontTextView") : contentHeight / 25 + 3 + config.GetValue("fontDetailView"));
    fontSmall = cFont::CreateFont(config.fontName, contentHeight / 30 + config.GetValue("fontDetailViewSmall"));
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + 5 + config.GetValue("fontDetailViewHeaderLarge"));
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.GetValue("fontDetailViewHeader"));
    fontFixed = cFont::CreateFont(config.fontFixedName, contentHeight / 30 + config.GetValue("fontFixedTextView"));
}

void cNopacityView::LoadAddInfo(std::string *text) {
    if (!text->empty())
        addInfo.Set(text->c_str(), font, contentWidth - 2 * border);
    else
        addInfo.Set("", font, contentWidth);
}

int cNopacityView::HeightActorPics(void) {
    int numActors = 0;
    if (isMovie)
        numActors = movie.actors.size();
    else if (isSeries)
        numActors = series.actors.size();
    if (numActors < 1)
        return 0;
    int picsPerLine = config.GetValue("numPicturesPerLineLight");
    actorThumbWidth = (contentWidth - 2 * picsPerLine * border) / picsPerLine;
    if (isMovie) {
        actorThumbHeight = ((double)movie.actors[0].actorThumb.height / (double)movie.actors[0].actorThumb.width) * actorThumbWidth;
    } else if (isSeries) {
        actorThumbHeight = ((double)series.actors[0].actorThumb.height / (double)series.actors[0].actorThumb.width) * actorThumbWidth;
    }

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

int cNopacityView::HeightScraperInfo(void) {
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
            info << tr("TheTVDB Rating") << ": " << series.rating << "\n";
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
            info << tr("TheTVDB Rating") << ": " << series.episode.rating << "\n";
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
    return scrapInfo.Lines() * lineHeight;
}

int cNopacityView::HeightFanart(void) {
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
    if (fanartWidthOrig > 0 && fanartWidthOrig > (contentWidth - 2 * border)) {
        fanartWidth = contentWidth - 2 * border;
        fanartHeight = fanartHeightOrig * ((double)fanartWidth / (double)fanartWidthOrig);
        retVal = fanartHeight;
    }
    if (isSeries) {
        return retVal = (retVal + font->Height()) * series.fanarts.size();
    }
    if (isMovie) {
        if (movie.collectionFanart.path.size() > 0) {
            return retVal += font->Height() + fanartHeight;
        }
    }
    return retVal;
}

void cNopacityView::DrawTextWrapper(cTextWrapper *wrapper, int y) {
    if (!pixmapContent || y > contentDrawPortHeight)
        return;
           
    int linesText = wrapper->Lines();
    int textHeight = font->Height();
    int currentHeight = y;
    for (int i = 0; i < linesText; i++) {
        pixmapContent->DrawText(cPoint(border, currentHeight), wrapper->GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        currentHeight += textHeight;
    }
}      

void cNopacityView::DrawHeader(int wP) {
    if (!pixmapHeaderBack) {
        if (pixmapHeaderBack = CreatePixmap(osd, "pixmapHeaderBack", 4, cRect(x, y, width, headerHeight))) {
            PixmapFill(pixmapHeaderBack, config.GetValue("tabsInDetailView") ? Theme.Color(clrMenuDetailViewBack) : clrTransparent);
            pixmapHeaderBack->DrawRectangle(cRect(0, headerHeight - 2, width, 2), Theme.Color(clrMenuBorder));
        }
    }

    //Channel Logo
    int xText = border;
    if (channel) {
        if (!pixmapLogo)
            pixmapLogo = CreatePixmap(osd, "pixmapLogo", 5, cRect(x + border,
                                                                  y + std::max((headerHeight - geoManager->channelLogoHeight) / 2, 1),
                                                                  geoManager->channelLogoWidth,
                                                                  geoManager->channelLogoHeight));
        PixmapFill(pixmapLogo, clrTransparent);
        cImage *logo = imgCache->GetLogo(ctLogo, channel);
        if (pixmapLogo && logo) {
            int xLogo = std::max((geoManager->channelLogoWidth - logo->Width()) / 2, 0);
            int yLogo = std::max((geoManager->channelLogoHeight - logo->Height()) / 2, 0);
            pixmapLogo->DrawImage(cPoint(xLogo , yLogo), *logo);
            xText += geoManager->channelLogoWidth + border;
        }
    }

    // Text
    int textLength = width - xText - wP;
    if (!pixmapHeader) {
        if (!(pixmapHeader = CreatePixmap(osd, "pixmapHeader", 5, cRect(xText, y, textLength, headerHeight - 2)))) {
            return;
        }
    }
    PixmapFill(pixmapHeader, clrTransparent);

    //Date and Time, Title, Subtitle
    if (config.GetValue("tabsInDetailView")) {
        int yDateTime = border;
        int yTitle = (headerHeight - fontHeaderLarge->Height()) / 2;
        int ySubtitle = headerHeight - fontHeader->Height() - border;
        pixmapHeader->DrawText(cPoint(0, yDateTime), dateTime.c_str(), Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);
        pixmapHeader->DrawText(cPoint(0, yTitle), title.c_str(), Theme.Color(clrMenuFontDetailViewHeaderTitle), clrTransparent, fontHeaderLarge);
        pixmapHeader->DrawText(cPoint(0, ySubtitle), subTitle.c_str(), Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);
    } else {
        int lineHeight = fontHeaderLarge->Height();

        pixmapHeader->DrawText(cPoint(0, (lineHeight - fontHeader->Height()) / 2), dateTime.c_str(), Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);

        cTextWrapper wtitle;
        wtitle.Set(title.c_str(), fontHeaderLarge, textLength);
        int currentLineHeight = lineHeight;
        for (int i = 0; i < wtitle.Lines(); i++) {
            pixmapHeader->DrawText(cPoint(0, currentLineHeight), wtitle.GetLine(i), Theme.Color(clrMenuFontDetailViewHeaderTitle), clrTransparent, fontHeaderLarge);
            currentLineHeight += lineHeight;
        }

        cTextWrapper shortText;
        shortText.Set(subTitle.c_str(), fontHeader, textLength);
        currentLineHeight += (lineHeight - fontHeader->Height()) / 2;
        for (int i = 0; i < shortText.Lines(); i++) {
            if ((currentLineHeight + fontHeader->Height()) < headerHeight) {
                pixmapHeader->DrawText(cPoint(0, currentLineHeight), shortText.GetLine(i), Theme.Color(clrMenuFontDetailViewHeader), clrTransparent, fontHeader);
                currentLineHeight += fontHeader->Height();
            } else
                break;
        }
    }
}

void cNopacityView::DrawTabs(void) {
    if (!pixmapTabs) {
        if (!(pixmapTabs = CreatePixmap(osd, "pixmapTabs", 4, cRect(0, y + height - tabHeight, width, tabHeight)))) {
            return;
        }
    }
    PixmapFill(pixmapTabs, clrTransparent);

    tColor bgColor = Theme.Color(clrMenuDetailViewTabs);
    pixmapTabs->DrawRectangle(cRect(0, 0, width, 2), bgColor);
    int numTabs = tabs.size();
    int xCurrent = 0;
    for (int tab = 0; tab < numTabs; tab++) {
        std::string tabText = tabs[tab];
        int textWidth = font->Width(tabText.c_str());
        int tabWidth = textWidth + border;
        pixmapTabs->DrawRectangle(cRect(xCurrent, (tab == activeView) ? 0 : 2, tabWidth - 2, tabHeight), bgColor);
        pixmapTabs->DrawEllipse(cRect(xCurrent, tabHeight - border/2, border/2, border/2), clrTransparent, -3);
        pixmapTabs->DrawEllipse(cRect(xCurrent + tabWidth - border / 2 - 2, tabHeight - border/2, border/2, border/2), clrTransparent, -4);
        if (tab == activeView) {
            pixmapTabs->DrawRectangle(cRect(xCurrent + 2, 0, tabWidth - 6, tabHeight - border/2 - 1), clrTransparent);
            pixmapTabs->DrawRectangle(cRect(xCurrent + border / 2, tabHeight - border/2 - 1, tabWidth - border, border/2 - 1), clrTransparent);
            pixmapTabs->DrawEllipse(cRect(xCurrent + 2, tabHeight - border/2 - 2, border/2, border/2), clrTransparent, 3);
            pixmapTabs->DrawEllipse(cRect(xCurrent + tabWidth - border / 2 - 4, tabHeight - border/2 - 2, border/2, border/2), clrTransparent, 4);
        }
        pixmapTabs->DrawText(cPoint(xCurrent + (tabWidth - textWidth) / 2, 2 + (tabHeight - font->Height())/2), tabText.c_str(), Theme.Color(clrMenuFontDetailViewText), (tab == activeView) ? clrTransparent : bgColor, font);
        xCurrent += tabWidth;
    }
}

void cNopacityView::ClearContent(void) {
    if (pixmapContent) {
        osd->DestroyPixmap(pixmapContent);
        pixmapContent = NULL;
    }
}

void cNopacityView::CreateContent(int fullHeight) {
    scrollable = false;
    int drawPortHeight = contentHeight; 
    if (fullHeight > contentHeight) {
        drawPortHeight = fullHeight;
        scrollable = true;
    }
    if (!pixmapContentBack) {
        pixmapContentBack = CreatePixmap(osd, "pixmapContentBack", 3, cRect(x, y + headerHeight, width, contentHeight + tabHeight));
        PixmapFill(pixmapContentBack, Theme.Color(clrMenuDetailViewBack));
    }
    pixmapContent = CreatePixmap(osd, "pixmapContent", 4, cRect(x, y + headerHeight, width, contentHeight), cRect(0, 0, width, drawPortHeight));
    PixmapFill(pixmapContent, clrTransparent);
}

void cNopacityView::DrawContent(std::string *text, int y, bool useFixedFont) {
    const cFont *viewFont = useFixedFont ? fontFixed : font;
    cTextWrapper wText;
    wText.Set(text->c_str(), viewFont, contentWidth - 2 * border);
    int lineHeight = viewFont->Height();
    int textLines = wText.Lines();

    if (!pixmapContent) {
        int textHeight = lineHeight * textLines + 2 * border;
        CreateContent(textHeight);
    }

    if (!pixmapContent || y > contentDrawPortHeight)
        return;

    int yText = (y) ? y : border;
    for (int i = 0; i < textLines; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wText.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, viewFont);
        yText += lineHeight;
    }
}

void cNopacityView::DrawFloatingContent(std::string *infoText, cTvMedia *img, cTvMedia *img2) {
    cTextWrapper wTextTall;
    cTextWrapper wTextFull;
    int imgWidth = img->width;
    int imgHeight = img->height;
    int imgWidth2 = 0;
    int imgHeight2 = 0;
    if (imgHeight > (contentHeight - 2 * border)) {
        imgHeight = contentHeight - 2 * border;
        imgWidth = imgWidth * ((double)imgHeight / (double)img->height);
    }
    int imgHeightTotal = imgHeight;
    if (img2) {
        imgWidth2 = imgWidth;
        imgHeight2 = img2->height * ((double)img2->width / (double)imgWidth2);
        imgHeightTotal += img2->height + border;
    }
    CreateFloatingTextWrapper(&wTextTall, &wTextFull, infoText, imgWidth, imgHeightTotal);
    int lineHeight = font->Height();
    int textLinesTall = wTextTall.Lines();
    int textLinesFull = wTextFull.Lines();
    int textHeight = lineHeight * (textLinesTall + textLinesFull) + 2 * border;
    int yText = border;

    CreateContent(std::max(textHeight, imgHeight + 2 * border));
    if (!pixmapContent)
        return;

    for (int i = 0; i < textLinesTall; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wTextTall.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        yText += lineHeight;
    }
    for (int i = 0; i < textLinesFull; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wTextFull.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
        yText += lineHeight;
    }
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(img->path.c_str(), imgWidth, imgHeight)) {
        if (pixmapContent)
            pixmapContent->DrawImage(cPoint(width - imgWidth - border, border), imgLoader.GetImage());
    }
    if (!img2)
        return;
    if (imgLoader.LoadPoster(img2->path.c_str(), imgWidth2, imgHeight2)) {
        if (pixmapContent)
            pixmapContent->DrawImage(cPoint(width - imgWidth2 - border, imgHeight + 2*border), imgLoader.GetImage());
    }
}

void cNopacityView::CreateFloatingTextWrapper(cTextWrapper *twNarrow, cTextWrapper *twFull, std::string *text, int widthImg, int heightImg) {
    int lineHeight = font->Height();
    int linesNarrow = (heightImg + 2*border)/ lineHeight;
    int linesDrawn = 0;
    int y = 0;
    int widthNarrow = width - 3 * border - widthImg;
    bool drawNarrow = true;
    
    splitstring s(text->c_str());
    std::vector<std::string> flds = s.split('\n', 1);

    if (flds.size() < 1)
        return;

    std::stringstream sstrTextTall;
    std::stringstream sstrTextFull;
      
    for (unsigned int i = 0; i < flds.size(); i++) {
        if (!flds[i].size()) {
            //empty line
            linesDrawn++;
            y += lineHeight;
            if (drawNarrow)
                sstrTextTall << "\n";
            else
                sstrTextFull << "\n";
        } else {
            cTextWrapper wrapper;
            if (drawNarrow) {
                wrapper.Set((flds[i].c_str()), font, widthNarrow);
                int newLines = wrapper.Lines();
                //check if wrapper fits completely into narrow area
                if (linesDrawn + newLines < linesNarrow) {
                    for (int line = 0; line < wrapper.Lines(); line++) {
                        sstrTextTall << wrapper.GetLine(line) << " ";
                    }
                    sstrTextTall << "\n";
                    linesDrawn += newLines;
                } else {
                    //this wrapper has to be splitted
                    for (int line = 0; line < wrapper.Lines(); line++) {
                        if (line + linesDrawn < linesNarrow) {
                            sstrTextTall << wrapper.GetLine(line) << " ";
                        } else {
                            sstrTextFull << wrapper.GetLine(line) << " ";
                        }
                    }
                    sstrTextFull << "\n";
                    drawNarrow = false;
                }
            } else {
                wrapper.Set((flds[i].c_str()), font, width - 2*border);
                for (int line = 0; line < wrapper.Lines(); line++) {
                    sstrTextFull << wrapper.GetLine(line) << " ";        
                }
                sstrTextFull << "\n";
            }
        }
    }
    twNarrow->Set(sstrTextTall.str().c_str(), font, widthNarrow);
    twFull->Set(sstrTextFull.str().c_str(), font, width - 2 * border);
}

int cNopacityView::DrawHeaderPosterLight(void) {
    int imageBorder = 4;
    int posterHeight = headerHeight - 2 * imageBorder;
    int maxPosterWidth = 0.25 * width;
    bool roundedCorner = false;
    bool poster = false;

    cImageLoader imgLoader;
    if (isSeries && series.episode.episodeImage.path.size() > 0) {
        if (imgLoader.LoadPoster(series.episode.episodeImage.path.c_str(), maxPosterWidth, posterHeight)) {
            poster = true;
        }
    } else if (isSeries && series.seasonPoster.path.size() > 0) {
        if (imgLoader.LoadPoster(series.seasonPoster.path.c_str(), maxPosterWidth, posterHeight)) {
            poster = true;
        }
    } else if (((config.GetValue("displayAdditionalRecEPGPictures") == 1) && imgLoader.LoadRecordingImage(recFileName.c_str(), maxPosterWidth, posterHeight))
               || ((config.GetValue("displayAdditionalEPGPictures") == 1) && imgLoader.LoadEPGImage(eventID, maxPosterWidth, posterHeight))) {
        poster = true;
        roundedCorner = true;
    }

    if (!poster)
        return 0;

    cImage image = imgLoader.GetImage();
    int posterWidth = image.Width();
    int xP = width - posterWidth - border;

    if (!pixmapHeaderImage) {
        if (!(pixmapHeaderImage = CreatePixmap(osd, "pixmapHeaderImage", 5, cRect(xP, y + imageBorder, posterWidth, posterHeight)))) {
            return 0;
        }
    }
    PixmapFill(pixmapHeaderImage, clrTransparent);

    int posterY = std::max((posterHeight - image.Height()) / 2, 0);
    pixmapHeaderImage->DrawImage(cPoint(0, posterY), image);

    if (roundedCorner && config.GetValue("roundedCorners")) {
        int radius = config.GetValue("cornerRadius");
        DrawRoundedCorners(pixmapHeaderImage, radius, 0, 0, posterWidth, posterHeight);
    }
    return posterWidth + 2 * border;
}

void cNopacityView::DrawPoster(void) {
    if (!pixmapPoster)
        return;

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
        posterWidth = widthPoster - 2 * border;
        posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
    } else if ((posterWidthOrig < widthPoster) && (posterHeightOrig > contentHeight)) {
        posterHeight = contentHeight - 2 * border;
        posterWidth = posterWidthOrig * ((double)posterHeight / (double)posterHeightOrig);
    } else if ((posterWidthOrig > widthPoster) && (posterHeightOrig > contentHeight)) {
        int overlapWidth = posterWidthOrig - widthPoster;
        int overlapHeight = posterHeightOrig - contentHeight;
        if (overlapWidth >= overlapHeight) {
            posterWidth = widthPoster - 2 * border;
            posterHeight = posterHeightOrig * ((double)posterWidth / (double)posterWidthOrig);
        } else {
            posterHeight = contentHeight - 2 * border;
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
        pixmapPoster->DrawImage(cPoint(posterX, posterY), imgLoader.GetImage());
    }
}

void cNopacityView::DrawBanner(int height) {
    if (!pixmapContent)
        return;

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
            bannerX = ((contentWidth - seasonPosterWidth) - bannerWidth) / 2;
        }
    } else if (bannerWidthOrig > contentWidth - 2 * border) {
        bannerWidth = contentWidth - 2 * border;
        bannerHeight = bannerHeightOrig * ((double)bannerWidth / (double)bannerWidthOrig);
        bannerX = (contentWidth - bannerWidth) / 2;
    }
    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(bannerPath.c_str(), bannerWidth, bannerHeight)) {
        pixmapContent->DrawImage(cPoint(bannerX, height), imgLoader.GetImage());
        return;
    }
    if (imgLoader.LoadPoster(seasonPoster.c_str(), seasonPosterWidth, seasonPosterHeight)) {
        pixmapContent->DrawImage(cPoint(contentWidth - seasonPosterWidth - border, height), imgLoader.GetImage());
    }
}

void cNopacityView::DrawAdditionalBanners(int y, int bottom) {
    if (!pixmapContent || series.banners.size() < 2)
        return;

    int bannerWidthOrig = series.banners[1].width;
    int bannerHeightOrig = series.banners[1].height;
    int bannerWidth = bannerWidthOrig;
    int bannerHeight = bannerHeightOrig;

    if (bannerWidthOrig == 0)
        return;

    if (bannerWidthOrig > contentWidth - 2 * border) {
        bannerWidth = contentWidth - 2 * border;
        bannerHeight = bannerHeightOrig * ((double)bannerWidth / (double)bannerWidthOrig);
    }
    int bannerX = (contentWidth - bannerWidth) / 2;

    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(series.banners[1].path.c_str(), bannerWidth, bannerHeight)) {
        pixmapContent->DrawImage(cPoint(bannerX, y), imgLoader.GetImage());
    }

    if (series.banners.size() < 3)
        return;

    if (imgLoader.LoadPoster(series.banners[2].path.c_str(), bannerWidth, bannerHeight)) {
        pixmapContent->DrawImage(cPoint(bannerX, bottom - bannerHeight - font->Height()), imgLoader.GetImage());
    }
}

void cNopacityView::DrawActors(std::vector<cActor> *actors) {
    int numActors = actors->size();
    int picsPerLine = config.GetValue("numPicturesPerLine");
    int actorsToView = 0;
    int thumbWidth = 0;
    int thumbHeight = 0;
    cImageLoader imgLoader;

    if (numActors > 0) {
        thumbWidth = (width - 2 * picsPerLine * border) / picsPerLine;
        thumbHeight = ((double)actors->at(0).actorThumb.height / (double)actors->at(0).actorThumb.width) * thumbWidth;

        for (int a = 0; a < numActors; a++) {
            if (a == numActors)
                break;
            std::string path = actors->at(a).actorThumb.path;
            if (imgLoader.LoadPoster(path.c_str(), thumbWidth, thumbHeight, false)) {
                actorsToView++;
            }
        }
    }

    if (actorsToView < 1) {
        CreateContent(100);
        if (pixmapContent)
            pixmapContent->DrawText(cPoint(border, border), tr("No Cast available"), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontHeaderLarge);
        return;
    }

    int picLines = actorsToView / picsPerLine;
    if (actorsToView % picsPerLine != 0)
        picLines++;
    
    int totalHeight = picLines * (thumbHeight + 2 * fontSmall->Height() + border + border / 2) + 2 * border + fontHeaderLarge->Height();

    CreateContent(totalHeight);
    if (!pixmapContent)
        return;

    cString header = cString::sprintf("%s:", tr("Cast"));
    pixmapContent->DrawText(cPoint(border, border), *header, Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontHeaderLarge);

    int x = 0;
    int y = 2 * border + fontHeaderLarge->Height();
    int actor = 0;
    for (int row = 0; row < picLines; row++) {
        for (int col = 0; col < picsPerLine;) {
            if (actor == numActors)
                break;
            std::string path = actors->at(actor).actorThumb.path;
            if (imgLoader.LoadPoster(path.c_str(), thumbWidth, thumbHeight)) {
                pixmapContent->DrawImage(cPoint(x + border, y), imgLoader.GetImage());
                std::string name = actors->at(actor).name;
                std::stringstream sstrRole;
                sstrRole << "\"" << actors->at(actor).role << "\"";
                std::string role = sstrRole.str();
                if (fontSmall->Width(name.c_str()) > thumbWidth + 2 * border)
                    name = CutText(name, thumbWidth + 2 * border, fontSmall);
                if (fontSmall->Width(role.c_str()) > thumbWidth + 2 * border)
                    role = CutText(role, thumbWidth + 2 * border, fontSmall);
                int xName = x + ((thumbWidth + 2 * border) - fontSmall->Width(name.c_str())) / 2;
                int xRole = x + ((thumbWidth + 2 * border) - fontSmall->Width(role.c_str())) / 2;
                pixmapContent->DrawText(cPoint(xName, y + thumbHeight + border / 2), name.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                pixmapContent->DrawText(cPoint(xRole, y + thumbHeight + border / 2 + fontSmall->Height()), role.c_str(), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontSmall);
                x += thumbWidth + 2 * border;
                col++;
            }
            actor++;
        }
        x = 0;
        y += thumbHeight + 2 * fontSmall->Height() + border + border / 2;
    }
}

void cNopacityView::DrawActors(int height) {
    if (!pixmapContent || (isSeries && series.fanarts.size() < 1))
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

void cNopacityView::DrawFanart(int height) {
    if (!pixmapContent || (isSeries && series.fanarts.size() < 1))
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

    if (fanartWidthOrig > contentWidth - 2 * border) {
        fanartWidth = contentWidth - 2 * border;
        fanartHeight = fanartHeightOrig * ((double)fanartWidth / (double)fanartWidthOrig);
    }
    cImageLoader imgLoader;
    if (isMovie) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        if (imgLoader.LoadPoster(fanartPath.c_str(), fanartWidth, fanartHeight)) {
            pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
        }
        if (movie.collectionFanart.path.size() > 0) {
            if (imgLoader.LoadPoster(movie.collectionFanart.path.c_str(), fanartWidth, fanartHeight)) {
                pixmapContent->DrawImage(cPoint(fanartX, height + fanartHeight + font->Size()), imgLoader.GetImage());
            }
        }
        return;
    }
    if (isSeries) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        for (std::vector<cTvMedia>::iterator f = series.fanarts.begin(); f != series.fanarts.end(); f++) {
            cTvMedia m = *f;
            if (imgLoader.LoadPoster(m.path.c_str(), fanartWidth, fanartHeight)) {
                pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
                height += fanartHeight + font->Height();
            }
        }
        return;
    }
    if (imgLoader.LoadPoster(fanartPath.c_str(), fanartWidth, fanartHeight)) {
        int fanartX = (contentWidth - fanartWidth) / 2;
        pixmapContent->DrawImage(cPoint(fanartX, height), imgLoader.GetImage());
    }
}

void cNopacityView::ClearScrollbar(void) {
    PixmapFill(pixmapScrollbar, clrTransparent);
    PixmapFill(pixmapScrollbarBack, clrTransparent);
}

void cNopacityView::DrawScrollbar(void) {
    ClearScrollbar();
    if (!scrollable || !pixmapContent || !pixmapScrollbar || !pixmapScrollbarBack)
        return;

    int totalBarHeight = pixmapScrollbar->ViewPort().Height() - 6;
    
    int aktHeight = (-1) * pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();

    int barHeight = (double)(screenHeight * totalBarHeight) / (double)totalHeight ;
    int barTop = (double)(aktHeight * totalBarHeight) / (double)totalHeight ;

    if (config.GetValue("displayType") == dtGraphical) {
        cImage *image = imgCache->GetSkinElement(seScrollbar);
        if (image)
            pixmapScrollbarBack->DrawImage(cPoint(0, 0), *image);
    } else {
        PixmapFill(pixmapScrollbarBack, Theme.Color(clrMenuScrollBar));
        pixmapScrollbarBack->DrawRectangle(cRect(2, 2, geoManager->menuWidthScrollbar - 4, pixmapScrollbarBack->ViewPort().Height() - 4), Theme.Color(clrMenuScrollBarBack));
    }

    pixmapScrollbar->DrawRectangle(cRect(3, 3 + barTop, geoManager->menuWidthScrollbar - 6, barHeight), Theme.Color(clrMenuScrollBar));
}

bool cNopacityView::KeyUp(bool Page) {
    if (!scrollable || !pixmapContent)
        return false;

    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int screenHeight = pixmapContent->ViewPort().Height();
    if (aktHeight >= 0) {
        return false;
    }

    int step = config.GetValue("detailedViewScrollStep") * font->Height();
    if (Page)
        step = screenHeight;
    int newY = aktHeight + step;
    if (newY > 0)
        newY = 0;
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
}

bool cNopacityView::KeyDown(bool Page) {
    if (!scrollable || !pixmapContent)
        return false;

    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();
    if (totalHeight - ((-1) * aktHeight) == screenHeight) {
        return false;
    }

    int step = config.GetValue("detailedViewScrollStep") * font->Height();
    if (Page)
        step = screenHeight;
    int newY = aktHeight - step;
    if ((-1) * newY > totalHeight - screenHeight)
        newY = (-1) * (totalHeight - screenHeight);
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
}

void cNopacityView::KeyInput(bool Up, bool Page) {
    bool scrolled = false;
    if (Up && Page) {
        if ((config.GetValue("tabsInDetailView") == 0)) {
            scrolled = KeyUp(Page);
        } else {
            KeyLeft();
            Render();
        }
    } else if (!Up && Page) {
        if ((config.GetValue("tabsInDetailView") == 0)) {
            scrolled = KeyDown(Page);
        } else {
            KeyRight();
            Render();
        }
    } else if (Up && !Page) {
        scrolled = KeyUp();
    } else if (!Up && !Page) {
        scrolled = KeyDown();
    }
    if (scrolled)
        DrawScrollbar();
    osd->Flush();
}

/********************************************************************************************
* cNopacityEPGView : cNopacityView
********************************************************************************************/

cNopacityEPGView::cNopacityEPGView(cOsd *osd) : cNopacityView(osd) {
    tabbed = true;
    numEPGPics = -1;
    numTabs = 0;
}

void cNopacityEPGView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    if (numEPGPics > 0)
        tabs.push_back(tr("Image Galery"));
    numTabs = tabs.size();
}

int cNopacityEPGView::DrawHeaderEPGImage(void) {
    int imageBorder = 4;
    int imgHeight = headerHeight - 2 * imageBorder;
    int maxImgWidth = 0.25 * width;

    cImageLoader imgLoader;
    if (eventID > 0) {
        if (!imgLoader.LoadEPGImage(eventID, maxImgWidth, imgHeight))
            return 0;
    } else if (recFileName.size() > 0) {
        if (!imgLoader.LoadRecordingImage(recFileName.c_str(), maxImgWidth, imgHeight))
            return 0;
    } else
        return 0;

    cImage image = imgLoader.GetImage();
    int imgWidth = image.Width();
    int xImage = width - imgWidth - border;

    if (!pixmapHeaderEPGImage) {
        if (!(pixmapHeaderEPGImage = CreatePixmap(osd, "pixmapHeaderEPGImage", 5, cRect(xImage, y + imageBorder, imgWidth, imgHeight)))) {
            return 0;
        }
    }

    PixmapFill(pixmapHeaderEPGImage, clrTransparent);

    int bannerY = std::max((imgHeight - image.Height()) / 2, 0);
    pixmapHeaderEPGImage->DrawImage(cPoint(0, bannerY), image);
    return imgWidth + 2 * border;
}

void cNopacityEPGView::CheckEPGImages(void) {
    if (eventID > 0) {
        for (int i=1; i <= config.GetValue("numAdditionalEPGPictures"); i++) {
            cString epgimage;
            if (config.epgImagePathSet) {
                epgimage = cString::sprintf("%s%d_%d.jpg", *config.epgImagePath, eventID, i);
            } else {
                epgimage = cString::sprintf("%s%d_%d.jpg", *config.epgImagePathDefault, eventID, i);
            }
            FILE *fp = fopen(*epgimage, "r");
            if (fp) {
                std::stringstream ss;
                ss << i;
                epgPics.push_back(ss.str());
                fclose(fp);
            } else {
                break;
            }
        }
    } else if (recFileName.size() > 0) {
        DIR *dirHandle;
        struct dirent *dirEntry;
        dirHandle = opendir(recFileName.c_str());
        int picsFound = 0;
        if (dirHandle != NULL) {
            while ( 0 != (dirEntry = readdir(dirHandle))) {
                if (endswith(dirEntry->d_name, "jpg")) {
                    std::string fileName = dirEntry->d_name;
                    if (fileName.length() > 4) {
                        fileName = fileName.substr(0, fileName.length() - 4);
                        epgPics.push_back(fileName);
                        picsFound++;
                    }
                }
                if (picsFound >= config.GetValue("numAdditionalRecEPGPictures")) {
                    break;
                }
            }
            closedir(dirHandle);
        }
    } else {
        return;
    }
    numEPGPics = epgPics.size();
}

void cNopacityEPGView::DrawImages(void) {
    int imgWidth = config.GetValue("epgImageWidthLarge");
    int imgHeight = config.GetValue("epgImageHeightLarge");

    int totalHeight = numEPGPics * (imgHeight + border);

    CreateContent(totalHeight);
    
    if (!pixmapContent)
        return;

    cImageLoader imgLoader;
    int yPic = border;
    for (int pic = 0; pic < numEPGPics; pic++) {
        bool drawPic = false;
        if (eventID > 0) {
            cString epgimage = cString::sprintf("%d_%d", eventID, atoi(epgPics[pic].c_str()));
            if (imgLoader.LoadAdditionalEPGImage(epgimage)) {
                drawPic = true;
            }

        } else if (recFileName.size() > 0) {
            std::string path = recFileName + "/";
            if (imgLoader.LoadAdditionalRecordingImage(path.c_str(), epgPics[pic].c_str())) {
                drawPic = true;
            }
        }
        if (drawPic) {
            pixmapContent->DrawImage(cPoint((width - imgWidth) / 2, yPic), imgLoader.GetImage());
            yPic += imgHeight + border;
        }
    } 
}

void cNopacityEPGView::KeyLeft(void) {
    activeView--;
    if (activeView < 0)
        activeView = numTabs - 1; 
}

void cNopacityEPGView::KeyRight(void) {
    activeView = (activeView + 1) % numTabs;
}

void cNopacityEPGView::Render(void) {
    ClearContent();
    if (!headerDrawn) {
        int widthPixmap = DrawHeaderEPGImage();
        DrawHeader(widthPixmap);
        headerDrawn = true;
    }
    if (tabs.size() == 0) {
        CheckEPGImages();
        SetTabs();
    }
    DrawTabs();
    switch (activeView) {
        case evtInfo:
            DrawContent(&infoText);
            break;
        case evtAddInfo:
            DrawContent(&addInfoText);
            break;
        case evtImages:
            DrawImages();
            break;
    }
    DrawScrollbar();
}

/********************************************************************************************
* cNopacitySeriesView : cNopacityView
********************************************************************************************/

cNopacitySeriesView::cNopacitySeriesView(cOsd *osd, int seriesId, int episodeId) : cNopacityView(osd) {
    this->seriesId = seriesId;
    this->episodeId = episodeId;
    tvdbInfo = "";
    tabbed = true;
}

void cNopacitySeriesView::LoadMedia(void) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper || (config.GetValue("scraperInfo") == 0))
        return;

    series.seriesId = seriesId;
    series.episodeId = episodeId;
    pScraper->Service("GetSeries", &series);
}

void cNopacitySeriesView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    tabs.push_back(tr("Cast"));
    tabs.push_back(tr("TheTVDB Info"));
    tabs.push_back(tr("Image Galery"));
}

void cNopacitySeriesView::CreateTVDBInfo(void) {
    if (tvdbInfo.size() > 0)
        return;
    std::stringstream info;
    info << tr("TheTVDB Information") << ":\n\n";

    if (series.episode.name.size() > 0) {
        info << tr("Episode") << ": " << series.episode.name << " (" << tr("Season") << " " << series.episode.season << ", " << tr("Episode") << " " << series.episode.number << ")\n\n";
    }
    if (series.episode.overview.size() > 0) {
        info << tr("Episode Overview") << ": " << series.episode.overview << "\n\n";
    }
    if (series.episode.firstAired.size() > 0) {
        info << tr("First aired") << ": " << series.episode.firstAired << "\n\n";
    }
    if (series.episode.guestStars.size() > 0) {
        info << tr("Guest Stars") << ": " << series.episode.guestStars << "\n\n";
    }
    if (series.episode.rating > 0) {
        info << tr("TheTVDB Rating") << ": " << series.episode.rating << "\n\n";
    }
    if (series.overview.size() > 0) {
        info << tr("Series Overview") << ": " << series.overview << "\n\n";
    }
    if (series.firstAired.size() > 0) {
        info << tr("First aired") << ": " << series.firstAired << "\n\n";
    }
    if (series.genre.size() > 0) {
        info << tr("Genre") << ": " << series.genre << "\n\n";
    }
    if (series.network.size() > 0) {
        info << tr("Network") << ": " << series.network << "\n\n";
    }
    if (series.rating > 0) {
        info << tr("TheTVDB Rating") << ": " << series.rating << "\n\n";
    }
    if (series.status.size() > 0) {
        info << tr("Status") << ": " << series.status << "\n\n";
    }
    tvdbInfo = info.str();
}

int cNopacitySeriesView::DrawHeaderBanner(void) {
    if (series.banners.size() == 0 || series.banners[0].height == 0 || series.banners[0].width == 0)
        return 0;
 
    int imageBorder = 4;
    int bannerHeight = headerHeight - 2 * imageBorder;
    int maxBannerWidth = 0.25 * width;

    cImageLoader imgLoader;
    if (!(imgLoader.LoadPoster(series.banners[0].path.c_str(), maxBannerWidth, bannerHeight))) {
        return 0;
    }

    cImage banner = imgLoader.GetImage();
    int bannerWidth = banner.Width();
    int xBanner = width - bannerWidth - border;

    if (!pixmapHeaderBanner) {
        if (!(pixmapHeaderBanner = CreatePixmap(osd, "pixmapHeaderBanner", 5, cRect(xBanner, y + imageBorder, bannerWidth, bannerHeight)))){
            return 0;
        }
    }
    PixmapFill(pixmapHeaderBanner, clrTransparent);

    int bannerY = std::max((bannerHeight - banner.Height()) / 2, 0);
    pixmapHeaderBanner->DrawImage(cPoint(0, bannerY), banner);
    return bannerWidth + 2 * border;
}

void cNopacitySeriesView::DrawImages(void) {
    int numPosters = series.posters.size();
    int numFanarts = series.fanarts.size();
    int numBanners = series.banners.size();
    
    int totalHeight = border;
    //Fanart Height
    int fanartWidth = width - 2 * border;
    int fanartHeight = 0;
    if (numFanarts > 0 && series.fanarts[0].width > 0) {
        fanartHeight = series.fanarts[0].height * ((double)fanartWidth / (double)series.fanarts[0].width);
        if (fanartHeight > contentHeight - 2 * border) {
            int fanartHeightOrig = fanartHeight;
            fanartHeight = contentHeight - 2 * border;
            fanartWidth = fanartWidth * ((double)fanartHeight / (double)fanartHeightOrig);
        }
        totalHeight += series.fanarts.size() * (fanartHeight + border);
    }
    //Poster Height
    int posterWidth = (width - 4 * border) / 2;
    int posterHeight = 0;
    if (numPosters > 0 && series.posters[0].width > 0) {
        posterHeight = series.posters[0].height * ((double)posterWidth / (double)series.posters[0].width);
    }
    if (numPosters > 0)
        totalHeight += posterHeight + border;
    if (numPosters == 3)
        totalHeight += posterHeight + border;
    //Banners Height
    if (numBanners > 0)
        totalHeight += (series.banners[0].height + border) * numBanners;
       
    CreateContent(totalHeight);
    
    if (!pixmapContent)
        return;

    cImageLoader imgLoader;
    int yPic = border;
    for (int i=0; i < numFanarts; i++) {
        if (numBanners > i) {
            if (imgLoader.LoadPoster(series.banners[i].path.c_str(), series.banners[i].width, series.banners[i].height)) {
                pixmapContent->DrawImage(cPoint((width - series.banners[i].width) / 2, yPic), imgLoader.GetImage());
                yPic += series.banners[i].height + border;
            }
        }
        if (imgLoader.LoadPoster(series.fanarts[i].path.c_str(), fanartWidth, fanartHeight)) {
            pixmapContent->DrawImage(cPoint((width - fanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += fanartHeight + border;
        }
    }
    if (numPosters >= 1) {
        if (imgLoader.LoadPoster(series.posters[0].path.c_str(), posterWidth, posterHeight)) {
            pixmapContent->DrawImage(cPoint(border, yPic), imgLoader.GetImage());
            yPic += posterHeight + border;
        }
    }
    if (numPosters >= 2) {
        if (imgLoader.LoadPoster(series.posters[1].path.c_str(), posterWidth, posterHeight)) {
            pixmapContent->DrawImage(cPoint(2 * border + posterWidth, yPic - posterHeight - border), imgLoader.GetImage());
        }
    }
    if (numPosters == 3) {
        if (imgLoader.LoadPoster(series.posters[2].path.c_str(), posterWidth, posterHeight)) {
            pixmapContent->DrawImage(cPoint((width - posterWidth) / 2, yPic), imgLoader.GetImage());
        }
    }
}

int cNopacitySeriesView::GetRandomPoster(void) {
    int numPosters = series.posters.size();
    if (numPosters == 0)
        return -1;
    srand((unsigned)time(NULL));
    int randPoster = rand()%numPosters;
    return randPoster;
}

void cNopacitySeriesView::KeyLeft(void) {
    activeView--;
    if (activeView < 0)
        activeView = mvtCount - 1; 
}

void cNopacitySeriesView::KeyRight(void) {
    activeView = (activeView + 1) % mvtCount;
}

void cNopacitySeriesView::Render(void) {
    ClearContent();
    if (!headerDrawn) {
        int widthPixmap = DrawHeaderBanner();
        DrawHeader(widthPixmap);
        headerDrawn = true;
    }
    if (tabs.size() == 0) {
        SetTabs();
    }
    DrawTabs();
    int randomPoster = GetRandomPoster();
    switch (activeView) {
        case mvtInfo:
            if (randomPoster >= 0) {
                DrawFloatingContent(&infoText, &series.posters[randomPoster]);
            } else
                DrawContent(&infoText);
            break;
        case mvtAddInfo:
            if (randomPoster >= 0)
                DrawFloatingContent(&addInfoText, &series.posters[randomPoster]);
            else
                DrawContent(&addInfoText);
            break;
        case mvtCast:
            DrawActors(&series.actors);
            break;
        case mvtOnlineInfo:
            CreateTVDBInfo();
            if ((series.seasonPoster.path.size() > 0) && series.episode.episodeImage.path.size() > 0)
                DrawFloatingContent(&tvdbInfo, &series.episode.episodeImage, &series.seasonPoster);
            else if (series.seasonPoster.path.size() > 0)
                DrawFloatingContent(&tvdbInfo, &series.seasonPoster);
            else if (series.episode.episodeImage.path.size() > 0)
                DrawFloatingContent(&tvdbInfo, &series.episode.episodeImage);
            else if (randomPoster >= 0)
                DrawFloatingContent(&tvdbInfo, &series.posters[randomPoster]);
            else
                DrawContent(&tvdbInfo);
            break;
        case mvtImages:
            DrawImages();
            break;
    }
    DrawScrollbar();
}

/********************************************************************************************
* cNopacityMovieView : cNopacityView
********************************************************************************************/

cNopacityMovieView::cNopacityMovieView(cOsd *osd, int movieId) : cNopacityView(osd) {
    this->movieId = movieId;
    tabbed = true;
}

void cNopacityMovieView::LoadMedia(void) {
    static cPlugin *pScraper = GetScraperPlugin();
    if (!pScraper || (config.GetValue("scraperInfo") == 0))
        return;

    movie.movieId = movieId;
    pScraper->Service("GetMovie", &movie);    
}

void cNopacityMovieView::SetTabs(void) {
    tabs.push_back(tr("EPG Info"));
    if (eventID > 0)
        tabs.push_back(tr("Reruns"));
    else
        tabs.push_back(tr("Recording Information"));
    tabs.push_back(tr("Cast"));
    tabs.push_back(tr("TheMovieDB Info"));
    tabs.push_back(tr("Image Galery"));
}

void cNopacityMovieView::CreateMovieDBInfo(void) {
    if (movieDBInfo.size() > 0)
        return;
    std::stringstream info;
    info << tr("TheMovieDB Information") << ":\n\n";
    if (movie.originalTitle.size() > 0) {
        info << tr("Original Title") << ": " << movie.originalTitle << "\n\n";
    }
    if (movie.tagline.size() > 0) {
        info << tr("Tagline") << ": " << movie.tagline << "\n\n";
    }
    if (movie.overview.size() > 0) {
        info << tr("Overview") << ": " << movie.overview << "\n\n";
    }
    std::string strAdult = (movie.adult)?(tr("yes")):(tr("no"));
    info << tr("Adult") << ": " << strAdult << "\n\n";
    if (movie.collectionName.size() > 0) {
        info << tr("Collection") << ": " << movie.collectionName << "\n\n";
    }
    if (movie.budget > 0) {
        info << tr("Budget") << ": " << movie.budget << "$\n\n";
    }
    if (movie.revenue > 0) {
        info << tr("Revenue") << ": " << movie.revenue << "$\n\n";
    }
    if (movie.genres.size() > 0) {
        info << tr("Genre") << ": " << movie.genres << "\n\n";
    }
    if (movie.homepage.size() > 0) {
        info << tr("Homepage") << ": " << movie.homepage << "\n\n";
    }
    if (movie.releaseDate.size() > 0) {
        info << tr("Release Date") << ": " << movie.releaseDate << "\n\n";
    }
    if (movie.runtime > 0) {
        info << tr("Runtime") << ": " << movie.runtime << " " << tr("minutes") << "\n\n";
    }
    if (movie.popularity > 0) {
        info << tr("TheMovieDB Popularity") << ": " << movie.popularity << "\n\n";
    }
    if (movie.voteAverage > 0) {
        info << tr("TheMovieDB Vote Average") << ": " << movie.voteAverage << "\n\n";
    }
    movieDBInfo = info.str();
}

int cNopacityMovieView::DrawHeaderPoster(void) {
    if (movie.poster.width == 0 || movie.poster.height == 0 || movie.poster.path.size() == 0)
        return 0;
 
    int imageBorder = 4;
    int posterHeight = headerHeight - 2 * imageBorder;
    int maxPosterWidth = 0.25 * width;

    cImageLoader imgLoader;
    if (!(imgLoader.LoadPoster(movie.poster.path.c_str(), maxPosterWidth, posterHeight))) {
        return 0;
    }

    cImage poster = imgLoader.GetImage();
    int posterWidth = poster.Width();
    int xPoster = width - posterWidth - border;

    if (!pixmapHeaderPoster) {
        if (!(pixmapHeaderPoster = CreatePixmap(osd, "pixmapHeaderPoster", 5, cRect(xPoster, y + imageBorder, posterWidth, posterHeight)))) {
            return 0;
        }
    }
    PixmapFill(pixmapHeaderPoster, clrTransparent);

    int posterY = std::max((posterHeight - poster.Height()) / 2, 0);
    pixmapHeaderPoster->DrawImage(cPoint(0, posterY), poster);
    return posterWidth + 2 * border;
}

void cNopacityMovieView::DrawImages(void) {
    int totalHeight = border;
    //Fanart Height
    int fanartWidth = width - 2 * border;
    int fanartHeight = 0;
    if (movie.fanart.width > 0 && movie.fanart.height > 0 && movie.fanart.path.size() > 0) {
        fanartHeight = movie.fanart.height * ((double)fanartWidth / (double)movie.fanart.width);
        if (fanartHeight > contentHeight - 2 * border) {
            int fanartHeightOrig = fanartHeight;
            fanartHeight = contentHeight - 2 * border;
            fanartWidth = fanartWidth * ((double)fanartHeight / (double)fanartHeightOrig);
        }
        totalHeight += fanartHeight + border;
    }
    //Collection Fanart Height
    int collectionFanartWidth = width - 2 * border;
    int collectionFanartHeight = 0;
    if (movie.collectionFanart.width > 0 && movie.collectionFanart.height > 0 && movie.collectionFanart.path.size() > 0) {
        collectionFanartHeight = movie.collectionFanart.height * ((double)collectionFanartWidth / (double)movie.collectionFanart.width);
        if (collectionFanartHeight > contentHeight - 2 * border) {
            int fanartHeightOrig = collectionFanartHeight;
            collectionFanartHeight = contentHeight - 2 * border;
            collectionFanartWidth = collectionFanartWidth * ((double)collectionFanartHeight / (double)fanartHeightOrig);
        }
        totalHeight += collectionFanartHeight + border;
    }
    //Poster Height
    if (movie.poster.width > 0 && movie.poster.height > 0 && movie.poster.path.size() > 0) {
        totalHeight += movie.poster.height + border;
    }
    //Collection Popster Height
    if (movie.collectionPoster.width > 0 && movie.collectionPoster.height > 0 && movie.collectionPoster.path.size() > 0) {
        totalHeight += movie.collectionPoster.height + border;
    }

    CreateContent(totalHeight);
    
    if (!pixmapContent)
        return;

    cImageLoader imgLoader;
    int yPic = border;
    if (movie.fanart.width > 0 && movie.fanart.height > 0 && movie.fanart.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.fanart.path.c_str(), fanartWidth, fanartHeight)) {
            pixmapContent->DrawImage(cPoint((width - fanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += fanartHeight + border;
        }
    }
    if (movie.collectionFanart.width > 0 && movie.collectionFanart.height > 0 && movie.collectionFanart.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.collectionFanart.path.c_str(), collectionFanartWidth, collectionFanartHeight)) {
            pixmapContent->DrawImage(cPoint((width - collectionFanartWidth)/2, yPic), imgLoader.GetImage());
            yPic += collectionFanartHeight + border;
        }
    }
    if (movie.poster.width > 0 && movie.poster.height > 0 && movie.poster.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.poster.path.c_str(), movie.poster.width, movie.poster.height)) {
            pixmapContent->DrawImage(cPoint((width - movie.poster.width) / 2, yPic), imgLoader.GetImage());
            yPic += movie.poster.height + border;
        }
    }
    if (movie.collectionPoster.width > 0 && movie.collectionPoster.height > 0 && movie.collectionPoster.path.size() > 0) {
        if (imgLoader.LoadPoster(movie.collectionPoster.path.c_str(), movie.collectionPoster.width, movie.collectionPoster.height)) {
            pixmapContent->DrawImage(cPoint((width - movie.collectionPoster.width) / 2, yPic), imgLoader.GetImage());
            yPic += movie.collectionPoster.height + border;
        }
    }
}

void cNopacityMovieView::KeyLeft(void) {
    activeView--;
    if (activeView < 0)
        activeView = mvtCount - 1; 
}

void cNopacityMovieView::KeyRight(void) {
    activeView = (activeView + 1) % mvtCount;
}

void cNopacityMovieView::Render(void) {
    ClearContent();
    if (!headerDrawn) {
        int widthPixmap = DrawHeaderPoster();
        DrawHeader(widthPixmap);
        headerDrawn = true;
    }
    if (tabs.size() == 0) {
        SetTabs();
    }
    DrawTabs();
    bool posterAvailable = (movie.poster.path.size() > 0 && movie.poster.width > 0 && movie.poster.height > 0) ? true : false;
    switch (activeView) {
        case mvtInfo:
            if (posterAvailable)
                DrawFloatingContent(&infoText, &movie.poster);
            else
                DrawContent(&infoText);
            break;
        case mvtAddInfo:
            if (posterAvailable)
                DrawFloatingContent(&addInfoText, &movie.poster);
            else
                DrawContent(&addInfoText);
            break;
        case mvtCast:
            DrawActors(&movie.actors);
            break;
        case mvtOnlineInfo:
            CreateMovieDBInfo();
            if (posterAvailable)
                DrawFloatingContent(&movieDBInfo, &movie.poster);
            else
                DrawContent(&movieDBInfo);
            break;
        case mvtImages:
            DrawImages();
            break;
    }
    DrawScrollbar();
}

/********************************************************************************************
* cNopacityTextView : cNopacityView
********************************************************************************************/

cNopacityTextView::cNopacityTextView(cOsd *osd) : cNopacityView(osd) {
    textView = true;
}

void cNopacityTextView::KeyLeft(void) {
    if (!pixmapContent)
        return;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int screenHeight = pixmapContent->ViewPort().Height();
    int newY = aktHeight + screenHeight;
    if (newY > 0)
        newY = 0;
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    DrawScrollbar();
}

void cNopacityTextView::KeyRight(void) {
    if (!pixmapContent)
        return;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int screenHeight = pixmapContent->ViewPort().Height();
    int totalHeight = pixmapContent->DrawPort().Height();
    int newY = aktHeight - screenHeight;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    DrawScrollbar();
}

void cNopacityTextView::Render(void) {
    if (pixmapContent)
        return;
    DrawContent(&infoText, 0, FixedFont);
    DrawScrollbar();
}

/********************************************************************************************
* cNopacityMenuDetailEventViewLight
********************************************************************************************/

cNopacityMenuDetailEventViewLight::cNopacityMenuDetailEventViewLight(cOsd *osd, const cEvent *Event) : cNopacityView(osd) {
    event = Event;
    numEPGPics = 0;
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
            if (call.type == tSeries) {
                series.seriesId = seriesId;
                series.episodeId = episodeId;
                if (pScraper->Service("GetSeries", &series)) {
                    isSeries = true;
                }
            } else if (call.type == tMovie) {
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
    }
}

void cNopacityMenuDetailEventViewLight::SetContentHeight(void) {
    int lineHeight = font->Height();
    int totalHeight = 0;

    yBanner = 0;

    //Height of banner (only for series)
    if (isSeries) {
        if (isSeries && series.banners.size() > 0) {
            totalHeight  += lineHeight + series.banners[0].height;
        }
    }
    yEPGText = totalHeight;

    //Height of EPG Text
    if (epgText.Lines() > 0) {
        totalHeight  += lineHeight + epgText.Lines() * lineHeight;
    }
    yAddInf = totalHeight;

    //Height of rerun information
    if (config.GetValue("displayRerunsDetailEPGView")) {
        LoadAddInfo(&addInfoText);
        if (addInfo.Lines() > 0) {
            totalHeight += lineHeight + addInfo.Lines() * lineHeight;
        }
    }
    yActors = totalHeight;

    //Height of actor pictures
    if (isMovie || isSeries) {
        int heightActorPics = HeightActorPics();
        if (heightActorPics)
            totalHeight += lineHeight + heightActorPics;
    }
    yScrapInfo = totalHeight;

    //Height of additional scraper info
    if (isMovie || isSeries) {
        int heightScraperInfo = HeightScraperInfo();
        if (heightScraperInfo)
            totalHeight += lineHeight + heightScraperInfo;
    }
    yFanart = totalHeight;

    //Height of fanart
    if (isMovie || isSeries) {
        int heightFanart = HeightFanart();
        if (heightFanart)
            totalHeight += heightFanart;
    }
    yEPGPics = totalHeight;

    //Height of EPG Pictures
    if ((config.GetValue("displayAdditionalEPGPictures") == 1) || ((config.GetValue("displayAdditionalEPGPictures") == 2) && !isMovie &&  !isSeries)) {
        int heightEPGPics = HeightEPGPics();
        if (heightEPGPics)
            totalHeight += HeightEPGPics();
    }

    //check if pixmap content has to be scrollable
    if (totalHeight > contentHeight) {
        contentDrawPortHeight = totalHeight;
        scrollable = true;
    } else {
        contentDrawPortHeight = contentHeight;
    }
}

void cNopacityMenuDetailEventViewLight::CreatePixmaps(void) {
    contentHeight = contentHeight - border;

    pixmapContent = CreatePixmap(osd, "pixmapContent", 3, cRect(x + contentX, y + headerHeight + border, contentWidth, contentHeight),
                                                          cRect(0, 0, contentWidth, contentDrawPortHeight));

    if (isSeries || isMovie) {
        pixmapPoster = CreatePixmap(osd, "pixmapPoster", 4, cRect(x, y + headerHeight, widthPoster, contentHeight));
    }

    PixmapFill(pixmapContent, clrTransparent);
    PixmapFill(pixmapPoster, clrTransparent);
}

void cNopacityMenuDetailEventViewLight::Render(void) {
    SetContent();
    SetContentHeight();
    CreatePixmaps();
    int widthPixmap = DrawHeaderPosterLight();
    DrawHeader(widthPixmap);
    //draw EPG text
    DrawTextWrapper(&epgText, yEPGText);
    //draw reruns
    if (config.GetValue("displayRerunsDetailEPGView")) {
        DrawContent(&addInfoText, yAddInf);
    }
    //draw additional scraper info
    if (isSeries || isMovie) {
        DrawTextWrapper(&scrapInfo, yScrapInfo);
    } 
    if (isSeries || isMovie) {
        DrawPoster();
    }
    //draw banner only for series
    if (isSeries) {
        DrawBanner(yBanner);
    }
    //draw actors
    if (isSeries || isMovie) {
       DrawActors(yActors);
    }
    //draw additional banners
    if (isSeries) {
       DrawAdditionalBanners(yScrapInfo, yFanart);
    }
    //draw fanart
    if (isSeries || isMovie) {
       DrawFanart(yFanart);
    }
    //draw additional EPG Pictures
    if ((config.GetValue("displayAdditionalEPGPictures") == 1) || ((config.GetValue("displayAdditionalEPGPictures") == 2) && !isMovie && !isSeries)) {
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
    return picLines * (config.GetValue("epgImageHeightLarge") + border);
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

/********************************************************************************************
* cNopacityMenuDetailRecordingViewLight
********************************************************************************************/

cNopacityMenuDetailRecordingViewLight::cNopacityMenuDetailRecordingViewLight(cOsd *osd, const cRecording *Recording) : cNopacityView(osd) {
    recording = Recording;
    info = Recording->Info();
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
                if (call.type == tSeries) {
                    series.seriesId = seriesId;
                    series.episodeId = episodeId;
                    if (pScraper->Service("GetSeries", &series)) {
                        isSeries = true;
                    }
                } else if (call.type == tMovie) {
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
    }
}

void cNopacityMenuDetailRecordingViewLight::SetContentHeight(void) {
    int lineHeight = font->Height();
    int totalHeight = 0;

    yBanner = 0;

    //Height of banner (only for series)
    if (!hasManualPoster && isSeries) {
        if (isSeries && series.banners.size() > 0) {
            totalHeight  += lineHeight + series.banners[0].height;
        }
    }
    yEPGText = totalHeight;

    //Height of Recording EPG Info
    if (recInfo.Lines() > 0) {
        totalHeight  += lineHeight + recInfo.Lines() * lineHeight;
    }
    yActors = totalHeight;

    //Height of actor pictures
    if (!hasManualPoster && (isMovie || isSeries)) {
        int heightActorPics = HeightActorPics();
        if (heightActorPics)
            totalHeight += lineHeight + heightActorPics;
    }
    yScrapInfo = totalHeight;

    //Height of additional scraper info
    if (isMovie || isSeries) {
        int heightScraperInfo = HeightScraperInfo();
        if (heightScraperInfo)
            totalHeight += lineHeight + heightScraperInfo;
    }
    yFanart = totalHeight;

    //Height of fanart
    if (!hasManualPoster && (isMovie || isSeries)) {
        int heightFanart = HeightFanart();
        if (heightFanart)
            totalHeight += heightFanart;
    }
    yEPGPics = totalHeight;

    //Height of EPG Pictures
    if (((config.GetValue("displayAdditionalRecEPGPictures") == 1) || ((config.GetValue("displayAdditionalRecEPGPictures") == 2) && !(!isMovie && !isSeries)))) {
//    if ((config.GetValue("displayAdditionalRecEPGPictures") == 1)) {
        if (LoadEPGPics())
            totalHeight += HeightEPGPics();
    }
    yAddInf = totalHeight;

    //additional recording Info
    if (config.GetValue("displayRerunsDetailEPGView")) {
        LoadAddInfo(&addInfoText);
        if (addInfo.Lines() > 0) {
            totalHeight += addInfo.Lines() * lineHeight;
        }
    }

    //check if pixmap content has to be scrollable
    if (totalHeight > contentHeight) {
        contentDrawPortHeight = totalHeight;
        scrollable = true;
    } else {
        contentDrawPortHeight = contentHeight;
    }
}

void cNopacityMenuDetailRecordingViewLight::CreatePixmaps(void) {
    contentHeight = contentHeight - border;

    pixmapContent = CreatePixmap(osd, "pixmapContent", 3, cRect(x + contentX, y + headerHeight + border, contentWidth, contentHeight),
                                                          cRect(0, 0, contentWidth, contentDrawPortHeight));

    if (hasManualPoster || isMovie || isSeries) {
        pixmapPoster = CreatePixmap(osd, "pixmapPoster", 4, cRect(x, y + headerHeight, widthPoster, contentHeight));
    }

    PixmapFill(pixmapContent, clrTransparent);
    PixmapFill(pixmapPoster, clrTransparent);
}

void cNopacityMenuDetailRecordingViewLight::Render(void) {
    SetContent();
    SetContentHeight();
    CreatePixmaps();
    int widthPixmap = DrawHeaderPosterLight();
    DrawHeader(widthPixmap);
    if (hasManualPoster || isSeries || isMovie) {
        DrawPoster();
    }
    //draw banner only for series
    if (!hasManualPoster && isSeries) {
        DrawBanner(yBanner);
    }
    //draw Recording EPG text
    DrawTextWrapper(&recInfo, yEPGText);
    //draw actors
    if (!hasManualPoster && (isSeries || isMovie)) {
        DrawActors(yActors);
    }
    //draw additional scraper info
    if (isSeries || isMovie) {
        DrawTextWrapper(&scrapInfo, yScrapInfo);
    }
    //draw fanart
    if (!hasManualPoster && (isSeries || isMovie)) {
       DrawFanart(yFanart);
    }
    //draw additional EPG Pictures
    if (((config.GetValue("displayAdditionalRecEPGPictures") == 1) || ((config.GetValue("displayAdditionalRecEPGPictures") == 2) && !(!isMovie && !isSeries)))) {
        DrawEPGPictures(yEPGPics);
    }
    //draw additional Info
    if (config.GetValue("displayRerunsDetailEPGView")) {
        DrawContent(&addInfoText, yAddInf);
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
