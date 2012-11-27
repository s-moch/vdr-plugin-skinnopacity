#ifndef __NOPACITY_MENUDETAILVIEW_H
#define __NOPACITY_MENUDETAILVIEW_H

class cNopacityMenuDetailView {
protected:
	cOsd *osd;
	bool hasScrollbar;
	int width, height, top;
	int headerHeight;
	int contentHeight;
	int contentDrawPortHeight;
	int border;
	cTextWrapper content;
	cFont *font, *fontHeader, *fontHeaderLarge;
	cPixmap *pixmapHeader;
	cPixmap *pixmapLogo;
	cPixmap *pixmapContent;
	void DrawContent(void);
public:
	cNopacityMenuDetailView(cOsd *osd);
	virtual ~cNopacityMenuDetailView(void);
	void SetGeometry(int width, int height, int top, int contentBorder, int headerHeight);
	virtual void SetFonts(void) = 0;
	void SetContent(const char *textContent);
	bool Scrollable(void) {return hasScrollbar;}
	double ScrollbarSize(void);
	double Offset(void);
	bool Scroll(bool Up, bool Page);
	virtual void CreatePixmaps(void) = 0;
	virtual void Render(void) = 0;
};

class cNopacityMenuDetailEventView : public cNopacityMenuDetailView {
private:
	void DrawHeader(void);
	const char *channelName;
	const cEvent *event;
public:
	cNopacityMenuDetailEventView(cOsd *osd, const cEvent *Event, const char *channel);
	virtual ~cNopacityMenuDetailEventView(void);
	void CreatePixmaps(void);
	void SetFonts(void);
	void Render(void);
};

class cNopacityMenuDetailRecordingView : public cNopacityMenuDetailView {
private:
	void DrawHeader(void);
	const cRecording *recording;
	const cRecordingInfo *info;
public:
	cNopacityMenuDetailRecordingView(cOsd *osd, const cRecording *Recording);
	virtual ~cNopacityMenuDetailRecordingView(void);
	void CreatePixmaps(void);
	void SetFonts(void);
	void Render(void);
};

class cNopacityMenuDetailTextView : public cNopacityMenuDetailView {
private:
public:
	cNopacityMenuDetailTextView(cOsd *osd);
	virtual ~cNopacityMenuDetailTextView(void);
	void CreatePixmaps(void);
	void SetFonts(void);
	void Render(void);
};
#endif //__NOPACITY_MENUDETAILVIEW_H