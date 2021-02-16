#include "displaymessage.h"
#include "config.h"
#include "imageloader.h"
#include "helpers.h"

cNopacityDisplayMessage::cNopacityDisplayMessage(void) {
    FadeTime = config.GetValue("messageFadeTime");
    FrameTime = FadeTime / 10;
    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->messageHeight - config.GetValue("messageBorderBottom");
    int left = geoManager->osdLeft + (geoManager->osdWidth - geoManager->messageWidth) / 2;
    osd = CreateOsd(left, top, geoManager->messageWidth, geoManager->messageHeight);
    messageBox = NULL;
}

cNopacityDisplayMessage::~cNopacityDisplayMessage() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    delete messageBox;
    delete osd;
}

void cNopacityDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
    delete messageBox;
    messageBox = new cNopacityMessageBox(osd, cRect(0, 0, geoManager->messageWidth, geoManager->messageHeight), Type, Text);
    if (FadeTime) {
        messageBox->SetAlpha(0);
        Start();
    }
}

void cNopacityDisplayMessage::Flush(void) {
    osd->Flush();
}

void cNopacityDisplayMessage::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = std::min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        messageBox->SetAlpha(Alpha);
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
