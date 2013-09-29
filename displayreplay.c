#include "displayreplay.h"

cNopacityDisplayReplay::cNopacityDisplayReplay(cImageCache *imgCache, bool ModeOnly) {
    this->imgCache = imgCache;
    config.setDynamicValues();
    initial = true;
    modeOnly = ModeOnly;
    lastDate = "";
    FrameTime = config.replayFrameTime;
    FadeTime = config.replayFadeTime;
    createOSD();
    CreatePixmaps();
    DrawBackground();
}

cNopacityDisplayReplay::~cNopacityDisplayReplay() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    if (!modeOnly) {
        osd->DestroyPixmap(pixmapHeader);
        osd->DestroyPixmap(pixmapBackground);
        osd->DestroyPixmap(pixmapInfo);
        osd->DestroyPixmap(pixmapDate);
        osd->DestroyPixmap(pixmapInfo2);
        osd->DestroyPixmap(pixmapProgressBar);
        osd->DestroyPixmap(pixmapCurrent);
        osd->DestroyPixmap(pixmapTotal);
        osd->DestroyPixmap(pixmapScreenResolution);
        osd->DestroyPixmap(pixmapJump);
        osd->DestroyPixmap(pixmapFooter);
    }
    osd->DestroyPixmap(pixmapControls);
    osd->DestroyPixmap(pixmapRew);
    osd->DestroyPixmap(pixmapPause);
    osd->DestroyPixmap(pixmapPlay);
    osd->DestroyPixmap(pixmapFwd);
    delete osd;
}

void cNopacityDisplayReplay::createOSD(void) {
    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->replayHeight - config.replayBorderBottom;
    osd = CreateOsd(geoManager->osdLeft + config.replayBorderVertical, top, geoManager->replayWidth, geoManager->replayHeight);
}

void cNopacityDisplayReplay::CreatePixmaps(void) {
    if (!modeOnly) {
        pixmapHeader = osd->CreatePixmap(1, cRect(0, 0, geoManager->replayWidth, geoManager->replayHeaderHeight));
        pixmapBackground = osd->CreatePixmap(1, cRect(0, geoManager->replayHeaderHeight, geoManager->replayWidth, geoManager->replayInfo2Height + geoManager->replayProgressBarHeight + geoManager->replayControlsHeight));
        pixmapFooter = osd->CreatePixmap(1, cRect(0, geoManager->replayHeaderHeight + geoManager->replayInfo2Height + geoManager->replayProgressBarHeight + geoManager->replayControlsHeight, geoManager->replayWidth, geoManager->replayFooterHeight));
        pixmapInfo = osd->CreatePixmap(2, cRect(config.resolutionIconSize + 10, 0, geoManager->replayInfoWidth, geoManager->replayHeaderHeight));
        pixmapDate = osd->CreatePixmap(2, cRect(geoManager->replayInfoWidth, 0, geoManager->replayDateWidth, geoManager->replayHeaderHeight));
        pixmapInfo2 = osd->CreatePixmap(2, cRect(config.resolutionIconSize + 10, geoManager->replayHeaderHeight, geoManager->replayInfoWidth, geoManager->replayInfo2Height));
        pixmapProgressBar = osd->CreatePixmap(2, cRect(0, geoManager->replayHeaderHeight + geoManager->replayInfo2Height, geoManager->replayWidth, geoManager->replayProgressBarHeight));
        pixmapCurrent = osd->CreatePixmap(3, cRect(0, geoManager->replayHeaderHeight + geoManager->replayInfo2Height + geoManager->replayProgressBarHeight, geoManager->replayWidth/5, geoManager->replayCurrentHeight));
        pixmapTotal = osd->CreatePixmap(3, cRect(4*geoManager->replayWidth/5, geoManager->replayHeaderHeight + geoManager->replayInfo2Height + geoManager->replayProgressBarHeight, geoManager->replayWidth/5, geoManager->replayCurrentHeight));
        pixmapScreenResolution = osd->CreatePixmap(5, cRect(geoManager->replayResolutionX, geoManager->replayResolutionY, config.resolutionIconSize, config.resolutionIconSize));
        pixmapJump = osd->CreatePixmap(4, cRect(geoManager->replayJumpX, geoManager->replayJumpY, geoManager->replayJumpWidth, geoManager->replayJumpHeight));
    }
    
    int controlY = geoManager->replayHeaderHeight + geoManager->replayInfo2Height + geoManager->replayProgressBarHeight;
    int iconBorder = geoManager->replayIconBorder;
    int iconSize = geoManager->replayIconSize;
    int backgroundWidth = 2 * iconBorder + iconSize;

    if (!modeOnly) {
        pixmapControls = osd->CreatePixmap(2, cRect(0, controlY, geoManager->replayWidth, geoManager->replayControlsHeight));
    } else {
        pixmapControls = osd->CreatePixmap(2, cRect( (geoManager->replayWidth - (5 * backgroundWidth))/2, controlY - 10, 5 * backgroundWidth, geoManager->replayControlsHeight + 20));
    }
    pixmapRew = osd->CreatePixmap(4, cRect((geoManager->replayWidth - 4 * backgroundWidth)/2 + iconBorder, controlY + iconBorder, iconSize, iconSize));   
    pixmapPause = osd->CreatePixmap(4, cRect((geoManager->replayWidth - 4 * backgroundWidth)/2 + (iconSize + 2*iconBorder) + iconBorder, controlY + iconBorder, iconSize, iconSize)); 
    pixmapPlay = osd->CreatePixmap(4, cRect((geoManager->replayWidth - 4 * backgroundWidth)/2 + 2*(iconSize + 2*iconBorder) + iconBorder, controlY + iconBorder, iconSize, iconSize));    
    pixmapFwd = osd->CreatePixmap(4, cRect((geoManager->replayWidth - 4 * backgroundWidth)/2 + 3*(iconSize + 2*iconBorder) + iconBorder, controlY + iconBorder, iconSize, iconSize)); 
    
    if (config.replayFadeTime) {
        if (!modeOnly) {
            pixmapHeader->SetAlpha(0);
            pixmapBackground->SetAlpha(0);
            pixmapInfo->SetAlpha(0);
            pixmapDate->SetAlpha(0);
            pixmapInfo2->SetAlpha(0);
            pixmapProgressBar->SetAlpha(0);
            pixmapCurrent->SetAlpha(0);
            pixmapTotal->SetAlpha(0);
            pixmapScreenResolution->SetAlpha(0);
            pixmapJump->SetAlpha(0);
            pixmapFooter->SetAlpha(0);
        }
        pixmapControls->SetAlpha(0);
        pixmapRew->SetAlpha(0); 
        pixmapPause->SetAlpha(0);   
        pixmapPlay->SetAlpha(0);    
        pixmapFwd->SetAlpha(0);
    }
}

void cNopacityDisplayReplay::DrawBackground(void) {
    if (!modeOnly) {
        if (config.doBlending && (Theme.Color(clrReplayBackground) != Theme.Color(clrReplayBackBlend))) {
            DrawBlendedBackground(pixmapHeader, Theme.Color(clrReplayBackground), Theme.Color(clrReplayBackBlend), true);
            DrawBlendedBackground(pixmapFooter, Theme.Color(clrReplayBackground), Theme.Color(clrReplayBackBlend), false);
        } else {
            pixmapHeader->Fill(Theme.Color(clrReplayBackground));
            pixmapFooter->Fill(Theme.Color(clrReplayBackground));
        }
        if (config.roundedCornersChannel) {
            int cornerTopSize = geoManager->replayHeaderHeight/2;
            int cornerBottomSize = geoManager->replayFooterHeight/2;
            if ((cornerTopSize > 2)&&(cornerBottomSize > 2)) {
                pixmapHeader->DrawEllipse(cRect(0,0, cornerTopSize, cornerTopSize), clrTransparent, -2);
                pixmapHeader->DrawEllipse(cRect(geoManager->replayWidth - cornerTopSize, 0, cornerTopSize, cornerTopSize), clrTransparent, -1);
                pixmapFooter->DrawEllipse(cRect(0, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -3);
                pixmapFooter->DrawEllipse(cRect(geoManager->replayWidth - cornerBottomSize, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -4);
            }
        }
        pixmapBackground->Fill(Theme.Color(clrReplayBackground));
        pixmapControls->Fill(clrTransparent);
        pixmapProgressBar->Fill(clrTransparent);
        pixmapScreenResolution->Fill(clrTransparent);
        pixmapJump->Fill(clrTransparent);
    } else {
        pixmapControls->Fill(Theme.Color(clrMenuBorder));
        pixmapControls->DrawRectangle(cRect(2, 2, pixmapControls->ViewPort().Width() - 4, pixmapControls->ViewPort().Height() - 4),Theme.Color(clrReplayBackground));
    }
}

void cNopacityDisplayReplay::LoadControlIcons(void) {
    pixmapRew->Fill(clrTransparent);
    pixmapPause->Fill(clrTransparent);
    pixmapPlay->Fill(clrTransparent);
    pixmapFwd->Fill(clrTransparent);
    int iconSize = geoManager->replayIconSize;
    cImage *imgRew = imgCache->GetSkinIcon("skinIcons/rewInactive", iconSize, iconSize);
    if (imgRew)
        pixmapRew->DrawImage(cPoint(0,0), *imgRew);
    cImage *imgPause = imgCache->GetSkinIcon("skinIcons/pauseInactive", iconSize, iconSize);
    if (imgPause)
        pixmapPause->DrawImage(cPoint(0,0), *imgPause);
    cImage *imgPlay = imgCache->GetSkinIcon("skinIcons/playInactive", iconSize, iconSize);
    if (imgPlay)
        pixmapPlay->DrawImage(cPoint(0,0), *imgPlay);
    cImage *imgFwd = imgCache->GetSkinIcon("skinIcons/fwdInactive", iconSize, iconSize);
    if (imgFwd)
        pixmapFwd->DrawImage(cPoint(0,0), *imgFwd);
}


void cNopacityDisplayReplay::DrawDate(void) {
    cString curDate = DayDateTime();
    if (initial || strcmp(curDate, lastDate)) {
        int strDateWidth = fontManager->replayText->Width(curDate);
        int strDateHeight = fontManager->replayText->Height();
        int x = geoManager->replayDateWidth - strDateWidth - geoManager->replayHeaderHeight/2;
        int y = (geoManager->replayHeaderHeight - strDateHeight) / 2;
        pixmapDate->Fill(clrTransparent);
        pixmapDate->DrawText(cPoint(x, y), curDate, Theme.Color(clrReplayHead), clrTransparent, fontManager->replayText);
        lastDate = curDate;
    }
}

void cNopacityDisplayReplay::DrawScreenResolution(void) {
    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;
    cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);
    cString iconName("");
    switch (screenWidth) {
        case 1920:
        case 1440:
            iconName = "skinIcons/hd1080i";
            break;
        case 1280:
            if (screenHeight == 720)
                iconName = "skinIcons/hd720p";
            else
                iconName = "skinIcons/hd1080i";
            break;
        case 720:
            iconName = "skinIcons/sd576i";
            break;
        default:
            iconName = "skinIcons/sd576i";
            break;
    }
    cImage *imgRes = imgCache->GetSkinIcon(*iconName, config.resolutionIconSize, config.resolutionIconSize);
    if (imgRes)
        pixmapScreenResolution->DrawImage(cPoint(0,0), *imgRes);
}

void cNopacityDisplayReplay::SetRecording(const cRecording *Recording) {
    const cRecordingInfo *RecordingInfo = Recording->Info();
    SetTitle(RecordingInfo->Title());
    cString info2;
    if (RecordingInfo->ShortText())
        info2 = cString::sprintf("%s - %s %s", RecordingInfo->ShortText(), *ShortDateString(Recording->Start()), *TimeString(Recording->Start()));
    else
        info2 = cString::sprintf("%s %s", *ShortDateString(Recording->Start()), *TimeString(Recording->Start()));
    
    pixmapInfo2->Fill(clrTransparent);
    pixmapInfo2->DrawText(cPoint(geoManager->replayHeaderHeight/2, max( (geoManager->replayInfo2Height - fontManager->replayText->Height())/2 - 10,0 )), *info2, Theme.Color(clrReplayDescription), clrTransparent, fontManager->replayText);
    DrawScreenResolution();
}

void cNopacityDisplayReplay::SetTitle(const char *Title) {
    pixmapInfo->Fill(clrTransparent);
    pixmapInfo->DrawText(cPoint(geoManager->replayHeaderHeight/2, 0), Title, Theme.Color(clrReplayHead), clrTransparent, fontManager->replayHeader);
}

void cNopacityDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
    LoadControlIcons();
    int iconSize = geoManager->replayIconSize;
    if (Speed == -1) {
        if (Play) {
            pixmapPlay->Fill(clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/play", iconSize, iconSize);
            if (imgIcon)
                pixmapPlay->DrawImage(cPoint(0,0), *imgIcon);
        } else {
            pixmapPause->Fill(clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/pause", iconSize, iconSize);
            if (imgIcon)
                pixmapPause->DrawImage(cPoint(0,0), *imgIcon);
        }
    } else if (Forward) {
        if (!Play) {
            pixmapPause->Fill(clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/pause", iconSize, iconSize);
            if (imgIcon)
                pixmapPause->DrawImage(cPoint(0,0), *imgIcon);
        }
        pixmapFwd->Fill(clrTransparent);
        if (Speed > 0) {
            cString trickIcon = cString::sprintf("skinIcons/fwdx%d", Speed);
            cImage *imgIcon = imgCache->GetSkinIcon(*trickIcon, iconSize, iconSize);
            if (imgIcon)
                pixmapFwd->DrawImage(cPoint(0,0), *imgIcon);
        } else {
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/fwd", iconSize, iconSize);
            if (imgIcon)
                pixmapFwd->DrawImage(cPoint(0,0), *imgIcon);
        }
    } else {
        if (!Play) {
            pixmapPause->Fill(clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/pause", iconSize, iconSize);
            if (imgIcon)
                pixmapPause->DrawImage(cPoint(0,0), *imgIcon);
        }
        pixmapRew->Fill(clrTransparent);
        if (Speed > 0) {
            cString trickIcon = cString::sprintf("skinIcons/rewx%d", Speed);
            cImage *imgIcon = imgCache->GetSkinIcon(*trickIcon, iconSize, iconSize);
            if (imgIcon)
                pixmapRew->DrawImage(cPoint(0,0), *imgIcon);
        } else {
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/rew", iconSize, iconSize);
            if (imgIcon)
                pixmapRew->DrawImage(cPoint(0,0), *imgIcon);
        }
    }
}

void cNopacityDisplayReplay::SetProgress(int Current, int Total) {
    if (geoManager->replayProgressBarHeight < 5)
        return;
    int barWidth = geoManager->replayWidth - 2*geoManager->replayProgressBarHeight;
    cProgressBar pb(barWidth, geoManager->replayProgressBarHeight-2, Current, Total, marks, Theme.Color(clrReplayProgressSeen), Theme.Color(clrReplayProgressRest), Theme.Color(clrReplayProgressSelected), Theme.Color(clrReplayProgressMark), Theme.Color(clrReplayProgressCurrent));
    pixmapProgressBar->DrawEllipse(cRect(geoManager->replayProgressBarHeight/2, 0, geoManager->replayProgressBarHeight, geoManager->replayProgressBarHeight), Theme.Color(clrProgressBarBack));
    pixmapProgressBar->DrawEllipse(cRect(barWidth + geoManager->replayProgressBarHeight/2, 0, geoManager->replayProgressBarHeight, geoManager->replayProgressBarHeight), Theme.Color(clrReplayProgressRest));
    pixmapProgressBar->DrawRectangle(cRect( geoManager->replayProgressBarHeight, 0, barWidth, geoManager->replayProgressBarHeight), Theme.Color(clrProgressBarBack));
    pixmapProgressBar->DrawEllipse(cRect(geoManager->replayProgressBarHeight/2+1, 1, geoManager->replayProgressBarHeight-1, geoManager->replayProgressBarHeight-2), Theme.Color(clrReplayProgressSeen));
    pixmapProgressBar->DrawBitmap(cPoint(geoManager->replayProgressBarHeight, 1), pb);
}

void cNopacityDisplayReplay::SetCurrent(const char *Current) {
    pixmapCurrent->Fill(clrTransparent);
    pixmapCurrent->DrawText(cPoint(geoManager->replayHeaderHeight/2, 0), Current, Theme.Color(clrReplayCurrentTotal), clrTransparent, fontManager->replayText);
}

void cNopacityDisplayReplay::SetTotal(const char *Total) {
    pixmapTotal->Fill(clrTransparent);
    pixmapTotal->DrawText(cPoint(geoManager->replayWidth/5 - (fontManager->replayText->Width(Total) + geoManager->replayHeaderHeight/2), 0), Total, Theme.Color(clrReplayCurrentTotal), clrTransparent, fontManager->replayText);
}

void cNopacityDisplayReplay::SetJump(const char *Jump) {
    pixmapJump->Fill(clrTransparent);
    if (Jump) {
        pixmapJump->DrawText(cPoint(0, (geoManager->replayJumpHeight - fontManager->replayHeader->Height())/2), Jump, Theme.Color(clrReplayCurrentTotal), clrTransparent, fontManager->replayHeader);
    }
}

void cNopacityDisplayReplay::SetMessage(eMessageType Type, const char *Text) {
}

void cNopacityDisplayReplay::Flush(void) {
    if (!modeOnly) {
        DrawDate();
    }
    if (initial) {
        if (config.replayFadeTime)
            Start();
    }
    initial = false;
    osd->Flush();
}

void cNopacityDisplayReplay::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        if (!modeOnly) {
            pixmapHeader->SetAlpha(Alpha);
            pixmapBackground->SetAlpha(Alpha);
            pixmapInfo->SetAlpha(Alpha);
            pixmapDate->SetAlpha(Alpha);
            pixmapInfo2->SetAlpha(Alpha);
            pixmapProgressBar->SetAlpha(Alpha);
            pixmapCurrent->SetAlpha(Alpha);
            pixmapTotal->SetAlpha(Alpha);
            pixmapScreenResolution->SetAlpha(Alpha);
            pixmapJump->SetAlpha(Alpha);
            pixmapFooter->SetAlpha(Alpha);
        }
        pixmapControls->SetAlpha(Alpha);
        pixmapRew->SetAlpha(Alpha); 
        pixmapPause->SetAlpha(Alpha);   
        pixmapPlay->SetAlpha(Alpha);    
        pixmapFwd->SetAlpha(Alpha);
        cPixmap::Unlock();
        if (Running())
            osd->Flush();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
