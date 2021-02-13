#include "config.h"
#include "helpers.h"
#include "imageloader.h"
#include "messagebox.h"

cNopacityMessageBox::cNopacityMessageBox(cOsd *Osd, cImageCache *imgCache, const cRect &Rect, eMessageType Type, const char *Text) {
  osd = Osd;
  pixmap = osd->CreatePixmap(7, Rect);
  pixmapBackground = osd->CreatePixmap(6, Rect);

  tColor col;
  tColor colFont;
  eSkinElementType seType;
  switch (Type) {
  case mtInfo:
    col = Theme.Color(clrMessageInfo);
    colFont = Theme.Color(clrMessageFontInfo);
    seType = seMessageInfo;
    break;
  case mtWarning:
    col = Theme.Color(clrMessageWarning);
    colFont = Theme.Color(clrMessageFontWarning);
    seType = seMessageWarning;
    break;
  case mtError:
    col = Theme.Color(clrMessageError);
    colFont = Theme.Color(clrMessageFontError);
    seType = seMessageError;
    break;
  default: // case mtStatus:
    col = Theme.Color(clrMessageStatus);
    colFont = Theme.Color(clrMessageFontStatus);
    seType = seMessageStatus;
    break;
  }

  pixmap->Fill(clrTransparent);
  if (config.GetValue("displayType") == dtGraphical) {
    pixmapBackground->Fill(clrTransparent);
    cImage *imgBack = imgCache->GetSkinElement(seType);
    if (imgBack) {
      pixmapBackground->DrawImage(cPoint(0, 0), *imgBack);
    }
  } else {
    pixmapBackground->Fill(col);
    if (config.GetValue("displayType") == dtBlending) {
      cImage imgBack = imgCache->GetBackground(Theme.Color(clrMessageBlend), col, Rect.Width()-2, Rect.Height()-2, true);
      pixmapBackground->DrawImage(cPoint(1, 1), imgBack);
    }
    if (config.GetValue("roundedCorners")) {
      DrawRoundedCornersWithBorder(pixmapBackground, col, config.GetValue("cornerRadius"), Rect.Width(), Rect.Height());
    }
  }
  int textWidth = fontManager->messageText->Width(Text);
  pixmap->DrawText(cPoint((Rect.Width() - textWidth) / 2,
			  (Rect.Height() - fontManager->messageText->Height()) / 2),
		   Text,
		   colFont,
		   clrTransparent,
		   fontManager->messageText);
}

cNopacityMessageBox::~cNopacityMessageBox() {
  osd->DestroyPixmap(pixmap);
  osd->DestroyPixmap(pixmapBackground);
}

void cNopacityMessageBox::SetAlpha(int Alpha) {
  pixmap->SetAlpha(Alpha);
  pixmapBackground->SetAlpha(Alpha);
}
