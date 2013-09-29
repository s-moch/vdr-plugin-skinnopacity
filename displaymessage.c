#include "displaymessage.h"

cNopacityDisplayMessage::cNopacityDisplayMessage(cImageCache *imgCache) {
    this->imgCache = imgCache;
    config.setDynamicValues();
    int top = geoManager->osdTop + geoManager->osdHeight - geoManager->messageHeight - config.messageBorderBottom;
    int left = (geoManager->osdLeft + geoManager->osdWidth - geoManager->messageWidth) / 2;
    osd = CreateOsd(left, top, geoManager->messageWidth, geoManager->messageHeight);
    pixmap = osd->CreatePixmap(2, cRect(0, 0, geoManager->messageWidth, geoManager->messageHeight));
    pixmapBackground = osd->CreatePixmap(1, cRect(0, 0, geoManager->messageWidth, geoManager->messageHeight));
    if (config.messageFadeTime) {
        pixmap->SetAlpha(0);
        pixmapBackground->SetAlpha(0);
    }
    FrameTime = config.messageFrameTime;
    FadeTime = config.messageFadeTime;
}

cNopacityDisplayMessage::~cNopacityDisplayMessage() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmap);
    osd->DestroyPixmap(pixmapBackground);
    delete osd;
}

void cNopacityDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
    tColor col = Theme.Color(clrMessageStatus);
    tColor colFont = Theme.Color(clrMessageFontStatus);
    switch (Type) {
        case mtStatus: 
            col = Theme.Color(clrMessageStatus);
            colFont = Theme.Color(clrMessageFontStatus);
            break;
        case mtInfo:
            col = Theme.Color(clrMessageInfo);
            colFont = Theme.Color(clrMessageFontInfo);
            break;
        case mtWarning:
            col = Theme.Color(clrMessageWarning);
            colFont = Theme.Color(clrMessageFontWarning);
            break;
        case mtError:
            col = Theme.Color(clrMessageError);
            colFont = Theme.Color(clrMessageFontError);
            break;
    }
    pixmapBackground->Fill(clrBlack);
    pixmap->Fill(col);
    if (config.doBlending) {
        cImage imgBack = imgCache->GetBackground(Theme.Color(clrMessageBlend), col, geoManager->messageWidth-2, geoManager->messageHeight-2, true);
        pixmap->DrawImage(cPoint(1, 1), imgBack);
    }
    if (config.roundedCorners) {
        DrawRoundedCornersWithBorder(pixmap, col, config.cornerRadius, geoManager->messageWidth, geoManager->messageHeight, pixmapBackground);
    }
    int textWidth = fontManager->messageText->Width(Text);
    pixmap->DrawText(cPoint((geoManager->messageWidth - textWidth) / 2, (geoManager->messageHeight - fontManager->messageText->Height()) / 2), Text, colFont, (config.doBlending)?clrTransparent:col, fontManager->messageText);
    if (config.messageFadeTime)
        Start();
}

void cNopacityDisplayMessage::Flush(void) {
    osd->Flush();
}

void cNopacityDisplayMessage::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        pixmapBackground->SetAlpha(Alpha);
        pixmap->SetAlpha(Alpha);
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
