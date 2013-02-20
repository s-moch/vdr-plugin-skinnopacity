#include "timers.h"

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
    osd->DestroyPixmap(pixmap);
    osd->DestroyPixmap(pixmapLogo);
}

void cNopacityTimer::SetGeometry(int width, int y) {
    this->width = width;
    this->y = y;
}

void cNopacityTimer::SetAlpha(int alpha) {
    pixmap->SetAlpha(alpha);
    pixmapLogo->SetAlpha(alpha);
}

void cNopacityTimer::Show(void) {
    pixmap->SetLayer(2);
    pixmapLogo->SetLayer(3);
}

void cNopacityTimer::Hide(void) {
    pixmap->SetLayer(-1);
    pixmapLogo->SetLayer(-1);
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
        height = numLines * lineHeight + 2*space;    
    } else {
        int lineHeight = font->Height();
        height = config.timersLogoHeight + (numLines +1)* lineHeight + 2*space;    
    }
}

void cNopacityTimer::CreatePixmaps(int x) {
    pixmap = osd->CreatePixmap(2, cRect(x, y, width, height));
    pixmapLogo = osd->CreatePixmap(3, cRect(x, y, width, height));
}

void cNopacityTimer::Render(void) {
    cImageLoader imgLoader;
    if (isTimerConflict) {
        pixmapLogo->Fill(clrTransparent);
        pixmap->Fill(Theme.Color(clrDiskAlert));
        imgLoader.DrawBackground(Theme.Color(clrDiskAlert), Theme.Color(clrMenuItemHigh), width-2, height-2);
        pixmap->DrawImage(cPoint(1,1), imgLoader.GetImage());
        int numLines = showName.Lines();
        int textWidth = 0;
        int x = 0;
        int y = 5;
        for (int line=0; line<numLines; line++) {
            textWidth = fontLarge->Width(showName.GetLine(line));
            x = (width - textWidth)/2;
            y += line*fontLarge->Height();
            pixmap->DrawText(cPoint(x, y), showName.GetLine(line), Theme.Color(clrMenuFontTimersHeader), clrTransparent, fontLarge);
        }
    } else {
        DrawLogo();
        if (timer->Recording()) {
            pixmap->Fill(Theme.Color(clrDiskAlert));
            imgLoader.DrawBackground(Theme.Color(clrDiskAlert), Theme.Color(clrMenuItemHigh), width-2, height-2);
            pixmap->DrawImage(cPoint(1,1), imgLoader.GetImage());
        } else {
            pixmap->Fill(Theme.Color(clrMenuBorder));
            imgLoader.DrawBackground(Theme.Color(clrMenuItemHighBlend), Theme.Color(clrMenuItemHigh), width-2, height-2);
            pixmap->DrawImage(cPoint(1,1), imgLoader.GetImage());
        }

        pixmap->DrawText(cPoint(5, config.timersLogoHeight), *Date, Theme.Color(clrMenuFontTimersHeader), clrTransparent, fontLarge);
        
        int lineHeight = font->Height();
        int yStart = config.timersLogoHeight + lineHeight + 3;
        int numLines = showName.Lines();
        for (int line=0; line<numLines; line++)
            pixmap->DrawText(cPoint(5, yStart+line*(lineHeight-2)), showName.GetLine(line), Theme.Color(clrMenuFontTimers), clrTransparent, font);
    }   
}

void cNopacityTimer::DrawLogo(void) {
    pixmapLogo->Fill(clrTransparent);
    int logoWidth = config.timersLogoWidth;
    int logoHeight = config.timersLogoHeight;
    const cChannel *Channel = timer->Channel();
    if (Channel && Channel->Name()) {
        cImageLoader imgLoader;
        if (imgLoader.LoadLogo(Channel->Name(), logoWidth, logoHeight)) {
            pixmapLogo->DrawImage(cPoint((width - logoWidth)/2, 1), imgLoader.GetImage());
        } else {
            cTextWrapper channel;
            channel.Set(Channel->Name(), fontLarge, width - 10);
            int lines = channel.Lines();
            int lineHeight = fontLarge->Height();
            int y = 1;
            for (int line = 0; line < lines; line++) {
                pixmapLogo->DrawText(cPoint((width - fontLarge->Width(channel.GetLine(line)))/2, y+lineHeight*line), channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, fontLarge);
            }   
        }
    }
}
