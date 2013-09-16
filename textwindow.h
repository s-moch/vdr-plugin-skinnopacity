#ifndef __NOPACITY_TEXTWINDOW_H
#define __NOPACITY_TEXTWINDOW_H
class cNopacityTextWindow : public cThread {
private:
    cOsd *osd;
    cFont *font;
    cFont *fontHeader;
    cPixmap *pixmapBackground;
    cPixmap *pixmap;
    cRect *geometry;
    int drawportHeight;
    cRect *vidWin;
    cRect oldVidWin;
    bool scaledWindow;
    cString text;
    cTextWrapper twTextTall;
    cTextWrapper twTextFull;
    bool drawTextTall;
    bool drawTextFull;
    bool hasManualPoster;
    cString manualPosterPath;
    bool hasPoster;
    TVScraperGetPoster poster;
    int posterWidth, posterHeight;
    bool SetTextScroller(int border, int left);
    void CreatePixmap(void);
    void CreatePixmapFullScreen(void);
    void DrawText(int border, int left);
    void DrawPoster(int border);
    int DrawTextWrapper(const char *text, int width, int top, int x, const cFont *font, tColor color, int maxHeight = 0);
    int DrawTextWrapperFloat(const char *text, int widthSmall, int widthFull, int top, int heightNarrow, int x, const cFont *font, tColor color, int maxHeight);
    void DoSleep(int duration);
    virtual void Action(void);
    void ScaleVideoWindow(void);
public:
    cNopacityTextWindow(cOsd *osd, cFont *font, cRect *vidWin);
    cNopacityTextWindow(cOsd *osd, cFont *font, cFont *fontHeader);
    virtual ~cNopacityTextWindow(void);
    void SetGeometry(cRect *geo) {geometry = geo;};
    void SetEvent(const cEvent *event);
    void SetRecording(const cRecording *recording);
    void SetText(cString Text) {text = Text;};
    bool SetManualPoster(const cRecording *recording, bool fullscreen = false);
    void SetPoster(const cEvent *event, bool isRecording, bool fullscreen = false);
};

#endif //__NOPACITY_TEXTWINDOW_H