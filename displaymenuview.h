#ifndef __NOPACITY_DISPLAYMENUVIEW_H
#define __NOPACITY_DISPLAYMENUVIEW_H

enum eDetailViewType {dvEvent = 0, dvRecording, dvText};

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
        cFont *fontMenuitemChannel;
        cFont *fontMenuitemChannelSmall;
        cFont *fontMenuitemRecordings;
        cFont *fontMenuitemRecordingsSmall;
        cFont *fontMenuitemTimers;
        cFont *fontMenuitemTimersSmall;
        cFont *fontMenuitemDefault;
        cFont *fontDiskUsage;
        cFont *fontDiskUsagePercent;
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
        int contentWidthFull;
        int contentWidthMain;
        int contentWidthSchedules;
        int contentWidthChannels;
        int contentWidthTimers;
        int contentWidthRecordings;
        int menuItemWidthDefault, menuItemHeightDefault;
        int menuItemWidthMain, menuItemHeightMain;
        int menuItemWidthSchedule, menuItemHeightSchedule;
        int menuItemWidthChannel;
        int menuItemWidthTimer;
        int menuItemWidthRecording, menuItemHeightRecordings;
        int diskUsageWidth, diskUsageHeight;
        int timersWidth;
        int buttonsBorder, buttonWidth, buttonHeight;
        int messageWidth, messageHeight;
        int avrgFontWidth;
        cRect textWindowSizeSchedules;
        cRect textWindowSizeRecordings;
        void SetDescriptionTextWindowSize(void);
        int GetContentWidth(eMenuCategory menuCat);
    public:
        cNopacityDisplayMenuView();
        virtual ~cNopacityDisplayMenuView(void);
        cOsd *createOsd(void);
        void SetGeometry(void);
        void CreatePixmaps(void);
        void SetPixmapAlpha(int Alpha);
        void CreateFonts(void);
        cFont *GetMenuItemFont(eMenuCategory menuCat);
        cFont *GetMenuItemFontSmall(eMenuCategory menuCat);
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
        cRect *GetDescriptionTextWindowSize(eMenuCategory menuCat);
        void CreateBackgroundImages(int *handleBackgrounds, int *handleButtons);
        void AdjustContentBackground(eMenuCategory menuCat, eMenuCategory menuCatLast, cRect & vidWin);
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
        cNopacityTimer *DrawTimerConflict(int numConflicts, int y);
        cNopacityTimer *DrawTimer(const cTimer *Timer, int y);
        void DrawScrollbar(double Height, double Offset);
        void ClearScrollbar(void);
        void DrawMessage(eMessageType Type, const char *Text);
        void ClearMessage(void);
        void SetDetailViewSize(eDetailViewType detailViewType, cNopacityMenuDetailView *detailView);
        int spaceMenu;
};



#endif //__NOPACITY_DISPLAYMENUVIEW_H