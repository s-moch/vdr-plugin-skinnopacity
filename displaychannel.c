#include "config.h"
#include "nopacity.h"
#include "displaychannel.h"
#include "imageloader.h"
#include "helpers.h"

#include <vdr/osd.h>
#include <vdr/menu.h>

cNopacityDisplayChannel::cNopacityDisplayChannel(bool WithInfo) : cThread("DisplayChannel") {
    groupSep = false;
    present = NULL;
    following = NULL;
    currentLast = 0;
    channelChange = false;
    fadeout = false;
    initial = true;

    osd = CreateOsd(geoManager->osdLeft, geoManager->osdTop, geoManager->osdWidth, geoManager->osdHeight);
    channelView = new cNopacityDisplayChannelView(osd);
}

cNopacityDisplayChannel::~cNopacityDisplayChannel(void) {
    if (config.GetValue("animation") && config.GetValue("channelFadeOutTime")) {
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
    delete channelView;
    delete osd;
}

void cNopacityDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    channelChange = true;
    groupSep = false;

    cString ChannelNumber("");
    cString ChannelName("");

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

    channelView->ClearChannelLogo();
    channelView->ClearChannelName();
    channelView->ClearEPGInfo();
    channelView->ClearStatusIcons();
    channelView->ClearSourceInfo();
    if (!groupSep) {
        channelView->DrawChannelName(ChannelNumber, ChannelName);
        if (Channel && config.GetValue("logoPosition") != lpNone)
            channelView->DrawChannelLogo(Channel);
        if (Channel)
            channelView->DrawStatusIcons(Channel);
    } else {
        channelView->HideSignalMeter();
        channelView->ClearProgressBar();
        if (Channel)
            channelView->DrawChannelGroups(Channel, ChannelName);
    }

}

void cNopacityDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
    present = Present;
    following = Following;
    channelView->ClearProgressBar();
    if (!groupSep) {
        channelView->DrawProgressbarBackground();
        channelView->ClearEPGInfo();
    }
}

void cNopacityDisplayChannel::SetProgressBar(const cEvent *present) {
    int Current = 0;
    int Total = 0;
    time_t t = time(NULL);
    if (t > present->StartTime())
        Current = t - present->StartTime();
    Total = present->Duration();
    if ((Current > currentLast + 3) || initial || channelChange){
        currentLast = Current;
        channelView->DrawProgressBar(Current, Total);
    }
}


void cNopacityDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
    channelView->DisplayMessage(Type, Text);
}

void cNopacityDisplayChannel::Flush(void) {
    if (Running())
        return;

    channelView->DrawDate();

    if (present && !groupSep) {
        SetProgressBar(present);
    } else
        channelView->ClearProgressBar();

    if (!groupSep) {
       channelView->ClearEPGInfo();
       channelView->DrawEvents(present, following);
       }

    if (config.GetValue("displayPoster") && (initial || channelChange))
       channelView->DrawPoster(present, initial);

    if (!groupSep)
        channelView->DrawScreenResolution();
    else
        channelView->ClearStatusIcons();

    if (config.GetValue("displaySignalStrength") && !groupSep) {
        channelView->ShowSignalMeter();
        channelView->DrawSignal();
    } else
        channelView->HideSignalMeter();

    if (config.GetValue("displaySourceInfo") && !groupSep) {
        channelView->ClearSourceInfo();
	channelView->DrawSourceInfo();
    } else
        channelView->ClearSourceInfo();

    if (initial && config.GetValue("animation") && config.GetValue("channelFadeTime")) {
        channelView->SetAlpha(0);
        Start();
    }
    initial = false;
    channelChange = false;
    osd->Flush();
}

void cNopacityDisplayChannel::Action(void) {
    int x = (fadeout) ? 255 : 0;
    int FadeTime = (fadeout) ? config.GetValue("channelFadeOutTime") : config.GetValue("channelFadeTime");
    int FrameTime = FadeTime / 10;
    uint64_t First = cTimeMs::Now();
    cPixmap::Lock();
    cPixmap::Unlock();
    uint64_t Start = cTimeMs::Now();
    dsyslog ("skinnopacity: First Lock(): %lims\n", Start - First);
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        double t = std::min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = std::abs(x - (int(t * ALPHA_OPAQUE)));
        cPixmap::Lock();
        channelView->SetAlpha(Alpha);
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
