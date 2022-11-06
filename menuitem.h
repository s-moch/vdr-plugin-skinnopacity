#ifndef __NOPACITY_MENUITEM_H
#define __NOPACITY_MENUITEM_H

#include "textwindow.h"
#include <string>
#include "imagecache.h"

class cNopacityMenuItem : public cThread {
protected:
    cOsd *osd;
    cPixmap *pixmapBackground = NULL;
    cPixmap *pixmapStatic = NULL;
    cPixmap *pixmapTextScroller = NULL;
    cPixmap *pixmapForeground = NULL;
    const char *Text;
    bool selectable;
    bool current;
    bool wasCurrent;
    bool scrollable;
    bool drawn;
    cFont *font = NULL;
    cFont *fontSmall = NULL;
    cFont *fontEPGWindow = NULL;
    cFont *fontEPGWindowLarge = NULL;
    int width, height;
    int top, left;
    int spaceMenu;
    int textLeft;
    int iconwidth = 0;
    int index;
    cString *itemTabs;
    int *tabWidth;
    int numTabs;
    cRect *textWindow;
    cNopacityTextWindow *infoTextWindow;
    virtual void SetTextFull(void) {};
    virtual void SetTextShort(void) {};
    virtual void DrawBackground(eSkinElementType type, eSkinElementType fgType);
    void DrawDelimiter(const char *del, const char *icon, eSkinElementType seType);
    void DrawRoundedCorners(tColor borderColor);
    void DrawChannelLogoBackground(void);
    void DrawLogo(const cChannel *Channel, int logoWidth, int logoHeight, cFont *font, bool drawText = false);
    virtual void Action(void);
    void DoSleep(int duration);
public:
    cNopacityMenuItem(cOsd *osd, const char *text, bool sel);
    virtual ~cNopacityMenuItem(void);
    void CreatePixmaps(bool createPixmapFg = true);
    virtual void CreatePixmapTextScroller(int totalWidth, int pixmapLeft = 0, int pixmapWidth = 0);
    void SetGeometry(int index, int top, int left, int width, int height, int spaceMenu);
    void SetCurrent(bool cur);
    void SetAlpha(int alpha = 0);
    void SetTabs(cString *tabs, int *tabWidths, int numtabs);
    void SetTextWindow(cRect *window) {textWindow = window;};
    virtual void CreateText(void) {};
    virtual void SetPoster(void) {};
    virtual int CheckScrollable1(int maxwidth = 0) { return 0; };
    virtual int CheckScrollable(bool hasIcon) {return 0;};
    virtual void Render(bool initial = false, bool fadeout = false) = 0;
};

// avoid confusion between vdr swap() and std::swap() for std::unique_ptr<cNopacityMenuItem>
inline void swap(cNopacityMenuItem*& a, cNopacityMenuItem*& b) {std::swap(a,b);}

class cNopacityMainMenuItem : public cNopacityMenuItem {
private:
    bool isSetup;
    cString menuNumber;
    cString menuEntry;
    std::string strEntry;
    std::string strEntryFull;
    static std::string items[16];
    cString GetIconName();
    void DrawBackgroundMainMenu(void);
    void SetTextFull(void);
    void SetTextShort(void);
public:
    cNopacityMainMenuItem(cOsd *osd, const char *text, bool sel, bool setup);
    ~cNopacityMainMenuItem(void);
    void CreatePixmapTextScroller(int totalWidth, int pixmapLeft = 0, int pixmapWidth = 0);
    void CreateText(void);
    int CheckScrollable(bool hasIcon);
    void Render(bool initial = false, bool fadeout = false);
};

class cNopacityScheduleMenuItem : public cNopacityMenuItem {
private:
    eMenuCategory category;
    const cEvent *Event;
    const cChannel *Channel;
    eTimerMatch TimerMatch;
    cRect *vidWin;
    std::string strDateTime;
    std::string strTitle;
    std::string strSubTitle;
    std::string strTitleFull;
    std::string strSubTitleFull;
    bool scrollTitle;
    bool scrollSubTitle;
    int titleY;
    void DrawStatic(int textLeft);
    void DrawRemaining(int x, int y, int width);
    void SetTextFull(void);
    void SetTextShort(void);
public:
    cNopacityScheduleMenuItem(cOsd *osd, const cEvent *Event, const cChannel *Channel, eTimerMatch TimerMatch, bool sel, eMenuCategory category, cRect *vidWin);
    ~cNopacityScheduleMenuItem(void);
    void CreatePixmapTextScroller(int totalWidth, int pixmapLeft = 0, int pixmapWidth = 0);
    void CreateText(void);
    int CheckScrollable(bool hasIcon);
    void Render(bool initial = false, bool fadeout = false);
};

class cNopacityChannelMenuItem : public cNopacityMenuItem {
private:
    const cChannel *Channel;
    std::string strEntry;
    std::string strEntryFull;
    cString strChannelSource = "";
    cString strChannelInfo = "";
    std::string strEpgInfo;
    std::string strEpgInfoFull;
    std::string strTimeInfo;
    cRect *vidWin;
    void SetText(bool full = false);
    void SetTextFull(void) { SetText(true); };
    void DrawStatic(void);
    void readCurrentEPG(void);
    int CheckScrollable1(int maxwidth = 0);
    std::string readEPG(void);
public:
    cNopacityChannelMenuItem(cOsd *osd, const cChannel *Channel, bool sel, cRect *vidWin);
    ~cNopacityChannelMenuItem(void) {};
    void CreateText(void);
    void Render(bool initial = false, bool fadeout = false);
};

class cNopacityTimerMenuItem : public cNopacityMenuItem {
private:
    const cTimer *Timer;
    std::string strEntry;
    std::string strEntryFull;
    std::string strDateTime;
    cRect *vidWin;
    void SetText(bool full = false);
    void SetTextFull(void) { SetText(true); };
    void DrawStatic(int textLeft);
    int CheckScrollable1(int maxwidth = 0);
    std::string CreateDate(void);
public:
    cNopacityTimerMenuItem(cOsd *osd, const cTimer *Timer, bool sel, cRect *vidWin);
    ~cNopacityTimerMenuItem(void) {};
    void CreateText(void);
    void Render(bool initial = false, bool fadeout = false);
};

class cNopacityRecordingMenuItem : public cNopacityMenuItem {
private:
    const cRecording *Recording;
    bool isFolder;
    bool hasManualPoster = false;
    cString manualPosterPath = "";
    bool hasPoster = false;
    bool hasThumb = false;
    bool done = false;
    int posterWidth = 0;
    int posterHeight = 0;
    ScraperGetPosterThumb thumb;
    int Level, Total, New;
    cRect *vidWin;
    std::string strRecName;
    std::string strRecNameFull;
    void SetText(bool full = false);
    void SetTextFull(void) { SetText(true); };
    void SetTextFolder(bool full = false);
    void SetTextRecording(bool full = false);
    void DrawRecordingIcons(void);
    void DrawFolderIcon(void);
    void DrawRecDateTime(void);
    void DrawFolderNewSeen(void);
    void DrawPoster(void);
    int CheckScrollable1(int maxwidth = 0);
public:
    cNopacityRecordingMenuItem(cOsd *osd, const cRecording *Recording, bool sel, int Level, int Total, int New, cRect *vidWin);
    ~cNopacityRecordingMenuItem(void) {};
    void CreateText(void);
    void SetPoster(void);
    void Render(bool initial = false, bool fadeout = false);
};

class cNopacityDefaultMenuItem : public cNopacityMenuItem {
private:
    bool CheckProgressBar(const char *text);
    void DrawProgressBar(int x, int width, const char *bar, tColor color);
    bool DrawHeaderElement(void);
    std::string strEntry;
    std::string strEntryFull;
    int scrollCol;
    void SetTextFull(void);
    void SetTextShort(void);
public:
    cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool sel);
    ~cNopacityDefaultMenuItem(void);
    int CheckScrollable(bool hasIcon);
    void Render(bool initial = false, bool fadeout = false);
};

class cNopacityTrackMenuItem : public cNopacityMenuItem {
private:
public:
    cNopacityTrackMenuItem(cOsd *osd, const char *text);
    ~cNopacityTrackMenuItem(void);
    void Render(bool initial = false, bool fadeout = false);
};
#endif //__NOPACITY_MENUITEM_H
