#ifndef __HELPERS_H
#define __HELPERS_H

#include <vdr/osd.h>
#include <vdr/plugin.h>

cOsd *CreateOsd(int Left, int Top, int Width, int Height);
cPixmap *CreatePixmap(cOsd *osd, cString Name = "", int Layer = 0, const cRect &ViewPort = cRect::Null, const cRect &DrawPort = cRect::Null);
void PixmapSetAlpha(cPixmap *pixmap, int Alpha);
void PixmapFill(cPixmap *pixmap, tColor Color);
void DrawBlendedBackground(cPixmap *pixmap, int xStart, int width, tColor color, tColor colorBlending, bool fromTop);
void DrawRoundedCorners(cPixmap *p, int radius, int x, int y, int width, int height);
void DrawRoundedCornersWithBorder(cPixmap *p, tColor borderColor, int radius, int width, int height);
void DrawProgressbar(cPixmap *p, int left, int top, int width, int height, int Current, int Total, tColor color, tColor colorBlending, bool blend = false);
cSize ScaleToFit(int widthMax, int heightMax, int widthOriginal, int heightOriginal);
int Minimum(int a, int b, int c, int d, int e, int f);
std::string CutText(std::string text, int width, const cFont *font);
std::string StrToLowerCase(std::string str);
cString GetScreenResolutionIcon(void);

class splitstring : public std::string {
    std::vector<std::string> flds;
public:
    splitstring(const char *s) : std::string(s) { };
    std::vector<std::string>& split(char delim, int rep=0);
};

cPlugin *GetScraperPlugin(void);

#endif // __HELPERS_H
