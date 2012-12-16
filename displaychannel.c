#include "symbols/audio.xpm"
#include "symbols/dolbydigital.xpm"
#include "symbols/encrypted.xpm"
#include "symbols/radio.xpm"
#include "symbols/recording.xpm"
#include "symbols/teletext.xpm"

#include "displaychannel.h"

cNopacityDisplayChannel::cNopacityDisplayChannel(bool WithInfo) {
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
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmapBackgroundTop);
    osd->DestroyPixmap(pixmapBackgroundBottom);
    osd->DestroyPixmap(pixmapLogo);
    osd->DestroyPixmap(pixmapChannelInfo);
    osd->DestroyPixmap(pixmapDate);
    if (withInfo) {
        osd->DestroyPixmap(pixmapBackgroundMiddle);
        osd->DestroyPixmap(pixmapProgressBar);
        osd->DestroyPixmap(pixmapEPGInfo);
    }
    if (pixmapScreenResolution)
        osd->DestroyPixmap(pixmapScreenResolution); 
    osd->DestroyPixmap(pixmapStreamInfo);
    if (config.displaySignalStrength && showSignal) {
        osd->DestroyPixmap(pixmapSignalStrength);
        osd->DestroyPixmap(pixmapSignalQuality);
        osd->DestroyPixmap(pixmapSignalMeter);
        osd->DestroyPixmap(pixmapSignalLabel);
    }
    if (config.displaySignalStrength && showSignal) {
        delete fontInfoline;
    }
    delete fontHeader;
    delete fontDate;
    delete fontEPG;
    delete fontEPGSmall;
    delete osd;
}

cBitmap cNopacityDisplayChannel::bmTeletext(teletext_xpm);
cBitmap cNopacityDisplayChannel::bmRadio(radio_xpm);
cBitmap cNopacityDisplayChannel::bmAudio(audio_xpm);
cBitmap cNopacityDisplayChannel::bmDolbyDigital(dolbydigital_xpm);
cBitmap cNopacityDisplayChannel::bmEncrypted(encrypted_xpm);
cBitmap cNopacityDisplayChannel::bmRecording(recording_xpm);

void cNopacityDisplayChannel::SetGeometry(void) {
    height = cOsd::OsdHeight() * config.channelHeight / 100;
    int top = cOsd::OsdTop() + cOsd::OsdHeight() - height - config.channelBorderBottom;
    osd = CreateOsd(cOsd::OsdLeft(), top, cOsd::OsdWidth(), height);
    infoWidth = osd->Width() - (config.logoWidth + 2 * config.channelBorderVertical + config.logoBorder);
    infoX = config.logoWidth + config.channelBorderVertical + config.logoBorder;
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
}

void cNopacityDisplayChannel::CreatePixmaps(void) {
    int channelInfoY = 0;
    if (withInfo) {
        pixmapProgressBar = osd->CreatePixmap(2, cRect(infoX, channelInfoHeight, infoWidth, progressBarHeight));
        pixmapEPGInfo     = osd->CreatePixmap(2, cRect(infoX, channelInfoHeight + progressBarHeight, infoWidth, epgInfoHeight));
        pixmapBackgroundMiddle = osd->CreatePixmap(1, cRect(infoX, channelInfoHeight, infoWidth, progressBarHeight + epgInfoHeight));
    } else {
        channelInfoY = (height - channelInfoHeight) / 3;
        streamInfoY = (height - channelInfoHeight) / 3 + channelInfoHeight;
    }
    pixmapBackgroundTop = osd->CreatePixmap(1, cRect(infoX, channelInfoY, infoWidth, channelInfoHeight));
    pixmapBackgroundBottom = osd->CreatePixmap(1, cRect(infoX, streamInfoY, infoWidth, streamInfoHeight));
        
    pixmapChannelInfo = osd->CreatePixmap(2, cRect(infoX, channelInfoY, channelInfoWidth, channelInfoHeight));
    pixmapDate = osd->CreatePixmap(2, cRect(infoX + channelInfoWidth, channelInfoY, dateWidth, channelInfoHeight));
    pixmapStreamInfo  = osd->CreatePixmap(2, cRect(infoX, streamInfoY, infoWidth, streamInfoHeight));
    pixmapLogo = osd->CreatePixmap(1, cRect(0, 0, config.logoWidth + 2 * config.logoBorder, height));
    
    if (config.channelFadeTime) {
        pixmapBackgroundTop->SetAlpha(0);
        pixmapBackgroundBottom->SetAlpha(0);
        pixmapChannelInfo->SetAlpha(0);
        pixmapDate->SetAlpha(0);
        pixmapLogo->SetAlpha(0);
        pixmapStreamInfo->SetAlpha(0);
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
}

void cNopacityDisplayChannel::CreateFonts(void) {
    fontHeader = cFont::CreateFont(config.fontName, channelInfoHeight - 8 + config.fontChannelHeaderSize);
    fontDate = cFont::CreateFont(config.fontName, channelInfoHeight/2 + config.fontChannelDateSize);
    fontEPG = cFont::CreateFont(config.fontName, epgInfoLineHeight + config.fontEPGSize);
    fontEPGSmall = cFont::CreateFont(config.fontName, epgInfoLineHeight - 6 + config.fontEPGSmallSize);
}

void cNopacityDisplayChannel::DrawBackground(void){
    
    DrawBlendedBackground(pixmapBackgroundTop, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), true);
    if (withInfo)
        pixmapBackgroundMiddle->Fill(Theme.Color(clrChannelBackground));
    DrawBlendedBackground(pixmapBackgroundBottom, Theme.Color(clrChannelBackground), Theme.Color(clrChannelBackBlend), false);

    pixmapBackgroundTop->DrawEllipse(cRect(0, 0, channelInfoHeight/2, channelInfoHeight/2), clrTransparent, -2);
    pixmapBackgroundTop->DrawEllipse(cRect(infoWidth - channelInfoHeight/2, 0, channelInfoHeight/2, channelInfoHeight/2), clrTransparent, -1);
    pixmapBackgroundBottom->DrawEllipse(cRect(0, streamInfoHeight/2, streamInfoHeight/2, streamInfoHeight/2), clrTransparent, -3);
    pixmapBackgroundBottom->DrawEllipse(cRect(infoWidth - streamInfoHeight/2, streamInfoHeight/2, streamInfoHeight/2, streamInfoHeight/2), clrTransparent, -4);

    
    pixmapChannelInfo->Fill(clrTransparent);
    pixmapDate->Fill(clrTransparent);
    pixmapLogo->Fill(clrTransparent);
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

void cNopacityDisplayChannel::DrawIcons(const cChannel *Channel) {
    int spacing = 10;
    int x = infoWidth - config.resolutionIconSize - 3*spacing;
    int y = 0;
    
    bool rec = cRecordControls::Active();
    x -= bmRecording.Width() + spacing;
    y = (streamInfoHeight - bmRecording.Height()) / 2;
    pixmapStreamInfo->DrawBitmap(cPoint(x,y), bmRecording, Theme.Color(rec ? clrChannelRecActive : clrChannelSymbolOff), clrTransparent);

    x -= bmEncrypted.Width() + spacing;
    y = (streamInfoHeight - bmEncrypted.Height()) / 2;
    pixmapStreamInfo->DrawBitmap(cPoint(x,y), bmEncrypted, Theme.Color(Channel->Ca() ? clrChannelSymbolOn : clrChannelSymbolOff), clrTransparent);

    x -= bmDolbyDigital.Width() + spacing;
    y = (streamInfoHeight - bmDolbyDigital.Height()) / 2;
    pixmapStreamInfo->DrawBitmap(cPoint(x,y), bmDolbyDigital, Theme.Color(Channel->Dpid(0) ? clrChannelSymbolOn : clrChannelSymbolOff), clrTransparent);
    
    x -= bmAudio.Width() + spacing;
    y = (streamInfoHeight - bmAudio.Height()) / 2;
    pixmapStreamInfo->DrawBitmap(cPoint(x,y), bmAudio, Theme.Color(Channel->Apid(1) ? clrChannelSymbolOn : clrChannelSymbolOff), clrTransparent);
    
    if (Channel->Vpid()) {
        x -= bmTeletext.Width() + spacing;
        y = (streamInfoHeight - bmTeletext.Height()) / 2;
        pixmapStreamInfo->DrawBitmap(cPoint(x,y), bmTeletext, Theme.Color(Channel->Tpid() ? clrChannelSymbolOn : clrChannelSymbolOff), clrTransparent);
    } else if (Channel->Apid(0)) {
        x -= bmRadio.Width() + spacing;
        y = (streamInfoHeight - bmTeletext.Height()) / 2;
        pixmapStreamInfo->DrawBitmap(cPoint(x,y), bmRadio, Theme.Color(clrChannelSymbolOn), clrTransparent);
    }
    
}

void cNopacityDisplayChannel::DrawScreenResolution(void) {
    int spacing = 10;
    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;

    if (!pixmapScreenResolution) {
        int x = infoX + infoWidth - config.resolutionIconSize - 2*spacing;
        int y = streamInfoY + streamInfoHeight - config.resolutionIconSize - 3;
        pixmapScreenResolution = osd->CreatePixmap(3, cRect(x, y, config.resolutionIconSize, config.resolutionIconSize));
        pixmapScreenResolution->Fill(clrTransparent);
        if ((initial)&&(config.channelFadeTime))
            pixmapScreenResolution->SetAlpha(0);
    }
    
    cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);
    if (screenWidth != lastScreenWidth) {
        cImageLoader imgLoader;
        cString iconName("");
        switch (screenWidth) {
            case 1920:
            case 1440:
                iconName = "hd1080i";
                break;
            case 1280:
                iconName = "hd720p";
                break;
            case 720:
                iconName = "sd576i";
                break;
            default:
                iconName = "sd576i";
                break;
        }
        if (imgLoader.LoadIcon(*iconName, config.resolutionIconSize)) {
            pixmapScreenResolution->DrawImage(cPoint(0,0), imgLoader.GetImage());
        }
        lastScreenWidth = screenWidth;
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
        if (imgLoader.LoadIcon("signal", signalWidth, signalHeight)) {
            cImage imgSignal = imgLoader.GetImage();
            signalWidth = imgSignal.Width();
            signalHeight = imgSignal.Height();
            int signalMeterY = streamInfoY + (streamInfoHeight - 2*signalHeight - 5)/2;
            fontInfoline = cFont::CreateFont(config.fontName, signalHeight - 2);
            int labelWidth = max(fontInfoline->Width(*signalStrength), fontInfoline->Width(*signalQuality)) + 2;
            signalX = streamInfoHeight / 2 + labelWidth;
            showSignal = true;
            pixmapSignalStrength = osd->CreatePixmap(3, cRect(infoX + signalX, signalMeterY + 2, signalWidth + 2, signalHeight + 2));
            pixmapSignalQuality  = osd->CreatePixmap(3, cRect(infoX + signalX, signalMeterY + signalHeight + 5, signalWidth + 2, signalHeight + 2));
            pixmapSignalMeter    = osd->CreatePixmap(4, cRect(infoX + signalX + 1, signalMeterY + 3, signalWidth, 2*signalHeight + 3));
            pixmapSignalLabel    = osd->CreatePixmap(3, cRect(infoX + streamInfoHeight / 2, signalMeterY + 2, labelWidth, 2*signalHeight + 3));
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

void cNopacityDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    pixmapLogo->Fill(clrTransparent);
    pixmapChannelInfo->Fill(clrTransparent);
    
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
            DrawIcons(Channel);
            ChannelNumber = cString::sprintf("%d%s", Channel->Number(), Number ? "-" : "");
        } else
            groupSep = true;
        } else if (Number) {
        ChannelNumber = cString::sprintf("%d-", Number);
    } else {
        ChannelName = ChannelString(NULL, 0);
    }

    cString channelString = cString::sprintf("%s %s", *ChannelNumber, *ChannelName);

    if (!groupSep) {
        pixmapChannelInfo->DrawText(cPoint(channelInfoHeight/2, (channelInfoHeight-fontHeader->Height())/2), channelString, Theme.Color(clrChannelHead), clrTransparent, fontHeader);
        
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(*ChannelName)) {
            pixmapLogo->DrawImage(cPoint(config.logoBorder, (height-config.logoHeight)/2), imgLoader.GetImage());
        }
    } else {
        if (withInfo) {
            pixmapProgressBar->Fill(clrTransparent);
            pixmapEPGInfo->Fill(clrTransparent);
            pixmapEPGInfo->DrawText(cPoint(channelInfoHeight/2, (epgInfoHeight-fontHeader->Height())/2 - fontHeader->Height()/2), channelString, Theme.Color(clrChannelHead), clrTransparent, fontHeader);
        } else {
            pixmapChannelInfo->DrawText(cPoint(channelInfoHeight/2, (channelInfoHeight-fontHeader->Height())/2), channelString, Theme.Color(clrChannelHead), clrTransparent, fontHeader);
        }
        cImageLoader imgLoader;
        if (imgLoader.LoadIcon("Channelseparator", config.logoHeight)) {
            pixmapLogo->DrawImage(cPoint(config.logoBorder + (config.logoWidth - config.logoHeight)/2, (height-config.logoHeight)/2), imgLoader.GetImage());
        }
    }
}

void cNopacityDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
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
                int seen = (int)(time(NULL) - e->StartTime())/60;
                strSeen = cString::sprintf("%d/%dmin", seen, e->Duration()/60);
            } else {
                strSeen = cString::sprintf("%dmin", e->Duration()/60);
            }
            int seenWidth = fontEPG->Width(*strSeen);
            int space = infoWidth - 9*indent - seenWidth - startTimeWidth - config.resolutionIconSize;
            cString strEPG;
            if (space < epgWidth) {
                cTextWrapper epgInfoWrapper(e->Title(), fontEPG, space);
                strEPG = epgInfoWrapper.GetLine(0);
                strEPG = cString::sprintf("%s...", *strEPG);
            } else {
                strEPG = e->Title();
            }
            cString strEPGShort("");
            int spaceShort = infoWidth - 6*indent - startTimeWidth - config.resolutionIconSize;
            if (spaceShort < epgWidthShort) {
                cTextWrapper epgInfoWrapper(e->ShortText(), fontEPGSmall, spaceShort);
                strEPGShort = epgInfoWrapper.GetLine(0);
                strEPGShort = cString::sprintf("%s...", *strEPGShort);
            } else {
                strEPGShort = e->ShortText();
            }
            tColor fontColor = (i==0)?Theme.Color(clrChannelEPG):Theme.Color(clrChannelEPGInfo);
            pixmapEPGInfo->DrawText(cPoint(indent, y * epgInfoLineHeight), *startTime, fontColor, clrTransparent, fontEPG);
            pixmapEPGInfo->DrawText(cPoint(2 * indent + startTimeWidth, y * epgInfoLineHeight), *strEPG, fontColor, clrTransparent, fontEPG);
            pixmapEPGInfo->DrawText(cPoint(2 * indent + startTimeWidth, (y+1) * epgInfoLineHeight + 3), *strEPGShort, fontColor, clrTransparent, fontEPGSmall);
            int x = infoWidth - indent - seenWidth - config.resolutionIconSize - indent;
            pixmapEPGInfo->DrawText(cPoint(x, y * epgInfoLineHeight), *strSeen, fontColor, clrTransparent, fontEPG);
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
        int barFullWidth = pixmapProgressBar->ViewPort().Width() - 20 - barHeight;
        double percentSeen = ((double)Current) / (double)Total;

        pixmapProgressBar->DrawEllipse(cRect(9, 3, barHeight+2, barHeight+2), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawEllipse(cRect(9 + barFullWidth, 3, barHeight+2, barHeight+2), Theme.Color(clrProgressBarBack));
        pixmapProgressBar->DrawRectangle(cRect( 9 + barHeight/2, 3, barFullWidth, barHeight+1), Theme.Color(clrProgressBarBack));

        pixmapProgressBar->DrawEllipse(cRect(10, 4, barHeight, barHeight), Theme.Color(clrProgressBarBlend));
        if (Current > 0) {
            tColor colAct = DrawProgressbarBackground(10 + barHeight/2, 4, barFullWidth * percentSeen, barHeight-1);
            pixmapProgressBar->DrawEllipse(cRect(10 + barFullWidth * percentSeen, 4, barHeight, barHeight), colAct);
            //pixmapProgressBar->DrawRectangle(cRect(   10 + barHeight/2, 4, barFullWidth * percentSeen, barHeight-1), Theme.Color(clrProgressBar));
        }
    }
}

tColor cNopacityDisplayChannel::DrawProgressbarBackground(int left, int top, int width, int height) {

    tColor clr1 = Theme.Color(clrProgressBar);
    tColor clr2 = Theme.Color(clrProgressBarBlend);
    tColor clr = 0x00000000;
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
    DrawDate();
    if (!groupSep)
        DrawScreenResolution();
    if (config.displaySignalStrength && showSignal) {
        DrawSignal();
    }
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
        pixmapLogo->SetAlpha(Alpha);
        pixmapChannelInfo->SetAlpha(Alpha);
        pixmapDate->SetAlpha(Alpha);
        if (withInfo) {
            pixmapBackgroundMiddle->SetAlpha(Alpha);
            pixmapProgressBar->SetAlpha(Alpha);
            pixmapEPGInfo->SetAlpha(Alpha);
        }
        pixmapStreamInfo->SetAlpha(Alpha);
        if (pixmapScreenResolution)
            pixmapScreenResolution->SetAlpha(Alpha);
        if (config.displaySignalStrength && showSignal) {
            pixmapSignalStrength->SetAlpha(Alpha);
            pixmapSignalQuality->SetAlpha(Alpha);
            pixmapSignalMeter->SetAlpha(Alpha);
            pixmapSignalLabel->SetAlpha(Alpha);
        }
        if (Running())
            osd->Flush();
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime) {
            break;
        }
    }
}