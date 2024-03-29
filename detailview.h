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
    cPixmap *pixmapHeaderBack = NULL;
    cPixmap *pixmapLogo = NULL;
    cPixmap *pixmapContent = NULL;
    cPixmap *pixmapContentBack = NULL;
    cPixmap *pixmapScrollbar = NULL;
    cPixmap *pixmapScrollbarBack = NULL;
    cPixmap *pixmapTabs = NULL;
    cPixmap *pixmapHeaderEPGImage = NULL;
    cPixmap *pixmapHeaderBanner = NULL;
    cPixmap *pixmapHeaderPoster = NULL;
    cPixmap *pixmapHeaderImage = NULL;
    cPixmap *pixmapPoster = NULL;
    cFont *font = NULL;
    cFont *fontSmall = NULL;
    cFont *fontHeader = NULL;
    cFont *fontHeaderLarge = NULL;
    cFont *fontFixed = NULL;
    cMovie movie;
    cSeries series;
    bool textView = false;
    int activeView;
    bool scrollable;
    bool tabbed;
    bool isMovie = false;
    bool isSeries = false;
    bool FixedFont = false;
    int x, y;
    int width, height;
    int border;
    int headerHeight, contentHeight, contentWidth, tabHeight;
    int actorThumbWidth;
    int actorThumbHeight;
    int contentDrawPortHeight;
    int contentX = 0;;
    int yBanner = 0;
    int yEPGText = 0;
    int yActors = 0;
    int yScrapInfo = 0;
    int yFanart = 0;
    int yAddInf = 0;
    int yEPGPics = 0;
    bool hasManualPoster = false;
    int widthPoster = 0;
    cString manualPosterPath = "";
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
    void LoadAddInfo(std::string *text);
    int HeightActorPics(void);
    int HeightScraperInfo(void);
    int HeightFanart(void);
    cTextWrapper addInfo;
    cTextWrapper scrapInfo;
    void DrawTextWrapper(cTextWrapper *wrapper, int top);
    void DrawHeader(int wP = 0);
    void ClearContent(void);
    void CreateContent(int fullHeight);
    void DrawContent(std::string *text, int y = 0, bool useFixedFont = false);
    void DrawFloatingContent(std::string *infoText, cTvMedia *img, cTvMedia *img2 = NULL);
    void CreateFloatingTextWrapper(cTextWrapper *twNarrow, cTextWrapper *twFull, std::string *text, int widthImg, int heightImg);
    int DrawHeaderPosterLight(void);
    void DrawPoster(void);
    void DrawBanner(int height);
    void DrawAdditionalBanners(int top, int bottom);
    void DrawActors(std::vector<cActor> *actors);
    void DrawActors(int height);
    void DrawFanart(int height);
    void ClearScrollbar(void);
    virtual void SetTabs(void) {};
    void DrawTabs(void);
public:    
    cNopacityView(cOsd *osd);
    virtual ~cNopacityView(void);
    void SetAlpha(int Alpha = 0);
    void SetFixedFont(bool FixedFont) { this->FixedFont = FixedFont; };
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
    bool KeyUp(bool Page = false);
    bool KeyDown(bool Page = false);
    virtual void KeyLeft(void) {};
    virtual void KeyRight(void) {};
    void KeyInput(bool Up, bool Page);
    void DrawScrollbar(void);
    virtual void SetContent(void) {};
    virtual void SetContentHeight(void) {};
    virtual void CreatePixmaps(void) {};
    virtual void Render(void) {};
};

class cNopacityEPGView : public cNopacityView {
protected:
    std::vector<std::string> epgPics;
    int numEPGPics;
    int numTabs;
    void SetTabs(void);
    int DrawHeaderEPGImage(void);
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
    int DrawHeaderBanner(void);
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
    int DrawHeaderPoster(void);
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

class cNopacityMenuDetailEventViewLight : public cNopacityView {
private:
    const cEvent *event;
    cTextWrapper epgText;
    int numEPGPics;
    int HeightEPGPics(void);
    void DrawEPGPictures(int height);
public:
    cNopacityMenuDetailEventViewLight(cOsd *osd, const cEvent *Event);
    virtual ~cNopacityMenuDetailEventViewLight(void) {};
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void Render(void);
};

class cNopacityMenuDetailRecordingViewLight : public cNopacityView {
private:
    const cRecording *recording;
    const cRecordingInfo *info;
    cTextWrapper recInfo;
    std::vector<std::string> epgpics;
    bool LoadEPGPics(void);
    int HeightEPGPics(void);
    void DrawEPGPictures(int height);
public:
    cNopacityMenuDetailRecordingViewLight(cOsd *osd, const cRecording *Recording);
    virtual ~cNopacityMenuDetailRecordingViewLight(void) {};
    void SetContent(void);
    void SetContentHeight(void);
    void CreatePixmaps(void);
    void Render(void);
};

#endif //__NOPACITY_DETAILVIEW_H
