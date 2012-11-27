#ifndef __NOPACITY_MENUITEM_H
#define __NOPACITY_MENUITEM_H

class cNopacityMenuItem : public cListObject {
protected:
	cOsd *osd;
	cPixmap *pixmap;
	cPixmap *pixmapIcon;
	bool hasIcon;
	bool iconDrawn;
	int *handleBackgrounds;
	const char *Text;
	bool selectable;
	bool current;
	cFont *font;
	int width, height;
	cString *itemTabs;
	int *tabWidth;
	int numTabs;
	void DrawDelimiter(const char *del, const char *icon, int handleBgrd);
public:
	cNopacityMenuItem(cOsd *osd, const char *text, bool cur, bool sel);
	virtual ~cNopacityMenuItem(void);
	void CreatePixmap(int top, int space, int index, int width, int height);
	void CreatePixmapIcon(int top, int space, int index, int itemHeight, int iconWidth, int iconHeight);
	void SetFont(cFont *font) {this->font = font;}
	void SetCurrent(bool cur) {current = cur;}
	void SetAlpha(int alpha) {this->pixmap->SetAlpha(alpha);}
	void SetAlphaIcon(int alpha) {if (hasIcon) this->pixmapIcon->SetAlpha(alpha);}
	void SetTabs(cString *tabs, int *tabWidths, int numtabs);
	void SetBackgrounds(int *handleBackgrounds);
	virtual void Render() = 0;
};

class cNopacityMainMenuItem : public cNopacityMenuItem {
private:
	cString menuNumber;
	cString menuEntry;
	void SplitMenuItem();
	static std::string items[6];
	cString GetIconName();
public:
	cNopacityMainMenuItem(cOsd *osd, const char *text, bool cur, bool sel);
	~cNopacityMainMenuItem(void);
	void Render();
};

enum eMenuSubCategory { mcSubUndefined = -1, mcSubSchedule = 0, mcSubScheduleWhatsOn, mcSubScheduleTimer, mcSubChannels, mcSubChannelEdit};

class cNopacityScheduleMenuItem : public cNopacityMenuItem {
private:
	eMenuSubCategory subCategory;
	void DrawRemaining(cString remaining, int x, int y, int width);
public:
	cNopacityScheduleMenuItem(cOsd *osd, const char *text, bool cur, bool sel, eMenuSubCategory subCat);
	~cNopacityScheduleMenuItem(void);
	void Render();
};

class cNopacityChannelMenuItem : public cNopacityMenuItem {
private:
public:
	cNopacityChannelMenuItem(cOsd *osd, const char *text, bool cur, bool sel);
	~cNopacityChannelMenuItem(void);
	void Render();
};

class cNopacityDefaultMenuItem : public cNopacityMenuItem {
private:
public:
	cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool cur, bool sel);
	~cNopacityDefaultMenuItem(void);
	void Render();
};

class cNopacityTrackMenuItem : public cNopacityMenuItem {
private:
public:
	cNopacityTrackMenuItem(cOsd *osd, const char *text, bool cur);
	~cNopacityTrackMenuItem(void);
	void Render();
};
#endif //__NOPACITY_MENUITEM_H
