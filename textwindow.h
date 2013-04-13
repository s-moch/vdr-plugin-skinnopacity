#ifndef __NOPACITY_TEXTWINDOW_H
#define __NOPACITY_TEXTWINDOW_H
class cNopacityTextWindow : public cThread {
private:
    cOsd *osd;
    cFont *font;
    cPixmap *pixmapBackground;
    cPixmap *pixmap;
    cRect *geometry;
    cRect *vidWin;
    int oldVidWinHeight;
    const char *text;
    cTextWrapper twText;
    bool CreatePixmap(int border);
    void DrawText(int border);
    void DoSleep(int duration);
    virtual void Action(void);
public:
    cNopacityTextWindow(cOsd *osd, cFont *font, cRect *vidWin);
    virtual ~cNopacityTextWindow(void);
    void SetGeometry(cRect *geo) {geometry = geo;};
    void SetText(const char *Text) {text = Text;};
};

#endif //__NOPACITY_TEXTWINDOW_H