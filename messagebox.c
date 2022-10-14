#include "config.h"
#include "helpers.h"
#include "imageloader.h"
#include "messagebox.h"

cNopacityMessageBox::cNopacityMessageBox(cOsd *Osd, const cRect &Rect, eMessageType Type, const char *Text, bool isMenuMessage) {
  osd = Osd;
  pixmap = CreatePixmap(osd, "pixmap", 7, Rect);
  pixmapBackground = CreatePixmap(osd, "pixmapBackground", 6, Rect);

  if (!pixmap || !pixmapBackground)
      return;

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

  PixmapFill(pixmap, clrTransparent);
  if (config.GetValue("displayType") == dtGraphical) {
    PixmapFill(pixmapBackground, clrTransparent);
    cImage *imgBack = imgCache->GetSkinElement(seType);
    if (imgBack) {
      pixmapBackground->DrawImage(cPoint(0, 0), *imgBack);
    }
  } else {
    PixmapFill(pixmapBackground, col);
    if (config.GetValue("displayType") == dtBlending) {
      cImage imgBack = imgCache->GetBackground(Theme.Color(clrMessageBlend), col, Rect.Width()-2, Rect.Height()-2, true);
      pixmapBackground->DrawImage(cPoint(1, 1), imgBack);
    }
    if (config.GetValue("roundedCorners")) {
      DrawRoundedCornersWithBorder(pixmapBackground, col, config.GetValue("cornerRadius"), Rect.Width(), Rect.Height());
    }
  }
  cFont *font = isMenuMessage ? fontManager->menuMessage : fontManager->messageText;
  pixmap->DrawText(cPoint((Rect.Width() - font->Width(Text)) / 2,
			  (Rect.Height() - font->Height()) / 2),
		   Text,
		   colFont,
		   clrTransparent,
		   font);
}

cNopacityMessageBox::~cNopacityMessageBox() {
  osd->DestroyPixmap(pixmap);
  osd->DestroyPixmap(pixmapBackground);
}

void cNopacityMessageBox::SetAlpha(int Alpha) {
  PixmapSetAlpha(pixmap, Alpha);
  PixmapSetAlpha(pixmapBackground, Alpha);
}
