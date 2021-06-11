#include "displaymessage.h"
#include "config.h"
#include "imageloader.h"
#include "helpers.h"

cNopacityDisplayMessage::cNopacityDisplayMessage(void) : cThread("DisplayMessage") {
    fadeout = false;
    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->messageHeight - config.GetValue("messageBorderBottom");
    int left = geoManager->osdLeft + (geoManager->osdWidth - geoManager->messageWidth) / 2;
    osd = CreateOsd(left, top, geoManager->messageWidth, geoManager->messageHeight);
    messageBox = NULL;
}

cNopacityDisplayMessage::~cNopacityDisplayMessage() {
    if (config.GetValue("messageFadeOutTime")) {
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
    delete messageBox;
    delete osd;
}

void cNopacityDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
    delete messageBox;
    messageBox = new cNopacityMessageBox(osd, cRect(0, 0, geoManager->messageWidth, geoManager->messageHeight), Type, Text);
    if (config.GetValue("messageFadeTime")) {
        messageBox->SetAlpha(0);
        Start();
    }
}

void cNopacityDisplayMessage::Flush(void) {
    if (Running())
        return;
    osd->Flush();
}

void cNopacityDisplayMessage::Action(void) {
    int x = (fadeout) ? 255 : 0;
    int FadeTime = (fadeout) ? config.GetValue("messageFadeOutTime") : config.GetValue("messageFadeTime");
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
        messageBox->SetAlpha(Alpha);
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
