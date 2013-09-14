#include "displaychannel.h"
#include "services/tvscraper.h"

cNopacityDisplayChannel::cNopacityDisplayChannel(bool WithInfo) {
    if (firstDisplay) {
        firstDisplay = false;
        doOutput = false;
        return;
    } else
        doOutput = true;  
    config.setDynamicValues();
    withInfo = WithInfo;
    groupSep = false;
    present = NULL;
    lastSeen = -1;
    lastSignalDisplay = 0;
    lastSignalStrength = 0;
    lastSignalQuality = 0;
    lastScreenWidth = 0;
    currentLast = 0;
    channelChange = false;
    isRadioChannel = false;
    radioIconDrawn = false;
    initial = true;
    FrameTime = config.channelFrameTime; 
    FadeTime = config.channelFadeTime;
    lastDate = "";
    SetGeometry();
    CreatePixmaps();
    CreateFonts();
    DrawBackground();
    DrawSignalMeter();
}

cNopacityDisplayChannel::~cNopacityDisplayChannel() {
    if (!doOutput)
        return;
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);

    osd->DestroyPixmap(pixmapBackgroundTop);
    osd->DestroyPixmap(pixmapBackgroundBottom);
    osd->DestroyPixmap(pixmapLogo);
    osd->DestroyPixmap(pixmapLogoBackground);
    osd->DestroyPixmap(pixmapLogoBackgroundTop);
    osd->DestroyPixmap(pixmapLogoBackgroundBottom);
    osd->DestroyPixmap(pixmapChannelInfo);
    osd->DestroyPixmap(pixmapDate);
    if (withInfo) {
        osd->DestroyPixmap(pixmapBackgroundMiddle);
        osd->DestroyPixmap(pixmapProgressBar);
        osd->DestroyPixmap(pixmapEPGInfo);
    }
    if (pixmapScreenResolution)
        osd->DestroyPixmap(pixmapScreenResolution); 
    osd->DestroyPixmap(pixmapFooter);
    osd->DestroyPixmap(pixmapStreamInfo);
    osd->DestroyPixmap(pixmapStreamInfoBack);
    if (config.displaySignalStrength && showSignal) {
        osd->DestroyPixmap(pixmapSignalStrength);
        osd->DestroyPixmap(pixmapSignalQuality);
        osd->DestroyPixmap(pixmapSignalMeter);
        osd->DestroyPixmap(pixmapSignalLabel);
    }
    if (pixmapPoster)
        osd->DestroyPixmap(pixmapPoster);
    if (config.displaySignalStrength && showSignal) {
        delete fontInfoline;
    }
    delete fontHeader;
    delete fontDate;
    delete fontEPG;
    delete fontEPGSmall;
    delete fontChannelGroup;
    delete fontChannelGroupSmall;
    delete osd;
}

void cNopacityDisplayChannel::SetGeometry(void) {
    height = cOsd::OsdHeight() * config.channelHeight / 100;
    top = cOsd::OsdTop() + cOsd::OsdHeight() - height - config.channelBorderBottom;
    osd = CreateOsd(cOsd::OsdLeft(), cOsd::OsdTop(), cOsd::OsdWidth(), cOsd::OsdHeight());
    switch (config.logoPosition) {
        case lpLeft:
            infoWidth = osd->Width() - (config.logoWidth + 2 * config.channelBorderVertical + config.logoBorder);
            infoX = config.logoWidth + config.channelBorderVertical + config.logoBorder;
            break;
        case lpRight:
            infoWidth = osd->Width() - (config.logoWidth + 2 * config.channelBorderVertical + config.logoBorder);
            infoX = config.channelBorderVertical;
            break;
        case lpNone:
            infoWidth = osd->Width() - 2 * config.channelBorderVertical;
            infoX = config.channelBorderVertical;
            break;
    }
    channelInfoWidth = infoWidth * 0.7;
    dateWidth = infoWidth - channelInfoWidth;   
    channelInfoHeight = height * 0.2;
    if (channelInfoHeight%2 != 0)
        channelInfoHeight++;
    progressBarHeight = height * 0.1;
    streamInfoHeight = height * 0.2;
    if (streamInfoHeight%2 != 0)
        streamInfoHeight++;
    epgInfoHeight = height - channelInfoHeight - streamInfoHeight - progressBarHeight;
    epgInfoLineHeight = epgInfoHeight / 4;
    streamInfoY = channelInfoHeight + progressBarHeight + epgInfoHeight;
    iconSize = config.statusIconSize;
    iconsWidth = 5*iconSize;
    signalX = 0;
}

void cNopacityDisplayChannel::CreatePixmaps(void) {
    int channelInfoY = 0;
    if (withInfo) {
        pixmapProgressBar = osd->CreatePixmap(2, cRect(infoX, top + channelInfoHeight, infoWidth, progressBarHeight));
        pixmapEPGInfo = osd->CreatePixmap(2, cRect(infoX, top + channelInfoHeight + progressBarHeight, infoWidth, epgInfoHeight));
        pixmapBackgroundMiddle = osd->CreatePixmap(1, cRect(infoX, top + channelInfoHeight, infoWidth, progressBarHeight + epgInfoHeight));
    } else {
        channelInfoY = (height - channelInfoHeight) / 3;
        streamInfoY = (height - channelInfoHeight) / 3 + channelInfoHeight;
    }
    pixmapBackgroundTop = osd->CreatePixmap(1, cRect(infoX, top + channelInfoY, infoWidth, channelInfoHeight));
    pixmapBackgroundBottom = osd->CreatePixmap(1, cRect(infoX, top + streamInfoY, infoWidth, streamInfoHeight));
        
    pixmapChannelInfo = osd->CreatePixmap(2, cRect(infoX, top + channelInfoY, channelInfoWidth, channelInfoHeight));
    pixmapDate = osd->CreatePixmap(2, cRect(infoX + channelInfoWidth, top + channelInfoY, dateWidth, channelInfoHeight));
    pixmapFooter  = osd->CreatePixmap(2, cRect(infoX, top + streamInfoY, infoWidth, streamInfoHeight));
    pixmapStreamInfo =     osd->CreatePixmap(4, cRect(infoX + (infoWidth - iconsWidth - config.resolutionIconSize - 35), top + height - iconSize - 10, iconsWidth, iconSize));
    pixmapStreamInfoBack = osd->CreatePixmap(3, cRect(infoX + (infoWidth - iconsWidth - config.resolutionIconSize - 35), top + height - iconSize - 10, iconsWidth, iconSize));
    
    switch (config.logoPosition) {
        case lpLeft:
            pixmapLogoBackgroundTop = osd->CreatePixmap(1, cRect(config.channelBorderVertical, top, infoX - config.channelBorderVertical, channelInfoHeight));
            pixmapLogoBackground = osd->CreatePixmap(1, cRect(config.channelBorderVertical, top + channelInfoHeight, infoX - config.channelBorderVertical, progressBarHeight + epgInfoHeight));
            pixmapLogoBackgroundBottom = osd->CreatePixmap(1, cRect(config.channelBorderVertical, top + streamInfoY, infoX - config.channelBorderVertical, streamInfoHeight));
            pixmapLogo = osd->CreatePixmap(2, cRect(0, top, config.logoWidth + 2 * config.logoBorder, height));
            break;
        case lpRight:
            pixmapLogoBackgroundTop = osd->CreatePixmap(1, cRect(infoX + infoWidth, top, cOsd::OsdWidth() - 2*config.channelBorderVertical - infoWidth, channelInfoHeight));
            pixmapLogoBackground = osd->CreatePixmap(1, cRect(infoX + infoWidth, top + channelInfoHeight, cOsd::OsdWidth() - 2*config.channelBorderVertical - infoWidth, progressBarHeight + epgInfoHeight));
            pixmapLogoBackgroundBottom = osd->CreatePixmap(1, cRect(infoX + infoWidth, top + streamInfoY, cOsd::OsdWidth() - 2*config.channelBorderVertical - infoWidth, streamInfoHeight));
            pixmapLogo = osd->CreatePixmap(2, cRect(infoX + infoWidth, top, config.logoWidth + 2 * config.logoBorder, height));
            break;
        case lpNone:
            pixmapLogo = osd->CreatePixmap(-1, cRect(0, top, 1, 1));
            pixmapLogoBackground = osd->CreatePixmap(-1, cRect(0, top, 1, 1));
            pixmapLogoBackgroundTop = osd->CreatePixmap(-1, cRect(0, top, 1, 1));
            pixmapLogoBackgroundBottom = osd->CreatePixmap(-1, cRect(0, top, 1, 1));
            break;
    }
    
    if (config.channelFadeTime) {
        pixmapBackgroundTop->SetAlpha(0);
        pixmapBackgroundBottom->SetAlpha(0);
        pixmapChannelInfo->SetAlpha(0);
        pixmapDate->SetAlpha(0);
        pixmapLogo->SetAlpha(0);
        pixmapLogoBackground->SetAlpha(0);
        pixmapLogoBackgroundTop->SetAlpha(0);
        pixmapLogoBackgroundBottom->SetAlpha(0);
        pixmapFooter->SetAlpha(0);
        pixmapStreamInfo->SetAlpha(0);
        pixmapStreamInfoBack->SetAlpha(0);
        if (withInfo) {
            pixmapBackgroundMiddle->SetAlpha(0);
            pixmapProgressBar->SetAlpha(0);
            pixmapEPGInfo->SetAlpha(0);
        }
    }
    if (withInfo) {
        pixmapProgressBar->Fill(clrTransparent);
        pixmapEPGInfo->Fill(clrTransparent);
    }
    pixmapScreenResolution = NULL;
    pixmapSignalMeter = NULL;
    pixmapSignalStrength = NULL;
    pixmapSignalQuality = NULL;
    pixmapSignalLabel = NULL;
    pixmapPoster = NULL;
}

void cNopacityDisplayChannel::CreateFonts(void) {
    fontHeader = cFont::CreateFont(config.fontName, channelInfoHeight - 8 + config.fontChannelHeaderSize);
    fontDate = cFont::CreateFont(config.fontName, channelInfoHeight/2 + config.fontChannelDateSize);
    fontEPG = cFont::CreateFont(config.fontName, epgInfoLineHeight + config.fontEPGSize);
    fontEPGSmall = cFont::CreateFont(config.fontName, epgInfoLineHeight - 6 + config.fontEPGSmallSize);
    fontChannelGroup = cFont::CreateFont(config.fontName,  epgInfoHeight/3 + config.fontChannelGroupSize);
    fontChannelGroupSmall = cFont::CreateFont(config.fontName,  epgInfoHeight/3 - 5 + config.fontChannelGroupSmallSize);
}

void cNopacityDisplayChannel::DrawBackground(void){
    if (config.doBlending) {
        DrawBlendedBackground(pixmapBackgroundTop, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), true);
        DrawBlendedBackground(pixmapBackgroundBottom, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), false);
    } else {
        pixmapBackgroundTop->Fill(Theme.Color(clrChannelBackground));
        pixmapBackgroundBottom->Fill(Theme.Color(clrChannelBackground));
    }
    if (withInfo)
        pixmapBackgroundMiddle->Fill(Theme.Color(clrChannelBackground));
    
    if ((config.backgroundStyle == bsFull) && withInfo) {
        pixmapLogoBackground->Fill(Theme.Color(clrChannelBackground));
        if (config.doBlending) {
            DrawBlendedBackground(pixmapLogoBackgroundTop, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), true);
            DrawBlendedBackground(pixmapLogoBackgroundBottom, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), false);
        } else {
            pixmapLogoBackgroundTop->Fill(Theme.Color(clrChannelBackground));
            pixmapLogoBackgroundBottom->Fill(Theme.Color(clrChannelBackground));
        }
    } else {
        pixmapLogoBackgroundTop->Fill(clrTransparent);
        pixmapLogoBackground->Fill(clrTransparent);
        pixmapLogoBackgroundBottom->Fill(clrTransparent);
    }
    if (config.roundedCornersChannel) {
        int cornerTopSize = channelInfoHeight/2;
        int cornerBottomSize = streamInfoHeight/2;
        if ((cornerTopSize > 2)&&(cornerBottomSize > 2)) {
            if ((config.backgroundStyle == bsTrans) || ((config.logoPosition == lpNone))) {
                pixmapBackgroundTop->DrawEllipse(cRect(0, 0, cornerTopSize, cornerTopSize), clrTransparent, -2);
                pixmapBackgroundTop->DrawEllipse(cRect(infoWidth - cornerTopSize, 0, cornerTopSize, cornerTopSize), clrTransparent, -1);
                pixmapBackgroundBottom->DrawEllipse(cRect(0, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -3);
                pixmapBackgroundBottom->DrawEllipse(cRect(infoWidth - cornerBottomSize, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -4);
            } else if ((config.backgroundStyle == bsFull) && (config.logoPosition == lpLeft)){
                pixmapLogoBackgroundTop->DrawEllipse(cRect(0, 0, cornerTopSize, cornerTopSize), clrTransparent, -2);
                pixmapBackgroundTop->DrawEllipse(cRect(infoWidth - cornerTopSize, 0, cornerTopSize, cornerTopSize), clrTransparent, -1);
                pixmapLogoBackgroundBottom->DrawEllipse(cRect(0, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -3);
                pixmapBackgroundBottom->DrawEllipse(cRect(infoWidth - cornerBottomSize, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -4);
            } else if ((config.backgroundStyle == bsFull) && (config.logoPosition == lpRight)){
                pixmapBackgroundTop->DrawEllipse(cRect(0, 0, cornerTopSize, cornerTopSize), clrTransparent, -2);
                pixmapLogoBackgroundTop->DrawEllipse(cRect(pixmapLogoBackgroundTop->ViewPort().Width() - cornerTopSize, 0, cornerTopSize, cornerTopSize), clrTransparent, -1);
                pixmapBackgroundBottom->DrawEllipse(cRect(0, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -3);
                pixmapLogoBackgroundBottom->DrawEllipse(cRect(pixmapLogoBackgroundBottom->ViewPort().Width() - cornerBottomSize, cornerBottomSize, cornerBottomSize, cornerBottomSize), clrTransparent, -4);
            }
        }
    }
    pixmapChannelInfo->Fill(clrTransparent);
    pixmapDate->Fill(clrTransparent);
    pixmapLogo->Fill(clrTransparent);

    pixmapFooter->Fill(clrTransparent);
    pixmapStreamInfo->Fill(clrTransparent);
}

void cNopacityDisplayChannel::DrawDate(void) {
    cString curDate = DayDateTime();
    if (initial || channelChange || strcmp(curDate, lastDate)) {
        int strDateWidth = fontDate->Width(curDate);
        int strDateHeight = fontDate->Height();
        int x = dateWidth - strDateWidth - channelInfoHeight/2;
        int y = (channelInfoHeight - strDateHeight) / 2;
        pixmapDate->Fill(clrTransparent);
        pixmapDate->DrawText(cPoint(x, y), curDate, Theme.Color(clrChannelHead), clrTransparent, fontDate);
        lastDate = curDate;
    }
}

void cNopacityDisplayChannel::DrawIconMask(void) {
    cImageLoader imgLoader;
    if (imgLoader.LoadIcon("skinIcons/channelsymbols", iconsWidth, iconSize)) {
        pixmapStreamInfo->DrawImage(cPoint(0, 0), imgLoader.GetImage());
    }
}

void cNopacityDisplayChannel::DrawIcons(const cChannel *Channel) {
    isRadioChannel = ((!Channel->Vpid())&&(Channel->Apid(0)))?true:false;
    
    int backX = 5;
    int backY = 5;
    
    tColor colVT = Theme.Color(clrChannelSymbolOff);
    if (Channel->Vpid() && Channel->Tpid()) {
        colVT = Theme.Color(clrChannelSymbolOn);
    }
    pixmapStreamInfoBack->DrawRectangle(cRect(backX, backY, iconSize-10, iconSize-10), colVT);

    backX += iconSize;
    tColor colStereo = Theme.Color(clrChannelSymbolOff);
    if (Channel->Apid(0)) {
        colStereo = Theme.Color(clrChannelSymbolOn);
    }
    pixmapStreamInfoBack->DrawRectangle(cRect(backX, backY, iconSize-10, iconSize-10), colStereo);

    backX += iconSize;
    tColor colDolby = Theme.Color(clrChannelSymbolOff);
    if (Channel->Dpid(0)) {
        colDolby = Theme.Color(clrChannelSymbolOn);
    }
    pixmapStreamInfoBack->DrawRectangle(cRect(backX, backY, iconSize-10, iconSize-10), colDolby);

    backX += iconSize;
    tColor colCrypted = Theme.Color(clrChannelSymbolOff);
    if (Channel->Ca()) {
        colCrypted = Theme.Color(clrChannelSymbolOn);
    }
    pixmapStreamInfoBack->DrawRectangle(cRect(backX, backY, iconSize-10, iconSize-10), colCrypted);

    backX += iconSize;
    tColor colRecording = Theme.Color(clrChannelSymbolOff);
    if (cRecordControls::Active()) {
        colRecording = Theme.Color(clrChannelRecActive);
    }
    pixmapStreamInfoBack->DrawRectangle(cRect(backX, backY, iconSize-10, iconSize-10), colRecording);
}

void cNopacityDisplayChannel::DrawIconsSingle(const cChannel *Channel) {
    isRadioChannel = ((!Channel->Vpid())&&(Channel->Apid(0)))?true:false;
    pixmapStreamInfo->Fill(clrTransparent);
    int iconX = 0;
    cImageLoader imgLoader;

    if (Channel->Vpid() && Channel->Tpid()) {
        if (imgLoader.LoadIcon("skinIcons/txton", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    } else {
        if (imgLoader.LoadIcon("skinIcons/txtoff", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    }

    iconX += iconSize;

    if (Channel->Apid(0)) {
        if (imgLoader.LoadIcon("skinIcons/stereoon", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    } else {
        if (imgLoader.LoadIcon("skinIcons/stereooff", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    }

    iconX += iconSize;
    
    if (Channel->Dpid(0)) {
        if (imgLoader.LoadIcon("skinIcons/dolbyon", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    } else {
        if (imgLoader.LoadIcon("skinIcons/dolbyoff", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    }

    iconX += iconSize;

    if (Channel->Ca()) {
        if (imgLoader.LoadIcon("skinIcons/crypted", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    } else {
        if (imgLoader.LoadIcon("skinIcons/fta", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    }

    iconX += iconSize;
    
    if (cRecordControls::Active()) {
        if (imgLoader.LoadIcon("skinIcons/recon", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    } else {
        if (imgLoader.LoadIcon("skinIcons/recoff", iconSize)) {
            pixmapStreamInfo->DrawImage(cPoint(iconX, 0), imgLoader.GetImage());
        }
    }

}

void cNopacityDisplayChannel::DrawScreenResolution(void) {
    int spacing = 10;
    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;

    if (!pixmapScreenResolution) {
        int x = infoX + infoWidth - config.resolutionIconSize - 2*spacing;
        int y = top + height - config.resolutionIconSize - 10;
        pixmapScreenResolution = osd->CreatePixmap(3, cRect(x, y, config.resolutionIconSize, config.resolutionIconSize));
        pixmapScreenResolution->Fill(clrTransparent);
        if ((initial)&&(config.channelFadeTime))
            pixmapScreenResolution->SetAlpha(0);
    }
    
    cImageLoader imgLoader;
    if (isRadioChannel) {
        if (!radioIconDrawn) {
            if (imgLoader.LoadIcon("skinIcons/radio", config.resolutionIconSize)) {
                    pixmapScreenResolution->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
            lastScreenWidth = 0;
            radioIconDrawn = true;
        }
    } else {
        cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);
        if (screenWidth != lastScreenWidth) {
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
            if (imgLoader.LoadIcon(*iconName, config.resolutionIconSize)) {
                pixmapScreenResolution->DrawImage(cPoint(0,0), imgLoader.GetImage());
            }
            lastScreenWidth = screenWidth;
            radioIconDrawn = false;
        }
    }
}

void cNopacityDisplayChannel::DrawSignalMeter(void) {
    signalWidth = infoWidth * 0.15;
    signalHeight = signalWidth *15 / 200;
    showSignal = false;
    if (config.displaySignalStrength) {
        cImageLoader imgLoader;
        cString signalStrength = "STR";
        cString signalQuality = "SNR";
        if (imgLoader.LoadIcon("skinIcons/signal", signalWidth, signalHeight, false)) {
            cImage imgSignal = imgLoader.GetImage();
            signalWidth = imgSignal.Width();
            signalHeight = imgSignal.Height();
            int signalMeterY = streamInfoY + (streamInfoHeight - 2*signalHeight - 5)/2;
            fontInfoline = cFont::CreateFont(config.fontName, signalHeight - 2);
            int labelWidth = max(fontInfoline->Width(*signalStrength), fontInfoline->Width(*signalQuality)) + 2;
            signalX = streamInfoHeight / 2 + labelWidth;
            showSignal = true;
            pixmapSignalStrength = osd->CreatePixmap(3, cRect(infoX + signalX, top + signalMeterY + 2, signalWidth + 2, signalHeight + 2));
            pixmapSignalQuality  = osd->CreatePixmap(3, cRect(infoX + signalX, top + signalMeterY + signalHeight + 5, signalWidth + 2, signalHeight + 2));
            pixmapSignalMeter    = osd->CreatePixmap(4, cRect(infoX + signalX + 1, top + signalMeterY + 3, signalWidth, 2*signalHeight + 3));
            pixmapSignalLabel    = osd->CreatePixmap(3, cRect(infoX + streamInfoHeight / 2, top + signalMeterY + 2, labelWidth, 2*signalHeight + 3));
            pixmapSignalStrength->Fill(Theme.Color(clrProgressBarBack));
            pixmapSignalQuality->Fill(Theme.Color(clrProgressBarBack));
            pixmapSignalMeter->Fill(clrTransparent);
            pixmapSignalLabel->Fill(clrTransparent);
            if (config.channelFadeTime) {
                pixmapSignalStrength->SetAlpha(0);
                pixmapSignalQuality->SetAlpha(0);
                pixmapSignalMeter->SetAlpha(0);
                pixmapSignalLabel->SetAlpha(0);
            }
            pixmapSignalStrength->DrawImage(cPoint(1,1), imgSignal);
            pixmapSignalQuality->DrawImage(cPoint(1,1), imgSignal);
            pixmapSignalLabel->DrawText(cPoint(0, 2), *signalStrength, Theme.Color(clrChannelEPGInfo), clrTransparent, fontInfoline);
            pixmapSignalLabel->DrawText(cPoint(0, signalHeight + 4), *signalQuality, Theme.Color(clrChannelEPGInfo), clrTransparent, fontInfoline);
        }
    }
}

void cNopacityDisplayChannel::ShowSignalMeter(void) {
    if(pixmapSignalStrength)
        pixmapSignalStrength->SetLayer(3);
    if(pixmapSignalQuality)
        pixmapSignalQuality->SetLayer(3);
    if(pixmapSignalMeter)
        pixmapSignalMeter->SetLayer(4);
    if(pixmapSignalLabel)
        pixmapSignalLabel->SetLayer(3);
}

void cNopacityDisplayChannel::HideSignalMeter(void) {
    if(pixmapSignalStrength)
        pixmapSignalStrength->SetLayer(-1);
    if(pixmapSignalQuality)
        pixmapSignalQuality->SetLayer(-1);
    if(pixmapSignalMeter)
        pixmapSignalMeter->SetLayer(-1);
    if(pixmapSignalLabel)
        pixmapSignalLabel->SetLayer(-1);
}

void cNopacityDisplayChannel::DrawSignal(void) {
    time_t Now = time(NULL);
    if (Now != lastSignalDisplay) {
        int SignalStrength = cDevice::ActualDevice()->SignalStrength();
        int SignalQuality = cDevice::ActualDevice()->SignalQuality();
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

void cNopacityDisplayChannel::DrawSourceInfo(const cChannel *Channel) {
    const cSource *source = Sources.Get(Channel->Source());
    cString channelInfo = "";
    if (source)
        channelInfo = cString::sprintf("%s - %s", *cSource::ToString(source->Code()),  source->Description());
    int x = signalX + 20;
    if (config.displaySignalStrength)
        x += signalWidth;
    pixmapFooter->Fill(clrTransparent);    
    pixmapFooter->DrawText(cPoint(x, (streamInfoHeight - fontDate->Height())/2), channelInfo, Theme.Color(clrChannelHead), clrTransparent, fontDate);   
}

void cNopacityDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    if (!doOutput)
        return;
    pixmapLogo->Fill(clrTransparent);
    pixmapChannelInfo->Fill(clrTransparent);
    pixmapStreamInfo->Fill(clrTransparent);
    pixmapStreamInfoBack->Fill(clrTransparent);
    if (withInfo) {
        pixmapProgressBar->Fill(clrTransparent);
        pixmapEPGInfo->Fill(clrTransparent);
    }
    
    channelChange = true;
    lastSignalStrength = 0;
    lastSignalQuality = 0;
    cString ChannelNumber("");
    cString ChannelName("");
    groupSep = false;
    if (Channel) {
        ChannelName = Channel->Name();
        if (!Channel->GroupSep()) {
            ChannelNumber = cString::sprintf("%d%s", Channel->Number(), Number ? "-" : "");
        } else {
            groupSep = true;
        }
    } else if (Number) {
        ChannelNumber = cString::sprintf("%d-", Number);
    } else {
        ChannelName = ChannelString(NULL, 0);
    }
    
    if (!groupSep) {
        if (withInfo && Channel) {
            if (config.symbolStyle == 0) {
                DrawIconMask();
                DrawIcons(Channel);
            } else {
                DrawIconsSingle(Channel);
            }
            if (config.displaySourceInfo)
                DrawSourceInfo(Channel);
        }
        cString channelString = cString::sprintf("%s %s", *ChannelNumber, *ChannelName);
        pixmapChannelInfo->DrawText(cPoint(channelInfoHeight/2, (channelInfoHeight-fontHeader->Height())/2), channelString, Theme.Color(clrChannelHead), clrTransparent, fontHeader);
        if (config.logoPosition != lpNone) {
            cImageLoader imgLoader;
            if (imgLoader.LoadLogo(*ChannelName)) {
                pixmapLogo->DrawImage(cPoint(config.logoBorder, (height-config.logoHeight)/2), imgLoader.GetImage());
            } else if (imgLoader.LoadLogo(*(Channel->GetChannelID().ToString()))) {
                pixmapLogo->DrawImage(cPoint(config.logoBorder, (height-config.logoHeight)/2), imgLoader.GetImage());
            }
        }
        ShowSignalMeter();
    } else {
        DrawChannelGroups(Channel, ChannelName);
        HideSignalMeter();
        pixmapFooter->Fill(clrTransparent);
    }
}

void cNopacityDisplayChannel::DrawChannelGroups(const cChannel *Channel, cString ChannelName) {
    int ySep;
    int ySepNextPrevIcon;
    cPixmap *infoPixmap;
    int prevNextSize = 64;

    if (withInfo) {
        pixmapProgressBar->Fill(clrTransparent);
        pixmapEPGInfo->Fill(clrTransparent);
        ySep = (epgInfoHeight-fontChannelGroup->Height())/2 - fontChannelGroup->Height()/2;
        ySepNextPrevIcon = (epgInfoHeight - prevNextSize)/2 - fontChannelGroup->Height()/2;
        infoPixmap = pixmapEPGInfo;
    } else {
        ySep = (channelInfoHeight - fontChannelGroup->Height())/2;
        ySepNextPrevIcon = (channelInfoHeight - prevNextSize)/2;
        infoPixmap = pixmapChannelInfo;
    }
    int widthSep = fontChannelGroup->Width(*ChannelName);
    int xSep = (config.displayPrevNextChannelGroup)?(infoWidth * 2 / 5):0;
    infoPixmap->DrawText(cPoint(xSep, ySep), *ChannelName, Theme.Color(clrChannelHead), clrTransparent, fontChannelGroup);

    cImageLoader imgLoader;
    if (config.logoPosition != lpNone) {
        cString separator = cString::sprintf("separatorlogos/%s", *ChannelName);
        if (imgLoader.LoadLogo(*separator)) {
            pixmapLogo->DrawImage(cPoint(config.logoBorder, (height-config.logoHeight)/2), imgLoader.GetImage());
        } else if (imgLoader.LoadIcon("skinIcons/Channelseparator", config.logoWidth, config.logoHeight)) {
            pixmapLogo->DrawImage(cPoint(config.logoBorder + (config.logoWidth - config.logoWidth)/2, (height-config.logoHeight)/2), imgLoader.GetImage());
        }
    }
    
    if (!config.displayPrevNextChannelGroup)
        return;
    
    cString prevChannelSep = GetChannelSep(Channel, true);
    cString nextChannelSep = GetChannelSep(Channel, false);
    bool prevAvailable = (strlen(*prevChannelSep) > 0)?true:false;
    bool nextAvailable = (strlen(*nextChannelSep) > 0)?true:false;


    int ySepNextPrev = ySep + (fontChannelGroup->Height() - fontChannelGroupSmall->Height())/2;
    if (prevAvailable) {
        int xSymbol = xSep - prevNextSize - 10;
        if (imgLoader.LoadIcon("skinIcons/arrowLeftChannelSep", prevNextSize)) {
            infoPixmap->DrawImage(cPoint(xSymbol, ySepNextPrevIcon), imgLoader.GetImage());
        }
        int xSepPrev = xSymbol - 10 - fontChannelGroupSmall->Width(prevChannelSep);
        if (xSepPrev < 0) {
            prevChannelSep = CutText(*prevChannelSep, xSymbol, fontChannelGroupSmall).c_str();
            xSepPrev = xSymbol - 10 - fontChannelGroupSmall->Width(prevChannelSep);
        }
        infoPixmap->DrawText(cPoint(xSepPrev, ySepNextPrev), *prevChannelSep, Theme.Color(clrChannelEPGInfoNext), clrTransparent, fontChannelGroupSmall);
    }
    if (nextAvailable) {
        int xSymbol = xSep + widthSep + 10;
        if (imgLoader.LoadIcon("skinIcons/arrowRightChannelSep", prevNextSize)) {
            infoPixmap->DrawImage(cPoint(xSymbol, ySepNextPrevIcon), imgLoader.GetImage());
        }
        int xSepNext = xSymbol + 10 + prevNextSize;
        int spaceAvailable = infoPixmap->DrawPort().Width() - xSepNext;
        if (fontChannelGroupSmall->Width(nextChannelSep) > spaceAvailable) {
            nextChannelSep = CutText(*nextChannelSep, spaceAvailable, fontChannelGroupSmall).c_str();
        }
        infoPixmap->DrawText(cPoint(xSepNext, ySepNextPrev), *nextChannelSep, Theme.Color(clrChannelEPGInfoNext), clrTransparent, fontChannelGroupSmall);
    }
}

cString cNopacityDisplayChannel::GetChannelSep(const cChannel *channel, bool prev) {
    cString sepName("");
    const cChannel *sep = prev ? Channels.Prev(channel) : 
                                 Channels.Next(channel);
    for (; sep; (prev)?(sep = Channels.Prev(sep)):(sep = Channels.Next(sep))) {
        if (sep->GroupSep()) {
            sepName = sep->Name();
            break;
        }
    }
    return sepName;
}

void cNopacityDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
    if (!doOutput)
        return;
    if (!withInfo)
        return;
    if (present != Present)
        lastSeen = -1;
    present = Present;
    
    if (Present || Following) {
        pixmapProgressBar->Fill(clrTransparent);
        pixmapEPGInfo->Fill(clrTransparent);
    }
    int indent = 20;
    
    for (int i = 0; i < 2; i++) {
        const cEvent *e = !i ? Present : Following;
        int y = !i ? 0 : 2;
        if (e) {
            cString startTime = e->GetTimeString();
            int startTimeWidth = fontEPG->Width(*startTime);
            int epgWidth = fontEPG->Width(e->Title());
            int epgWidthShort = fontEPGSmall->Width(e->ShortText());
            cString strSeen("");
            if (i==0) {
                if (config.progressCurrentSchedule == 0) {
                    int seen = (int)(time(NULL) - e->StartTime())/60;
                    strSeen = cString::sprintf("%d/%dmin", seen, e->Duration()/60);
                } else if (config.progressCurrentSchedule == 1) {
                    int remaining = (int)(e->EndTime() - time(NULL))/60;
                    strSeen = cString::sprintf("-%d/%dmin", remaining, e->Duration()/60);
                }
                DrawPoster(e);
            } else {
                strSeen = cString::sprintf("%dmin", e->Duration()/60);
            }
            int seenWidth = fontEPG->Width(*strSeen);
            int space = infoWidth - 9*indent - seenWidth - startTimeWidth - config.resolutionIconSize;
            bool drawRecIcon = false;
            int widthRecIcon = 0;
            if (e->HasTimer()) {
                drawRecIcon = true;
                widthRecIcon = fontEPGSmall->Width(" REC ");
                space -= widthRecIcon + indent/2;
            }
            cString strEPG = e->Title();
            if (space < epgWidth) {
                strEPG = CutText(e->Title(), space, fontEPG).c_str();
            }
            cString strEPGShort = e->ShortText();
            int spaceShort = infoWidth - 6*indent - startTimeWidth - config.resolutionIconSize;
            if (spaceShort < epgWidthShort) {
                strEPGShort = CutText(e->ShortText(), spaceShort, fontEPGSmall).c_str();
            }
            tColor fontColor = (i==0)?Theme.Color(clrChannelEPG):Theme.Color(clrChannelEPGNext);
            tColor fontColorInfo = (i==0)?Theme.Color(clrChannelEPGInfo):Theme.Color(clrChannelEPGInfoNext);
            
            pixmapEPGInfo->DrawText(cPoint(indent, y * epgInfoLineHeight), *startTime, fontColor, clrTransparent, fontEPG);
            int xEPGInfo = 2 * indent + startTimeWidth;
            if (drawRecIcon) {
                tColor clrRecIcon = (i==0)?Theme.Color(clrChannelRecActive):Theme.Color(clrRecNext);
                tColor clrRecIconText = (i==0)?Theme.Color(clrRecNowFont):Theme.Color(clrRecNextFont);
                pixmapEPGInfo->DrawRectangle(cRect(xEPGInfo, y * epgInfoLineHeight , widthRecIcon, epgInfoLineHeight), clrRecIcon);
                int xRecText = xEPGInfo + (widthRecIcon - fontEPGSmall->Width("REC"))/2;
                int yRecText = y * epgInfoLineHeight + (epgInfoLineHeight - fontEPGSmall->Height())/2;
                pixmapEPGInfo->DrawText(cPoint(xRecText, yRecText), "REC", clrRecIconText, clrRecIcon, fontEPGSmall);
                xEPGInfo += widthRecIcon + indent/2;
            }
            pixmapEPGInfo->DrawText(cPoint(xEPGInfo, y * epgInfoLineHeight), *strEPG, fontColor, clrTransparent, fontEPG);
            pixmapEPGInfo->DrawText(cPoint(2 * indent + startTimeWidth, (y+1) * epgInfoLineHeight + 3), *strEPGShort, fontColorInfo, clrTransparent, fontEPGSmall);
            int x = infoWidth - indent - seenWidth - config.resolutionIconSize - indent;
            pixmapEPGInfo->DrawText(cPoint(x, y * epgInfoLineHeight), *strSeen, fontColor, clrTransparent, fontEPG);
        }
    }
}

void cNopacityDisplayChannel::DrawPoster(const cEvent *event) {
    if (pixmapPoster) {
        osd->DestroyPixmap(pixmapPoster);
        pixmapPoster = NULL;
    }
    static cPlugin *pTVScraper = cPluginManager::GetPlugin("tvscraper");
    if (pTVScraper) {
        TVScraperGetPosterOrBanner call;
        call.event = event;
        if (pTVScraper->Service("TVScraperGetPosterOrBanner", &call)) {
            int mediaWidth = 0;
            int mediaHeight = 0;
            if (call.type == typeSeries) {
                mediaWidth = call.media.width;
                mediaHeight = call.media.height;
            } else if (call.type == typeMovie) {
                double ratio = (double)(cOsd::OsdHeight()/3) / (double)call.media.height;
                mediaWidth = ratio * call.media.width;
                mediaHeight = ratio * call.media.height;
            }
            pixmapPoster = osd->CreatePixmap(1, cRect(config.channelBorderVertical,
                                                      config.channelBorderBottom, 
                                                      mediaWidth + 2*config.channelBorderVertical, 
                                                      mediaHeight + 2*config.channelBorderBottom));
            if (initial && config.channelFadeTime)
                pixmapPoster->SetAlpha(0);
            cImageLoader imgLoader;
            if (imgLoader.LoadPoster(call.media.path.c_str(), mediaWidth, mediaHeight)) {
                pixmapPoster->Fill(Theme.Color(clrChannelBackground));
                pixmapPoster->DrawImage(cPoint(config.channelBorderVertical, config.channelBorderBottom), imgLoader.GetImage());
            } else {
                pixmapPoster->Fill(clrTransparent);                            
            }
        }
    }
}

void cNopacityDisplayChannel::DrawProgressBar(int Current, int Total) {
    if ((Current < currentLast + 3) && !channelChange)
        return;
    currentLast = Current;
    if ((Current > 0) || (Total > 0)) {
        int barHeight = pixmapProgressBar->ViewPort().Height()-8;
        if (barHeight%2 != 0)
            barHeight++;
        if (barHeight < 3)   
            return;
        int barFullWidth = pixmapProgressBar->ViewPort().Width() - 20 - barHeight;
        double percentSeen = ((double)Current) / (double)Total;

        pixmapProgressBar->DrawEllipse(cRect(9, 3, barHeight+2, barHeight+2), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawEllipse(cRect(9 + barFullWidth, 3, barHeight+2, barHeight+2), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawRectangle(cRect( 9 + barHeight/2, 3, barFullWidth, barHeight+1), Theme.Color(clrProgressBarBack));

        pixmapProgressBar->DrawEllipse(cRect(10, 4, barHeight, barHeight), Theme.Color(clrProgressBarBlend));
        if (Current > 0) {
            tColor colAct = DrawProgressbarBackground(10 + barHeight/2, 4, barFullWidth * percentSeen, barHeight-1);
            pixmapProgressBar->DrawEllipse(cRect(10 + barFullWidth * percentSeen, 4, barHeight, barHeight), colAct);
        }
    }
}

tColor cNopacityDisplayChannel::DrawProgressbarBackground(int left, int top, int width, int height) {

    tColor clr1 = Theme.Color(clrProgressBar);
    tColor clr2 = Theme.Color(clrProgressBarBlend);
    tColor clr = 0x00000000;
    width = (width==0)?1:width;
    int step = width / 256;
    int alpha = 0x0;
    int alphaStep;
    int maximum = 0;
    if (step == 0) {    //width < 256
        step = 1;
        alphaStep = 256 / width;
        maximum = width;
    } else {            //width > 256
        alphaStep = 0x1;
        maximum = 256;
    }
    int x = 0;
    for (int i = 0; i < maximum; i++) {
        x = left + i*step;
        clr = AlphaBlend(clr1, clr2, alpha);
        pixmapProgressBar->DrawRectangle(cRect(x,top,step,height), clr);
        alpha += alphaStep;
    }
    if (step > 0) {
        int rest = width - step*256;
        pixmapProgressBar->DrawRectangle(cRect(left+step*256, top, rest, height), clr);
    }
    return clr;
}

void cNopacityDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
}

void cNopacityDisplayChannel::Flush(void) {
    if (!doOutput)
        return;
    DrawDate();
    if (!groupSep)
        DrawScreenResolution();
    else {
        if (pixmapScreenResolution)
            pixmapScreenResolution->Fill(clrTransparent);
    }
    
    if (config.displaySignalStrength && showSignal && !groupSep) {
        DrawSignal();
    } else
        if (pixmapSignalMeter)
            pixmapSignalMeter->Fill(clrTransparent);
    if (withInfo) {
        int Current = 0;
        int Total = 0;
        if (present) {
            time_t t = time(NULL);
            if (t > present->StartTime())
                Current = t - present->StartTime();
            Total = present->Duration();
            DrawProgressBar(Current, Total);
        }
    }
    if (initial) {
        if (config.channelFadeTime)
            Start();
    }
    initial = false;
    channelChange = false;
    osd->Flush();
}

void cNopacityDisplayChannel::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        pixmapBackgroundTop->SetAlpha(Alpha);
        pixmapBackgroundBottom->SetAlpha(Alpha);
        pixmapLogoBackground->SetAlpha(Alpha);
        pixmapLogoBackgroundTop->SetAlpha(Alpha);
        pixmapLogoBackgroundBottom->SetAlpha(Alpha);
        pixmapLogo->SetAlpha(Alpha);
        pixmapChannelInfo->SetAlpha(Alpha);
        pixmapDate->SetAlpha(Alpha);
        if (withInfo) {
            pixmapBackgroundMiddle->SetAlpha(Alpha);
            pixmapProgressBar->SetAlpha(Alpha);
            pixmapEPGInfo->SetAlpha(Alpha);
        }
        pixmapFooter->SetAlpha(Alpha);
        pixmapStreamInfo->SetAlpha(Alpha);
        pixmapStreamInfoBack->SetAlpha(Alpha);
        if (pixmapScreenResolution)
            pixmapScreenResolution->SetAlpha(Alpha);
        if (config.displaySignalStrength && showSignal) {
            pixmapSignalStrength->SetAlpha(Alpha);
            pixmapSignalQuality->SetAlpha(Alpha);
            pixmapSignalMeter->SetAlpha(Alpha);
            pixmapSignalLabel->SetAlpha(Alpha);
        }
        if (pixmapPoster)
            pixmapPoster->SetAlpha(Alpha);
        cPixmap::Unlock();
        if (Running())
            osd->Flush();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime) {
            break;
        }
    }
}
