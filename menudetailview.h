#ifndef __NOPACITY_MENUDETAILVIEW_H
#define __NOPACITY_MENUDETAILVIEW_H

#include <vdr/recording.h>
#include <vdr/skins.h>

#include <vector>
#include <string>
#include <sstream>
#include "services/scraper2vdr.h"
#include "services/epgsearch.h"
#include "imagecache.h"
#include "config.h"
#include "detailview.h"

enum eDetailViewType {
    dvEvent = 0, 
    dvRecording, 
    dvText
};

class cNopacityMenuDetailViewLight;

class cNopacityDetailView {
protected:
    eDetailViewType type;
    cOsd *osd;
    const cEvent *ev;
    const cRecording *rec;
    const char *text;
    cNopacityMenuDetailViewLight *viewLight;
    cNopacityView *view;
    cPixmap *scrollBar;
    cPixmap *scrollBarBack;
    int x, width, height, top, border;
    int headerHeight;
    void InitiateViewType(void);
    std::string LoadReruns(void);
    std::string LoadRecordingInformation(void);
    std::string StripXmlTag(std::string &Line, const char *Tag);
    int ReadSizeVdr(const char *strPath);
    void Init(void);
public:
    cNopacityDetailView(cOsd *osd, const cEvent *ev, cPixmap *s, cPixmap *sBack);
    cNopacityDetailView(cOsd *osd, const cRecording *rec, cPixmap *s, cPixmap *sBack);
    cNopacityDetailView(cOsd *osd, const char *text, cPixmap *s, cPixmap *sBack);
    virtual ~cNopacityDetailView(void);
    void SetAlpha(int Alpha = 0);
    void KeyInput(bool Up, bool Page);
    void Render(void);
};

class cNopacityMenuDetailViewLight {
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

#endif //__NOPACITY_MENUDETAILVIEW_H
