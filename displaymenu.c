#include "displaymenu.h"
#include <string>

cNopacityDisplayMenu::cNopacityDisplayMenu(void) {
	int start = cTimeMs::Now();
	config.setDynamicValues();
	menuCategoryLast = mcUndefined;
	menuSubCategory = mcSubUndefined;
	menuSubCategoryLast = mcSubUndefined;
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
	menuHasIcons = true;
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
	dsyslog("nopacity: Construktor needed %d ms", int(cTimeMs::Now()-start));
}

cNopacityDisplayMenu::~cNopacityDisplayMenu() {
	if (Running()) {
		Cancel();
		cCondWait::SleepMs(300);
	}
	delete menuView;
	menuItems.Clear();
	if (detailView) {
		delete detailView;
	}
	timers.Clear();
	for (int i=0; i<6; i++)
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
	if (((MenuCategory() == mcChannel) && (menuSubCategory == mcSubChannelEdit) && (menuSubCategoryLast != mcSubChannelEdit))
		|| (menuSubCategory == mcSubScheduleTimer)){
		maxItems = config.numDefaultMenuItems;
	} else {
		maxItems = menuView->GetMaxItems(MenuCategory());
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
		mcMain = 1, 
		mcSchedule = 2, 
		mcChannel = 3, 
		mcTimer = 4, 
		mcRecording = 5, 
		mcPlugin = 6, 
		mcSetup = 7, 
		mcCommand = 8, 
		mcEvent = 9, 
		mcText = 10, 
		mcFolder = 11, 
		mcCam = 12
	*/
	menuCategoryLast = this->MenuCategory();
	cSkinDisplayMenu::SetMenuCategory(MenuCategory);
	if ((menuCategoryLast == mcMain) && (MenuCategory != mcMain)) {
		menuView->ShowDiskUsage(false);
		for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
			t->SetLayer(-1);
		} 
		diskUsageDrawn = false;
		timersDrawn = false;
	}
}

void cNopacityDisplayMenu::SetTitle(const char *Title) {
	contentNarrowLast = contentNarrow;
	menuSubCategoryLast = menuSubCategory;
	int left = 5;
	menuView->DestroyHeaderIcon();
	if (Title) {
		cString title = Title;
		if (MenuCategory() == mcMain) {
			title = cString::sprintf("%s %s", Title, VDRVERSION);
			left += menuView->ShowHeaderLogo(true);
			contentNarrow = true;
			menuHasIcons = true;
		} else {
			menuView->ShowHeaderLogo(false);
			if (MenuCategory() == mcSchedule) {
				if (startswith(Title, trVDR("Schedule"))) {
					menuSubCategory = mcSubSchedule;
					left += menuView->ShowHeaderIconChannelLogo(Title);
					menuHasIcons = true;
					contentNarrow = true;
				} else if (endswith(Title, "%")) {	
				  //hack for epgsearch timer conflict view
					menuSubCategory = mcSubScheduleTimer;
					menuHasIcons = false;
					contentNarrow = false;
					currentNumItems = config.numDefaultMenuItems;
				} else {
					menuSubCategory = mcSubScheduleWhatsOn;
					left += menuView->DrawHeaderIcon(mcSchedule);
					menuHasIcons = true;
					contentNarrow = true;
				}
			} else if (MenuCategory() == mcChannel) {
				left += menuView->DrawHeaderIcon(mcChannel);
				if (startswith(Title, trVDR("Channels"))) {
					contentNarrow = true;
					menuSubCategory = mcSubChannels;
					menuHasIcons = true;
				} else {
					contentNarrow = false;
					menuSubCategory = mcSubChannelEdit;
					menuHasIcons = false;
				}
			} else {
				left += menuView->DrawHeaderIcon(MenuCategory());
				contentNarrow = false;
				menuHasIcons = false;
			}
		}
		menuView->AdjustContentBackground(contentNarrow, contentNarrowLast);		
		menuView->DrawHeaderLabel(left, title);
		if (MenuCategory() == mcEvent) {
			channelName = Title;
		}
	}
}

void cNopacityDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
	tColor lutBg[] = { Theme.Color(clrButtonRedBorder), Theme.Color(clrButtonGreenBorder), Theme.Color(clrButtonYellowBorder), Theme.Color(clrButtonBlueBorder) };
	if (Red) {
		menuView->DrawButton(Red, handleButtons[0], lutBg[Setup.ColorKey0], 0);
	} else 
	    menuView->ClearButton(0);
	
	if (Green) {
		menuView->DrawButton(Green, handleButtons[1], lutBg[Setup.ColorKey1], 1);
	} else 
	    menuView->ClearButton(1);
	
	if (Yellow) {
		menuView->DrawButton(Yellow, handleButtons[2], lutBg[Setup.ColorKey2], 2);
	} else 
	    menuView->ClearButton(2);
	
	if (Blue) {
		menuView->DrawButton(Blue, handleButtons[3], lutBg[Setup.ColorKey3], 3);
	} else 
	    menuView->ClearButton(3);
}

void cNopacityDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
	if (Text) {
		menuView->DrawMessage(Type, Text);
    } else {
		menuView->ClearMessage();
	}
}

void cNopacityDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
	cString *strItems = new cString[MaxTabs];
	int *tabItems = new int[2*MaxTabs];
	for (int i=0; i<MaxTabs; i++) {
		strItems[i] = "";
		tabItems[i] = 0;
		tabItems[i+MaxTabs] = 0;
	}
	SplitItem(Text, strItems, tabItems);
	int menuIconWidth = 0;
	int menuIconHeight = 0;
	if (initMenu) {
		if (Index > menuItemIndexLast) {
			cNopacityMenuItem *item;
			cPoint itemSize;
			if (MenuCategory() == mcMain) {
				item = new cNopacityMainMenuItem(osd, Text, Current, Selectable);
				menuView->GetMenuItemSize(mcMain, &itemSize);
				item->SetFont(menuView->GetMenuItemFont(mcMain));
				menuIconWidth = menuIconHeight = config.iconHeight;
			} else if (MenuCategory() == mcSchedule) {
				if (menuSubCategory == mcSubScheduleTimer) {
					item = new cNopacityDefaultMenuItem(osd, Text, Current, Selectable);
					menuView->GetMenuItemSize(mcUnknown, &itemSize);
					item->SetFont(menuView->GetMenuItemFont(mcUnknown));
				} else {
					item = new cNopacityScheduleMenuItem(osd, Text, Current, Selectable, menuSubCategory);
					menuView->GetMenuItemSize(mcSchedule, &itemSize);
					item->SetFont(menuView->GetMenuItemFont(mcSchedule));
					menuIconWidth = config.menuItemLogoWidth;
					menuIconHeight = config.menuItemLogoHeight;
				}
			} else if (MenuCategory() == mcChannel) {
				if (menuSubCategory == mcSubChannels) {
					item = new cNopacityChannelMenuItem(osd, Text, Current, Selectable);
					menuView->GetMenuItemSize(mcChannel, &itemSize);
					item->SetFont(menuView->GetMenuItemFont(mcChannel));
					menuIconWidth = config.menuItemLogoWidth;
					menuIconHeight = config.menuItemLogoHeight;
				} else {
					item = new cNopacityDefaultMenuItem(osd, Text, Current, Selectable);
					menuView->GetMenuItemSize(mcUnknown, &itemSize);
					item->SetFont(menuView->GetMenuItemFont(mcUnknown));
				}
			} else {
				item = new cNopacityDefaultMenuItem(osd, Text, Current, Selectable);
				menuView->GetMenuItemSize(mcUnknown, &itemSize);
				item->SetFont(menuView->GetMenuItemFont(mcUnknown));
			}
			item->SetBackgrounds(handleBackgrounds);
			item->SetTabs(strItems, tabItems, MaxTabs);
			int spaceTop = menuView->GetMenuTop(currentNumItems, itemSize.Y());
			if (menuHasIcons) {
				item->CreatePixmapIcon(spaceTop, menuView->spaceMenu, Index, itemSize.Y(), menuIconWidth, menuIconHeight);
			}
			item->CreatePixmap(spaceTop, menuView->spaceMenu, Index, itemSize.X(), itemSize.Y());
			menuItems.Add(item);
			item->Render();
			menuItemIndexLast = Index;
			if (initial) {
				if (FadeTime) {
					item->SetAlpha(0);
					item->SetAlphaIcon(0);
				}
			}
		} else {
			//adjust Current if item was added twice
			cNopacityMenuItem *item = menuItems.Get(Index);
			item->SetCurrent(Current);
			item->Render();
		}
	} else {
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
			tabItems[i] = Tab(i);
			if (i>0) {
				tabItems[(i-1) + MaxTabs] = Tab(i) - x;
			}
			x += Tab(i) - x;
        }
		if (!Tab(i + 1)) {
			tabItems[i + MaxTabs] = menuView->GetWidthDefaultMenu() - x;
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
	detailView = new cNopacityMenuDetailEventView(osd, Event, channelName);
	menuView->SetDetailViewSize(dvEvent, detailView);
	detailView->SetFonts();
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
		DrawDisk();
		DrawTimers();
	}
	if (initial) {
		if (FadeTime)
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
		for (cNopacityMenuItem *item = menuItems.First(); item; item = menuItems.Next(item)) {
			item->SetAlpha(Alpha);
			item->SetAlphaIcon(Alpha);
		}
		for (cNopacityTimer *t = timers.First(); t; t = timers.Next(t)) {
			t->SetAlpha(Alpha);
		} 
		osd->Flush();
		cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Delta < FrameTime)
           cCondWait::SleepMs(FrameTime - Delta);
		if ((int)(Now - Start) > FadeTime)
			break;
    }
}
