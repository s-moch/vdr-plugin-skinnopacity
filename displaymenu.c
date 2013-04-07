#include "services/remotetimers.h"
namespace PluginRemoteTimers {
    static int CompareTimers(const void *a, const void *b) {
        return (*(const cTimer **)a)->Compare(**(const cTimer **)b);
    }
}

#include "displaymenu.h"
#include <string>

cNopacityDisplayMenu::cNopacityDisplayMenu(void) {
esyslog("nopacity: starting menu");
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
    SetButtonPositions();
    menuView = new cNopacityDisplayMenuView();
    osd = menuView->createOsd();
    menuView->SetGeometry();
    menuView->CreatePixmaps();
    menuView->CreateFonts();
    menuView->SetAvrgFontWidth();
    menuView->CreateBackgroundImages(handleBackgrounds, handleButtons);
    menuView->DrawHeaderLogo();
    menuView->DrawBorderDecoration();
    currentFeed = 0;
    if (config.displayRSSFeed) {
        menuView->DrawRssFeed(config.rssFeeds[config.rssFeed[currentFeed]].name);
        rssReader = new cRssReader(osd, menuView->GetRssFeedFont(), menuView->GetRssFeedPosition(), menuView->GetRssFeedSize());
        rssReader->SetFeed(config.rssFeeds[config.rssFeed[currentFeed]].url);
        rssReader->Start();
    } else
        rssReader = NULL;
}

cNopacityDisplayMenu::~cNopacityDisplayMenu() {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);
    if (rssReader) {
        rssReader->Stop();
        while (rssReader->Active())
            cCondWait::SleepMs(10);
        delete rssReader;
        rssReader = NULL;
    }
    delete menuView;
    menuItems.Clear();
    if (detailView) {
        delete detailView;
    }
    timers.Clear();
    for (int i=0; i<14; i++)
        cOsdProvider::DropImage(handleBackgrounds[i]);
    for (int i=0; i<4; i++)
        cOsdProvider::DropImage(handleButtons[i]);

    delete osd;
    cDevice::PrimaryDevice()->ScaleVideo(cRect::Null);
    menuActive = false;
}

void cNopacityDisplayMenu::DrawDisk(void) {
    if (!config.narrowMainMenu)
        return;
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain)&&!diskUsageDrawn)) {
        if (cVideoDiskUsage::HasChanged(lastDiskUsageState)) {
            menuView->DrawDiskUsage();
        } else {
            menuView->ShowDiskUsage(true);
        }
        diskUsageDrawn = true;
    }
}

int cNopacityDisplayMenu::CheckTimerConflict(bool timersChanged) {
    int numConflicts = 0;
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain))) {
        if (timersChanged) {
            cPlugin *p = cPluginManager::GetPlugin("epgsearch");
            if (p) {
                Epgsearch_lastconflictinfo_v1_0 *serviceData = new Epgsearch_lastconflictinfo_v1_0;
                if (serviceData) {
                    serviceData->nextConflict = 0;
                    serviceData->relevantConflicts = 0;
                    serviceData->totalConflicts = 0;
                    p->Service("Epgsearch-lastconflictinfo-v1.0", serviceData);
                    if (serviceData->relevantConflicts > 0) {
                        numConflicts = serviceData->relevantConflicts;
                    }
                    delete serviceData;
                }
            }
        }
    }
    return numConflicts;
}

void cNopacityDisplayMenu::DrawTimers(bool timersChanged, int numConflicts) {
    if (!config.narrowMainMenu)
        return;
    int maxTimersHeight = menuView->GetTimersMaxHeight();
    if (initial || ((menuCategoryLast!=mcMain)&&(MenuCategory()==mcMain)&&!timersDrawn)) {
        if (timersChanged) {
            //check if remotetimers plugin is available
            static cPlugin* pRemoteTimers = cPluginManager::GetPlugin("remotetimers");
            bool drawRemoteTimers = false;
            cString errorMsg;
            if (pRemoteTimers) {
                drawRemoteTimers = pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg);
            }
            timers.Clear();
            cSortedTimers SortedTimers; 
            //if remotetimers plugin is available, take timers also from him
            if (drawRemoteTimers) {
                cTimer* remoteTimer = NULL;
                int numRemoteTimers = 0;
                while (pRemoteTimers->Service("RemoteTimers::ForEach-v1.0", &remoteTimer) && remoteTimer != NULL) {
                    SortedTimers.Append(remoteTimer);
                    numRemoteTimers++;
                }
                SortedTimers.Sort(PluginRemoteTimers::CompareTimers);
            }
            int numTimers = SortedTimers.Size();
            int currentHeight = menuView->GetTimersInitHeight();
            if (numConflicts > 0) {
                cNopacityTimer *t = menuView->DrawTimerConflict(numConflicts, currentHeight);
                if (initial)
                    if (FadeTime)
                        t->SetAlpha(0);
                currentHeight += t->GetHeight() + menuView->spaceMenu;
                timers.Add(t);
            }
            int numTimersDisplayed = 0;
            for (int i = 0; i < numTimers; i++) {
                if (const cTimer *Timer = SortedTimers[i]) {
                    if (Timer->HasFlags(tfActive)) {
                        cNopacityTimer *t = menuView->DrawTimer(Timer, currentHeight);
                        if (initial)
                            if (FadeTime)
                                t->SetAlpha(0);
                        currentHeight += t->GetHeight() + menuView->spaceMenu;
                        if (currentHeight < maxTimersHeight) {
                            timers.Add(t);
                        numTimersDisplayed++;
                        if (numTimersDisplayed == config.numberTimers)
                            break;
                        } else {
                            delete t;
                            break;
                        }
                    }
                }
            }
        } else {
            for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
                t->Show();
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
            if (config.narrowMainMenu)
                maxItems = menuView->GetMaxItems(MenuCategory());
            else
                maxItems = config.numDefaultMenuItems;
            break;
        case mcSetup:
            if (config.narrowSetupMenu)
                maxItems = menuView->GetMaxItems(MenuCategory());
            else
                maxItems = config.numDefaultMenuItems;
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            if (config.narrowScheduleMenu)
                maxItems = menuView->GetMaxItems(MenuCategory());
            else
                maxItems = config.numDefaultMenuItems;
            break;
        case mcChannel:
            if (config.narrowChannelMenu)
                maxItems = menuView->GetMaxItems(MenuCategory());
            else
                maxItems = config.numDefaultMenuItems;
            break;
        case mcTimer:
            if (config.narrowTimerMenu)
                maxItems = menuView->GetMaxItems(MenuCategory());
            else
                maxItems = config.numDefaultMenuItems;
            break;
        case mcRecording:
            if (config.narrowRecordingMenu)
                maxItems = menuView->GetMaxItems(MenuCategory());
            else
                maxItems = config.numDefaultMenuItems;
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
    cSkinDisplayMenu::SetMenuCategory(MenuCategory);
    if ((menuCategoryLast == mcMain) && (MenuCategory != mcMain)) {
        if (config.showDiscUsage) {
            menuView->ShowDiskUsage(false);
            diskUsageDrawn = false;
        }
        if (config.showTimers) {
            for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
                t->Hide();
            } 
            timersDrawn = false;
        }
    }
}

void cNopacityDisplayMenu::SetTitle(const char *Title) {
    //resetting menuitems if no call to clear
    if (!initMenu) {
        initMenu = true;
        menuItemIndexLast = -1;
        menuItems.Clear();
    }
    int left = 5;
    menuView->DestroyHeaderIcon();
    if (Title) {
        cString title = Title;
        switch (MenuCategory()) {
            case mcMain:
                switch (config.mainMenuTitleStyle) {
                    case 0:
                        title = cString::sprintf("%s %s", Title, VDRVERSION);
                        break;
                    case 1:
                        title = cString::sprintf("%s", VDRVERSION);
                        break;
                    case 2:
                        title = " ";
                        break;
                }
                left += menuView->ShowHeaderLogo(true);
                break;
            case mcSchedule:
                menuView->ShowHeaderLogo(false);
                left += menuView->ShowHeaderIconChannelLogo(Title);
                break;
            case mcRecording:
                menuView->ShowHeaderLogo(false);
                title = cString::sprintf("%s (%s)", Title, *cVideoDiskUsage::String());
                left += menuView->DrawHeaderIcon(MenuCategory());
                break;
            default:
                menuView->ShowHeaderLogo(false);
                left += menuView->DrawHeaderIcon(MenuCategory());
        }
        menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
        menuView->DrawHeaderLabel(left, title);
    }
}

void cNopacityDisplayMenu::SetButtonPositions(void) {
    for (int i=0; i < 4; i++) {
        positionButtons[i] = -1;
    }
    /*
    red button = 0
    green button = 1
    yellow button = 2
    blue button = 3
    */
    for (int button=0; button<4; button++) {
        if (Setup.ColorKey0 == button) {
            positionButtons[button] = 0;
            continue;
        }
        if (Setup.ColorKey1 == button) {
            positionButtons[button] = 1;
            continue;
        }
        if (Setup.ColorKey2 == button) {
            positionButtons[button] = 2;
            continue;
        }
        if (Setup.ColorKey3 == button) {
            positionButtons[button] = 3;
            continue;
        }
    }
    
}

void cNopacityDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    if (Red) {
        menuView->DrawButton(Red, handleButtons[0], Theme.Color(clrButtonRedBorder), positionButtons[0]);
    } else
        menuView->ClearButton(positionButtons[0]);

    if (Green) {
        menuView->DrawButton(Green, handleButtons[1], Theme.Color(clrButtonGreenBorder), positionButtons[1]);
    } else
        menuView->ClearButton(positionButtons[1]);

    if (Yellow) {
        menuView->DrawButton(Yellow, handleButtons[2], Theme.Color(clrButtonYellowBorder), positionButtons[2]);
    } else
        menuView->ClearButton(positionButtons[2]);

    if (Blue) {
        menuView->DrawButton(Blue, handleButtons[3], Theme.Color(clrButtonBlueBorder), positionButtons[3]);
    } else
        menuView->ClearButton(positionButtons[3]);
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
    if (!config.narrowScheduleMenu)
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityScheduleMenuItem(osd, Event, Channel, TimerMatch, Selectable, MenuCategory());
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(menuView->GetMenuItemFont(mcSchedule));
        item->SetFontSmall(menuView->GetMenuItemFontSmall(mcSchedule));
        item->SetFontEPGWindow(menuView->GetEPGWindowFont());
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), menuView->spaceMenu);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcSchedule));
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
    if (!config.narrowTimerMenu)
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityTimerMenuItem(osd, Timer, Selectable);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(menuView->GetMenuItemFont(mcTimer));
        item->SetFontSmall(menuView->GetMenuItemFontSmall(mcTimer));
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), menuView->spaceMenu);
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

bool cNopacityDisplayMenu::SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider) { 
    if (!config.narrowChannelMenu)
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        cNopacityMenuItem *item = new cNopacityChannelMenuItem(osd, Channel, Selectable);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(menuView->GetMenuItemFont(mcChannel));
        item->SetFontSmall(menuView->GetMenuItemFontSmall(mcChannel));
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), menuView->spaceMenu);
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
    if (!config.narrowRecordingMenu)
        return false;
    if ((initMenu)&&(Index > menuItemIndexLast)) {
        bool isFolder = false;
        if (Total > 0)
            isFolder = true;
        cNopacityMenuItem *item = new cNopacityRecordingMenuItem(osd, Recording, Selectable, isFolder, Level, Total, New);
        cPoint itemSize;
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        item->SetFont(menuView->GetMenuItemFont(mcRecording));
        item->SetFontSmall(menuView->GetMenuItemFontSmall(mcRecording));
        item->SetFontEPGWindow(menuView->GetEPGWindowFont());
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), menuView->spaceMenu);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcRecording));
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
            if (((MenuCategory() == mcMain)&&(config.narrowMainMenu)) || ((MenuCategory() == mcSetup)&&(config.narrowSetupMenu))){
                bool isSetup = (MenuCategory() == mcSetup)?true:false;
                item = new cNopacityMainMenuItem(osd, Text, Selectable, isSetup);
                menuView->GetMenuItemSize(MenuCategory(), &itemSize);
                item->SetFont(menuView->GetMenuItemFont(mcMain));
                if (config.useMenuIcons)
                    hasIcons = true;
            } else {
                item = new cNopacityDefaultMenuItem(osd, Text, Selectable);
                menuView->GetMenuItemSize(mcUnknown, &itemSize);
                item->SetFont(menuView->GetMenuItemFont(mcUnknown));
            }
            int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
            item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), menuView->spaceMenu);
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
        if (item) {
            item->SetTabs(strItems, tabItems, MaxTabs);
            item->SetCurrent(Current);
            item->Render();
        }
    }
    SetEditableWidth(menuView->GetEditableWidth());
}

void cNopacityDisplayMenu::SplitItem(const char *Text, cString *strItems, int *tabItems) {
    int x = 0;
    for (int i = 0; i < MaxTabs; i++) {
        const char *s = GetTabbedText(Text, i);
        if (s) {
            strItems[i] = s;
            tabItems[i] = Tab(i);
        } else {
            if (i>0)
                tabItems[i - 1 + MaxTabs] = menuView->GetWidthDefaultMenu() - x;
            break;
        }
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
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityMenuDetailEventView(osd, Event);
    menuView->SetDetailViewSize(dvEvent, detailView);
    detailView->SetFonts();
    detailView->SetContent();
    detailView->SetContentHeight();
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
    const cRecordingInfo *Info = Recording->Info();
    if (!Info) {
        return;
    }
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityMenuDetailRecordingView(osd, Recording);
    menuView->SetDetailViewSize(dvRecording, detailView);
    detailView->SetFonts();
    detailView->SetContent();
    detailView->SetContentHeight();
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
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityMenuDetailTextView(osd, Text);
    menuView->SetDetailViewSize(dvText, detailView);
    detailView->SetFonts();
    detailView->SetContent();
    detailView->SetContentHeight();
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
        bool timersChanged = Timers.Modified(lastTimersState);
        int numConflicts = 0;
        if (config.checkTimerConflict)
            numConflicts = CheckTimerConflict(timersChanged);
        if (config.showTimers)
            DrawTimers(timersChanged, numConflicts);
    }
    if (initial) {
        if (config.menuFadeTime)
            Start();
    }
    initMenu = false;
    initial = false;
    osd->Flush();
    cDevice::PrimaryDevice()->ScaleVideo(videoWindowRect);
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

void cNopacityDisplayMenu::SwitchNextRssMessage(void) {
    if (!config.displayRSSFeed)
        return;
    if (rssReader) {
        rssReader->SwitchNextMessage();
    }
}

void cNopacityDisplayMenu::SwitchNextRssFeed(void) {
    if (!config.displayRSSFeed)
        return;
    if (rssReader) {
        rssReader->Stop();
        while (rssReader->Active())
            cCondWait::SleepMs(10);
        delete rssReader;
        rssReader = NULL;
    }
    SetNextFeed();
    int feedNum = (config.rssFeed[currentFeed]==0)?0:(config.rssFeed[currentFeed]-1);
    menuView->DrawRssFeed(config.rssFeeds[feedNum].name);
    rssReader = new cRssReader(osd, menuView->GetRssFeedFont(), menuView->GetRssFeedPosition(), menuView->GetRssFeedSize());
    rssReader->SetFeed(config.rssFeeds[feedNum].url);
    rssReader->Start();
}

void cNopacityDisplayMenu::SetNextFeed(void) {
    int nextFeed = 0;
    for (int i = 1; i<6; i++) {
        nextFeed = (currentFeed + i)%5;
        if ((nextFeed == 0)||(config.rssFeed[nextFeed] > 0)) {
            currentFeed = nextFeed;
            break;
        }    
    }
} 