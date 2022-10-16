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

#endif //__NOPACITY_MENUDETAILVIEW_H
