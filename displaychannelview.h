#ifndef __NOPACITY_DISPLAYCHANNELVIEW_H
#define __NOPACITY_DISPLAYCHANNELVIEW_H

#include "imagecache.h"
#include "messagebox.h"
#include "volumebox.h"
#include <vdr/osd.h>

class cNopacityDisplayChannelView {
private:
    cOsd *osd;
    cString lastDate;
    int statusIconBorder;
    int statusIconSize;
    bool isRadioChannel;
    int signalWidth, signalHeight, signalX;
    int lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
    int lastVolume;
    time_t lastVolumeTime;
    cPixmap *pixmapBackground = NULL;
    cPixmap *pixmapTop = NULL;
    cPixmap *pixmapLogo = NULL;
    cPixmap *pixmapLogoBackground = NULL;
    cPixmap *pixmapChannelName = NULL;
    cPixmap *pixmapDate = NULL;
    cPixmap *pixmapProgressBar = NULL;
    cPixmap *pixmapEPGInfo = NULL;
    cPixmap *pixmapStatusIcons = NULL;
    cPixmap *pixmapStatusIconsBackground = NULL;
    cPixmap *pixmapSignalStrength = NULL;
    cPixmap *pixmapSignalQuality = NULL;
    cPixmap *pixmapSignalMeter = NULL;
    cPixmap *pixmapSignalLabel = NULL;
    cPixmap *pixmapSourceInfo = NULL;
    cPixmap *pixmapPoster = NULL;
    cNopacityMessageBox *messageBox;
    cNopacityVolumeBox *volumeBox;
    std::string GetChannelSep(const cChannel *channel, bool prev);
    void CreatePixmaps(void);
    void DrawBackground(void);
    void DrawSignalMeter(void);
public:
    cNopacityDisplayChannelView(cOsd *osd);
    virtual ~cNopacityDisplayChannelView(void);
    void SetAlpha(int alpha = 0);
    void DrawChannelLogo(const cChannel *Channel);
    void ClearChannelLogo(void);
    void DrawChannelName(cString number, cString name);
    void ClearChannelName(void);
    void DrawDate(void);
    void DrawProgressbarBackground(void);
    void DrawProgressBar(int Current, int Total);
    void ClearProgressBar(void);
    void DrawEvents(const cEvent *Present, const cEvent *Following);
    void DrawEPGInfo(const cEvent *e, bool present, bool recording);
    void ClearEPGInfo(void);
    void DrawStatusIcons(const cChannel *Channel);
    void DrawScreenResolution(void);
    void ClearStatusIcons(void);
    void DrawPoster(const cEvent *event, bool initial);
    void DrawSignal(void);
    void ShowSignalMeter(bool show = true);
    void DrawChannelGroups(const cChannel *Channel, cString ChannelName);
    void DrawSourceInfo(void);
    void ClearSourceInfo(void);
    void DisplayMessage(eMessageType Type, const char *Text);
    void DrawVolume(void);
};
#endif //__NOPACITY_DISPLAYCHANNELVIEW_H
