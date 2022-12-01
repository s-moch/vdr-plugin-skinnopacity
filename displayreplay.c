#include "displayreplay.h"
#include "config.h"
#include "helpers.h"
#include "imageloader.h"
#include "status.h"

cNopacityDisplayReplay::cNopacityDisplayReplay(bool ModeOnly) : cThread("DisplayReplay") {
    initial = true;
    fadeout = false;
    modeOnly = ModeOnly;
    lastDate = "";
    createOSD();
    CreatePixmaps();
    DrawBackground();
    LoadControlIcons();
    messageBox = NULL;
    volumeBox = NULL;
    lastVolume = statusMonitor->GetVolume();
    lastVolumeTime = time(NULL);
}

cNopacityDisplayReplay::~cNopacityDisplayReplay() {
    if (config.GetValue("animation") && config.GetValue("replayFadeOutTime")) {
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
    if (!modeOnly) {
        osd->DestroyPixmap(pixmapBackground);
        osd->DestroyPixmap(pixmapTop);
        osd->DestroyPixmap(pixmapInfo);
        osd->DestroyPixmap(pixmapDate);
        osd->DestroyPixmap(pixmapInfo2);
        osd->DestroyPixmap(pixmapProgressBar);
        osd->DestroyPixmap(pixmapCurrent);
        osd->DestroyPixmap(pixmapTotal);
        osd->DestroyPixmap(pixmapScreenResBackground);
        osd->DestroyPixmap(pixmapScreenRes);
        osd->DestroyPixmap(pixmapJump);
    }
    osd->DestroyPixmap(pixmapControls);
    osd->DestroyPixmap(pixmapRew);
    osd->DestroyPixmap(pixmapPause);
    osd->DestroyPixmap(pixmapPlay);
    osd->DestroyPixmap(pixmapFwd);
    delete messageBox;
    delete volumeBox;
    delete osd;
}

void cNopacityDisplayReplay::createOSD(void) {
    osd = CreateOsd(geoManager->replayOsdLeft, geoManager->replayOsdTop, geoManager->replayOsdWidth, geoManager->replayOsdHeight);
}

void cNopacityDisplayReplay::CreatePixmaps(void) {
    if (!modeOnly) {
        pixmapBackground = CreatePixmap(osd, "pixmapBackground", 1,
                                        cRect(0,
                                              0,
                                              geoManager->replayOsdWidth,
                                              geoManager->replayOsdHeight));

        pixmapTop = CreatePixmap(osd, "pixmapTop", 5,
                                        cRect(0,
                                              0,
                                              geoManager->replayOsdWidth,
                                              geoManager->replayOsdHeight));

        pixmapInfo = CreatePixmap(osd, "pixmapInfo", 2,
                                        cRect(0,
                                              0,
                                              geoManager->replayInfoWidth,
                                              geoManager->replayHeaderHeight));

        pixmapDate = CreatePixmap(osd, "pixmapDate", 2,
                                        cRect(geoManager->replayInfoWidth,
                                              0,
                                              geoManager->replayDateWidth,
                                              geoManager->replayHeaderHeight));

        pixmapInfo2 = CreatePixmap(osd, "pixmapInfo2", 2,
                                        cRect(0,
                                              geoManager->replayHeaderHeight,
                                              geoManager->replayInfoWidth,
                                              geoManager->replayInfo2Height));

        pixmapProgressBar = CreatePixmap(osd, "pixmapProgressBar", 2,
                                        cRect(0,
                                              geoManager->replayHeaderHeight + geoManager->replayInfo2Height,
                                              geoManager->replayOsdWidth,
                                              geoManager->replayProgressBarHeight));

        int y = geoManager->replayHeaderHeight + geoManager->replayInfo2Height + geoManager->replayProgressBarHeight;
        pixmapCurrent = CreatePixmap(osd, "pixmapCurrent", 3,
                                        cRect(0,
                                              y,
                                              geoManager->replayOsdWidth / 5,
                                              geoManager->replayCurrentHeight));

        pixmapTotal = CreatePixmap(osd, "pixmapTotal", 3,
                                        cRect(4 * geoManager->replayOsdWidth / 5,
                                              y,
                                              geoManager->replayOsdWidth / 5,
                                              geoManager->replayCurrentHeight));

        pixmapScreenResBackground = CreatePixmap(osd, "pixmapScreenResBackground", 3,
                                        cRect(geoManager->replayResolutionX - 10,
                                              geoManager->replayResolutionY - 5,
                                              geoManager->replayResolutionSize * 3 + 20,
                                              geoManager->replayResolutionSize + 10));

        pixmapScreenRes = CreatePixmap(osd, "pixmapScreenRes", 4,
                                        cRect(geoManager->replayResolutionX,
                                              geoManager->replayResolutionY,
                                              geoManager->replayResolutionSize * 3,
                                              geoManager->replayResolutionSize));

        pixmapJump = CreatePixmap(osd, "pixmapJump", 4,
                                        cRect(geoManager->replayJumpX,
                                              geoManager->replayJumpY,
                                              geoManager->replayJumpWidth,
                                              geoManager->replayJumpHeight));
    }

    int controlY = geoManager->replayHeaderHeight
                    + geoManager->replayInfo2Height
                    + geoManager->replayProgressBarHeight;
    int iconBorder = geoManager->replayIconBorder;
    int iconSize = geoManager->replayIconSize;
    int iconWidth = 2 * iconBorder + iconSize;

    if (!modeOnly) {
        pixmapControls = CreatePixmap(osd, "pixmapControls", 2,
                                        cRect(0,
                                              controlY,
                                              geoManager->replayOsdWidth,
                                              geoManager->replayControlsHeight));
    } else {
        pixmapControls = CreatePixmap(osd, "pixmapControls", 2,
                                       cRect((geoManager->replayOsdWidth - (5 * iconWidth)) / 2,
                                              controlY - 10,
                                              5 * iconWidth,
                                              geoManager->replayControlsHeight + 20));
    }

    int iconX = (geoManager->replayOsdWidth - (4 * iconSize + 3 * iconBorder)) / 2;
    int iconY = controlY + iconBorder;
    pixmapRew = CreatePixmap(osd, "pixmapRew", 4, cRect(iconX, iconY, iconSize, iconSize));

    iconX = iconX + iconSize + iconBorder;
    pixmapPause = CreatePixmap(osd, "pixmapPause", 4, cRect(iconX, iconY, iconSize, iconSize));

    iconX = iconX + iconSize + iconBorder;
    pixmapPlay = CreatePixmap(osd, "pixmapPlay", 4, cRect(iconX, iconY, iconSize, iconSize));

    iconX = iconX + iconSize + iconBorder;
    pixmapFwd = CreatePixmap(osd, "pixmapFwd", 4, cRect(iconX, iconY, iconSize, iconSize));

    if (!modeOnly) {
        int alphaBack = (100 - config.GetValue("channelBackgroundTransparency"))*255/100;
        PixmapSetAlpha(pixmapBackground, alphaBack);
    }
}

void cNopacityDisplayReplay::SetAlpha(int Alpha) {
    if (!modeOnly) {
        int alphaBack = (100 - config.GetValue("channelBackgroundTransparency"))*Alpha/100;
        PixmapSetAlpha(pixmapBackground, alphaBack);
        PixmapSetAlpha(pixmapTop, Alpha);
        PixmapSetAlpha(pixmapInfo, Alpha);
        PixmapSetAlpha(pixmapInfo2, Alpha);
        PixmapSetAlpha(pixmapDate, Alpha);
        PixmapSetAlpha(pixmapProgressBar, Alpha);
        PixmapSetAlpha(pixmapCurrent, Alpha);
        PixmapSetAlpha(pixmapTotal, Alpha);
        PixmapSetAlpha(pixmapScreenResBackground, Alpha);
        PixmapSetAlpha(pixmapScreenRes, Alpha);
        PixmapSetAlpha(pixmapJump, Alpha);
    }
    PixmapSetAlpha(pixmapControls, Alpha);
    PixmapSetAlpha(pixmapRew, Alpha);
    PixmapSetAlpha(pixmapPause, Alpha);
    PixmapSetAlpha(pixmapPlay, Alpha);
    PixmapSetAlpha(pixmapFwd, Alpha);
    if (volumeBox) volumeBox->SetAlpha(Alpha);
}

void cNopacityDisplayReplay::DrawBackground(void) {
    if (!modeOnly) {
        if (config.GetValue("displayType") == dtGraphical) {
            cImage *imgBack = imgCache->GetSkinElement(seReplayBackground);
            if (pixmapBackground && imgBack)
                pixmapBackground->DrawImage(cPoint(0,0), *imgBack);
            cImage *imgTop = imgCache->GetSkinElement(seReplayTop);
            if (pixmapTop && imgTop)
                pixmapTop->DrawImage(cPoint(0,0), *imgTop);
            else
	        PixmapFill(pixmapTop, clrTransparent);
        } else {
            PixmapFill(pixmapBackground, Theme.Color(clrReplayBackground));
            PixmapFill(pixmapTop, clrTransparent);
            if (config.GetValue("displayType") == dtBlending &&
                   (Theme.Color(clrReplayBackground) != Theme.Color(clrReplayBackBlend))) {
                DrawBlendedBackground(pixmapBackground,
                                      0,
                                      geoManager->replayOsdWidth,
                                      Theme.Color(clrReplayBackground),
                                      Theme.Color(clrReplayBackBlend),
                                      true);
                DrawBlendedBackground(pixmapBackground,
                                      0,
                                      geoManager->replayOsdWidth,
                                      Theme.Color(clrReplayBackground),
                                      Theme.Color(clrReplayBackBlend),
                                      false);
            }
            if (config.GetValue("roundedCornersChannel")) {
                int cornerRadius = geoManager->replayHeaderHeight/2;
                if (cornerRadius > 2) {
                    DrawRoundedCorners(pixmapBackground,
                                       cornerRadius,
                                       0,
                                       0,
                                       geoManager->replayOsdWidth,
                                       geoManager->replayOsdHeight);
                }
            }
        }
        PixmapFill(pixmapControls, clrTransparent);
        PixmapFill(pixmapProgressBar, clrTransparent);
        PixmapFill(pixmapScreenResBackground, clrTransparent);
        PixmapFill(pixmapScreenRes, clrTransparent);
        PixmapFill(pixmapJump, clrTransparent);
    } else {
        PixmapFill(pixmapControls, Theme.Color(clrMenuBorder));
	if (pixmapControls)
            pixmapControls->DrawRectangle(cRect(2, 2, pixmapControls->ViewPort().Width() - 4, pixmapControls->ViewPort().Height() - 4),Theme.Color(clrReplayBackground));
    }
}

void cNopacityDisplayReplay::LoadControlIcons(void) {
    PixmapFill(pixmapRew, clrTransparent);
    PixmapFill(pixmapPause, clrTransparent);
    PixmapFill(pixmapPlay, clrTransparent);
    PixmapFill(pixmapFwd, clrTransparent);

    int iconSize = geoManager->replayIconSize;
    cImage *imgRew = imgCache->GetSkinIcon("skinIcons/rewInactive", iconSize, iconSize);
    if (pixmapRew && imgRew)
        pixmapRew->DrawImage(cPoint(0,0), *imgRew);
    cImage *imgPause = imgCache->GetSkinIcon("skinIcons/pauseInactive", iconSize, iconSize);
    if (pixmapPause && imgPause)
        pixmapPause->DrawImage(cPoint(0,0), *imgPause);
    cImage *imgPlay = imgCache->GetSkinIcon("skinIcons/playInactive", iconSize, iconSize);
    if (pixmapPlay && imgPlay)
        pixmapPlay->DrawImage(cPoint(0,0), *imgPlay);
    cImage *imgFwd = imgCache->GetSkinIcon("skinIcons/fwdInactive", iconSize, iconSize);
    if (pixmapFwd && imgFwd)
        pixmapFwd->DrawImage(cPoint(0,0), *imgFwd);
}


void cNopacityDisplayReplay::DrawDate(void) {
    if (!pixmapDate)
        return;

    cString curDate = DayDateTime();
    if (initial || strcmp(curDate, lastDate)) {
        int strDateWidth = fontManager->replayText->Width(curDate);
        int strDateHeight = fontManager->replayText->Height();
        int x = geoManager->replayDateWidth - strDateWidth - geoManager->replayHeaderHeight/2;
        int y = (geoManager->replayHeaderHeight - strDateHeight) / 2;
        PixmapFill(pixmapDate, clrTransparent);
        pixmapDate->DrawText(cPoint(x, y),
                             curDate,
                             Theme.Color(clrReplayHead),
                             clrTransparent,
                             fontManager->replayText);
        lastDate = curDate;
    }
}

void cNopacityDisplayReplay::DrawScreenResolution(void) {
    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;
    cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);
    cString iconName = GetScreenResolutionIcon();
    if (strcasecmp(iconName, "") == 0)
       return;
    int replaySize = geoManager->replayResolutionSize;
    cImage *imgRes = imgCache->GetSkinIcon(*iconName, 3 * replaySize, replaySize);
    PixmapFill(pixmapScreenResBackground, Theme.Color(clrStatusIconsBack));
    if (pixmapScreenResBackground) {
        DrawRoundedCorners(pixmapScreenResBackground,
                           5,
                           0,
                           0,
                           pixmapScreenResBackground->ViewPort().Width(),
                           pixmapScreenResBackground->ViewPort().Height());
    }
    if (pixmapScreenRes && imgRes)
        pixmapScreenRes->DrawImage(cPoint(0,0), *imgRes);
}

void cNopacityDisplayReplay::SetRecording(const cRecording *Recording) {
    const cRecordingInfo *RecordingInfo = Recording->Info();
    const char *recName = RecordingInfo->Title();
    if (!recName) {
        recName = Recording->Name();
    }
    SetTitle(recName);
    cString info2;
    if (RecordingInfo->ShortText())
        info2 = cString::sprintf("%s - %s %s", RecordingInfo->ShortText(), *ShortDateString(Recording->Start()), *TimeString(Recording->Start()));
    else
        info2 = cString::sprintf("%s %s", *ShortDateString(Recording->Start()), *TimeString(Recording->Start()));

    PixmapFill(pixmapInfo2, clrTransparent);
    if (pixmapInfo2) {
        pixmapInfo2->DrawText(cPoint(geoManager->replayHeaderHeight / 2,
                                     std::max((geoManager->replayInfo2Height
                                               - fontManager->replayText->Height()) / 2 - 10,
                                               0)),
                              *info2,
                              Theme.Color(clrReplayDescription),
                              clrTransparent,
                              fontManager->replayText);
    }
    DrawScreenResolution();
}

void cNopacityDisplayReplay::SetTitle(const char *Title) {
    PixmapFill(pixmapInfo, clrTransparent);
    PixmapFill(pixmapInfo2, clrTransparent);
    if (!pixmapInfo || !Title)
        return;

    int titleLength = fontManager->replayHeader->Width(Title);
    int titleSpace = geoManager->replayInfoWidth - geoManager->replayHeaderHeight/2;
    std::string strTitle = Title;
    if (titleLength > titleSpace)
        strTitle = CutText(strTitle, titleSpace, fontManager->replayHeader);
    pixmapInfo->DrawText(cPoint(geoManager->replayHeaderHeight/2, 0),
                         strTitle.c_str(),
                         Theme.Color(clrReplayHead),
                         clrTransparent,
                         fontManager->replayHeader);
}

void cNopacityDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
    LoadControlIcons();
    int iconSize = geoManager->replayIconSize;
    if (Speed == -1) {
        if (Play) {
            PixmapFill(pixmapPlay, clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/play", iconSize, iconSize);
            if (pixmapPlay && imgIcon)
                pixmapPlay->DrawImage(cPoint(0,0), *imgIcon);
        } else {
            PixmapFill(pixmapPause, clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/pause", iconSize, iconSize);
            if (pixmapPause && imgIcon)
                pixmapPause->DrawImage(cPoint(0,0), *imgIcon);
        }
    } else if (Forward) {
        if (!Play) {
            PixmapFill(pixmapPause, clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/pause", iconSize, iconSize);
            if (pixmapPause && imgIcon)
                pixmapPause->DrawImage(cPoint(0,0), *imgIcon);
        }
        PixmapFill(pixmapFwd, clrTransparent);
        if (Speed > 0) {
            cString trickIcon = cString::sprintf("skinIcons/fwdx%d", Speed);
            cImage *imgIcon = imgCache->GetSkinIcon(*trickIcon, iconSize, iconSize);
            if (pixmapFwd && imgIcon)
                pixmapFwd->DrawImage(cPoint(0,0), *imgIcon);
        } else {
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/fwd", iconSize, iconSize);
            if (pixmapFwd && imgIcon)
                pixmapFwd->DrawImage(cPoint(0,0), *imgIcon);
        }
    } else {
        if (!Play) {
            PixmapFill(pixmapPause, clrTransparent);
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/pause", iconSize, iconSize);
            if (pixmapPause && imgIcon)
                pixmapPause->DrawImage(cPoint(0,0), *imgIcon);
        }
        PixmapFill(pixmapRew, clrTransparent);
        if (Speed > 0) {
            cString trickIcon = cString::sprintf("skinIcons/rewx%d", Speed);
            cImage *imgIcon = imgCache->GetSkinIcon(*trickIcon, iconSize, iconSize);
            if (pixmapRew && imgIcon)
                pixmapRew->DrawImage(cPoint(0,0), *imgIcon);
        } else {
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/rew", iconSize, iconSize);
            if (pixmapRew && imgIcon)
                pixmapRew->DrawImage(cPoint(0,0), *imgIcon);
        }
    }
}

void cNopacityDisplayReplay::SetProgress(int Current, int Total) {
    if (Running() || !pixmapProgressBar || geoManager->replayProgressBarHeight < 5)
        return;

    int barWidth = geoManager->replayOsdWidth - 2*geoManager->replayProgressBarHeight;
    cProgressBar pb(barWidth,
                    geoManager->replayProgressBarHeight-2,
                    Current,
                    Total,
                    marks,
                    Theme.Color(clrReplayProgressSeen),
                    Theme.Color(clrReplayProgressRest),
                    Theme.Color(clrReplayProgressSelected),
                    Theme.Color(clrReplayProgressMark),
                    Theme.Color(clrReplayProgressCurrent));
    pixmapProgressBar->DrawEllipse(cRect(geoManager->replayProgressBarHeight/2,
                                         0,
                                         geoManager->replayProgressBarHeight,
                                         geoManager->replayProgressBarHeight),
                                   Theme.Color(clrProgressBarBack));
    pixmapProgressBar->DrawEllipse(cRect(barWidth + geoManager->replayProgressBarHeight/2,
                                         0,
                                         geoManager->replayProgressBarHeight,
                                         geoManager->replayProgressBarHeight),
                                   Theme.Color(clrReplayProgressRest));
    pixmapProgressBar->DrawRectangle(cRect(geoManager->replayProgressBarHeight,
                                           0,
                                           barWidth,
                                           geoManager->replayProgressBarHeight),
                                     Theme.Color(clrProgressBarBack));
    pixmapProgressBar->DrawEllipse(cRect(geoManager->replayProgressBarHeight/2+1,
                                         1,
                                         geoManager->replayProgressBarHeight-1,
                                         geoManager->replayProgressBarHeight-2),
                                   Theme.Color(clrReplayProgressSeen));
    pixmapProgressBar->DrawBitmap(cPoint(geoManager->replayProgressBarHeight, 1), pb);
}

void cNopacityDisplayReplay::SetCurrent(const char *Current) {
    if (Running())
        return;

    PixmapFill(pixmapCurrent, clrTransparent);
    if (pixmapCurrent && Current) {
        pixmapCurrent->DrawText(cPoint(geoManager->replayHeaderHeight/2, 0),
                                Current,
                                Theme.Color(clrReplayCurrentTotal),
                                clrTransparent,
                                fontManager->replayText);
    }
}

void cNopacityDisplayReplay::SetTotal(const char *Total) {
    PixmapFill(pixmapTotal, clrTransparent);
    if (pixmapTotal && Total) {
        pixmapTotal->DrawText(cPoint(geoManager->replayOsdWidth/5
                                     - (fontManager->replayText->Width(Total)
                                     + geoManager->replayHeaderHeight/2),
                                     0),
                              Total,
                              Theme.Color(clrReplayCurrentTotal),
                              clrTransparent,
                              fontManager->replayText);
    }
}

void cNopacityDisplayReplay::SetJump(const char *Jump) {
    PixmapFill(pixmapJump, clrTransparent);
    if (pixmapJump && Jump) {
        pixmapJump->DrawText(cPoint(0,
                                    (geoManager->replayJumpHeight
                                    - fontManager->replayHeader->Height())/2),
                             Jump,
                             Theme.Color(clrReplayCurrentTotal),
                             clrTransparent,
                             fontManager->replayHeader);
    }
}

void cNopacityDisplayReplay::SetMessage(eMessageType Type, const char *Text) {
    DELETENULL(volumeBox);
    DELETENULL(messageBox);
    if (!Text)
        return;
    messageBox = new cNopacityMessageBox(osd,
					 cRect((geoManager->replayOsdWidth - geoManager->messageWidth) / 2,
					       geoManager->replayOsdHeight - geoManager->messageHeight - 20,
					       geoManager->messageWidth, geoManager->messageHeight),
					 Type, Text);
}

void cNopacityDisplayReplay::DrawVolume(void) {
    int volume = statusMonitor->GetVolume();
    if (volume != lastVolume) {
        if (!volumeBox) {
            bool simple = (config.GetValue("displayReplayVolume") == vbSimple) ? true : false;
            volumeBox = new cNopacityVolumeBox(osd,
                                               cRect(geoManager->replayVolumeLeft,
                                                     geoManager->replayVolumeTop,
                                                     geoManager->replayVolumeWidth,
                                                     geoManager->replayVolumeHeight),
                                               simple ? fontManager->channelSourceInfo : fontManager->volumeText,
                                               simple);
        }
        volumeBox->SetVolume(volume, MAXVOLUME, volume ? false : true);
        lastVolumeTime = time(NULL);
        lastVolume = volume;
    }
    else {
        if (volumeBox && (time(NULL) - lastVolumeTime > 2))
            DELETENULL(volumeBox);
    }
}

void cNopacityDisplayReplay::Flush(void) {
    if (Running())
        return;

    if (!modeOnly) {
        DrawDate();
    }

    if (config.GetValue("displayReplayVolume"))
        DrawVolume();

    if (initial && config.GetValue("animation") && config.GetValue("replayFadeTime")) {
        SetAlpha(0);
        Start();
    }
    initial = false;
    osd->Flush();
}

void cNopacityDisplayReplay::Action(void) {
    int x = (fadeout) ? 255 : 0;
    int FadeTime = (fadeout) ? config.GetValue("replayFadeOutTime") : config.GetValue("replayFadeTime");
    int FrameTime = FadeTime / 10;
    uint64_t First = cTimeMs::Now();
    cPixmap::Lock();
    cPixmap::Unlock();
    uint64_t Start = cTimeMs::Now();
    dsyslog ("skinnopacity: First Lock(): %llums \n", (long long)(Start - First));
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        double t = std::min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = std::abs(x - (int(t * ALPHA_OPAQUE)));
        cPixmap::Lock();
        SetAlpha(Alpha);
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
