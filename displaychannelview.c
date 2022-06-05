#include "services/scraper2vdr.h"
#include "displaychannelview.h"
#include "config.h"
#include "helpers.h"
#include "fontmanager.h"
#include "geometrymanager.h"
#include "imageloader.h"
#include "status.h"
#include <vdr/menu.h>

cNopacityDisplayChannelView::cNopacityDisplayChannelView(cOsd *osd) {
    this->osd = osd;
    lastDate = "";
    isRadioChannel = false;
    statusIconBorder = geoManager->channelStatusIconBorder;
    statusIconSize = geoManager->channelStatusIconSize;
    signalWidth = 0;
    signalHeight = 0;
    signalX = 0;
    lastSignalDisplay = 0;
    lastSignalStrength = 0;
    lastSignalQuality = 0;
    pixmapSignalMeter = NULL;
    pixmapSignalStrength = NULL;
    pixmapSignalQuality = NULL;
    pixmapSignalLabel = NULL;
    pixmapPoster = NULL;
    messageBox = NULL;
    volumeBox = NULL;
    lastVolume = statusMonitor->GetVolume();
    lastVolumeTime = time(NULL);
    CreatePixmaps();
    DrawBackground();
    if (config.GetValue("displaySignalStrength")) {
        DrawSignalMeter();
    }
}

cNopacityDisplayChannelView::~cNopacityDisplayChannelView(void) {
    osd->DestroyPixmap(pixmapBackground);
    osd->DestroyPixmap(pixmapTop);
    osd->DestroyPixmap(pixmapLogo);
    osd->DestroyPixmap(pixmapLogoBackground);
    osd->DestroyPixmap(pixmapChannelName);
    osd->DestroyPixmap(pixmapDate);
    osd->DestroyPixmap(pixmapProgressBar);
    osd->DestroyPixmap(pixmapEPGInfo);
    osd->DestroyPixmap(pixmapStatusIcons);
    osd->DestroyPixmap(pixmapStatusIconsBackground);
    osd->DestroyPixmap(pixmapSourceInfo);
    if (pixmapSignalStrength)
        osd->DestroyPixmap(pixmapSignalStrength);
    if (pixmapSignalQuality)
        osd->DestroyPixmap(pixmapSignalQuality);
    if (pixmapSignalMeter)
        osd->DestroyPixmap(pixmapSignalMeter);
    if (pixmapSignalLabel)
        osd->DestroyPixmap(pixmapSignalLabel);
    if (pixmapPoster)
        osd->DestroyPixmap(pixmapPoster);
    delete messageBox;
    delete volumeBox;
}

void cNopacityDisplayChannelView::CreatePixmaps(void) {
    pixmapBackground = osd->CreatePixmap(1,
                              cRect(geoManager->channelX,
                                    geoManager->channelTop,
                                    geoManager->channelWidth,
                                    geoManager->channelHeight)
                            );
    pixmapTop = osd->CreatePixmap(5,
                              cRect(geoManager->channelX,
                                    geoManager->channelTop,
                                    geoManager->channelWidth,
                                    geoManager->channelHeight)
                            );
    pixmapLogo  = osd->CreatePixmap(3,
                              cRect(geoManager->channelX + geoManager->channelLogoX,
                                    geoManager->channelLogoY,
                                    geoManager->channelLogoWidth,
                                    geoManager->channelLogoHeight)
                            );
    pixmapLogoBackground  = osd->CreatePixmap(2,
                              cRect(geoManager->channelX + geoManager->channelLogoBgX,
                                    geoManager->channelLogoBgY,
                                    geoManager->channelLogoBgWidth,
                                    geoManager->channelLogoBgHeight)
                            );
    pixmapChannelName = osd->CreatePixmap(2,
                              cRect(geoManager->channelX + geoManager->channelContentX,
                                    geoManager->channelTop,
                                    geoManager->channelChannelNameWidth,
                                    geoManager->channelHeaderHeight)
                            );
    pixmapDate  = osd->CreatePixmap(2,
                              cRect(geoManager->channelX + geoManager->channelContentX +
                                    geoManager->channelChannelNameWidth,
                                    geoManager->channelTop,
                                    geoManager->channelDateWidth,
                                    geoManager->channelHeaderHeight)
                            );
    pixmapProgressBar  = osd->CreatePixmap(2,
                              cRect(geoManager->channelX + geoManager->channelContentX,
                                    geoManager->channelTop + geoManager->channelHeaderHeight,
                                    geoManager->channelContentWidth,
                                    geoManager->channelProgressBarHeight)
                            );
    pixmapEPGInfo  = osd->CreatePixmap(2,
                              cRect(geoManager->channelX + geoManager->channelContentX,
                                    geoManager->channelTop + geoManager->channelHeaderHeight +
                                    geoManager->channelProgressBarHeight,
                                    geoManager->channelContentWidth,
                                    geoManager->channelEpgInfoHeight)
                            );
    pixmapStatusIcons  = osd->CreatePixmap(3,
                              cRect(geoManager->channelStatusIconX,
                                    geoManager->channelTop + geoManager->channelHeaderHeight +
                                    geoManager->channelProgressBarHeight +
                                    geoManager->channelEpgInfoHeight,
                                    geoManager->channelStatusIconsWidth,
                                    geoManager->channelFooterHeight)
                            );
    pixmapStatusIconsBackground = osd->CreatePixmap(2,
                              cRect(geoManager->channelStatusIconX - 2 * statusIconBorder,
                                    geoManager->channelTop + geoManager->channelHeaderHeight +
                                    geoManager->channelProgressBarHeight +
                                    geoManager->channelEpgInfoHeight + 1,
                                    geoManager->channelStatusIconsWidth + 3 * geoManager->channelStatusIconBorder,
                                    geoManager->channelFooterHeight - 2)
                            );
    pixmapSourceInfo  = osd->CreatePixmap(2,
                              cRect(geoManager->channelSourceInfoX,
                                    geoManager->channelTop + geoManager->channelHeaderHeight +
                                    geoManager->channelProgressBarHeight +
                                    geoManager->channelEpgInfoHeight,
                                    geoManager->channelStatusIconX - geoManager->channelSourceInfoX,
                                    geoManager->channelFooterHeight)
                            );

    int alphaBack = (100 - config.GetValue("channelBackgroundTransparency"))*255/100;
    pixmapBackground->SetAlpha(alphaBack);
}

void cNopacityDisplayChannelView::SetAlpha(int alpha) {
    int alphaBack = (100 - config.GetValue("channelBackgroundTransparency"))*alpha/100;
    pixmapBackground->SetAlpha(alphaBack);
    pixmapTop->SetAlpha(alpha);
    pixmapLogo->SetAlpha(alpha);
    pixmapLogoBackground->SetAlpha(alpha);
    pixmapChannelName->SetAlpha(alpha);
    pixmapDate->SetAlpha(alpha);
    pixmapProgressBar->SetAlpha(alpha);
    pixmapEPGInfo->SetAlpha(alpha);
    pixmapStatusIcons->SetAlpha(alpha);
    pixmapStatusIconsBackground->SetAlpha(alpha);
    pixmapSourceInfo->SetAlpha(alpha);
    if (pixmapSignalStrength)
        pixmapSignalStrength->SetAlpha(alpha);
    if (pixmapSignalQuality)
        pixmapSignalQuality->SetAlpha(alpha);
    if (pixmapSignalMeter)
        pixmapSignalMeter->SetAlpha(alpha);
    if (pixmapSignalLabel)
        pixmapSignalLabel->SetAlpha(alpha);
    if (pixmapPoster)
        pixmapPoster->SetAlpha(alpha);
    if (volumeBox)
        volumeBox->SetAlpha(alpha);
}

void cNopacityDisplayChannelView::DrawBackground(void) {
    pixmapBackground->Fill(clrTransparent);
    pixmapTop->Fill(clrTransparent);
    if (config.GetValue("displayType") == dtGraphical) {
        cImage *imgBack = imgCache->GetSkinElement(seChannelBackground);
        if (imgBack)
            pixmapBackground->DrawImage(cPoint(0,0), *imgBack);
        cImage *imgTop = imgCache->GetSkinElement(seChannelTop);
        if (imgTop)
            pixmapTop->DrawImage(cPoint(0,0), *imgTop);
    } else {
        int backgroundX;
        int backgroundWidth;
        if (config.GetValue("backgroundStyle") == bsFull) {
            backgroundX = 0;
            backgroundWidth = geoManager->channelWidth;
        } else {
            backgroundX = geoManager->channelContentX;
            backgroundWidth = geoManager->channelContentWidth;
        }
        pixmapBackground->DrawRectangle(cRect(backgroundX,
                                              0,
                                              backgroundWidth,
                                              geoManager->channelHeight),
                                        Theme.Color(clrChannelBackground));
        if (config.GetValue("displayType") == dtBlending) {
            DrawBlendedBackground(pixmapBackground,
                                  backgroundX,
                                  backgroundWidth,
                                  Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend),
                                  true);
            DrawBlendedBackground(pixmapBackground,
                                  backgroundX,
                                  backgroundWidth,
                                  Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend),
                                  false);
        }
        if (config.GetValue("roundedCornersChannel")) {
            int cornerRadius = geoManager->channelHeaderHeight/2;
            if (cornerRadius > 2) {
                DrawRoundedCorners(pixmapBackground,
                                   cornerRadius,
                                   backgroundX,
                                   0,
                                   backgroundWidth,
                                   geoManager->channelHeight);
            }
        }
    }
}

void cNopacityDisplayChannelView::DrawChannelLogo(const cChannel *Channel) {
    cImage *imgLogo = imgCache->GetLogo(ctLogo, Channel);
    if (imgLogo) {
        pixmapLogo->DrawImage(cPoint((geoManager->channelLogoWidth - imgLogo->Width()) / 2, (geoManager->channelLogoHeight - imgLogo->Height()) / 2), *imgLogo);
    }

    if (config.GetValue("displayType") != dtFlat && ((config.GetValue("channelUseLogoBackground") == 1)
                                                 || ((config.GetValue("channelUseLogoBackground") == 2) && imgLogo)
                                                 || ((config.GetValue("channelUseLogoBackground") == 3) && !imgLogo))) {
        cImage *imgLogoBack = imgCache->GetSkinElement(seChannelLogoBack);
        if (imgLogoBack)
            pixmapLogoBackground->DrawImage(cPoint((geoManager->channelLogoBgWidth - imgLogoBack->Width()) / 2, (geoManager->channelLogoBgHeight - imgLogoBack->Height()) / 2), *imgLogoBack);
    }
}

void cNopacityDisplayChannelView::ClearChannelLogo(void) {
    pixmapLogoBackground->Fill(clrTransparent);
    pixmapLogo->Fill(clrTransparent);
}


void cNopacityDisplayChannelView::DrawChannelName(cString number, cString name) {
    cString channelString = cString::sprintf("%s %s", *number, *name);
    pixmapChannelName->DrawText(cPoint(geoManager->channelHeaderHeight/2,
                       (geoManager->channelHeaderHeight-fontManager->channelHeader->Height())/2),
                       channelString,
                       Theme.Color(clrChannelHead),
                       clrTransparent,
                       fontManager->channelHeader);
}

void cNopacityDisplayChannelView::ClearChannelName(void) {
    pixmapChannelName->Fill(clrTransparent);
}


void cNopacityDisplayChannelView::DrawDate(void) {
    cString curDate = DayDateTime();
    if (strcmp(curDate, lastDate)) {
        int strDateWidth = fontManager->channelDate->Width(curDate);
        int strDateHeight = fontManager->channelDate->Height();
        int x = geoManager->channelDateWidth - strDateWidth - geoManager->channelHeaderHeight/2;
        int y = (geoManager->channelHeaderHeight - strDateHeight) / 2;
        pixmapDate->Fill(clrTransparent);
        pixmapDate->DrawText(cPoint(x, y), curDate, Theme.Color(clrChannelHead), clrTransparent, fontManager->channelDate);
        lastDate = curDate;
    }
}

void cNopacityDisplayChannelView::DrawProgressBar(int Current, int Total) {
    int barHeight = pixmapProgressBar->ViewPort().Height() - 8;
    if (barHeight % 2 != 0)
        barHeight++;
    if (barHeight < 3)
        return;
    if (Current > Total)
        Current = Total;
    if ((Current > 0) || (Total > 0)) {
        int barWidth = pixmapProgressBar->ViewPort().Width() - 24;
        DrawProgressbar(pixmapProgressBar, 11, 4, barWidth, barHeight, Current, Total, Theme.Color(clrProgressBar), Theme.Color(clrProgressBarBlend), false);
    }
}

void cNopacityDisplayChannelView::DrawProgressbarBackground(void) {
    int barHeight = pixmapProgressBar->ViewPort().Height() - 6;
    if (barHeight % 2 != 0)
        barHeight++;
    if (barHeight < 3)
        return;
    int barWidth = pixmapProgressBar->ViewPort().Width() - 20;
    pixmapProgressBar->DrawEllipse(cRect(9, 3, barHeight, barHeight), Theme.Color(clrChannelProgressBarBack));
    pixmapProgressBar->DrawEllipse(cRect(9 + barWidth - barHeight, 3, barHeight, barHeight), Theme.Color(clrChannelProgressBarBack));
    pixmapProgressBar->DrawRectangle(cRect(9 + barHeight / 2, 3, barWidth - barHeight, barHeight), Theme.Color(clrChannelProgressBarBack));
}

void cNopacityDisplayChannelView::ClearProgressBar(void) {
    pixmapProgressBar->Fill(clrTransparent);
}

void cNopacityDisplayChannelView::DrawEvents(const cEvent *Present, const cEvent *Following) {
    for (int i = 0; i < 2; i++) {
       bool rec = false;
       bool present = !i ? true : false;
       const cEvent *e = !i ? Present : Following;
       if (e) {
          {
          LOCK_TIMERS_READ;
          eTimerMatch TimerMatch = tmNone;
          const cTimer *Timer = Timers->GetMatch(e, &TimerMatch);
          if (Timer && Timer->HasFlags(tfActive) && TimerMatch == tmFull)
             rec = true;
          }
          DrawEPGInfo(e, present, rec);
       }
    }
}

void cNopacityDisplayChannelView::DrawEPGInfo(const cEvent *e, bool present, bool recording) {
    int indent = 20;
    cString startTime = e->GetTimeString();
    cString strEPG = e->Title();
    cString strEPGShort = e->ShortText();
    cString strSeen("");
    if (present) {
        if (config.GetValue("progressCurrentSchedule") == 0) {
            int seen = (int)(time(NULL) - e->StartTime()) / 60;
            strSeen = cString::sprintf("%d/%dmin", seen, e->Duration() / 60);
        } else if (config.GetValue("progressCurrentSchedule") == 1) {
            int remaining = (int)(e->EndTime() - time(NULL)) / 60;
            strSeen = cString::sprintf("-%d/%dmin", remaining, e->Duration() / 60);
        }
    } else {
        strSeen = cString::sprintf("%dmin", e->Duration() / 60);
    }

    int startTimeWidth = fontManager->channelEPG->Width(*startTime);
    int epgWidth = fontManager->channelEPG->Width(*strEPG);
    int epgWidthShort = fontManager->channelEPGSmall->Width(*strEPGShort);
    int seenWidth = fontManager->channelEPG->Width(*strSeen);

    int widthRecIcon = 0;
    if (recording) {
        widthRecIcon = fontManager->channelEPGSmall->Width(" REC ") + indent / 2;
    }
    int spaceEPGText = geoManager->channelContentWidth - seenWidth
                       - startTimeWidth - 3 * indent - widthRecIcon
                       - fontManager->channelEPG->Width("...");
    int spaceEPGTextSmall = spaceEPGText + widthRecIcon;
    if (spaceEPGText < epgWidth) {
        strEPG = CutText(*strEPG, spaceEPGText, fontManager->channelEPG).c_str();
    }
    if (spaceEPGTextSmall < epgWidthShort) {
        strEPGShort = CutText(*strEPGShort, spaceEPGText, fontManager->channelEPGSmall).c_str();
    }

    int lineHeight = geoManager->channelEpgInfoLineHeight;
    int yEPG = (present) ? 0 : 2 * lineHeight;
    int yEPGSmall = (present) ? lineHeight : 3 * lineHeight;
    int xEPG = 2 * indent + startTimeWidth;

    tColor fontColor = (present) ? Theme.Color(clrChannelEPG)
                                 : Theme.Color(clrChannelEPGNext);
    tColor fontColorInfo = (present) ? Theme.Color(clrChannelEPGInfo)
                                     : Theme.Color(clrChannelEPGInfoNext);

    pixmapEPGInfo->DrawText(cPoint(indent, yEPG), *startTime, fontColor, clrTransparent, fontManager->channelEPG);

    pixmapEPGInfo->DrawText(cPoint(xEPG + widthRecIcon, yEPG), *strEPG, fontColor, clrTransparent, fontManager->channelEPG);

    pixmapEPGInfo->DrawText(cPoint(xEPG, yEPGSmall), *strEPGShort, fontColorInfo, clrTransparent, fontManager->channelEPGSmall);

    int x = geoManager->channelContentWidth - 2 * indent - seenWidth;

    pixmapEPGInfo->DrawText(cPoint(x, yEPG), *strSeen, fontColor, clrTransparent, fontManager->channelEPG);

    //Recording Icon
    if (recording) {
        tColor clrRecIcon = (present) ? Theme.Color(clrRecNow)
                                      : Theme.Color(clrRecNext);
        tColor clrRecIconText = (present) ? Theme.Color(clrRecNowFont)
                                          : Theme.Color(clrRecNextFont);
        pixmapEPGInfo->DrawRectangle(cRect(xEPG, yEPG, widthRecIcon - indent / 2, lineHeight), clrRecIcon);
        int xRecText = xEPG + (widthRecIcon - indent / 2 - fontManager->channelEPGSmall->Width("REC")) / 2;
        int yRecText = yEPG + (lineHeight - fontManager->channelEPGSmall->Height()) / 2;
        pixmapEPGInfo->DrawText(cPoint(xRecText, yRecText), "REC", clrRecIconText, clrRecIcon, fontManager->channelEPGSmall);
    }
}

void cNopacityDisplayChannelView::ClearEPGInfo(void) {
    pixmapEPGInfo->Fill(clrTransparent);
}

void cNopacityDisplayChannelView::DrawStatusIcons(const cChannel *Channel) {
    pixmapStatusIconsBackground->Fill(Theme.Color(clrStatusIconsBack));
    DrawRoundedCorners(pixmapStatusIconsBackground,
                       5,
                       0,
                       0,
                       pixmapStatusIconsBackground->ViewPort().Width(),
                       pixmapStatusIconsBackground->ViewPort().Height()
                      );

    isRadioChannel = ((!Channel->Vpid())&&(Channel->Apid(0)))?true:false;
    int iconX = 0;

    if (Channel->Vpid() && Channel->Tpid()) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/txton", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(0,statusIconBorder), *imgIcon);
    } else {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/txtoff", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(0,statusIconBorder), *imgIcon);
    }

    iconX += statusIconBorder + statusIconSize;

    if (Channel->Apid(0)) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/stereoon", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    } else {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/stereooff", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    }

    iconX += statusIconBorder + statusIconSize;

    if (Channel->Dpid(0)) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/dolbyon", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    } else {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/dolbyoff", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    }

    iconX += statusIconBorder + statusIconSize;

    if (Channel->Ca()) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/crypted", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    } else {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/fta", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    }

    iconX += statusIconBorder + statusIconSize;

    if (cRecordControls::Active()) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/recon", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    } else {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/recoff", statusIconSize, statusIconSize);
        if (imgIcon)
            pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgIcon);
    }

    iconX += statusIconBorder + statusIconSize;


}

void cNopacityDisplayChannelView::DrawScreenResolution(void) {
    cString resolutionIcon("");
    if (isRadioChannel) {
        resolutionIcon = "skinIcons/radio";
    } else {
        resolutionIcon = GetScreenResolutionIcon();
    }
    if (strcasecmp(resolutionIcon, "") == 0)
       return;
    int iconX = 5 * (statusIconBorder + statusIconSize);
    cImage *imgRes = imgCache->GetSkinIcon(*resolutionIcon, 3 * statusIconSize, statusIconSize);
    if (imgRes)
        pixmapStatusIcons->DrawImage(cPoint(iconX,statusIconBorder), *imgRes);

}

void cNopacityDisplayChannelView::ClearStatusIcons(void) {
    pixmapStatusIcons->Fill(clrTransparent);
    pixmapStatusIconsBackground->Fill(clrTransparent);
}

void cNopacityDisplayChannelView::DrawPoster(const cEvent *event, bool initial) {
    if (pixmapPoster) {
        osd->DestroyPixmap(pixmapPoster);
        pixmapPoster = NULL;
    }

    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper && (config.GetValue("scraperInfo") == 1)) {
        ScraperGetPosterBanner call;
        call.event = event;
        if (pScraper->Service("GetPosterBanner", &call)) {
            int mediaWidth = 0;
            int mediaHeight = 0;
            std::string mediaPath = "";
            if ((call.type == tSeries) && call.banner.path.size() > 0) {
                mediaWidth = call.banner.width;
                mediaHeight = call.banner.height;
                mediaPath = call.banner.path;
            } else if (call.type == tMovie && call.poster.path.size() > 0 && call.poster.height > 0) {
                double ratio = (double)(cOsd::OsdHeight()/3) / (double)call.poster.height;
                mediaWidth = ratio * call.poster.width;
                mediaHeight = ratio * call.poster.height;
                mediaPath = call.poster.path;
            } else
                return;
            int border = config.GetValue("channelPosterBorder");
            pixmapPoster = osd->CreatePixmap(1, cRect(0,
                                                      0,
                                                      mediaWidth + 2 * border,
                                                      mediaHeight + 2 * border));
            if (initial && config.GetValue("animation") && config.GetValue("channelFadeTime")) {
                pixmapPoster->SetAlpha(0);
            }
            cImageLoader imgLoader;
            if (imgLoader.LoadPoster(mediaPath.c_str(), mediaWidth, mediaHeight)) {
                pixmapPoster->Fill(Theme.Color(clrChannelBackground));
                pixmapPoster->DrawImage(cPoint(border, border), imgLoader.GetImage());
                DrawRoundedCorners(pixmapPoster, border, 0, 0, pixmapPoster->ViewPort().Width(), pixmapPoster->ViewPort().Height());
            } else {
                pixmapPoster->Fill(clrTransparent);
            }
        }
        return;
    }
}

void cNopacityDisplayChannelView::DrawSignalMeter(void) {
    signalWidth = geoManager->channelWidth * 0.15;
    signalHeight = signalWidth * 15 / 200;
    cFont *fontInfoline = cFont::CreateFont(config.fontName, signalHeight - 2);
    cString signalStrength = "STR";
    cString signalQuality = "SNR";
    cImage *imgSignal = imgCache->GetSkinIcon("skinIcons/signal", signalWidth, signalHeight, true);
    if (imgSignal) {
        signalWidth = imgSignal->Width();
        signalHeight = imgSignal->Height();
        int signalMeterY = geoManager->channelFooterY +
                           (geoManager->channelFooterHeight - 2 * signalHeight - 7) / 2; // 2 * (signalHeight + 2) + 3 + 2
        int labelWidth = std::max(fontInfoline->Width(*signalStrength),
                             fontInfoline->Width(*signalQuality)) + 2;
        signalX = geoManager->channelFooterHeight / 2 + labelWidth;
        pixmapSignalStrength = osd->CreatePixmap(3,
                                                 cRect(geoManager->channelContentX + 10 + signalX,
                                                       signalMeterY + 2,
                                                       signalWidth + 2,
                                                       signalHeight + 2));
        pixmapSignalQuality  = osd->CreatePixmap(3,
                                                 cRect(geoManager->channelContentX + 10 + signalX,
                                                       signalMeterY + signalHeight + 5,
                                                       signalWidth + 2,
                                                       signalHeight + 2));
        pixmapSignalMeter    = osd->CreatePixmap(4,
                                                 cRect(geoManager->channelContentX + 10 + signalX + 1,
                                                       signalMeterY + 3,
                                                       signalWidth,
                                                       2 * signalHeight + 3));
        pixmapSignalLabel    = osd->CreatePixmap(3,
                                                 cRect(geoManager->channelContentX + 10 + geoManager->channelFooterHeight / 2,
                                                       signalMeterY + 2,
                                                       labelWidth,
                                                       2 * signalHeight + 3));
        pixmapSignalStrength->Fill(Theme.Color(clrProgressBarBack));
        pixmapSignalQuality->Fill(Theme.Color(clrProgressBarBack));
        pixmapSignalMeter->Fill(clrTransparent);
        pixmapSignalLabel->Fill(clrTransparent);
        if (config.GetValue("animation") && config.GetValue("channelFadeTime")) {
            pixmapSignalStrength->SetAlpha(0);
            pixmapSignalQuality->SetAlpha(0);
            pixmapSignalMeter->SetAlpha(0);
            pixmapSignalLabel->SetAlpha(0);
        }
        pixmapSignalStrength->DrawImage(cPoint(1,1), *imgSignal);
        pixmapSignalQuality->DrawImage(cPoint(1,1), *imgSignal);
        pixmapSignalLabel->DrawText(cPoint(0, 2), *signalStrength, Theme.Color(clrChannelEPGInfo), clrTransparent, fontInfoline);
        pixmapSignalLabel->DrawText(cPoint(0, signalHeight + 4), *signalQuality, Theme.Color(clrChannelEPGInfo), clrTransparent, fontInfoline);
    }
    delete fontInfoline;
}

void cNopacityDisplayChannelView::DrawSignal(void) {
    if (!pixmapSignalMeter)
        return;
    time_t Now = time(NULL);
    if (Now != lastSignalDisplay) {
        int SignalStrength = cDevice::ActualDevice()->SignalStrength();
        int SignalQuality = cDevice::ActualDevice()->SignalQuality();
        if (SignalStrength < 0) SignalStrength = 0;
        if (SignalQuality < 0) SignalQuality = 0;
        if ((SignalStrength == 0)&&(SignalQuality==0))
            return;
        if ((lastSignalStrength != SignalStrength) || (lastSignalQuality != SignalQuality)) {
            pixmapSignalMeter->Fill(clrTransparent);
            pixmapSignalMeter->DrawRectangle(cRect(double(SignalStrength) /100 * signalWidth, 0, signalWidth - (double)SignalStrength /100 * signalWidth + 1, signalHeight), Theme.Color(clrChannelBackground));
            pixmapSignalMeter->DrawRectangle(cRect(double(SignalQuality) /100 * signalWidth, signalHeight + 3, signalWidth - (double)SignalQuality / 100 * signalWidth + 1, signalHeight + 1), Theme.Color(clrChannelBackground));
        }
        lastSignalStrength = SignalStrength;
        lastSignalQuality = SignalQuality;
        lastSignalDisplay = Now;
    }
}


void cNopacityDisplayChannelView::ShowSignalMeter(void) {
    if(pixmapSignalStrength)
        pixmapSignalStrength->SetLayer(3);
    if(pixmapSignalQuality)
        pixmapSignalQuality->SetLayer(3);
    if(pixmapSignalMeter)
        pixmapSignalMeter->SetLayer(4);
    if(pixmapSignalLabel)
        pixmapSignalLabel->SetLayer(3);
}

void cNopacityDisplayChannelView::HideSignalMeter(void) {
    if(pixmapSignalStrength)
        pixmapSignalStrength->SetLayer(-1);
    if(pixmapSignalQuality)
        pixmapSignalQuality->SetLayer(-1);
    if(pixmapSignalMeter)
        pixmapSignalMeter->SetLayer(-1);
    if(pixmapSignalLabel)
        pixmapSignalLabel->SetLayer(-1);
}

void cNopacityDisplayChannelView::DrawChannelGroups(const cChannel *Channel, cString ChannelName) {
    int xSep, ySep;
    int prevNextIconSize = 64;

    cImageLoader imgLoader;
    if (config.GetValue("logoPosition") != lpNone) {
        cString separator = cString::sprintf("separatorlogos/%s", *ChannelName);
        if (imgLoader.LoadLogo(*separator, geoManager->channelLogoWidth, geoManager->channelLogoHeight)) {
            pixmapLogo->DrawImage(cPoint(0, 0), imgLoader.GetImage());
        } else {
            cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/Channelseparator", geoManager->channelLogoWidth, geoManager->channelLogoHeight);
            if (imgIcon)
                pixmapLogo->DrawImage(cPoint(0, 0), *imgIcon);
        }
    }

    ySep = (geoManager->channelContentHeight-fontManager->channelChannelGroup->Height())/2 - fontManager->channelChannelGroup->Height()/2;
    int widthSep = fontManager->channelChannelGroup->Width(*ChannelName);

    if (!config.GetValue("displayPrevNextChannelGroup")) {
        xSep = (geoManager->channelContentWidth - widthSep)/2;
        pixmapEPGInfo->DrawText(cPoint(xSep, ySep), *ChannelName, Theme.Color(clrChannelHead), clrTransparent, fontManager->channelChannelGroup);
        return;
    }

    int spaceSep = 4 * geoManager->channelContentWidth / 10;
    int spaceNextPrev = (geoManager->channelContentWidth - spaceSep - 2*prevNextIconSize - 10) / 2;

    int xPrev = 0;
    int xPrevIcon = spaceNextPrev;
    xSep = xPrevIcon + prevNextIconSize;
    int xNextIcon = xSep + spaceSep;
    int xNext = xNextIcon + prevNextIconSize + 10;

    std::string strSep = *ChannelName;
    if (widthSep > spaceSep)
        strSep = CutText(strSep, spaceSep, fontManager->channelChannelGroup);
    widthSep = fontManager->channelChannelGroup->Width(strSep.c_str());
    pixmapEPGInfo->DrawText(cPoint(xSep + (spaceSep - widthSep)/2, ySep), strSep.c_str(), Theme.Color(clrChannelHead), clrTransparent, fontManager->channelChannelGroup);

    std::string prevChannelSep = GetChannelSep(Channel, true);
    std::string nextChannelSep = GetChannelSep(Channel, false);
    bool prevAvailable = (prevChannelSep.size() > 0)?true:false;
    bool nextAvailable = (nextChannelSep.size() > 0)?true:false;

    int ySepNextPrevIcon = (geoManager->channelContentHeight - prevNextIconSize)/2 - fontManager->channelChannelGroup->Height()/2;

    int ySepNextPrev = ySep + (fontManager->channelChannelGroup->Height() - fontManager->channelChannelGroupSmall->Height())/2;
    if (prevAvailable) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/arrowLeftChannelSep", prevNextIconSize, prevNextIconSize);
        if (imgIcon)
            pixmapEPGInfo->DrawImage(cPoint(xPrevIcon, ySepNextPrevIcon), *imgIcon);
        if (fontManager->channelChannelGroupSmall->Width(prevChannelSep.c_str()) > spaceNextPrev)
            prevChannelSep = CutText(prevChannelSep, spaceNextPrev, fontManager->channelChannelGroupSmall);
        pixmapEPGInfo->DrawText(cPoint(xPrev, ySepNextPrev), prevChannelSep.c_str(), Theme.Color(clrChannelEPGInfoNext), clrTransparent, fontManager->channelChannelGroupSmall);
    }
    if (nextAvailable) {
        cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/arrowRightChannelSep", prevNextIconSize, prevNextIconSize);
        if (imgIcon)
            pixmapEPGInfo->DrawImage(cPoint(xNextIcon, ySepNextPrevIcon), *imgIcon);
        if (fontManager->channelChannelGroupSmall->Width(nextChannelSep.c_str()) > (spaceNextPrev-10))
            nextChannelSep = CutText(nextChannelSep, spaceNextPrev-10, fontManager->channelChannelGroupSmall);
        pixmapEPGInfo->DrawText(cPoint(xNext, ySepNextPrev), nextChannelSep.c_str(), Theme.Color(clrChannelEPGInfoNext), clrTransparent, fontManager->channelChannelGroupSmall);
    }
}

std::string cNopacityDisplayChannelView::GetChannelSep(const cChannel *channel, bool prev) {
    std::string sepName = "";
    LOCK_CHANNELS_READ;
    const cChannel *sep = prev ? Channels->Prev(channel) :
                                 Channels->Next(channel);
    for (; sep; (prev)?(sep = Channels->Prev(sep)):(sep = Channels->Next(sep))) {
        if (sep->GroupSep()) {
            sepName = sep->Name();
            break;
        }
    }
    return sepName;
}

void cNopacityDisplayChannelView::DrawSourceInfo(void) {
    if (volumeBox && (config.GetValue("displayChannelVolume") == vbSimple))
        return;
    const cChannel *channel = cDevice::ActualDevice()->GetCurrentlyTunedTransponder();
    const cSource *source = (channel) ? Sources.Get(channel->Source()) : NULL;
    cString channelInfo = "";
    if (source) {
        channelInfo = cString::sprintf("%s #%d", source->Description(), cDevice::ActualDevice()->DeviceNumber());
    }
    if (cRecordControls::Active()) {
        LOCK_TIMERS_READ;
        cSortedTimers SortedTimers(Timers);
        bool first = true;
        int truncPos = 0;
        for (int i = 0; i < SortedTimers.Size(); i++)
            if (const cTimer *Timer = SortedTimers[i])
                if (Timer->Recording()) {
                    if (cRecordControl *RecordControl = cRecordControls::GetRecordControl(Timer))
                        if (const cDevice *Device = RecordControl->Device()) {
                            cString name(Timer->File());
                            if (first) {
                                truncPos = strlen(*channelInfo) + 30;
                            } else {
                                name.Truncate(15);
                                if (truncPos) {
                                    channelInfo.Truncate(truncPos);
                                    truncPos = 0;
                                    ClearSourceInfo();
                                }
                            }
                            channelInfo = cString::sprintf("%s%s #%i: %s", *channelInfo, (first)?" - Rec:":",", Device->DeviceNumber(), *name);
                            first = false;
                        }
                } else if ((Timer->Flags() & tfActive) && !Timer->Pending())
                break;
    }
    int textY = (geoManager->channelFooterHeight - fontManager->channelSourceInfo->Height()) / 2;
    if (fontManager->channelSourceInfo->Width(*channelInfo) > pixmapSourceInfo->ViewPort().Width())
        channelInfo = CutText(*channelInfo, pixmapSourceInfo->ViewPort().Width(), fontManager->channelSourceInfo).c_str();
    pixmapSourceInfo->DrawText(cPoint(0, textY), channelInfo, Theme.Color(clrChannelHead), clrTransparent, fontManager->channelSourceInfo);
}

void cNopacityDisplayChannelView::ClearSourceInfo(void) {
    pixmapSourceInfo->Fill(clrTransparent);
}

void cNopacityDisplayChannelView::DisplayMessage(eMessageType Type, const char *Text) {
    DELETENULL(messageBox);
    if (!Text)
        return;
    messageBox = new cNopacityMessageBox(osd,
					 cRect((geoManager->channelWidth - geoManager->messageWidth) / 2,
					       geoManager->channelTop + geoManager->channelHeight - geoManager->messageHeight - 20,
					       geoManager->messageWidth, geoManager->messageHeight),
					 Type, Text);
}

void cNopacityDisplayChannelView::DrawVolume(void) {
    int volume = statusMonitor->GetVolume();
    if (volume != lastVolume) {
        if (!volumeBox) {
            bool simple = false;
            if (config.GetValue("displayChannelVolume") == vbSimple) {
                ClearSourceInfo();
                simple = true;
            }
            volumeBox = new cNopacityVolumeBox(osd,
                                               cRect(geoManager->channelVolumeLeft,
                                                     geoManager->channelVolumeTop,
                                                     geoManager->channelVolumeWidth,
                                                     geoManager->channelVolumeHeight),
                                               simple ? fontManager->channelSourceInfo : fontManager->volumeText,
                                               simple);
        }
        volumeBox->SetVolume(volume, MAXVOLUME, volume ? false : true);
        lastVolumeTime = time(NULL);
        lastVolume = volume;
    } else {
        if (volumeBox && (time(NULL) - lastVolumeTime > 2))
            DELETENULL(volumeBox);
    }
}
