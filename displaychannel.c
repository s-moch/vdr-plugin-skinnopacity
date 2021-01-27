#include "config.h"
#include "nopacity.h"
#include "displaychannel.h"
#include "imageloader.h"
#include "helpers.h"

#include <vdr/osd.h>
#include <vdr/menu.h>

cNopacityDisplayChannel::cNopacityDisplayChannel(cImageCache *imgCache, bool WithInfo) {
    if (firstDisplay) {
        imgCache->CreateCacheDelayed();
        firstDisplay = false;
        doOutput = false;
        return;
    } else
        doOutput = true;
    groupSep = false;
    present = NULL;
    following = NULL;
    currentLast = 0;
    channelChange = false;
    initial = true;
    FadeTime = config.GetValue("channelFadeTime");
    FrameTime = FadeTime / 10;

    channelView = new cNopacityDisplayChannelView(imgCache);
    if (!channelView->createOsd()) {
        doOutput = false;
    } else {
        channelView->CreatePixmaps();
        channelView->DrawBackground();
        if (config.GetValue("displaySignalStrength")) {
            channelView->DrawSignalMeter();
        }
    }
}

cNopacityDisplayChannel::~cNopacityDisplayChannel() {
    if (!doOutput)
        return;
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    delete channelView;
}

void cNopacityDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    if (!doOutput)
        return;

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
    if (!doOutput)
        return;
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
    if (!doOutput)
        return;
    channelView->ClearChannelLogo();
    channelView->ClearChannelName();
    channelView->ClearEPGInfo();
    channelView->ClearStatusIcons();
    channelView->ClearSourceInfo();
    channelView->ClearProgressBar();
    channelView->HideSignalMeter();
    channelView->DisplayMessage(Text);
    groupSep = true;
}

void cNopacityDisplayChannel::Flush(void) {
    if (!doOutput)
        return;
    if (initial || channelChange)
        channelView->DrawDate();

    if (present && !groupSep) {
        SetProgressBar(present);
    } else
        channelView->ClearProgressBar();

    if (!groupSep) {
       channelView->ClearEPGInfo();
       channelView->DrawEvents(present, following);
       }

    if (config.GetValue("displayPoster"))
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

    if (initial) {
        if (config.GetValue("channelFadeTime"))
            Start();
    }
    initial = false;
    channelChange = false;
    channelView->Flush();
}

void cNopacityDisplayChannel::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = std::min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        channelView->SetAlpha(Alpha);
        cPixmap::Unlock();
        if (Running())
            channelView->Flush();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime) {
            break;
        }
    }
}
