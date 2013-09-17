#include "displaymessage.h"

cNopacityDisplayMessage::cNopacityDisplayMessage(void) {
    config.setDynamicValues();
    height = cOsd::OsdHeight() * config.messageHeight / 100;
    int top = cOsd::OsdTop() + cOsd::OsdHeight() - height - config.messageBorderBottom;
    width = cOsd::OsdWidth() * config.messageWidth / 100;
    int left = (cOsd::OsdLeft() + cOsd::OsdWidth() - width) / 2;
    osd = CreateOsd(left, top, width, height);
    pixmap = osd->CreatePixmap(2, cRect(0, 0, width, height));
    pixmapBackground = osd->CreatePixmap(1, cRect(0, 0, width, height));
    if (config.messageFadeTime) {
        pixmap->SetAlpha(0);
        pixmapBackground->SetAlpha(0);
    }
    font = cFont::CreateFont(config.fontName, height / 4 + 15 + config.fontMessage);
    FrameTime = config.messageFrameTime;
    FadeTime = config.messageFadeTime;
}

cNopacityDisplayMessage::~cNopacityDisplayMessage() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    osd->DestroyPixmap(pixmap);
    osd->DestroyPixmap(pixmapBackground);
    delete font;
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
        cImageLoader imgLoader;
        imgLoader.DrawBackground2(Theme.Color(clrMessageBlend), col, width-2, height-2);
        pixmap->DrawImage(cPoint(1, 1), imgLoader.GetImage());
    }
    if (config.roundedCorners) {
        DrawRoundedCornersWithBorder(pixmap, col, config.cornerRadius, width, height, pixmapBackground);
    }
    int textWidth = font->Width(Text);
    pixmap->DrawText(cPoint((width - textWidth) / 2, (height - font->Height()) / 2), Text, colFont, (config.doBlending)?clrTransparent:col, font);
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
