#include "displaytracks.h"
#include "config.h"
#include "helpers.h"
#include "imageloader.h"


cNopacityDisplayTracks::cNopacityDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) : cThread("DisplayTracks") {
    initial = true;
    fadeout = false;
    currentIndex = -1;
    numTracks = NumTracks;
    audioChannelLast = -5;
    SetGeometry();
    CreatePixmaps();
    DrawHeader(Title);
    for (int i = 0; i < NumTracks; i++)
        SetItem(Tracks[i], i, false);
}

cNopacityDisplayTracks::~cNopacityDisplayTracks() {
    if (config.GetValue("animation") && config.GetValue("tracksFadeOutTime")) {
        fadeout = true;
        Start();
    }
    int count = 0;
    while (Active()) {
        cCondWait::SleepMs(10);
        count++;
        if (count > 150)
           Cancel(1);
    }
    osd->DestroyPixmap(pixmapContainer);
    osd->DestroyPixmap(pixmapHeader);
    osd->DestroyPixmap(pixmapHeaderAudio);
    if (pixmapIcon)
        osd->DestroyPixmap(pixmapIcon);
    menuItems.clear();
    delete osd;
}

void cNopacityDisplayTracks::SetGeometry(void) {
    width = geoManager->trackWidth;
    height = (config.GetValue("tracksItemHeight") +4) * (numTracks+1);

    menuItemWidth = geoManager->menuItemWidthTracks;
    menuItemHeight = geoManager->menuItemHeightTracks;

    int top, left;
    switch(config.GetValue("tracksPosition")) {
        case 0:     //middle bottom
            top = geoManager->osdTop  + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + (geoManager->osdWidth - width) / 2;
            break;
        case 1:     //left bottom
            top = geoManager->osdTop + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + config.GetValue("tracksBorderHorizontal");
            break;
        case 2:     //left middle
            top = geoManager->osdTop + (geoManager->osdHeight - height) / 2;
            left = geoManager->osdLeft + config.GetValue("tracksBorderHorizontal");
            break;
        case 3:     //left top
            top = geoManager->osdTop + config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + config.GetValue("tracksBorderHorizontal");
            break;
        case 4:     //top middle
            top = geoManager->osdTop + config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + (geoManager->osdWidth - width) / 2;
            break;
        case 5:     //top right
            top = geoManager->osdTop + config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + geoManager->osdWidth - width - config.GetValue("tracksBorderHorizontal");
            break;
        case 6:     //right middle
            top = geoManager->osdTop + (geoManager->osdHeight - height) / 2;
            left = geoManager->osdLeft + geoManager->osdWidth - width - config.GetValue("tracksBorderHorizontal");
            break;
        case 7:     //right bottom
            top = geoManager->osdTop + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + geoManager->osdWidth - width - config.GetValue("tracksBorderHorizontal");
            break;
        default:    //middle bottom
            top = geoManager->osdTop  + geoManager->osdHeight - height - config.GetValue("tracksBorderVertical");
            left = geoManager->osdLeft + (geoManager->osdWidth - width) / 2;
            break;
    }
    osd = CreateOsd(left, top, width, height);
}

void cNopacityDisplayTracks::CreatePixmaps(void) {
    pixmapContainer = osd->CreatePixmap(1, cRect(0, 0, width, height));
    pixmapHeader = osd->CreatePixmap(2, cRect(2, 2, menuItemWidth, menuItemHeight));
    pixmapHeaderAudio = osd->CreatePixmap(3, cRect(menuItemWidth - menuItemHeight, 2, menuItemHeight, menuItemHeight));
}

void cNopacityDisplayTracks::DrawHeader(const char *Title) {
    pixmapHeaderAudio->Fill(clrTransparent);
    pixmapContainer->Fill(Theme.Color(clrMenuBorder));
    pixmapContainer->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrMenuBack));
    if (config.GetValue("displayType") == dtBlending) {
        pixmapHeader->Fill(Theme.Color(clrMenuItem));
        cImage *back = imgCache->GetSkinElement(seTracks);
        if (back)
            pixmapHeader->DrawImage(cPoint(1, 1), *back);
    } else if (config.GetValue("displayType") == dtGraphical) {
        cImage *back = imgCache->GetSkinElement(seTracks);
        if (back)
            pixmapHeader->DrawImage(cPoint(0, 0), *back);
    } else {
        pixmapHeader->Fill(Theme.Color(clrMenuItem));
        pixmapHeader->DrawRectangle(cRect(1, 1, width-2, height-2), Theme.Color(clrAudioMenuHeader));
    }
    pixmapIcon = osd->CreatePixmap(3, cRect(2, 2, menuItemHeight-2, menuItemHeight-2));
    pixmapIcon->Fill(clrTransparent);

    cImage *imgTracks = imgCache->GetSkinIcon("skinIcons/tracks", menuItemHeight-6, menuItemHeight-6);
    if (imgTracks)
        pixmapIcon->DrawImage(cPoint(3,3), *imgTracks);
    pixmapHeader->DrawText(cPoint((width - fontManager->trackHeader->Width(Title)) / 2, (menuItemHeight - fontManager->trackHeader->Height()) / 2), Title, Theme.Color(clrTracksFontHead), clrTransparent, fontManager->trackHeader);
}

void cNopacityDisplayTracks::SetAlpha(int Alpha, bool Force) {
    if (config.GetValue("tracksFadeTime")) {
        pixmapContainer->SetAlpha(Alpha);
        pixmapHeader->SetAlpha(Alpha);
        pixmapHeaderAudio->SetAlpha(Alpha);
        pixmapIcon->SetAlpha(Alpha);
        for (auto i = menuItems.begin(); i != menuItems.end(); ++i) {
            if (*i && (Force || Running())) {
                cNopacityMenuItem *item = i->get();
                item->SetAlpha(Alpha);
            }
        }
    }
}

void cNopacityDisplayTracks::SetItem(const char *Text, int Index, bool Current) {
    cNopacityMenuItem *item;
    item = new cNopacityTrackMenuItem(osd, Text);
    if ((int)menuItems.size() <= Index)
        menuItems.resize(Index+4);
    menuItems[Index].reset(item);
    item->SetCurrent(Current);
    item->SetGeometry(Index, menuItemHeight+4, 2, menuItemWidth, menuItemHeight, 4);
    item->CreatePixmapBackground();
    item->CreatePixmapStatic();
    if (config.GetValue("displayType") == dtGraphical)
        item->CreatePixmapForeground();
    item->Render();
}

void cNopacityDisplayTracks::SetTrack(int Index, const char * const *Tracks) {
    cNopacityMenuItem *item;
    if (currentIndex >= 0) {
        item = menuItems[currentIndex].get();
        item->SetCurrent(false);
        item->Render();
    }
    item = menuItems[Index].get();
    item->SetCurrent(true);
    item->Render();
    currentIndex = Index;
}

void cNopacityDisplayTracks::SetAudioChannel(int AudioChannel) {
    if (AudioChannel != audioChannelLast) {
        audioChannelLast = AudioChannel;
        pixmapHeaderAudio->Fill(clrTransparent);
        cString icon("");
        switch (AudioChannel) {
            case -1:
                icon = "skinIcons/ac3";
                break;
            case 0:
                icon = "skinIcons/stereo";
                break;
            default:
                icon = "skinIcons/stereo";
                break;
        }
        cImage *imgIcon = imgCache->GetSkinIcon(*icon, menuItemHeight-2, menuItemHeight-2);
        if (imgIcon)
            pixmapHeaderAudio->DrawImage(cPoint(1,1), *imgIcon);

    }
}

void cNopacityDisplayTracks::Flush(void) {
    if (Running())
        return;
    if (initial && config.GetValue("animation") && config.GetValue("tracksFadeTime")) {
        SetAlpha(0, true);
        Start();
    }
    initial = false;
    osd->Flush();
}

void cNopacityDisplayTracks::Action(void) {
    int x = (fadeout) ? 255 : 0;
    int FadeTime = (fadeout) ? config.GetValue("replayFadeOutTime") : config.GetValue("replayFadeTime");
    int FrameTime = FadeTime / 10;
    uint64_t First = cTimeMs::Now();
    cPixmap::Lock();
    cPixmap::Unlock();
    uint64_t Start = cTimeMs::Now();
    dsyslog ("skinnopacity: First Lock(): %lims \n", Start - First);
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        double t = std::min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = std::abs(x - (int(t * ALPHA_OPAQUE)));
        cPixmap::Lock();
        SetAlpha(Alpha);
        if (Running()) {
            osd->Flush();
        }
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
