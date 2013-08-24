#ifndef __NOPACITY_TEXTWINDOW_H
#define __NOPACITY_TEXTWINDOW_H
class cNopacityTextWindow : public cThread {
private:
    cOsd *osd;
    cFont *font;
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
    bool hasPoster;
    TVScraperGetPoster poster;
    int posterWidth, posterHeight;
    bool SetTextScroller(int border, int left);
    void CreatePixmap(void);
    void DrawText(int border, int left);
    void DrawPoster(int border);
    void DoSleep(int duration);
    virtual void Action(void);
    void ScaleVideoWindow(void);
public:
    cNopacityTextWindow(cOsd *osd, cFont *font, cRect *vidWin);
    virtual ~cNopacityTextWindow(void);
    void SetGeometry(cRect *geo) {geometry = geo;};
    void SetText(cString Text) {text = Text;};
    void SetPoster(const cEvent *event, bool isRecording);
};

#endif //__NOPACITY_TEXTWINDOW_H