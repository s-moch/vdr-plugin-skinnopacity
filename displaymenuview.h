#ifndef __NOPACITY_DISPLAYMENUVIEW_H
#define __NOPACITY_DISPLAYMENUVIEW_H

#include "timers.h"
#include "menudetailview.h"
#include <vdr/skins.h>
#include <vdr/videodir.h>
#include "config.h"
#include "helpers.h"
#include "imagecache.h"
#include "imageloader.h"
#include "messagebox.h"
#include "volumebox.h"

class cNopacityDisplayMenuView {
    private:
        cOsd *osd;
        cString lastDate;
        int diskUsageAlert;
	bool menuAdjustLeft;
        int lastVolume;
        time_t lastVolumeTime;
        cPixmap *pixmapHeader;
        cPixmap *pixmapHeaderForeground;
        cPixmap *pixmapHeaderLogo;
        cPixmap *pixmapHeaderIcon;
        cPixmap *pixmapHeaderLabel;
        cPixmap *pixmapDate;
        cPixmap *pixmapFooter;
        cPixmap *pixmapFooterBack;
        cPixmap *pixmapButtonsText;
        cPixmap *pixmapContent;
        cPixmap *pixmapScrollbar;
        cPixmap *pixmapScrollbarBack;
        cPixmap *pixmapDiskUsage;
        cPixmap *pixmapDiskUsageIcon;
        cPixmap *pixmapDiskUsageLabel;
	cNopacityMessageBox *messageBox;
        cNopacityVolumeBox *volumeBox;
        int feedNameLength;
        cRect textWindowSizeSchedules;
        cRect textWindowSizeRecordings;
        cRect textWindowSizeChannels;
        cRect textWindowSizeTimers;
        int GetContentWidth(eMenuCategory menuCat);
        void SetDescriptionTextWindowSize(void);
        void CreatePixmaps(void);
        void DrawHeaderLogo(void);
        void DrawBorderDecoration(void);
    public:
        cNopacityDisplayMenuView(cOsd *osd);
        virtual ~cNopacityDisplayMenuView(void);
        void SetAlpha(int Alpha = 0);
        void GetMenuItemSize(eMenuCategory menuCat, cPoint *itemSize);
        int GetMenuTop(int numItems, int itemHeight);
        int GetMenuItemLeft(int itemWidth);
        int mytabs[cSkinDisplayMenu::MaxTabs];
        void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
        int GetEditableWidth(void);
        int GetWidthDefaultMenu(void) {return geoManager->menuItemWidthDefault;};
        int GetTextAreaWidth(void);
        const cFont *GetTextAreaFont(bool FixedFont);
        cRect *GetDescriptionTextWindowSize(eMenuCategory menuCat);
        void AdjustContentBackground(eMenuCategory menuCat, eMenuCategory menuCatLast, cRect & vidWin);
        int ShowHeaderLogo(bool show);
        int DrawHeaderIcon(eMenuCategory menuCat, bool initial = false);
        int ShowHeaderIconChannelLogo(const char *Title, bool initial = false);
        void DestroyHeaderIcon(void);
        void DrawHeaderLabel(int left, cString label);
        void DrawDate(bool initial);
        void DrawDiskUsage(void);
        void ShowDiskUsage(bool show);
        void DrawButton(const char *text, eSkinElementType seButton, tColor buttonColor, tColor borderColor, tColor fontColor, int num);
        void ClearButton(void);
        int GetTimersInitHeight(void);
        int GetTimersMaxHeight(void);
        cNopacityTimer *DrawTimerConflict(int numConflicts, int y);
        cNopacityTimer *DrawTimer(const cTimer *Timer, int y);
        void DrawScrollbar(double Height, double Offset);
        void ClearScrollbar(void);
        void DrawMessage(eMessageType Type, const char *Text);
        void DrawVolume(void);
        cPixmap *GetPixmapScrollbar(void) { return pixmapScrollbar; };
        cPixmap *GetPixmapScrollbarBack(void) { return pixmapScrollbarBack; };
};

#endif //__NOPACITY_DISPLAYMENUVIEW_H
