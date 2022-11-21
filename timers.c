#include "timers.h"
#include "config.h"
#include "imageloader.h"
#include "helpers.h"
#include <vdr/recording.h>

cNopacityTimer::cNopacityTimer(cOsd *osd, const cTimer *timer, const cFont *font, const cFont *fontLarge) {
    this->osd = osd;
    this->timer = timer;
    this->font = font;
    this->fontLarge = fontLarge;
    isTimerConflict = false;
}

cNopacityTimer::cNopacityTimer(cOsd *osd, int numConflicts, const cFont *font, const cFont *fontLarge) {
    this->osd = osd;
    this->numConflicts = numConflicts;
    this->font = font;
    this->fontLarge = fontLarge;
    isTimerConflict = true;
}

cNopacityTimer::~cNopacityTimer(void) {
    osd->DestroyPixmap(pixmapBackground);
    osd->DestroyPixmap(pixmap);
    osd->DestroyPixmap(pixmapLogo);
    osd->DestroyPixmap(pixmapText);
}

void cNopacityTimer::SetGeometry(int width, int y) {
    this->width = width;
    this->y = y;
}

void cNopacityTimer::SetAlpha(int alpha) {
    PixmapSetAlpha(pixmapBackground, alpha);
    PixmapSetAlpha(pixmap, alpha);
    PixmapSetAlpha(pixmapLogo, alpha);
    PixmapSetAlpha(pixmapText, alpha);
}

void cNopacityTimer::Show(void) {
    pixmapBackground->SetLayer(2);
    pixmap->SetLayer(3);
    pixmapLogo->SetLayer(4);
    pixmapText->SetLayer(5);
}

void cNopacityTimer::Hide(void) {
    pixmapBackground->SetLayer(-1);
    pixmap->SetLayer(-1);
    pixmapLogo->SetLayer(-1);
    pixmapText->SetLayer(-1);
}

void cNopacityTimer::CreateDate(void) {
    if (timer->Recording()) {
        Date = cString::sprintf("-%s", *TimeString(timer->StopTime()));
    } else {
        time_t Now = time(NULL);
        cString Today = WeekDayName(Now);
        cString Time = TimeString(timer->StartTime());
        cString Day = WeekDayName(timer->StartTime());
        if (timer->StartTime() > Now + 6 * SECSINDAY)
            Date = DayDateTime(timer->StartTime());
        else if (strcmp(Day, Today) != 0)
            Date = cString::sprintf("%s %s", *Day, *Time);
        else
            Date = Time;
        if (timer->Flags() & tfVps)
            Date = cString::sprintf("VPS %s", *Date);
    }
}

void cNopacityTimer::CreateShowName(void) {
    const cEvent *Event = timer->Event();
    cString title("");
    if (Event) {
        title = Event->Title();
    } else {
        const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(timer->File(), FOLDERDELIMCHAR);
        if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
            File++;
        else
            File = timer->File();
        title = File;
        std::string name(title);
        size_t index = name.find_last_of(FOLDERDELIMCHAR);
        if (index != std::string::npos)
            title = File + index + 1;
    }
    showName.Set(*title, font, width-10);
}

void cNopacityTimer::CreateConflictText(void) {
    cString conflictText;
    if (numConflicts == 1) {
        conflictText = cString::sprintf("%d Timer %s", numConflicts, tr("conflict"));
    } else {
        conflictText = cString::sprintf("%d Timer %s", numConflicts, tr("conflicts"));
    }
    showName.Set(*conflictText, fontLarge, width-10);
}

void cNopacityTimer::CalculateHeight(int space) {
    int numLines = showName.Lines();
    if (isTimerConflict) {
        int lineHeight = fontLarge->Height();
        height = numLines * lineHeight + 2 * space;
    } else {
        int lineHeight = font->Height();
        int channelLogoHeight = geoManager->menuTimersLogoHeight;
        if (config.GetValue("showTimers") >= 1) {
            const cChannel *Channel = timer->Channel();
            if (Channel) {
                if (config.GetValue("showTimers") == 1) {
                    cImage *logo = imgCache->GetLogo(ctLogoTimer, Channel);
                    if (logo) {
                        height = channelLogoHeight + (numLines + 1) * lineHeight + 2 * space;
                        return;
                    }
                }
                cTextWrapper channel;
                channel.Set(Channel->Name(), fontLarge, width - 10);
                int lines = channel.Lines();
                height = numLines * lineHeight + fontLarge->Height() * (lines + 1) + 2 * space;
                return;
            }
        }
        height = channelLogoHeight + (numLines + 1) * lineHeight + 2 * space;
    }
}

void cNopacityTimer::CreatePixmaps(int x) {
    pixmapBackground = CreatePixmap(osd, "pixmapBackground", 2, cRect(x, y, width, height));
    pixmap = CreatePixmap(osd, "pixmap", 3, cRect(x, y, width, height));
    pixmapLogo = CreatePixmap(osd, "pixmapLogo", 4, cRect(x, y, width, height));
    pixmapText = CreatePixmap(osd, "pixmapText", 5, cRect(x, y, width, height));
}

void cNopacityTimer::Render(void) {
    PixmapFill(pixmapBackground, clrTransparent);
    PixmapFill(pixmapText, clrTransparent);
    if (!pixmap || !pixmapText)
        return;

    if (isTimerConflict) {
        PixmapFill(pixmapLogo, clrTransparent);
        PixmapFill(pixmap, Theme.Color(clrDiskAlert));
        if (config.GetValue("displayType") == dtBlending) {
            cImage imgBack = imgCache->GetBackground(Theme.Color(clrDiskAlert), Theme.Color(clrMenuItemHigh), width - 2, height - 2);
            pixmap->DrawImage(cPoint(1, 1), imgBack);
        } else {
            pixmap->DrawRectangle(cRect(1, 1, width - 2, height - 2), Theme.Color(clrDiskAlert));
        }
        int numLines = showName.Lines();
        int textWidth = 0;
        int x = 0;
        int y = 5;
        for (int line = 0; line < numLines; line++) {
            textWidth = fontLarge->Width(showName.GetLine(line));
            x = (width - textWidth) / 2;
            y += line * fontLarge->Height();
            pixmapText->DrawText(cPoint(x, y), showName.GetLine(line), Theme.Color(clrMenuFontTimersHeader), clrTransparent, fontLarge);
        }
    } else {
        int y = DrawLogo();
        if (timer->Recording()) {
            pixmap->Fill(Theme.Color(clrDiskAlert));
            if (config.GetValue("displayType") == dtBlending) {
                cImage imgBack = imgCache->GetBackground(Theme.Color(clrDiskAlert), Theme.Color(clrMenuItemHigh), width - 2, height - 2);
                pixmap->DrawImage(cPoint(1, 1), imgBack);
            } else {
                pixmap->DrawRectangle(cRect(1, 1, width - 2, height - 2), Theme.Color(clrDiskAlert));
            }
        } else {
            pixmap->Fill(Theme.Color(clrMenuBorder));
            if (config.GetValue("displayType") == dtBlending) {
                cImage imgBack = imgCache->GetBackground(Theme.Color(clrTimersBack), Theme.Color(clrTimersBackBlend), width - 2, height - 2);
                pixmap->DrawImage(cPoint(1, 1), imgBack);
            } else {
                pixmap->DrawRectangle(cRect(1, 1, width - 2, height - 2), Theme.Color(clrTimersBack));
            }
        }

        pixmapText->DrawText(cPoint(5, y), *Date, Theme.Color(clrMenuFontTimersHeader), clrTransparent, fontLarge);

        int lineHeight = font->Height();
        int yStart = y + lineHeight + 3;
        int numLines = showName.Lines();
        for (int line = 0; line < numLines; line++)
            pixmapText->DrawText(cPoint(5, yStart + line * (lineHeight - 2)), showName.GetLine(line), Theme.Color(clrMenuFontTimers), clrTransparent, font);
    }
}

int cNopacityTimer::DrawLogo(void) {
    if (!pixmapLogo)
       return 0;

    pixmapLogo->Fill(clrTransparent);
    int logoHeight = geoManager->menuTimersLogoHeight;
    const cChannel *Channel = timer->Channel();
    if (Channel) {
        if (config.GetValue("showTimers") == 1) {
            cImage *logo = imgCache->GetLogo(ctLogoTimer, Channel);
            if (logo) {
                pixmapLogo->DrawImage(cPoint((width - logo->Width()) / 2, (logoHeight - logo->Height()) / 2), *logo);
		return logoHeight;
            }
        }
        cTextWrapper channel;
        channel.Set(Channel->Name(), fontLarge, width - 10);
        int lines = channel.Lines();
        int lineHeight = fontLarge->Height();
        int y = 5;
        for (int line = 0; line < lines; line++) {
            pixmapLogo->DrawText(cPoint((width - fontLarge->Width(channel.GetLine(line))) / 2, y + lineHeight * line), channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, fontLarge);
        }
        return lineHeight * lines + 10;
    }
    return 0;
}
