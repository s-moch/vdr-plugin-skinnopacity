#ifndef __NOPACITY_DISPLAYMENU_H
#define __NOPACITY_DISPLAYMENU_H

#include "nopacity.h"
#include "menuitem.h"

#include <vdr/thread.h>
#include <memory>
#include <vector>

class cNopacityDisplayMenu : public cSkinDisplayMenu , cThread {
private:
    cOsd *osd;
    cNopacityDisplayMenuView *menuView;
    cNopacityDetailView *detailView;
    eMenuCategory menuCategoryLast;
    int menuFadeTime = 0;
    int menuFadeOutTime = 0;
    bool fadeout;
    bool initial;
    bool deatilViewRenderDone = false;
    bool diskUsageDrawn;
    int lastDiskUsageState;
    int lastTimersState;
    bool timersDrawn;
    int currentNumItems;
    cList<cNopacityTimer> timers;
    std::vector<std::unique_ptr<cNopacityMenuItem>> menuItems;
    int positionButtons[4];
    cRect videoWindowRect;
    int currentFeed;
    void SetNextFeed(void);
    void DrawDisk(void);
    int CheckTimerConflict(bool timersChanged);
    void DrawTimers(bool timersChanged, int numConflicts);
    void SplitItem(const char *Text, cString *strItems, int *tabItems);
    void SetButtonPositions(void);
    void SetAlpha(int Alpha = 0, bool force = false);
    virtual void Action(void);
protected:
    int Tab(int n);
public:
#ifdef DEPRECATED_SKIN_SETITEMEVENT
    using cSkinDisplayMenu::SetItemEvent;
#endif
    cNopacityDisplayMenu(void);
    virtual ~cNopacityDisplayMenu(void);
    virtual void Scroll(bool Up, bool Page);
    virtual int MaxItems(void);
    virtual void Clear(void);
    virtual void SetMenuCategory(eMenuCategory MenuCategory);
    virtual void SetTitle(const char *Title);
    virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
    virtual bool SetItemEvent(const cEvent *Event, int Index, bool Current, bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch, bool TimerActive);
    virtual bool SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable);
    virtual bool SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider);
    virtual bool SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable, int Level, int Total, int New);
    virtual void SetScrollbar(int Total, int Offset);
    virtual void SetEvent(const cEvent *Event);
    virtual void SetRecording(const cRecording *Recording);
    virtual void SetText(const char *Text, bool FixedFont);
    virtual void Flush(void);
    virtual void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
    virtual int GetTextAreaWidth(void) const;
    virtual const cFont *GetTextAreaFont(bool FixedFont) const;
};

#endif //__NOPACITY_DISPLAYMENU_H
