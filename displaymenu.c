#include "displaymenu.h"
#include <string>

cNopacityDisplayMenu::cNopacityDisplayMenu(void) {
    config.setDynamicValues();
    menuCategoryLast = mcUndefined;
    FrameTime = config.menuFrameTime; 
    FadeTime = config.menuFadeTime;
    initial = true;
    initMenu = true;
    diskUsageDrawn = false;
    timersDrawn = false;
    lastDiskUsageState = -1;
    lastTimersState = -1;
    menuItemIndexLast = -1;
    currentNumItems = 0;
    detailView = NULL;
    contentNarrow = true;
    contentNarrowLast = true;
    menuView = new cNopacityDisplayMenuView();
    osd = menuView->createOsd();
    menuView->SetGeometry();
    menuView->CreatePixmaps();
    menuView->CreateFonts();
    menuView->SetAvrgFontWidth();
    menuView->CreateBackgroundImages(handleBackgrounds, handleButtons);
    menuView->DrawHeaderLogo();
    menuView->DrawBorderDecoration();
}

cNopacityDisplayMenu::~cNopacityDisplayMenu() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    delete menuView;
    menuItems.Clear();
    if (detailView) {
        delete detailView;
    }
    timers.Clear();
    for (int i=0; i<8; i++)
        cOsdProvider::DropImage(handleBackgrounds[i]);
    for (int i=0; i<4; i++)
        cOsdProvider::DropImage(handleButtons[i]);

    delete osd;
}

void cNopacityDisplayMenu::DrawDisk(void) {
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain)&&!diskUsageDrawn)) {
        if (cVideoDiskUsage::HasChanged(lastDiskUsageState)) {
            menuView->DrawDiskUsage();
        } else {
            menuView->ShowDiskUsage(true);
        }
        diskUsageDrawn = true;
    }
}

void cNopacityDisplayMenu::DrawTimers(void) {
    int maxTimersHeight = menuView->GetTimersMaxHeight();
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain)&&!timersDrawn)) {
        if (Timers.Modified(lastTimersState)) {
            timers.Clear();
            cSortedTimers SortedTimers; 
            int numTimers = SortedTimers.Size();
            int currentHeight = menuView->GetTimersInitHeight();
            for (int i = 0; i < numTimers; i++) {
                if (const cTimer *Timer = SortedTimers[i]) {
                    cNopacityTimer *t = menuView->DrawTimer(Timer, currentHeight);
                    if (initial)
                        if (FadeTime)
                            t->SetAlpha(0);
                    currentHeight += t->pixmap->ViewPort().Height() + menuView->spaceMenu;
                    if (currentHeight < maxTimersHeight) {
                        timers.Add(t);
                    } else {
                        delete t;
                        break;
                    }
                }
            }
        } else {
            for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
                t->SetLayer(2);
            } 
        }
        timersDrawn = true;
    }
}

void cNopacityDisplayMenu::Scroll(bool Up, bool Page) {
    bool scrolled;
    scrolled = detailView->Scroll(Up, Page);
    if (scrolled) {
        double height = detailView->ScrollbarSize();
        double offset = detailView->Offset();
        menuView->DrawScrollbar(height, offset);
    }
}

int cNopacityDisplayMenu::MaxItems(void) {
    int maxItems = 0;
    switch (MenuCategory()) {
        case mcMain:
        case mcSetup:
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
        case mcChannel:
        case mcRecording:
            maxItems = menuView->GetMaxItems(MenuCategory());
            break;
        default:
            maxItems = config.numDefaultMenuItems;      
    }
    currentNumItems = maxItems;
    return maxItems;
}

void cNopacityDisplayMenu::Clear(void) {
    if (detailView) {
        delete detailView;
        detailView = NULL;
    }
    menuItemIndexLast = -1;
    initMenu = true;
    menuItems.Clear();
}

void cNopacityDisplayMenu::SetMenuCategory(eMenuCategory MenuCategory) {
      /* Categories:
      mcUndefined = -1,
      mcUnknown = 0,
      1  mcMain,
      2  mcSchedule,
      3  mcScheduleNow,
      4  mcScheduleNext,
      5  mcChannel,
      6  mcChannelEdit,
      7  mcTimer,
      8  mcTimerEdit,
      9  mcRecording,
      10 mcRecordingInfo,
      11 mcPlugin,
      12 mcPluginSetup,
      13 mcSetup,
      14 mcSetupOsd,
      15 mcSetupEpg,
      16 mcSetupDvb,
      17 mcSetupLnb,
      18 mcSetupCam,
      19 mcSetupRecord,
      20 mcSetupReplay,
      21 mcSetupMisc,
      22 mcSetupPlugins,
      23 mcCommand,
      24 mcEvent,
      25 mcText,
      26 mcFolder,
      27 mcCam
      */
    menuCategoryLast = this->MenuCategory();
    contentNarrowLast = contentNarrow;
    cSkinDisplayMenu::SetMenuCategory(MenuCategory);
    switch (MenuCategory) {
        case mcMain:
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
        case mcChannel:
        case mcSetup:
        case mcRecording:
            contentNarrow = true;
            break;
        default:
            contentNarrow = false;
    }
    if ((menuCategoryLast == mcMain) && (MenuCategory != mcMain)) {
        if (config.showDiscUsage) {
            menuView->ShowDiskUsage(false);
            diskUsageDrawn = false;
        }
        if (config.showTimers) {
            for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
                t->SetLayer(-1);
            } 
            timersDrawn = false;
        }
    }
    esyslog("nopacity: menuCat %d", MenuCategory);
}

void cNopacityDisplayMenu::SetTitle(const char *Title) {
    int left = 5;
    menuView->DestroyHeaderIcon();
    if (Title) {
        cString title = Title;
        switch (MenuCategory()) {
            case mcMain:
                title = cString::sprintf("%s %s", Title, VDRVERSION);
                left += menuView->ShowHeaderLogo(true);
                break;
            case mcSchedule:
                menuView->ShowHeaderLogo(false);
                left += menuView->ShowHeaderIconChannelLogo(Title);
                break;
            default:
                menuView->ShowHeaderLogo(false);
                left += menuView->DrawHeaderIcon(MenuCategory());
        }
        menuView->AdjustContentBackground(contentNarrow, contentNarrowLast);        
        menuView->DrawHeaderLabel(left, title);
    }
}

void cNopacityDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    if (Red) {
        menuView->DrawButton(Red, handleButtons[0], Theme.Color(clrButtonRedBorder), Setup.ColorKey0);
    } else
        menuView->ClearButton(Setup.ColorKey0);

    if (Green) {
        menuView->DrawButton(Green, handleButtons[1], Theme.Color(clrButtonGreenBorder), Setup.ColorKey1);
    } else
        menuView->ClearButton(Setup.ColorKey1);

    if (Yellow) {
        menuView->DrawButton(Yellow, handleButtons[2], Theme.Color(clrButtonYellowBorder), Setup.ColorKey2);
    } else
        menuView->ClearButton(Setup.ColorKey2);

    if (Blue) {
        menuView->DrawButton(Blue, handleButtons[3], Theme.Color(clrButtonBlueBorder), Setup.ColorKey3);
    } else
        menuView->ClearButton(Setup.ColorKey3);
}

void cNopacityDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
    if (Text) {
        menuView->DrawMessage(Type, Text);
    } else {
        menuView->ClearMessage();
    }
}

bool cNopacityDisplayMenu::SetItemEvent(const cEvent *Event, int Index, bool Current, 
                                        bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch) { 
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityScheduleMenuItem(osd, Event, Channel, TimerMatch, Selectable, MenuCategory());
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(menuView->GetMenuItemFont(mcSchedule));
        item->SetFontSmall(menuView->GetMenuItemFontSmall(mcSchedule));
        item->SetFontEPGWindow(menuView->GetEPGWindowFont());
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->spaceMenu, itemSize.X(), itemSize.Y());
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize());
        item->SetCurrent(Current);
        item->SetBackgrounds(handleBackgrounds);
        item->CreateText();
        int textWidth = item->CheckScrollable((Channel)?true:false);
        item->CreatePixmap();
        item->CreatePixmapIcon();
        item->CreatePixmapTextScroller(textWidth);
        menuItems.Add(item);
        item->Render();
        menuItemIndexLast = Index;
        if (initial) {
            if (FadeTime) {
                item->SetAlpha(0);
                item->SetAlphaIcon(0);
                item->SetAlphaText(0);
            }
        }
    } else {
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable) { 
    return false; 
}

bool cNopacityDisplayMenu::SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider) { 
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityChannelMenuItem(osd, Channel, Selectable);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(menuView->GetMenuItemFont(mcChannel));
        item->SetFontSmall(menuView->GetMenuItemFontSmall(mcChannel));
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->spaceMenu, itemSize.X(), itemSize.Y());
        item->SetCurrent(Current);
        item->SetBackgrounds(handleBackgrounds);
        item->CreateText();
        int textWidth = item->CheckScrollable(true);
        item->CreatePixmap();
        item->CreatePixmapIcon();
        item->CreatePixmapTextScroller(textWidth);
        menuItems.Add(item);
        item->Render();
        menuItemIndexLast = Index;
        if (initial) {
            if (FadeTime) {
                item->SetAlpha(0);
                item->SetAlphaIcon(0);
                item->SetAlphaText(0);
            }
        }
    } else {
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable, 
                                            int Level, int Total, int New) {
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        bool isFolder = false;
        if (Total > 0)
            isFolder = true;
        cNopacityMenuItem *item = new cNopacityRecordingMenuItem(osd, Recording, Selectable, isFolder, Total, New);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(menuView->GetMenuItemFont(mcRecording));
        item->SetFontSmall(menuView->GetMenuItemFontSmall(mcRecording));
        item->SetFontEPGWindow(menuView->GetEPGWindowFont());
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->spaceMenu, itemSize.X(), itemSize.Y());
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize());
        item->SetCurrent(Current);
        item->SetBackgrounds(handleBackgrounds);
        item->CreateText();
        int textWidth = item->CheckScrollable(false);
        item->CreatePixmap();
        item->CreatePixmapIcon();
        item->CreatePixmapTextScroller(textWidth);
        menuItems.Add(item);
        item->Render();
        menuItemIndexLast = Index;
        if (initial) {
            if (FadeTime) {
                item->SetAlpha(0);
                item->SetAlphaIcon(0);
                item->SetAlphaText(0);
            }
        }
    } else {
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}


void cNopacityDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
    bool hasIcons = false;
    cString *strItems = new cString[MaxTabs];
    int *tabItems = new int[2*MaxTabs];
    for (int i=0; i<MaxTabs; i++) {
        strItems[i] = "";
        tabItems[i] = 0;
        tabItems[i+MaxTabs] = 0;
    }
    SplitItem(Text, strItems, tabItems);
    if (initMenu) {
        if (Index > menuItemIndexLast) {
            cNopacityMenuItem *item;
            cPoint itemSize;
            switch (MenuCategory()) {
                case mcMain:
                case mcSetup:
                    item = new cNopacityMainMenuItem(osd, Text, Selectable);
                    menuView->GetMenuItemSize(mcMain, &itemSize);
                    item->SetFont(menuView->GetMenuItemFont(mcMain));
                    hasIcons = true;
                    break;
                default:
                    item = new cNopacityDefaultMenuItem(osd, Text, Selectable);
                    menuView->GetMenuItemSize(mcUnknown, &itemSize);
                    item->SetFont(menuView->GetMenuItemFont(mcUnknown));
            }
            int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
            item->SetGeometry(Index, spaceTop, menuView->spaceMenu, itemSize.X(), itemSize.Y());
            item->SetCurrent(Current);
            item->SetBackgrounds(handleBackgrounds);
            item->SetTabs(strItems, tabItems, MaxTabs);
            item->CreateText();
            int textWidth = item->CheckScrollable(hasIcons);
            item->CreatePixmap();
            if (hasIcons) {
                item->CreatePixmapIcon();
            }
            if (textWidth > 0)
                item->CreatePixmapTextScroller(textWidth);
            menuItems.Add(item);
            item->Render();
            menuItemIndexLast = Index;
            if (initial) {
                if (FadeTime) {
                    item->SetAlpha(0);
                    item->SetAlphaIcon(0);
                    item->SetAlphaText(0);
                }
            }
        } else {
            //adjust Current if item was added twice
            cNopacityMenuItem *item = menuItems.Get(Index);
            item->SetCurrent(Current);
            item->Render();
        }
    } else {
        //redraw item when switching through menu
        cNopacityMenuItem *item = menuItems.Get(Index);
        item->SetTabs(strItems, tabItems, MaxTabs);
        item->SetCurrent(Current);
        item->Render();
    }
    SetEditableWidth(menuView->GetEditableWidth());
}

void cNopacityDisplayMenu::SplitItem(const char *Text, cString *strItems, int *tabItems) {
    int x = 0;
    for (int i = 0; i < MaxTabs; i++) {
        const char *s = GetTabbedText(Text, i);
        if (s) {
            strItems[i] = s;
        }
        tabItems[i] = Tab(i);
        if (i>0) {
            tabItems[(i-1) + MaxTabs] = Tab(i) - x;
            x += Tab(i) - x;
        }
        if (!Tab(i + 1)) {
            if (s)
                tabItems[i + MaxTabs] = menuView->GetWidthDefaultMenu() - x;
            else if (i==1) {
                tabItems[MaxTabs] = menuView->GetWidthDefaultMenu() - 1;
                tabItems[1] = 0;
                tabItems[MaxTabs+1] = 0;
            }
            break;
        }
    }
}

int cNopacityDisplayMenu::Tab(int n) { 
    return (n >= 0 && n < MaxTabs) ? menuView->mytabs[n] : 0; 
}

void cNopacityDisplayMenu::SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5) {
    menuView->SetTabs(Tab1, Tab2, Tab3, Tab4, Tab5);
}

int cNopacityDisplayMenu::GetTextAreaWidth(void) const {
    return menuView->GetTextAreaWidth();
}

const cFont *cNopacityDisplayMenu::GetTextAreaFont(bool FixedFont) const {
    return menuView->GetTextAreaFont(FixedFont);
}

void cNopacityDisplayMenu::SetScrollbar(int Total, int Offset) {
    if (MaxItems() >= Total) {
        menuView->ClearScrollbar();
        return;
    }
    double height = (double)MaxItems()/(double)Total;
    double offset = (double)Offset/(double)Total;
    
    menuView->DrawScrollbar(height, offset);
}

void cNopacityDisplayMenu::SetEvent(const cEvent *Event) {
    if (!Event)
        return;
    menuView->AdjustContentBackground(false, contentNarrowLast);
    detailView = new cNopacityMenuDetailEventView(osd, Event);
    menuView->SetDetailViewSize(dvEvent, detailView);
    detailView->SetFonts();
    if (config.displayRerunsDetailEPGView)
        detailView->LoadReruns(Event);
    detailView->SetContent(Event->Description());
    detailView->CreatePixmaps();
    detailView->Render();
    if (detailView->Scrollable()) {
        double height = detailView->ScrollbarSize();
        double offset = 0.0;
        menuView->DrawScrollbar(height, offset);
    }
}

void cNopacityDisplayMenu::SetRecording(const cRecording *Recording) {
    if (!Recording)
        return;
    menuView->AdjustContentBackground(false, contentNarrowLast);
    detailView = new cNopacityMenuDetailRecordingView(osd, Recording);
    menuView->SetDetailViewSize(dvRecording, detailView);
    detailView->SetFonts();
    detailView->SetContent(Recording->Info()->Description());
    detailView->CreatePixmaps();
    detailView->Render();
    if (detailView->Scrollable()) {
        double height = detailView->ScrollbarSize();
        double offset = 0.0;
        menuView->DrawScrollbar(height, offset);
    }
}

void cNopacityDisplayMenu::SetText(const char *Text, bool FixedFont) {
    if (!Text)
        return;
    menuView->AdjustContentBackground(false, contentNarrowLast);
    detailView = new cNopacityMenuDetailTextView(osd);
    menuView->SetDetailViewSize(dvText, detailView);
    detailView->SetFonts();
    detailView->SetContent(Text);
    detailView->CreatePixmaps();
    detailView->Render();
    if (detailView->Scrollable()) {
        double height = detailView->ScrollbarSize();
        double offset = 0.0;
        menuView->DrawScrollbar(height, offset);
    }
}

void cNopacityDisplayMenu::Flush(void) {
    menuView->DrawDate(initial);
    if (MenuCategory() == mcMain) {
        if (config.showDiscUsage)
            DrawDisk();
        if (config.showTimers)
            DrawTimers();
    }
    if (initial) {
        if (config.menuFadeTime)
            Start();
    }
    initMenu = false;
    initial = false;
    osd->Flush();
}

void cNopacityDisplayMenu::Action(void) {
    uint64_t Start = cTimeMs::Now();
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        double t = min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        menuView->SetPixmapAlpha(Alpha);
        for (cNopacityMenuItem *item = menuItems.First(); Running() && item; item = menuItems.Next(item)) {
            item->SetAlpha(Alpha);
            item->SetAlphaIcon(Alpha);
            item->SetAlphaText(Alpha);
        }
        for (cNopacityTimer *t = timers.First(); Running() && t; t = timers.Next(t))
            t->SetAlpha(Alpha);
        if (Running())
            osd->Flush();
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
        if ((int)(Now - Start) > FadeTime)
            break;
    }
}
