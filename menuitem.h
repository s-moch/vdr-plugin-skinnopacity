#ifndef __NOPACITY_MENUITEM_H
#define __NOPACITY_MENUITEM_H

class cNopacityMenuItem : public cListObject, public cThread {
protected:
    cOsd *osd;
    cPixmap *pixmap;
    cPixmap *pixmapIcon;
    cPixmap *pixmapTextScroller;
    bool hasIcon;
    int *handleBackgrounds;
    const char *Text;
    bool selectable;
    bool current;
    bool wasCurrent;
    bool scrollable;
    bool drawn;
    cFont *font;
    cFont *fontSmall;
    int width, height;
    int top, left;
    int textLeft;
    int index;
    cString *itemTabs;
    int *tabWidth;
    int numTabs;
    void DrawDelimiter(const char *del, const char *icon, int handleBgrd);
    virtual void Action(void);
    virtual void SetTextFull(void) {};
    virtual void SetTextShort(void) {};
public:
    cNopacityMenuItem(cOsd *osd, const char *text, bool sel);
    virtual ~cNopacityMenuItem(void);
    void CreatePixmap();
    void CreatePixmapIcon(int iconWidth, int iconHeight);
    virtual void CreatePixmapTextScroller(int totalWidth);
    void SetGeometry(int index, int top, int left, int width, int height);
    void SetFont(cFont *font) {this->font = font;}
    void SetFontSmall(cFont *fontSmall) {this->fontSmall = fontSmall;}
    void SetCurrent(bool cur); 
    void SetAlpha(int alpha) {this->pixmap->SetAlpha(alpha);}
    void SetAlphaIcon(int alpha) {if (hasIcon) this->pixmapIcon->SetAlpha(alpha);}
    void SetAlphaText(int alpha) {if (pixmapTextScroller) this->pixmapTextScroller->SetAlpha(alpha);}
    void SetTabs(cString *tabs, int *tabWidths, int numtabs);
    void SetBackgrounds(int *handleBackgrounds);
    virtual void CreateText(void) {};
    virtual void SetDisplayMode(void) {};
    virtual int CheckScrollable(bool hasIcon) {return 0;};
    virtual void Render() = 0;
};

class cNopacityMainMenuItem : public cNopacityMenuItem {
private:
    cString menuNumber;
    cString menuEntry;
    std::string strEntry;
    std::string strEntryFull;
    static std::string items[6];
    cString GetIconName();
    void SetTextFull(void);
    void SetTextShort(void);
public:
    cNopacityMainMenuItem(cOsd *osd, const char *text, bool sel);
    ~cNopacityMainMenuItem(void);
    void CreatePixmapTextScroller(int totalWidth);
    void CreateText(void);
    int CheckScrollable(bool hasIcon);
    void Render();
};

enum eMenuSubCategory { mcSubUndefined = -1, 
                        mcSubSchedule = 0, 
                        mcSubScheduleWhatsOn, 
                        mcSubScheduleWhatsOnNow, 
                        mcSubScheduleWhatsOnNext,
                        mcSubScheduleWhatsOnElse,
                        mcSubScheduleSearchResults,
                        mcSubScheduleFavorites,
                        mcSubScheduleTimerconflict,
                        mcSubScheduleTimer,
                        mcSubChannels, 
                        mcSubChannelEdit};

class cNopacityScheduleMenuItem : public cNopacityMenuItem {
private:
    eMenuSubCategory subCategory;
    std::string strDateTime;
    std::string strTitle;
    std::string strSubTitle;
    std::string strTitleFull;
    std::string strSubTitleFull;
    std::string strChannelName;
    std::string strProgressbar;
    bool hasProgressBar;
    eEPGModes mode;
    bool hasLogo;
    int titleY;
    std::string delimiterType;
    void DrawRemaining(cString remaining, int x, int y, int width);
    void SetTextFull(void);
    void SetTextShort(void);
public:
    cNopacityScheduleMenuItem(cOsd *osd, const char *text, bool sel, eMenuSubCategory subCat);
    ~cNopacityScheduleMenuItem(void);
    void CreatePixmapTextScroller(int totalWidth);
    void SetDisplayMode(void);
    void CreateText(void);
    int CheckScrollable(bool hasIcon);
    void Render();
};

class cNopacityChannelMenuItem : public cNopacityMenuItem {
private:
    std::string strLogo;
    std::string strEntry;
    std::string strEntryFull;
    void SetTextFull(void);
    void SetTextShort(void);
public:
    cNopacityChannelMenuItem(cOsd *osd, const char *text, bool sel);
    ~cNopacityChannelMenuItem(void);
    void CreatePixmapTextScroller(int totalWidth);
    void CreateText(void);
    int CheckScrollable(bool hasIcon);
    void Render();
};

class cNopacityDefaultMenuItem : public cNopacityMenuItem {
private:
    std::string strEntry;
    std::string strEntryFull;
    int scrollCol;
    void SetTextFull(void);
    void SetTextShort(void);
public:
    cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool sel);
    ~cNopacityDefaultMenuItem(void);
    int CheckScrollable(bool hasIcon);
    void Render();
};

class cNopacityTrackMenuItem : public cNopacityMenuItem {
private:
public:
    cNopacityTrackMenuItem(cOsd *osd, const char *text);
    ~cNopacityTrackMenuItem(void);
    void Render();
};
#endif //__NOPACITY_MENUITEM_H
