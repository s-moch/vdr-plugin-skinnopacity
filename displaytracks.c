
#include "displaytracks.h"

cNopacityDisplayTracks::cNopacityDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) {
    config.setDynamicValues();
    initial = true;
    currentIndex = -1;
    numTracks = NumTracks;
    FrameTime = config.tracksFrameTime; 
    FadeTime = config.tracksFadeTime;
    SetGeometry();
    CreatePixmaps();
    CreateFonts();
    CreateBackgroundImages();
    DrawHeader(Title);
    for (int i = 0; i < NumTracks; i++)
        SetItem(Tracks[i], i, false);
}

cNopacityDisplayTracks::~cNopacityDisplayTracks() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmapContainer);
    osd->DestroyPixmap(pixmapHeader);
    menuItems.Clear();
    for (int i=0; i<2; i++)
        cOsdProvider::DropImage(handleBackgrounds[i]);
    delete font;
    delete fontHeader;
    delete osd;
}

cBitmap cNopacityDisplayTracks::bmStereo(audio_xpm);
cBitmap cNopacityDisplayTracks::bmDolbyDigital(dolbydigital_xpm);

void cNopacityDisplayTracks::SetGeometry(void) {
    width = cOsd::OsdWidth() * config.tracksWidth / 100;
    height = cOsd::OsdHeight() * config.tracksHeight / 100;
    if (!height%(numTracks + 1)) {
        height += numTracks + 1 - height%(numTracks + 1);
    }
    menuItemWidth = width - 4;
    menuItemHeight = (height - 2*(numTracks + 1)) / (numTracks + 1) - 1;

    int top, left;
    switch(config.tracksPosition) {
        case 0:     //middle bottom
            top = cOsd::OsdHeight() - cOsd::OsdTop() - height - config.tracksBorderHorizontal;
            left = (cOsd::OsdWidth() - width) / 2;
            break;
        case 1:     //left bottom
            top = cOsd::OsdHeight() - cOsd::OsdTop() - height - config.tracksBorderHorizontal;
            left = cOsd::OsdLeft();
            break;
        case 2:     //left middle
            top = (cOsd::OsdHeight() - height) / 2;
            left = cOsd::OsdLeft() + config.tracksBorderVertical;
            break;
        case 3:     //left top
            top = cOsd::OsdTop() + config.tracksBorderHorizontal;
            left = cOsd::OsdLeft() + config.tracksBorderVertical;
            break;
        case 4:     //top middle
            top = cOsd::OsdTop() + config.tracksBorderHorizontal;
            left = (cOsd::OsdWidth() - width) / 2;
            break;
        case 5:     //top right
            top = cOsd::OsdTop() + config.tracksBorderHorizontal;
            left = cOsd::OsdWidth() - cOsd::OsdLeft() - width - config.tracksBorderVertical;
            break;
        case 6:     //right middle
            top = (cOsd::OsdHeight() - height) / 2;
            left = cOsd::OsdWidth() - cOsd::OsdLeft() - width - config.tracksBorderVertical;
            break;
        case 7:     //right bottom
            top = cOsd::OsdHeight() - cOsd::OsdTop() - height - config.tracksBorderHorizontal;
            left = cOsd::OsdWidth() - cOsd::OsdLeft() - width - config.tracksBorderVertical;
            break;
        default:    //middle bottom
            top = cOsd::OsdHeight() - cOsd::OsdTop() - height - config.tracksBorderHorizontal;
            left = (cOsd::OsdWidth() - width) / 2;
            break;
    }
    osd = CreateOsd(left, top, width, height);
}

void cNopacityDisplayTracks::CreatePixmaps(void) {
    pixmapContainer = osd->CreatePixmap(1, cRect(0, 0, width, height));
    pixmapHeader = osd->CreatePixmap(2, cRect(2, 2, menuItemWidth, menuItemHeight));
    if (config.tracksFadeTime) {
        pixmapContainer->SetAlpha(0);
        pixmapHeader->SetAlpha(0);
    }
}

void cNopacityDisplayTracks::CreateFonts(void) {
    font = cFont::CreateFont(config.fontName, menuItemHeight/3 + config.fontTracks);
    fontHeader = cFont::CreateFont(config.fontName, menuItemHeight/2 + config.fontTracksHeader);
}

void cNopacityDisplayTracks::CreateBackgroundImages(void) {
    cImageLoader imgLoader;
    imgLoader.DrawBackground(Theme.Color(clrMenuItem), Theme.Color(clrMenuItemBlend), menuItemWidth-2, menuItemHeight-2);
    handleBackgrounds[0] = cOsdProvider::StoreImage(imgLoader.GetImage());
    imgLoader.DrawBackground(Theme.Color(clrMenuItemHigh), Theme.Color(clrMenuItemHighBlend), menuItemWidth-2, menuItemHeight-2);
    handleBackgrounds[1] = cOsdProvider::StoreImage(imgLoader.GetImage());
}

void cNopacityDisplayTracks::DrawHeader(const char *Title) {
    pixmapContainer->Fill(Theme.Color(clrMenuBorder));
    pixmapContainer->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrMenuBack));

    pixmapHeader->Fill(Theme.Color(clrMenuItem));
    pixmapHeader->DrawImage(cPoint(1, 1), handleBackgrounds[0]);
    pixmapIcon = osd->CreatePixmap(3, cRect(2, 2, menuItemHeight-2, menuItemHeight-2));
    pixmapIcon->Fill(clrTransparent);
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("skinItems/tracks", menuItemHeight-6)) {
        pixmapIcon->DrawImage(cPoint(3, 3), imgLoader.GetImage());
    }
    pixmapHeader->DrawText(cPoint((width - fontHeader->Width(Title)) / 2, (menuItemHeight - fontHeader->Height()) / 2), Title, Theme.Color(clrTracksFontHead), clrTransparent, fontHeader);
}

void cNopacityDisplayTracks::SetItem(const char *Text, int Index, bool Current) {
    cNopacityMenuItem *item;
    item = new cNopacityTrackMenuItem(osd, Text);
    item->SetCurrent(Current);
    item->SetFont(font);
    item->SetGeometry(Index, menuItemHeight+5, 2, menuItemWidth, menuItemHeight, 5);
    item->CreatePixmap();
    item->SetBackgrounds(handleBackgrounds);
    menuItems.Add(item);
    item->Render();
}

void cNopacityDisplayTracks::SetTrack(int Index, const char * const *Tracks) {
    cNopacityMenuItem *item;
    if (currentIndex >= 0) {
        item = menuItems.Get(currentIndex);
        item->SetCurrent(false);
        item->Render();
    }
    item = menuItems.Get(Index);
    item->SetCurrent(true);
    item->Render();
    currentIndex = Index;
}

void cNopacityDisplayTracks::SetAudioChannel(int AudioChannel) {
    cBitmap *bm = NULL;
    switch (AudioChannel) {
        case -1: bm = &bmDolbyDigital; break;
        case 0: bm = &bmStereo; break;
        default: ;
    }
    if (bm)
        pixmapHeader->DrawBitmap(cPoint(width - bm->Width() - 10, (menuItemHeight - bm->Width()) / 2), *bm, Theme.Color(clrChannelSymbolOn), Theme.Color(clrMenuBack));
}

void cNopacityDisplayTracks::Flush(void) {
    if (initial)
        if (config.tracksFadeTime)
            Start();
    initial = false;
    osd->Flush();
}

void cNopacityDisplayTracks::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        pixmapContainer->SetAlpha(Alpha);
        pixmapHeader->SetAlpha(Alpha);
        for (cNopacityMenuItem *item = menuItems.First(); Running() && item; item = menuItems.Next(item)) {
            item->SetAlpha(Alpha);
        }
        if (Running())
            osd->Flush();
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
