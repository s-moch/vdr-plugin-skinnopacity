#ifndef __NOPACITY_TEXTWINDOW_H
#define __NOPACITY_TEXTWINDOW_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "services/scraper2vdr.h"

class cNopacityTextWindow : public cThread {
private:
    cOsd *osd;
    cFont *font;
    cFont *fontHeader;
    cPixmap *pixmapBackground = NULL;
    cPixmap *pixmap = NULL;
    cRect geotext;
    cRect *geometryback = NULL;
    cRect *geometry = NULL;
    int drawportHeight = 0;
    cRect *vidWin;
    cRect oldVidWin;
    bool initial = false;
    bool scaledWindow = false;
    cString text = "";
    cTextWrapper twTextTall;
    cTextWrapper twTextFull;
    bool drawTextTall = false;
    bool drawTextFull = false;
    bool hasManualPoster = false;
    cString manualPosterPath = "";
    bool hasPoster = false;
    ScraperGetPoster posterScraper2Vdr;
    int posterWidth, posterHeight;
    bool SetTextScroller(int left);
    void CreatePixmapWindow(void);
    void CreatePixmapFullScreen(void);
    void DrawText(int left);
    void DrawPoster(void);
    int DrawTextWrapper(const char *text, int width, int top, int x, const cFont *font, tColor color, int maxHeight);
    void DrawTextWrapperFloat(const char *text, int widthSmall, int widthFull, int top, int heightNarrow, int x, const cFont *font, tColor color, int maxHeight);
    void DoSleep(int duration);
    virtual void Action(void);
    void ScaleVideoWindow(void);
public:
    cNopacityTextWindow(cOsd *osd, cFont *font, cRect *vidWin);
    cNopacityTextWindow(cOsd *osd, cFont *font, cFont *fontHeader);
    virtual ~cNopacityTextWindow(void);
    void SetAlpha(int Alpha = 0);
    void SetGeometry(cRect *geo, int border = 0);
    void SetEvent(const cEvent *event);
    void SetRecording(const cRecording *recording);
    void SetText(cString Text) { text = Text;};
    bool SetManualPoster(const cRecording *recording, bool fullscreen = false);
    void SetPoster(const cEvent *event, const cRecording *recording, bool fullscreen = false);
    void SetInitial(bool initial) { this->initial = initial; };
};

#endif //__NOPACITY_TEXTWINDOW_H
