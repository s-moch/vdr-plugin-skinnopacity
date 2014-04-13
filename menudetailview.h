#ifndef __NOPACITY_MENUDETAILVIEW_H
#define __NOPACITY_MENUDETAILVIEW_H

#include <vdr/recording.h>
#include <vdr/skins.h>

#include <vector>
#include <string>
#include "services/scraper2vdr.h"
#include "services/tvscraper.h"
#include "imagecache.h"

class cNopacityMenuDetailView : public cThread {
protected:
    cOsd *osd;
    cImageCache *imgCache;
    bool hasScrollbar;
    int x, width, height, top;
    int headerHeight;
    int contentWidth;
    int contentX;
    int contentHeight;
    int contentDrawPortHeight;
    int widthPoster;
    int border;
    int yBanner;
    int yEPGText;
    int yActors;
    int yScrapInfo;
    int yFanart;
    int yAddInf;
    int yEPGPics;
    int actorThumbWidth;
    int actorThumbHeight;
    cFont *font, *fontSmall, *fontHeader, *fontHeaderLarge;
    cPixmap *pixmapHeader;
    cPixmap *pixmapLogo;
    cPixmap *pixmapContent;
    cPixmap *pixmapPoster;
    bool hasManualPoster;
    cString manualPosterPath;
    cMovie movie;
    cSeries series;
    bool isMovie;
    bool isSeries;
    cTextWrapper scrapInfo;
    TVScraperGetFullInformation mediaInfo;
    bool hasAdditionalMedia;
    void DrawTextWrapper(cTextWrapper *wrapper, int top);
    int HeightActorPics(void);
    int HeightScraperInfo(void);
    int HeightFanart(void);
    void DrawPoster(void);
    void DrawBanner(int height);
    void DrawAdditionalBanners(int top, int bottom);
    void DrawActors(int height);
    void DrawFanart(int height);
    virtual void Action(void) {};
public:
    cNopacityMenuDetailView(cOsd *osd, cImageCache *imgCache);
    virtual ~cNopacityMenuDetailView(void);
    void SetGeometry(int x, int width, int height, int top, int contentBorder, int headerHeight);
    virtual void SetFonts(void) = 0;
    bool Scrollable(void) {return hasScrollbar;}
    double ScrollbarSize(void);
    double Offset(void);
    bool Scroll(bool Up, bool Page);
    virtual void SetContent(void) = 0;
    virtual void SetContentHeight(void) = 0;
    virtual void CreatePixmaps(void) = 0;
    virtual void Render(void) = 0;
};

class cNopacityMenuDetailEventView : public cNopacityMenuDetailView {
private:
    const cEvent *event;
    cTextWrapper epgText;
    cTextWrapper reruns;
    int numEPGPics;
    void DrawHeader(void);
    void LoadReruns(void);
    int HeightEPGPics(void);
    void DrawEPGPictures(int height);
    void Action(void);
public:
    cNopacityMenuDetailEventView(cOsd *osd, cImageCache *imgCache, const cEvent *Event);
    virtual ~cNopacityMenuDetailEventView(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void SetFonts(void);
    void Render(void);
};

class cNopacityMenuDetailRecordingView : public cNopacityMenuDetailView {
private:
    const cRecording *recording;
    const cRecordingInfo *info;
    cTextWrapper recInfo;
    cTextWrapper additionalInfo;
    void DrawHeader(void);
    void LoadRecordingInformation(void);
    std::string StripXmlTag(std::string &Line, const char *Tag);
    int ReadSizeVdr(const char *strPath);
    std::vector<std::string> epgpics;
    bool LoadEPGPics(void);
    int HeightEPGPics(void);
    void DrawEPGPictures(int height);
    void Action(void);
public:
    cNopacityMenuDetailRecordingView(cOsd *osd, const cRecording *Recording);
    virtual ~cNopacityMenuDetailRecordingView(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void SetFonts(void);
    void Render(void);
};

class cNopacityMenuDetailTextView : public cNopacityMenuDetailView {
private:
    const char *text;
    cTextWrapper content;
public:
    cNopacityMenuDetailTextView(cOsd *osd, const char *text);
    virtual ~cNopacityMenuDetailTextView(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void SetFonts(void);
    void Render(void);
};
#endif //__NOPACITY_MENUDETAILVIEW_H