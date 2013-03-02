#ifndef __NOPACITY_MENUDETAILVIEW_H
#define __NOPACITY_MENUDETAILVIEW_H

class cNopacityMenuDetailView {
protected:
    cOsd *osd;
    bool hasScrollbar;
    int x, width, height, top;
    int headerHeight;
    int contentHeight;
    int contentDrawPortHeight;
    int border;
    cFont *font, *fontHeader, *fontHeaderLarge;
    cPixmap *pixmapHeader;
    cPixmap *pixmapLogo;
    cPixmap *pixmapContent;
    cTextWrapper content;
    int DrawTextWrapper(cTextWrapper *wrapper, int top);
public:
    cNopacityMenuDetailView(cOsd *osd);
    virtual ~cNopacityMenuDetailView(void);
    void SetGeometry(int x, int width, int height, int top, int contentBorder, int headerHeight);
    virtual void SetFonts(void) = 0;
    bool Scrollable(void) {return hasScrollbar;}
    double ScrollbarSize(void);
    double Offset(void);
    bool Scroll(bool Up, bool Page);
    virtual void SetContent(void) = 0;
    virtual void SetContentHeight(void) = 0;
    virtual void CreatePixmaps(void) = 0;
    virtual void Render(void) = 0;
};

class cNopacityMenuDetailEventView : public cNopacityMenuDetailView {
private:
    const cEvent *event;
    cTextWrapper reruns;
    int numEPGPics;
    void DrawHeader(void);
    void LoadReruns(void);
    int HeightEPGPics(void);
    void DrawEPGPictures(int height);
public:
    cNopacityMenuDetailEventView(cOsd *osd, const cEvent *Event);
    virtual ~cNopacityMenuDetailEventView(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void SetFonts(void);
    void Render(void);
};

class cNopacityMenuDetailRecordingView : public cNopacityMenuDetailView {
private:
    const cRecording *recording;
    const cRecordingInfo *info;
    cTextWrapper additionalInfo;
    void DrawHeader(void);
    void LoadRecordingInformation(void);
    int ReadSizeVdr(const char *strPath);
    std::vector<std::string> epgpics;
    bool LoadEPGPics(void);
    int HeightEPGPics(void);
    void DrawEPGPictures(int height);
public:
    cNopacityMenuDetailRecordingView(cOsd *osd, const cRecording *Recording);
    virtual ~cNopacityMenuDetailRecordingView(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void SetFonts(void);
    void Render(void);
};

class cNopacityMenuDetailTextView : public cNopacityMenuDetailView {
private:
    const char *text;
public:
    cNopacityMenuDetailTextView(cOsd *osd, const char *text);
    virtual ~cNopacityMenuDetailTextView(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void SetFonts(void);
    void Render(void);
};
#endif //__NOPACITY_MENUDETAILVIEW_H