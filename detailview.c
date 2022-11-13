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
    tabHeight = 0;
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
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapContent);
    osd->DestroyPixmap(pixmapContentBack);
    osd->DestroyPixmap(pixmapTabs);
    osd->DestroyPixmap(pixmapHeaderEPGImage);
    osd->DestroyPixmap(pixmapHeaderBanner);
    osd->DestroyPixmap(pixmapHeaderPoster);
}

void cNopacityView::SetGeometry(int x, int y, int width, int height, int border, int headerHeight) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->border = border;
    this->headerHeight = headerHeight;
    if (tabbed)
        tabHeight = 2 * border;
    contentHeight = height - headerHeight - tabHeight;
    SetFonts();
}

void cNopacityView::SetAlpha(int Alpha) {
    PixmapSetAlpha(pixmapHeader, Alpha);
    PixmapSetAlpha(pixmapTabs, Alpha);
    PixmapSetAlpha(pixmapContentBack, Alpha);
    PixmapSetAlpha(pixmapContent, Alpha);
    PixmapSetAlpha(pixmapScrollbar, Alpha);
    PixmapSetAlpha(pixmapScrollbarBack, Alpha);
    PixmapSetAlpha(pixmapHeaderEPGImage, Alpha);
    PixmapSetAlpha(pixmapHeaderBanner, Alpha);
    PixmapSetAlpha(pixmapHeaderPoster, Alpha);
}

void cNopacityView::SetFonts(void) {
    font = cFont::CreateFont(config.fontName, contentHeight / 25 + 3 + config.GetValue("fontDetailView"));
    fontSmall = cFont::CreateFont(config.fontName, contentHeight / 30 + config.GetValue("fontDetailViewSmall"));
    fontHeaderLarge = cFont::CreateFont(config.fontName, headerHeight / 4 + 5 + config.GetValue("fontDetailViewHeaderLarge"));
    fontHeader = cFont::CreateFont(config.fontName, headerHeight / 6 + config.GetValue("fontDetailViewHeader"));
}

int cNopacityView::HeightActorPics(void) {
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

void cNopacityView::DrawHeader(void) {
    if (!pixmapHeader) {
        if (!(pixmapHeader = CreatePixmap(osd, "pixmapHeader", 4, cRect(x, y, width, headerHeight)))) {
            return;
        }
    }
    PixmapFill(pixmapHeader, Theme.Color(clrMenuDetailViewBack));

    //Channel Logo
    int logoWidth = geoManager->channelLogoWidth;
    int xText = border;
    if (channel) {
        cImage *logo = imgCache->GetLogo(ctLogo, channel);
        if (logo) {
            pixmapHeader->DrawImage(cPoint(border, std::max((headerHeight - geoManager->channelLogoHeight - border) / 2, 0)), *logo);
            xText += logoWidth + border;
        }
    }
    //Date and Time, Title, Subtitle
    int yDateTime = border;
    int yTitle = (headerHeight - fontHeaderLarge->Height()) / 2;
    int ySubtitle = headerHeight - fontHeader->Height() - border;
    pixmapHeader->DrawText(cPoint(xText, yDateTime), dateTime.c_str(), Theme.Color(clrMenuFontDetailViewHeader), Theme.Color(clrMenuDetailViewBack), fontHeader);
    pixmapHeader->DrawText(cPoint(xText, yTitle), title.c_str(), Theme.Color(clrMenuFontDetailViewHeaderTitle), Theme.Color(clrMenuDetailViewBack), fontHeaderLarge);
    pixmapHeader->DrawText(cPoint(xText, ySubtitle), subTitle.c_str(), Theme.Color(clrMenuFontDetailViewHeader), Theme.Color(clrMenuDetailViewBack), fontHeader);
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

void cNopacityView::DrawContent(std::string *text) {
    cTextWrapper wText;
    wText.Set(text->c_str(), font, width - 2 * border);
    int lineHeight = font->Height();
    int textLines = wText.Lines();
    int textHeight = lineHeight * textLines + 2 * border;
    int yText = border;

    CreateContent(textHeight);
    if (!pixmapContent)
        return;

    for (int i = 0; i < textLines; i++) {
        pixmapContent->DrawText(cPoint(border, yText), wText.GetLine(i), Theme.Color(clrMenuFontDetailViewText), clrTransparent, font);
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

void cNopacityView::DrawActors(std::vector<cActor> *actors) {
    int numActors = actors->size();
    if (numActors < 1) {
        CreateContent(100);
        if (pixmapContent)
            pixmapContent->DrawText(cPoint(border, border), tr("No Cast available"), Theme.Color(clrMenuFontDetailViewText), clrTransparent, fontHeaderLarge);
        return;
    }

    int thumbWidth = actors->at(0).actorThumb.width;
    int thumbHeight = actors->at(0).actorThumb.height;
    
    int picsPerLine = width / (thumbWidth + 2 * border);
    if (picsPerLine < 1)
        return;

    cImageLoader imgLoader;
    int actorsToView = 0;
    for (int a = 0; a < numActors; a++) {
        if (a == numActors)
            break;
        std::string path = actors->at(a).actorThumb.path;
        if (imgLoader.LoadPoster(path.c_str(), thumbWidth, thumbHeight, false)) {
            actorsToView++;
        }
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
            std::string name = actors->at(actor).name;
            std::stringstream sstrRole;
            sstrRole << "\"" << actors->at(actor).role << "\"";
            std::string role = sstrRole.str();
            if (imgLoader.LoadPoster(path.c_str(), thumbWidth, thumbHeight)) {
                pixmapContent->DrawImage(cPoint(x + border, y), imgLoader.GetImage());
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

void cNopacityView::ClearScrollbar(void) {
    PixmapFill(pixmapScrollbar, clrTransparent);
    PixmapFill(pixmapScrollbarBack, clrTransparent);
}

void cNopacityView::DrawScrollbar(void) {
    ClearScrollbar();
    if (!scrollable || !pixmapContent || !pixmapScrollbar || !pixmapScrollbarBack)
        return;

    int totalBarHeight = pixmapScrollbar->ViewPort().Height() - 6;
    
    int aktHeight = (-1)*pixmapContent->DrawPort().Point().Y();
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

bool cNopacityView::KeyUp(void) {
    if (!scrollable || !pixmapContent)
        return false;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
//    int lineHeight = font->Height();
    if (aktHeight >= 0) {
        return false;
    }
    int step = config.GetValue("detailedViewScrollStep") * font->Height();
    int newY = aktHeight + step;
    if (newY > 0)
        newY = 0;
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
}

bool cNopacityView::KeyDown(void) {
    if (!scrollable || !pixmapContent)
        return false;
    int aktHeight = pixmapContent->DrawPort().Point().Y();
    int totalHeight = pixmapContent->DrawPort().Height();
    int screenHeight = pixmapContent->ViewPort().Height();
    
    if (totalHeight - ((-1)*aktHeight) == screenHeight) {
        return false;
    } 
    int step = config.GetValue("detailedViewScrollStep") * font->Height();
    int newY = aktHeight - step;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    pixmapContent->SetDrawPortPoint(cPoint(0, newY));
    return true;
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

void cNopacityEPGView::DrawHeaderEPGImage(void) {
    cImageLoader imgLoader;
    if (eventID > 0) {
        if (!imgLoader.LoadEPGImage(eventID))
            return;
    } else if (recFileName.size() > 0) {
        if (!imgLoader.LoadRecordingImage(recFileName.c_str()))
            return;    
    } else
        return;

    if (!pixmapHeaderEPGImage) {
        if (!(pixmapHeaderEPGImage = CreatePixmap(osd, "pixmapHeaderEPGImage", 3, cRect(x, y, width, headerHeight)))) {
            return;
        }
    }

    int imgWidthOrig = config.GetValue("epgImageWidth");
    int imgHeightOrig =  config.GetValue("epgImageHeight");

    int imgWidth =  imgWidthOrig;
    int imgHeight = imgHeightOrig;

    if (imgHeight > (headerHeight-10)) {
        imgHeight = headerHeight - 10;
        imgWidth = imgWidthOrig * ((double)imgHeight / (double)imgHeightOrig);     
    }

    PixmapFill(pixmapHeaderEPGImage, clrTransparent);
    pixmapHeaderEPGImage->DrawImage(cPoint(width - imgWidth - border, (headerHeight - imgHeight)/2), imgLoader.GetImage());
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
        DrawHeader();
        DrawHeaderEPGImage();
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
    if (!pScraper || (config.GetValue("scraperInfo") == 0) || seriesId < 1)
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
        info << tr("TheMovieDB Rating") << ": " << series.episode.rating << "\n\n";
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
        info << tr("TheMovieDB Rating") << ": " << series.rating << "\n\n";
    }
    if (series.status.size() > 0) {
        info << tr("Status") << ": " << series.status << "\n\n";
    }
    tvdbInfo = info.str();
}

void cNopacitySeriesView::DrawHeaderBanner(void) {
    if (series.banners.size() == 0)
        return;
    if (series.banners[0].height == 0)
        return;
 
    int bannerWidthOrig = series.banners[0].width;
    int bannerHeightOrig = series.banners[0].height;
    std::string bannerPath = series.banners[0].path;

    if (bannerWidthOrig == 0)
        return;

    int bannerWidth = bannerWidthOrig;
    int bannerHeight = bannerHeightOrig;

    if (headerHeight < bannerHeightOrig) {
        bannerHeight = headerHeight;
        bannerWidth = bannerWidthOrig * ((double)bannerHeight / (double)bannerHeightOrig);
    }

    int bannerX = width - bannerWidth - border;
    int bannerY = (headerHeight - bannerHeight) / 2;

    if (!pixmapHeaderBanner) {
        if (!(pixmapHeaderBanner = CreatePixmap(osd, "pixmapHeaderBanner", 3, cRect(x, y, width, headerHeight)))){
            return;
        }
    }
    PixmapFill(pixmapHeaderBanner, clrTransparent);

    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(bannerPath.c_str(), bannerWidth, bannerHeight)) {
        pixmapHeaderBanner->DrawImage(cPoint(bannerX, bannerY), imgLoader.GetImage());
    }   
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
        DrawHeader();
        DrawHeaderBanner();
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
    if (!pScraper || (config.GetValue("scraperInfo") == 0) || movieId < 1)
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
    tabs.push_back(tr("TheTVDB Info"));
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

void cNopacityMovieView::DrawHeaderPoster(void) {
    if (movie.poster.width == 0 || movie.poster.height == 0 || movie.poster.path.size() == 0)
        return;
 
    int posterHeight = headerHeight - 10;
    int posterWidth = movie.poster.width * ((double)posterHeight / (double)movie.poster.height);;
    int posterX = width - posterWidth - border;
    int posterY = (headerHeight - posterHeight) / 2;

    if (!pixmapHeaderPoster) {
        if (!(pixmapHeaderPoster = CreatePixmap(osd, "pixmapHeaderPoster", 3, cRect(x, y, width, headerHeight)))) {
            return;
        }
    }
    PixmapFill(pixmapHeaderPoster, clrTransparent);

    cImageLoader imgLoader;
    if (imgLoader.LoadPoster(movie.poster.path.c_str(), posterWidth, posterHeight)) {
        pixmapHeaderPoster->DrawImage(cPoint(posterX, posterY), imgLoader.GetImage());
    }   
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
        DrawHeader();
        DrawHeaderPoster();
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
    DrawContent(&infoText);
    DrawScrollbar();
}

/********************************************************************************************
* cNopacityDetailViewLight
********************************************************************************************/
cNopacityMenuDetailViewLight::cNopacityMenuDetailViewLight(cOsd *osd, cPixmap *s, cPixmap *sBack) : cNopacityView(osd) {
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
    contentHeight = height - headerHeight;
    widthPoster = 30 * width / 100;
}

cNopacityMenuDetailViewLight::~cNopacityMenuDetailViewLight(void) {
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
