#include "services/remotetimers.h"
namespace PluginRemoteTimers {
    static int CompareTimers(const void *a, const void *b) {
        return (*(const cTimer **)a)->Compare(**(const cTimer **)b);
    }
}

#include "displaymenuview.h"
#include "displaymenu.h"
#include "config.h"
#include <string>
#include "services/epgsearch.h"

cNopacityDisplayMenu::cNopacityDisplayMenu(void) : cThread("DisplayMenu") {
    menuCategoryLast = mcUndefined;
    menuFadeTime = config.GetValue("menuFadeTime");
    menuFadeOutTime = config.GetValue("menuFadeOutTime");
    fadeout = false;
    initial = true;
    diskUsageDrawn = false;
    timersDrawn = false;
    lastDiskUsageState = -1;
    lastTimersState = -1;
    currentNumItems = 0;
    detailView = NULL;
    SetButtonPositions();
    osd = CreateOsd(geoManager->osdLeft, geoManager->osdTop, geoManager->osdWidth, geoManager->osdHeight);
    menuView = new cNopacityDisplayMenuView(osd);
    currentFeed = 0;
    SetTabs(0);
}

cNopacityDisplayMenu::~cNopacityDisplayMenu(void) {
    if (config.GetValue("animation") && menuFadeOutTime) {
        if ((MenuCategory() != mcPluginSetup) && (MenuCategory() != mcSetupPlugins)) {
            fadeout = true;
            Start();
        }
    }
    int count = 0;
    while (Active()) {
        cCondWait::SleepMs(10);
        count++;
        if (count > 200)
           Cancel(1);
    }
    if (detailView)
        delete detailView;
    if (menuView)
        delete menuView;
    menuItems.clear();
    timers.Clear();
    delete osd;
    cDevice::PrimaryDevice()->ScaleVideo(cRect::Null);
}

void cNopacityDisplayMenu::DrawDisk(void) {
    if (!config.GetValue("narrowMainMenu"))
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
    if (!config.GetValue("narrowMainMenu"))
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
            LOCK_TIMERS_READ;
            cSortedTimers SortedTimers(Timers);
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
                currentHeight += t->GetHeight() + geoManager->menuSpace;
                timers.Add(t);
            }
            int numTimersDisplayed = 0;
            for (int i = 0; i < numTimers; i++) {
                if (const cTimer *Timer = SortedTimers[i]) {
                    if (Timer->HasFlags(tfActive)) {
                        cNopacityTimer *t = menuView->DrawTimer(Timer, currentHeight);
                        currentHeight += t->GetHeight() + geoManager->menuSpace;
                        if (currentHeight < maxTimersHeight) {
                            timers.Add(t);
                        numTimersDisplayed++;
                        if (numTimersDisplayed == config.GetValue("numberTimers"))
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
    if (!detailView)
        return;
    detailView->KeyInput(Up, Page);
}

int cNopacityDisplayMenu::MaxItems(void) {
    int maxItems = 0;
    switch (MenuCategory()) {
        case mcMain:
            if (config.GetValue("narrowMainMenu"))
                maxItems = config.GetValue("numMainMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcSetup:
            if (config.GetValue("narrowSetupMenu"))
                maxItems = config.GetValue("numMainMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            if (config.GetValue("narrowScheduleMenu"))
                maxItems = config.GetValue("numSchedulesMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcChannel:
            if (config.GetValue("narrowChannelMenu"))
                maxItems = config.GetValue("numSchedulesMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcTimer:
            if (config.GetValue("narrowTimerMenu"))
                maxItems = config.GetValue("numSchedulesMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        case mcRecording:
            if (config.GetValue("narrowRecordingMenu"))
                maxItems = config.GetValue("numRecordingsMenuItems");
            else
                maxItems = config.GetValue("numDefaultMenuItems");
            break;
        default:
            maxItems = config.GetValue("numDefaultMenuItems");
    }
    currentNumItems = maxItems;
    return maxItems;
}

void cNopacityDisplayMenu::Clear(void) {
    if (menuFadeOutTime)
        return;
    DELETENULL(detailView);
    menuItems.clear();
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
      11 mcRecordingEdit,
      12 mcPlugin,
      13 mcPluginSetup,
      14 mcSetup,
      15 mcSetupOsd,
      16 mcSetupEpg,
      17 mcSetupDvb,
      18 mcSetupLnb,
      19 mcSetupCam,
      20 mcSetupRecord,
      21 mcSetupReplay,
      22 mcSetupMisc,
      23 mcSetupPlugins,
      24 mcCommand,
      25 mcEvent,
      26 mcText,
      27 mcFolder,
      28 mcCam
      */
    menuCategoryLast = this->MenuCategory();
    cSkinDisplayMenu::SetMenuCategory(MenuCategory);
    if ((MenuCategory != mcRecordingInfo) && (MenuCategory != mcEvent && MenuCategory != mcText)) {
        DELETENULL(detailView);
    }
    if ((menuCategoryLast == mcMain) && (MenuCategory != mcMain)) {
        if (config.GetValue("showDiscUsage")) {
            menuView->ShowDiskUsage(false);
            diskUsageDrawn = false;
        }
        if (config.GetValue("showTimers")) {
            for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
                t->Hide();
            }
            timersDrawn = false;
        }
    }
}

void cNopacityDisplayMenu::SetTitle(const char *Title) {
    //resetting menuitems if no call to clear
    DELETENULL(detailView);
    menuItems.clear();
    int left = 5;
    menuView->DestroyHeaderIcon();
    if (Title) {
        cString title = Title;
        switch (MenuCategory()) {
            case mcMain:
                switch (config.GetValue("mainMenuTitleStyle")) {
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
                left += menuView->ShowHeaderIconChannelLogo(Title, initial);
                break;
            case mcRecording: {
                menuView->ShowHeaderLogo(false);
                cString Text = cString::sprintf(" %s", trVDR("Recordings"));
                if (config.GetValue("displayNumberOfRecordings") && (strcmp(Text, title) == 0)) {
                    {
#if APIVERSNUM > 20300
                    LOCK_RECORDINGS_READ;
#endif
                    countRecordings = Recordings->Count();
                    }
                    title = cString::sprintf("%i %s (%s)", countRecordings, Title, *cVideoDiskUsage::String());
                } else
                    title = cString::sprintf("%s (%s)", Title, *cVideoDiskUsage::String());
                left += menuView->DrawHeaderIcon(MenuCategory(), initial);
                }
                break;
            default:
                menuView->ShowHeaderLogo(false);
                left += menuView->DrawHeaderIcon(MenuCategory(), initial);
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
    menuView->ClearButton();
    if (Red)
        menuView->DrawButton(Red, seButtonRed, Theme.Color(clrButtonRed), Theme.Color(clrButtonRedBorder), Theme.Color(clrButtonRedFont), positionButtons[0]);
    if (Green)
        menuView->DrawButton(Green, seButtonGreen,Theme.Color(clrButtonGreen),  Theme.Color(clrButtonGreenBorder), Theme.Color(clrButtonGreenFont), positionButtons[1]);
    if (Yellow)
        menuView->DrawButton(Yellow, seButtonYellow, Theme.Color(clrButtonYellow), Theme.Color(clrButtonYellowBorder), Theme.Color(clrButtonYellowFont), positionButtons[2]);
    if (Blue)
        menuView->DrawButton(Blue, seButtonBlue, Theme.Color(clrButtonBlue), Theme.Color(clrButtonBlueBorder), Theme.Color(clrButtonBlueFont), positionButtons[3]);
}

void cNopacityDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
    menuView->DrawMessage(Type, Text);
}

bool cNopacityDisplayMenu::SetItemEvent(const cEvent *Event, int Index, bool Current,
                                        bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch, bool TimerActive) {

    if (!config.GetValue("narrowScheduleMenu"))
        return false;
    if ((int)menuItems.size() <= Index)
        menuItems.resize(currentNumItems);
    if (!menuItems[Index]) {
        cNopacityMenuItem *item = new cNopacityScheduleMenuItem(osd, Event, Channel, TimerMatch, Selectable, MenuCategory(), &videoWindowRect);
        cPoint itemSize;
        menuItems[Index].reset(item);
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcSchedule));
        item->SetCurrent(Current);
        item->CreateText();
        item->CreatePixmaps();
        int textWidth = item->CheckScrollable((Channel)?true:false);
        if (textWidth > 0)
            item->CreatePixmapTextScroller(textWidth);
        item->Render(initial, fadeout);
    } else {
        cNopacityMenuItem *item = menuItems[Index].get();
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable) {
    if (!config.GetValue("narrowTimerMenu"))
        return false;
    if ((int)menuItems.size() <= Index)
        menuItems.resize(currentNumItems);
    if (!menuItems[Index]) {
        cNopacityMenuItem *item = new cNopacityTimerMenuItem(osd, Timer, Selectable, &videoWindowRect);
        cPoint itemSize;
        menuItems[Index].reset(item);
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcTimer));
        item->SetCurrent(Current);
        item->CreateText();
        item->CreatePixmaps();
        item->Render(initial, fadeout);
    } else {
        cNopacityMenuItem *item = menuItems[Index].get();
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider) {
    if (!config.GetValue("narrowChannelMenu"))
        return false;
    if ((int)menuItems.size() <= Index)
        menuItems.resize(currentNumItems);
    if (!menuItems[Index]) {
        cNopacityMenuItem *item = new cNopacityChannelMenuItem(osd, Channel, Selectable, &videoWindowRect);
        cPoint itemSize;
        menuItems[Index].reset(item);
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcChannel));
        item->SetCurrent(Current);
        item->CreateText();
        item->CreatePixmaps();
        int textWidth = item->CheckScrollable(true);
        if (textWidth > 0)
            item->CreatePixmapTextScroller(textWidth);
        item->Render(initial, fadeout);
    } else {
        cNopacityMenuItem *item = menuItems[Index].get();
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

bool cNopacityDisplayMenu::SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable,
                                            int Level, int Total, int New) {
    if (!config.GetValue("narrowRecordingMenu"))
        return false;
    if ((int)menuItems.size() <= Index)
        menuItems.resize(currentNumItems);
    if (!menuItems[Index]) {
        cNopacityMenuItem *item = new cNopacityRecordingMenuItem(osd, Recording, Selectable, Level, Total, New, &videoWindowRect);
        cPoint itemSize;
        menuItems[Index].reset(item);
        menuView->GetMenuItemSize(MenuCategory(), &itemSize);
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetTextWindow(menuView->GetDescriptionTextWindowSize(mcRecording));
        item->SetCurrent(Current);
        item->CreateText();
        item->SetPoster();
        item->CreatePixmaps();
        item->Render(initial, fadeout);
    } else {
        cNopacityMenuItem *item = menuItems[Index].get();
        item->SetCurrent(Current);
        item->Render();
    }
    return true;
}

void cNopacityDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
    if (Index < 0)
        return;
    if ((int)menuItems.size() <= Index) {
        int maxitems = MaxItems();
        if (maxitems <= Index)
            return;
        menuItems.resize(maxitems);
    }
    cString *strItems = new cString[MaxTabs];
    int *tabItems = new int[2*MaxTabs];
    for (int i=0; i<MaxTabs; i++) {
        strItems[i] = "";
        tabItems[i] = 0;
        tabItems[i+MaxTabs] = 0;
    }
    SplitItem(Text, strItems, tabItems);
    menuItems[Index].reset();
    if (*Text != '\0') {
        bool hasIcons = false;
        bool MainOrSetup = false;
        cNopacityMenuItem *item;
        cPoint itemSize;
        if (((MenuCategory() == mcMain)&&(config.GetValue("narrowMainMenu"))) || ((MenuCategory() == mcSetup)&&(config.GetValue("narrowSetupMenu")))){
            MainOrSetup = true;
            bool isSetup = (MenuCategory() == mcSetup)?true:false;
            item = new cNopacityMainMenuItem(osd, Text, Selectable, isSetup);
            menuItems[Index].reset(item);
            menuView->GetMenuItemSize(MenuCategory(), &itemSize);
            if (config.GetValue("useMenuIcons"))
                hasIcons = true;
        } else {
            item = new cNopacityDefaultMenuItem(osd, Text, Selectable);
            menuItems[Index].reset(item);
            menuView->GetMenuItemSize(mcUnknown, &itemSize);
        }
        int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
        item->SetGeometry(Index, spaceTop, menuView->GetMenuItemLeft(itemSize.X()), itemSize.X(), itemSize.Y(), geoManager->menuSpace);
        item->SetCurrent(Current);
        item->SetTabs(strItems, tabItems, MaxTabs);
        item->CreateText();
        item->CreatePixmaps(MainOrSetup);
        int textWidth = item->CheckScrollable(hasIcons);
        if (textWidth > 0)
            item->CreatePixmapTextScroller(textWidth);
        item->Render(initial, fadeout);
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
    detailView = new cNopacityDetailView(osd, Event, menuView->GetPixmapScrollbar(), menuView->GetPixmapScrollbarBack());
    deatilViewRenderDone = false;
}

void cNopacityDisplayMenu::SetRecording(const cRecording *Recording) {
    if (!Recording)
        return;
    const cRecordingInfo *Info = Recording->Info();
    if (!Info) {
        return;
    }
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityDetailView(osd, Recording, menuView->GetPixmapScrollbar(), menuView->GetPixmapScrollbarBack());
    deatilViewRenderDone = false;
}

void cNopacityDisplayMenu::SetText(const char *Text, bool FixedFont) {
    if (!Text)
        return;
    menuView->AdjustContentBackground(this->MenuCategory(), menuCategoryLast, videoWindowRect);
    detailView = new cNopacityDetailView(osd, Text, menuView->GetPixmapScrollbar(), menuView->GetPixmapScrollbarBack());
    deatilViewRenderDone = false;
}

void cNopacityDisplayMenu::SetAlpha(int Alpha, bool Force) {
    if (menuView && (Force || Running()))
        menuView->SetAlpha(Alpha);
    if (detailView && (Force || Running()))
        detailView->SetAlpha(Alpha);
    for (auto i = menuItems.begin(); i != menuItems.end(); ++i) {
        if (*i && (Force || Running())) {
            cNopacityMenuItem *item = i->get();
            item->SetAlpha(Alpha);
        }
    }
    for (cNopacityTimer *t = timers.First(); (Force || Running()) && t; t = timers.Next(t))
        t->SetAlpha(Alpha);
}

void cNopacityDisplayMenu::Flush(void) {
    if (Running())
        return;

    if (detailView && !deatilViewRenderDone) {
        detailView->Render();
        deatilViewRenderDone = true;
    }
    menuView->DrawDate(initial);
    if (MenuCategory() == mcMain) {
        if (config.GetValue("showDiscUsage"))
            DrawDisk();
        bool timersChanged = true;
        int numConflicts = 0;
        if (config.GetValue("checkTimerConflict"))
            numConflicts = CheckTimerConflict(timersChanged);
        if (config.GetValue("showTimers"))
            DrawTimers(timersChanged, numConflicts);
    }
    if (initial && config.GetValue("animation") && menuFadeTime) {
        if ((MenuCategory() != mcPluginSetup) && (MenuCategory() != mcSetupPlugins)) {
            SetAlpha(0, true);
            Start();
        }
    }
    if (config.GetValue("displayMenuVolume"))
        menuView->DrawVolume();
    osd->Flush();
    initial = false;
    cDevice::PrimaryDevice()->ScaleVideo(videoWindowRect);
}

void cNopacityDisplayMenu::Action(void) {
    int x = (fadeout) ? 255 : 0;
    int FadeTime = (fadeout) ? menuFadeOutTime : menuFadeTime;
    int FrameTime = FadeTime / 10;
    uint64_t First = cTimeMs::Now();
    cPixmap::Lock();
    cPixmap::Unlock();
    uint64_t Start = cTimeMs::Now();
    dsyslog ("skinnopacity: First Lock(): %lims\n", Start - First);
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        double t = std::min(double(Now - Start) / FadeTime, 1.0);
        int Alpha = std::abs(x - (int(t * ALPHA_OPAQUE)));
        cPixmap::Lock();
        SetAlpha(Alpha);
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
