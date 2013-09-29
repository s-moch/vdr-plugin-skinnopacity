#ifndef __NOPACITY_DISPLAYMENUVIEW_H
#define __NOPACITY_DISPLAYMENUVIEW_H

enum eDetailViewType {dvEvent = 0, dvRecording, dvText};

class cNopacityDisplayMenuView {
    private:
        cOsd *osd;
        cImageCache *imgCache;
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
        cPixmap *pixmapRssFeed;
        cPixmap *pixmapRssFeedBackground;
        cPixmap *pixmapRssFeedIcon;
        int feedNameLength;
        int avrgFontWidth;
        cRect textWindowSizeSchedules;
        cRect textWindowSizeRecordings;
        cRect textWindowSizeChannels;
        int GetContentWidth(eMenuCategory menuCat);
    public:
        cNopacityDisplayMenuView(cImageCache *imgCache);
        virtual ~cNopacityDisplayMenuView(void);
        cOsd *createOsd(void);
        void SetDescriptionTextWindowSize(void);
        void CreatePixmaps(void);
        void SetPixmapAlpha(int Alpha);
        void GetMenuItemSize(eMenuCategory menuCat, cPoint *itemSize);
        int GetMaxItems(eMenuCategory menuCat);
        int GetMenuTop(int numItems, int itemHeight);
        int GetMenuItemLeft(int itemWidth);
        void SetAvrgFontWidth(void);
        int mytabs[cSkinDisplayMenu::MaxTabs];
        void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
        int GetEditableWidth(void);
        int GetWidthDefaultMenu(void) {return geoManager->menuItemWidthDefault;};
        int GetTextAreaWidth(void);
        const cFont *GetTextAreaFont(bool FixedFont);
        cRect *GetDescriptionTextWindowSize(eMenuCategory menuCat);
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
        void DrawButton(const char *text, eBackgroundType bgButton, tColor buttonColor, tColor borderColor, tColor fontColor, int num);
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
        void DrawRssFeed(std::string feedName);
        cPoint GetRssFeedPosition(void);
        cPoint GetRssFeedSize(void);
};



#endif //__NOPACITY_DISPLAYMENUVIEW_H