#ifndef __NOPACITY_DISPLAYMENUVIEW_H
#define __NOPACITY_DISPLAYMENUVIEW_H

enum eDetailViewType {dvEvent = 0, dvRecording, dvText};

class cNopacityTimer : public cListObject {
    private:
        cOsd *osd;
    public:
        cNopacityTimer(cOsd *osd);
        virtual ~cNopacityTimer(void);
        void SetAlpha(int alpha) {pixmap->SetAlpha(alpha);}
        void SetLayer(int layer) {pixmap->SetLayer(layer);}
        cPixmap *pixmap;
};

class cNopacityDisplayMenuView {
    private:
        cOsd *osd;
        cString lastDate;
        int diskUsageAlert;
        cPixmap *pixmapHeader;
        cPixmap *pixmapHeaderLogo;
        cPixmap *pixmapHeaderIcon;
        cPixmap *pixmapHeaderLabel;
        cPixmap *pixmapDate;
        cPixmap *pixmapFooter;
        cPixmap *pixmapContent;
        cPixmap *pixmapScrollbar;   
        cPixmap *pixmapDiskUsage;
        cPixmap *pixmapDiskUsageIcon;
        cPixmap *pixmapDiskUsageLabel;
        cPixmap *pixmapStatus;
        cFont *fontHeader;
        cFont *fontDate;
        cFont *fontMenuitemLarge;
        cFont *fontMenuitemSchedule;
        cFont *fontMenuitemScheduleSmall;
        cFont *fontMenuitemDefault;
        cFont *fontDiskUsage;
        cFont *fontTimers;
        cFont *fontTimersHead;
        cFont *fontButtons;
        cFont *fontMessage;
        cFont *fontEPGInfoWindow;
        int osdWidth, osdHeight;
        int osdLeft, osdTop;
        int widthScrollbar;
        int dateWidth;
        int headerHeight, footerHeight, contentHeight;
        int contentWidthNarrow, contentWidthFull;
        int menuItemWidthDefault, menuItemHeightDefault;
        int menuItemWidthMain, menuItemHeightMain;
        int menuItemHeightSchedule;
        int diskUsageWidth, diskUsageHeight;
        int timersWidth;
        int buttonsBorder, buttonWidth, buttonHeight;
        int messageWidth, messageHeight;
        int avrgFontWidth;
        cRect textWindowSize;
        void SetDescriptionTextWindowSize(void);
    public:
        cNopacityDisplayMenuView();
        virtual ~cNopacityDisplayMenuView(void);
        cOsd *createOsd(void);
        void SetGeometry(void);
        void CreatePixmaps(void);
        void SetPixmapAlpha(int Alpha);
        void CreateFonts(void);
        cFont *GetMenuItemFont(eMenuCategory menuCat);
        cFont *GetMenuItemFontSmall(void);
        cFont *GetEPGWindowFont(void);
        void GetMenuItemSize(eMenuCategory menuCat, cPoint *itemSize);
        int GetMaxItems(eMenuCategory menuCat);
        int GetMenuTop(int numItems, int itemHeight);
        void SetAvrgFontWidth(void);
        int mytabs[cSkinDisplayMenu::MaxTabs];
        void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
        int GetEditableWidth(void);
        int GetWidthDefaultMenu(void) {return menuItemWidthDefault;};
        int GetTextAreaWidth(void);
        const cFont *GetTextAreaFont(bool FixedFont);
        cRect *GetDescriptionTextWindowSize(void) {return &textWindowSize;};
        void CreateBackgroundImages(int *handleBackgrounds, int *handleButtons);
        void AdjustContentBackground(bool contentNarrow, bool contentNarrowLast);
        void DrawBorderDecoration(void);
        int ShowHeaderLogo(bool show);
        void DrawHeaderLogo(void);
        int DrawHeaderIcon(eMenuCategory menuCat);
        int ShowHeaderIconChannelLogo(const char *Title);
        void DestroyHeaderIcon(void);
        void DrawHeaderLabel(int left, cString label);
        void DrawDate(bool initial);
        void DrawDiskUsage(void);
        void ShowDiskUsage(bool show);
        void DrawButton(const char *text, int handleImage, tColor borderColor, int num);
        void ClearButton(int num);
        int GetTimersInitHeight(void);
        int GetTimersMaxHeight(void);
        cNopacityTimer *DrawTimer(const cTimer *Timer, int y);
        void DrawScrollbar(double Height, double Offset);
        void ClearScrollbar(void);
        void DrawMessage(eMessageType Type, const char *Text);
        void ClearMessage(void);
        void SetDetailViewSize(eDetailViewType detailViewType, cNopacityMenuDetailView *detailView);
        int spaceMenu;
};



#endif //__NOPACITY_DISPLAYMENUVIEW_H