#ifndef __NOPACITY_DISPLAYCHANNELVIEW_H
#define __NOPACITY_DISPLAYCHANNELVIEW_H

#include "imagecache.h"
#include "messagebox.h"
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
    cPixmap *pixmapBackground;
    cPixmap *pixmapTop;
    cPixmap *pixmapLogo;
    cPixmap *pixmapLogoBackground;
    cPixmap *pixmapChannelName;
    cPixmap *pixmapDate;
    cPixmap *pixmapProgressBar;
    cPixmap *pixmapEPGInfo;
    cPixmap *pixmapStatusIcons;
    cPixmap *pixmapStatusIconsBackground;
    cPixmap *pixmapSignalStrength;
    cPixmap *pixmapSignalQuality;
    cPixmap *pixmapSignalMeter;
    cPixmap *pixmapSignalLabel;
    cPixmap *pixmapSourceInfo;
    cPixmap *pixmapPoster;
    cNopacityMessageBox *messageBox;
    tColor DrawProgressbarProgress(int left, int top, int width, int height);
    cString GetScreenResolutionIcon(void);
    std::string GetChannelSep(const cChannel *channel, bool prev);
    void CreatePixmaps(void);
    void DrawBackground(void);
    void DrawSignalMeter(void);
public:
    cNopacityDisplayChannelView(cOsd *osd);
    virtual ~cNopacityDisplayChannelView(void);
    void SetAlpha(int alpha);
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
    void DrawPoster(const cEvent *event, bool initial, bool channelchange);
    void DrawSignal(void);
    void ShowSignalMeter(void);
    void HideSignalMeter(void);
    void DrawChannelGroups(const cChannel *Channel, cString ChannelName);
    void DrawSourceInfo(void);
    void ClearSourceInfo(void);
    void DisplayMessage(eMessageType Type, const char *Text);
};
#endif //__NOPACITY_DISPLAYCHANNELVIEW_H
