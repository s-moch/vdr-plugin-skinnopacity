#ifndef __NOPACITY_DISPLAYCHANNEL_H
#define __NOPACITY_DISPLAYCHANNEL_H

//enum eLogoPosition {lpNone = 0, lpLeft, lpRight};
enum eBackgroundStyle {bsTrans = 0, bsFull};

class cNopacityDisplayChannel : public cSkinDisplayChannel, cThread {
private:
    bool doOutput;
    int FrameTime;
    int FadeTime;
    bool withInfo;
    bool initial;
    bool groupSep;
    bool channelChange;
    bool isRadioChannel;
    bool radioIconDrawn;
    cString lastDate;
    int lastSeen;
    time_t lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
    int lastScreenWidth;
    int currentLast;
    bool showSignal;
    const cEvent *present;
    cOsd *osd;
    cImageCache *imgCache;
    cPixmap *pixmapBackgroundTop;
    cPixmap *pixmapBackgroundMiddle;
    cPixmap *pixmapBackgroundBottom;
    cPixmap *pixmapChannelInfo;
    cPixmap *pixmapDate;
    cPixmap *pixmapLogo;
    cPixmap *pixmapLogoBackground;
    cPixmap *pixmapLogoBackgroundTop;
    cPixmap *pixmapLogoBackgroundBottom;
    cPixmap *pixmapProgressBar;
    cPixmap *pixmapEPGInfo;
    cPixmap *pixmapFooter;
    cPixmap *pixmapStreamInfo;
    cPixmap *pixmapStreamInfoBack;
    cPixmap *pixmapSignalStrength;
    cPixmap *pixmapSignalQuality;
    cPixmap *pixmapSignalMeter;
    cPixmap *pixmapSignalLabel;
    cPixmap *pixmapScreenResolution;
    cPixmap *pixmapPoster;
    int signalWidth, signalHeight, signalX;
    cFont *fontInfoline;
    virtual void Action(void);
    void createOsd(void);
    void CreatePixmaps(void);
    void DrawBackground(void);
    void DrawDate(void);
    void DrawProgressBar(int Current, int Total);
    tColor DrawProgressbarBackground(int left, int top, int width, int height);
    void DrawIconMask(void);
    void DrawIcons(const cChannel *Channel);
    void DrawIconsSingle(const cChannel *Channel);
    void DrawScreenResolution(void);
    void DrawSignalMeter(void);
    void ShowSignalMeter(void);
    void HideSignalMeter(void);
    void DrawSignal(void);
    void DrawSourceInfo(const cChannel *Channel);
    void DrawChannelGroups(const cChannel *Channel, cString ChannelName);
    cString GetChannelSep(const cChannel *channel, bool prev);
    void DrawPoster(const cEvent *event);
public:
    cNopacityDisplayChannel(cImageCache *imgCache, bool WithInfo);
    virtual ~cNopacityDisplayChannel();
    virtual void SetChannel(const cChannel *Channel, int Number);
    virtual void SetEvents(const cEvent *Present, const cEvent *Following);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};
#endif //__NOPACITY_DISPLAYCHANNEL_H