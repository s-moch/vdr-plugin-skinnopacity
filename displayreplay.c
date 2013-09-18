#include "displayreplay.h"

cNopacityDisplayReplay::cNopacityDisplayReplay(bool ModeOnly) {
    config.setDynamicValues();
    initial = true;
    modeOnly = ModeOnly;
    lastDate = "";
    FrameTime = config.replayFrameTime;
    FadeTime = config.replayFadeTime;

    SetGeometry();
    CreatePixmaps();
    DrawBackground();
    CreateFonts();
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
    delete fontReplayHeader;
    delete fontReplay;
    delete osd;
}

void cNopacityDisplayReplay::SetGeometry(void) {
    height = cOsd::OsdHeight() * config.replayHeight / 100;
    width = cOsd::OsdWidth() - 2 * config.replayBorderVertical;
    int top = cOsd::OsdTop() + cOsd::OsdHeight() - height - config.replayBorderBottom;
    osd = CreateOsd(cOsd::OsdLeft() + config.replayBorderVertical, top, width, height);
    headerHeight = height * 0.2;
    if (headerHeight%2 != 0)
        headerHeight++;
    footerHeight = headerHeight;
    resolutionX = 10;
    resolutionY = 5;
    info2Height = max(headerHeight,config.resolutionIconSize+resolutionY*2-headerHeight);
    progressBarHeight = 0.1 * height;
    if (progressBarHeight%2 != 0)
        progressBarHeight++;
    currentHeight = progressBarHeight + config.fontReplay;
    controlsHeight = height - headerHeight - info2Height - footerHeight - progressBarHeight;
    if (controlsHeight < 11)
        controlsHeight = 11;
    infoWidth = 0.75 * width - config.resolutionIconSize - 10;
    dateWidth = width - infoWidth;

    jumpX = (width - 4 * controlsHeight)/2 + 5*controlsHeight;
    jumpY = headerHeight + info2Height + progressBarHeight;
    jumpWidth = width - jumpX;
    jumpHeight = controlsHeight;
}

void cNopacityDisplayReplay::CreatePixmaps(void) {
    if (!modeOnly) {
        pixmapHeader = osd->CreatePixmap(1, cRect(0, 0, width, headerHeight));
        pixmapBackground = osd->CreatePixmap(1, cRect(0, headerHeight, width, info2Height + progressBarHeight + controlsHeight));
        pixmapFooter = osd->CreatePixmap(1, cRect(0, headerHeight + info2Height + progressBarHeight + controlsHeight, width, footerHeight));
        pixmapInfo = osd->CreatePixmap(2, cRect(config.resolutionIconSize + 10, 0, infoWidth, headerHeight));
        pixmapDate = osd->CreatePixmap(2, cRect(infoWidth, 0, dateWidth, headerHeight));
        pixmapInfo2 = osd->CreatePixmap(2, cRect(config.resolutionIconSize + 10, headerHeight, infoWidth, info2Height));
        pixmapProgressBar = osd->CreatePixmap(2, cRect(0, headerHeight + info2Height, width, progressBarHeight));
        pixmapCurrent = osd->CreatePixmap(3, cRect(0, headerHeight + info2Height + progressBarHeight, width/5, currentHeight));
        pixmapTotal = osd->CreatePixmap(3, cRect(4*width/5, headerHeight + info2Height + progressBarHeight, width/5, currentHeight));
        pixmapScreenResolution = osd->CreatePixmap(5, cRect(resolutionX, resolutionY, config.resolutionIconSize, config.resolutionIconSize));
        pixmapJump = osd->CreatePixmap(4, cRect(jumpX, jumpY, jumpWidth, jumpHeight));
    }
    
    int controlY = headerHeight + info2Height + progressBarHeight;
    iconBorder = 5;
    iconSize = min(controlsHeight - 2*iconBorder, 128);
    int backgroundWidth = 2 * iconBorder + iconSize;

    if (!modeOnly) {
        pixmapControls = osd->CreatePixmap(2, cRect(0, controlY, width, controlsHeight));
    } else {
        pixmapControls = osd->CreatePixmap(2, cRect( (width - (5 * backgroundWidth))/2, controlY - 10, 5 * backgroundWidth, controlsHeight + 20));
    }
    pixmapRew = osd->CreatePixmap(4, cRect((width - 4 * backgroundWidth)/2 + iconBorder, controlY + iconBorder, iconSize, iconSize));   
    pixmapPause = osd->CreatePixmap(4, cRect((width - 4 * backgroundWidth)/2 + (iconSize + 2*iconBorder) + iconBorder, controlY + iconBorder, iconSize, iconSize)); 
    pixmapPlay = osd->CreatePixmap(4, cRect((width - 4 * backgroundWidth)/2 + 2*(iconSize + 2*iconBorder) + iconBorder, controlY + iconBorder, iconSize, iconSize));    
    pixmapFwd = osd->CreatePixmap(4, cRect((width - 4 * backgroundWidth)/2 + 3*(iconSize + 2*iconBorder) + iconBorder, controlY + iconBorder, iconSize, iconSize)); 
    
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

void cNopacityDisplayReplay::CreateFonts(void) {
    fontReplayHeader = cFont::CreateFont(config.fontName, headerHeight - 8 + config.fontReplayHeader);
    fontReplay = cFont::CreateFont(config.fontName, currentHeight);
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
            int cornerTopSize = headerHeight/2;
            int cornerBottomSize = footerHeight/2;
            if ((cornerTopSize > 2)&&(cornerBottomSize > 2)) {
                pixmapHeader->DrawEllipse(cRect(0,0, cornerTopSize, cornerTopSize), clrTransparent, -2);
                pixmapHeader->DrawEllipse(cRect(width - cornerTopSize, 0, cornerTopSize, cornerTopSize), clrTransparent, -1);
                pixmapFooter->DrawEllipse(cRect(0, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -3);
                pixmapFooter->DrawEllipse(cRect(width - cornerBottomSize, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -4);
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
    
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("skinIcons/rewInactive", iconSize)) {
        pixmapRew->DrawImage(cPoint(0,0), imgLoader.GetImage());
    }
    if (imgLoader.LoadIcon("skinIcons/pauseInactive", iconSize)) {
        pixmapPause->DrawImage(cPoint(0,0), imgLoader.GetImage());
    }
    if (imgLoader.LoadIcon("skinIcons/playInactive", iconSize)) {
        pixmapPlay->DrawImage(cPoint(0,0), imgLoader.GetImage());
    }
    if (imgLoader.LoadIcon("skinIcons/fwdInactive", iconSize)) {
        pixmapFwd->DrawImage(cPoint(0,0), imgLoader.GetImage());
    }
}


void cNopacityDisplayReplay::DrawDate(void) {
    cString curDate = DayDateTime();
    if (initial || strcmp(curDate, lastDate)) {
        int strDateWidth = fontReplay->Width(curDate);
        int strDateHeight = fontReplay->Height();
        int x = dateWidth - strDateWidth - headerHeight/2;
        int y = (headerHeight - strDateHeight) / 2;
        pixmapDate->Fill(clrTransparent);
        pixmapDate->DrawText(cPoint(x, y), curDate, Theme.Color(clrReplayHead), clrTransparent, fontReplay);
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
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon(*iconName, config.resolutionIconSize)) {
        pixmapScreenResolution->DrawImage(cPoint(0,0), imgLoader.GetImage());
    }
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
    pixmapInfo2->DrawText(cPoint(headerHeight/2, max( (info2Height - fontReplay->Height())/2 - 10,0 )), *info2, Theme.Color(clrReplayDescription), clrTransparent, fontReplay);
    DrawScreenResolution();
}

void cNopacityDisplayReplay::SetTitle(const char *Title) {
    pixmapInfo->Fill(clrTransparent);
    pixmapInfo->DrawText(cPoint(headerHeight/2, 0), Title, Theme.Color(clrReplayHead), clrTransparent, fontReplayHeader);
}

void cNopacityDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
    LoadControlIcons();
    cImageLoader imgLoader;
    if (Speed == -1) {
        if (Play) {
            pixmapPlay->Fill(clrTransparent);
            if (imgLoader.LoadIcon("skinIcons/play", iconSize)) {
                pixmapPlay->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
        } else {
            pixmapPause->Fill(clrTransparent);
            if (imgLoader.LoadIcon("skinIcons/pause", iconSize)) {
                pixmapPause->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
        }
    } else if (Forward) {
        if (!Play) {
            pixmapPause->Fill(clrTransparent);
            if (imgLoader.LoadIcon("skinIcons/pause", iconSize)) {
                pixmapPause->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
        }
        pixmapFwd->Fill(clrTransparent);
        if (Speed > 0) {
            cString trickIcon = cString::sprintf("skinIcons/fwdx%d", Speed);
            if (imgLoader.LoadIcon(*trickIcon, iconSize)) {
                pixmapFwd->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
        } else if (imgLoader.LoadIcon("skinIcons/fwd", iconSize)) {
            pixmapFwd->DrawImage(cPoint(0,0), imgLoader.GetImage());
        }
    } else {
        if (!Play) {
            pixmapPause->Fill(clrTransparent);
            if (imgLoader.LoadIcon("skinIcons/pause", iconSize)) {
                pixmapPause->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
        }
        pixmapRew->Fill(clrTransparent);
        if (Speed > 0) {
            cString trickIcon = cString::sprintf("skinIcons/rewx%d", Speed);
            if (imgLoader.LoadIcon(*trickIcon, iconSize)) {
                pixmapRew->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
        } else if (imgLoader.LoadIcon("skinIcons/rew", iconSize)) {
            pixmapRew->DrawImage(cPoint(0,0), imgLoader.GetImage());
        }
    }
}

void cNopacityDisplayReplay::SetProgress(int Current, int Total) {
    if (progressBarHeight < 5)
        return;
    int barWidth = width - 2*progressBarHeight;
    cProgressBar pb(barWidth, progressBarHeight-2, Current, Total, marks, Theme.Color(clrReplayProgressSeen), Theme.Color(clrReplayProgressRest), Theme.Color(clrReplayProgressSelected), Theme.Color(clrReplayProgressMark), Theme.Color(clrReplayProgressCurrent));
    pixmapProgressBar->DrawEllipse(cRect(progressBarHeight/2, 0, progressBarHeight, progressBarHeight), Theme.Color(clrProgressBarBack));
    pixmapProgressBar->DrawEllipse(cRect(barWidth + progressBarHeight/2, 0, progressBarHeight, progressBarHeight), Theme.Color(clrReplayProgressRest));
    pixmapProgressBar->DrawRectangle(cRect( progressBarHeight, 0, barWidth, progressBarHeight), Theme.Color(clrProgressBarBack));
    pixmapProgressBar->DrawEllipse(cRect(progressBarHeight/2+1, 1, progressBarHeight-1, progressBarHeight-2), Theme.Color(clrReplayProgressSeen));
    pixmapProgressBar->DrawBitmap(cPoint(progressBarHeight, 1), pb);
}

void cNopacityDisplayReplay::SetCurrent(const char *Current) {
    pixmapCurrent->Fill(clrTransparent);
    pixmapCurrent->DrawText(cPoint(headerHeight/2, 0), Current, Theme.Color(clrReplayCurrentTotal), clrTransparent, fontReplay);
}

void cNopacityDisplayReplay::SetTotal(const char *Total) {
    pixmapTotal->Fill(clrTransparent);
    pixmapTotal->DrawText(cPoint(width/5 - (fontReplay->Width(Total) + headerHeight/2), 0), Total, Theme.Color(clrReplayCurrentTotal), clrTransparent, fontReplay);
}

void cNopacityDisplayReplay::SetJump(const char *Jump) {
    pixmapJump->Fill(clrTransparent);
    if (Jump) {
        pixmapJump->DrawText(cPoint(0, (jumpHeight - fontReplayHeader->Height())/2), Jump, Theme.Color(clrReplayCurrentTotal), clrTransparent, fontReplayHeader);
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
