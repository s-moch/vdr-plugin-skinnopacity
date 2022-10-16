#ifndef __NOPACITY_DETAILVIEW_H
#define __NOPACITY_DETAILVIEW_H

#include <vector>
#include <string>
#include <sstream>
#include <vdr/skins.h>
#include "services/scraper2vdr.h"
#include "services/epgsearch.h"
#include "config.h"
#include "imagecache.h"
#include "imageloader.h"
#include "helpers.h"

enum eEPGViewTabs {
    evtInfo = 0,
    evtAddInfo,
    evtImages,
    evtCount
};

enum eMediaViewTabs {
    mvtInfo = 0,
    mvtAddInfo,
    mvtCast,
    mvtOnlineInfo,
    mvtImages,
    mvtCount
};

class cNopacityView {
protected:
    cOsd *osd;
    cPixmap *pixmapHeader = NULL;
    cPixmap *pixmapContent = NULL;
    cPixmap *pixmapContentBack = NULL;
    cPixmap *pixmapScrollbar = NULL;
    cPixmap *pixmapScrollbarBack = NULL;
    cPixmap *pixmapTabs = NULL;
    cPixmap *pixmapHeaderEPGImage = NULL;
    cPixmap *pixmapHeaderBanner = NULL;
    cPixmap *pixmapHeaderPoster = NULL;
    cFont *font, *fontSmall, *fontHeader, *fontHeaderLarge;
    int activeView;
    bool scrollable;
    bool tabbed;
    int x, y;
    int width, height;
    int border;
    int headerHeight, contentHeight, tabHeight;
    std::vector<std::string> tabs;
    std::string title;
    std::string subTitle;
    std::string dateTime;
    std::string infoText;
    std::string addInfoText;
    const cChannel *channel;
    int eventID;
    std::string recFileName;
    bool headerDrawn;
    void SetFonts(void);
    void DrawHeader(void);
    void ClearContent(void);
    void CreateContent(int fullHeight);
    void DrawContent(std::string *text);
    void DrawFloatingContent(std::string *infoText, cTvMedia *img, cTvMedia *img2 = NULL);
    void CreateFloatingTextWrapper(cTextWrapper *twNarrow, cTextWrapper *twFull, std::string *text, int widthImg, int heightImg);
    void DrawActors(std::vector<cActor> *actors);
    void ClearScrollbar(void);
    virtual void SetTabs(void) {};
    void DrawTabs(void);
public:    
    cNopacityView(cOsd *osd);
    virtual ~cNopacityView(void);
    void SetAlpha(int Alpha = 0);
    void SetTitle(const char *t) { title = t ? t : ""; };
    void SetSubTitle(const char *s) { subTitle = s ? s : ""; };
    void SetDateTime(const char *dt) { dateTime = dt; };
    void SetInfoText(const char *i) { infoText = i ? i : ""; };
    void SetAdditionalInfoText(std::string addInfo) { addInfoText = addInfo; };
    void SetChannel(const cChannel *c) { channel = c; };
    void SetEventID(int id) { eventID = id; };
    void SetRecFileName(const char *name) { recFileName = name ? name : ""; };
    virtual void LoadMedia(void) {};
    void SetGeometry(int x, int y, int width, int height, int border, int headerHeight);
    void SetScrollbarPixmaps(cPixmap *s, cPixmap *sb) { pixmapScrollbar = s; pixmapScrollbarBack = sb; };
    virtual bool KeyUp(void);
    virtual bool KeyDown(void);
    virtual void KeyLeft(void) {};
    virtual void KeyRight(void) {};
    void DrawScrollbar(void);
    virtual void Render(void) {};
};

class cNopacityEPGView : public cNopacityView {
protected:
    std::vector<std::string> epgPics;
    int numEPGPics;
    int numTabs;
    void SetTabs(void);
    void DrawHeaderEPGImage(void);
    void CheckEPGImages(void);
    void DrawImages(void);
public:    
    cNopacityEPGView(cOsd *osd);
    virtual ~cNopacityEPGView(void) {};
    void KeyLeft(void);
    void KeyRight(void);
    void Render(void);
};

class cNopacitySeriesView : public cNopacityView {
protected:
    int seriesId;
    int episodeId;
    cSeries series;
    std::string tvdbInfo;
    void SetTabs(void);
    void CreateTVDBInfo(void);
    void DrawHeaderBanner(void);
    void DrawImages(void);
    int GetRandomPoster(void);
public:    
    cNopacitySeriesView(cOsd *osd, int seriesId, int episodeId);
    virtual ~cNopacitySeriesView(void) {};
    void LoadMedia(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Render(void);
};

class cNopacityMovieView : public cNopacityView {
protected:
    int movieId;
    cMovie movie;
    std::string movieDBInfo;
    void SetTabs(void);
    void CreateMovieDBInfo(void);
    void DrawHeaderPoster(void);
    void DrawImages(void);
public:    
    cNopacityMovieView(cOsd *osd, int movieId);
    virtual ~cNopacityMovieView(void) {};
    void LoadMedia(void);
    void KeyLeft(void);
    void KeyRight(void);
    void Render(void);
};

class cNopacityTextView : public cNopacityView {
protected:
public:    
    cNopacityTextView(cOsd *osd);
    virtual ~cNopacityTextView(void) {};
    void KeyLeft(void);
    void KeyRight(void);
    void Render(void);
};

class cNopacityMenuDetailViewLight : public cNopacityView {
protected:
    cOsd *osd;
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
    cPixmap *scrollBar;
    cPixmap *scrollBarBack;
    bool hasManualPoster;
    cString manualPosterPath;
    cMovie movie;
    cSeries series;
    bool isMovie;
    bool isSeries;
    cTextWrapper scrapInfo;
    void SetFonts(void);
    void DrawTextWrapper(cTextWrapper *wrapper, int top);
    int HeightActorPics(void);
    int HeightScraperInfo(void);
    int HeightFanart(void);
    void DrawPoster(void);
    void DrawBanner(int height);
    void DrawAdditionalBanners(int top, int bottom);
    void DrawActors(int height);
    void DrawFanart(int height);
    void ClearScrollbar(void);
    void DrawScrollbar(void);
public:
    cNopacityMenuDetailViewLight(cOsd *osd, cPixmap *s, cPixmap *sBack);
    virtual ~cNopacityMenuDetailViewLight(void);
    void SetGeometry(int x, int y, int width, int height, int border, int headerHeight);
    void SetAlpha(int Alpha = 0);
    void KeyInput(bool Up, bool Page);
    virtual void SetContent(void) = 0;
    virtual void SetContentHeight(void) = 0;
    virtual void CreatePixmaps(void) = 0;
    virtual void Render(void) = 0;
};

class cNopacityMenuDetailEventViewLight : public cNopacityMenuDetailViewLight {
private:
    const cEvent *event;
    cTextWrapper epgText;
    cTextWrapper reruns;
    int numEPGPics;
    void DrawHeader(void);
    void LoadReruns(void);
    int HeightEPGPics(void);
    void DrawEPGPictures(int height);
public:
    cNopacityMenuDetailEventViewLight(cOsd *osd, const cEvent *Event, cPixmap *s, cPixmap *sBack);
    virtual ~cNopacityMenuDetailEventViewLight(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void Render(void);
};

class cNopacityMenuDetailRecordingViewLight : public cNopacityMenuDetailViewLight {
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
public:
    cNopacityMenuDetailRecordingViewLight(cOsd *osd, const cRecording *Recording, cPixmap *s, cPixmap *sBack);
    virtual ~cNopacityMenuDetailRecordingViewLight(void);
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void Render(void);
};

#endif //__NOPACITY_DETAILVIEW_H
