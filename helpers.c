#include <string>
#include <sstream>
#include <vector>
#include "config.h"
#include "helpers.h"
#include <vdr/skins.h>

cOsd *CreateOsd(int Left, int Top, int Width, int Height) {
    cOsd *osd = cOsdProvider::NewOsd(Left, Top);
    if (osd) {
        tArea Area = { 0, 0, Width - 1, Height - 1,  32 };
        if (osd->SetAreas(&Area, 1) == oeOk) {
            return osd;
        }
    }
    return NULL;
}

void DrawBlendedBackground(cPixmap *pixmap, int xStart, int width, tColor color, tColor colorBlending, bool fromTop) {
    int height = pixmap->ViewPort().Height();
    int numSteps = 16;
    int alphaStep = 0x0F;
    int alpha = 0x00;
    int step, begin, end;
    if (fromTop) {
        step = 1;
        begin = 0;
        end = numSteps;
    } else {
        step = -1;
        begin = height;
        end = height - numSteps;
    }
    tColor clr;
    bool cont = true;
    for (int i = begin; cont; i = i + step) {
        clr = AlphaBlend(color, colorBlending, alpha);
        pixmap->DrawRectangle(cRect(xStart,i,width,1), clr);
        alpha += alphaStep;
        if (i == end)
            cont = false;
    }
}

void DrawRoundedCorners(cPixmap *p, int radius, int x, int y, int width, int height) {
    if (radius > 2) {
        p->DrawEllipse(cRect(x, y, radius, radius), clrTransparent, -2);
        p->DrawEllipse(cRect(x + width - radius, y , radius, radius), clrTransparent, -1);
        p->DrawEllipse(cRect(x, y + height - radius, radius, radius), clrTransparent, -3);
        p->DrawEllipse(cRect(x + width - radius, y + height - radius, radius, radius), clrTransparent, -4);
    }
}

void DrawRoundedCornersWithBorder(cPixmap *p, tColor borderColor, int radius, int width, int height) {
    if (radius < 3)
        return;
    p->DrawEllipse(cRect(0,0,radius,radius), borderColor, -2);
    p->DrawEllipse(cRect(-1,-1,radius,radius), clrTransparent, -2);

    p->DrawEllipse(cRect(width-radius,0,radius,radius), borderColor, -1);
    p->DrawEllipse(cRect(width-radius+1,-1,radius,radius), clrTransparent, -1);

    p->DrawEllipse(cRect(0,height-radius,radius,radius), borderColor, -3);
    p->DrawEllipse(cRect(-1,height-radius+1,radius,radius), clrTransparent, -3);

    p->DrawEllipse(cRect(width-radius,height-radius,radius,radius), borderColor, -4);
    p->DrawEllipse(cRect(width-radius+1,height-radius+1,radius,radius), clrTransparent, -4);
}

void DrawProgressbar(cPixmap *p, int left, int top, int width, int height, int Current, int Total, tColor clr1, tColor clr2, bool blend) {
    if (Current == 0) {
        p->DrawEllipse(cRect(left, top, height, height), blend ? clr2 : clr1);
        return;
    } else
        p->DrawEllipse(cRect(left, top, height, height), clr2);

    width = width - height;                             // width of gradient (width - ellipse)
    double percent = ((double)Current) / (double)Total;
    double progresswidth = width * percent;

    tColor clr = blend ? clr2 : clr1;
    int alpha = 0x0;                                    // 0...255
    int alphaStep = 0x1;
    int maximumsteps = 256;                             // alphaStep * maximumsteps <= 256
    int factor = 2;                                     // max. 128 steps

    bool partial = false;
    double step = 0;
    if (partial) {
        step = (double)width / maximumsteps;            // shows a progresswidth part of color gradient
        maximumsteps = (double)maximumsteps * percent;
    } else
        step = progresswidth / maximumsteps;            // shows a progresswidth full color gradient

    if (!partial && progresswidth < 128) {              // width < 128
        factor = 4 * factor;                            // 32 steps
    } else if (progresswidth < 256) {                   // width < 256
        factor = 2 * factor;                            // 64 steps
    }

    step = step * factor;
    alphaStep = alphaStep * factor;
    maximumsteps = maximumsteps / factor;

    int x = left + height / 2;
    for (int i = 0; i < maximumsteps; i++) {
        x = left + height / 2 + i * step;
        clr = AlphaBlend(clr1, clr2, alpha);
        p->DrawRectangle(cRect(x, top, step + 1, height), clr);
        alpha += alphaStep;
    }
    x = x + step - height / 2;
    p->DrawEllipse(cRect(x, top, height, height), clr);
}

cSize ScaleToFit(int widthMax, int heightMax, int widthOriginal, int heightOriginal) {
    int width = 1;
    int height = 1;

    if ((widthMax == 0)||(heightMax==0)||(widthOriginal==0)||(heightOriginal==0))
        return cSize(width, height);

    if ((widthOriginal <= widthMax) && (heightOriginal <= heightMax)) {
        width = widthOriginal;
        height = heightOriginal;
    } else if ((widthOriginal > widthMax) && (heightOriginal <= heightMax)) {
        width = widthMax;
        height = (double)width/(double)widthOriginal * heightOriginal;
    } else if ((widthOriginal <= widthMax) && (heightOriginal > heightMax)) {
        height = heightMax;
        width = (double)height/(double)heightOriginal * widthOriginal;
    } else {
        width = widthMax;
        height = (double)width/(double)widthOriginal * heightOriginal;
        if (height > heightMax) {
            height = heightMax;
            width = (double)height/(double)heightOriginal * widthOriginal;
        }
    }
    return cSize(width, height);
}

int Minimum(int a, int b, int c, int d, int e, int f) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    if (d < min) min = d;
    if (e < min) min = e;
    if (f < min) min = f;
    return min;
}

std::string CutText(std::string text, int width, const cFont *font) {
    if (width <= font->Size())
        return text.c_str();
    cTextWrapper twText;
    twText.Set(text.c_str(), font, width);
    std::string cuttedTextNative = twText.GetLine(0);
    std::stringstream sstrText;
    sstrText << cuttedTextNative << "...";
    std::string cuttedText = sstrText.str();
    int actWidth = font->Width(cuttedText.c_str());
    if (actWidth > width) {
        int overlap = actWidth - width;
        int charWidth = font->Width(".");
        if (charWidth == 0)
            charWidth = 1;
        int cutChars = overlap / charWidth;
        if (cutChars > 0) {
            cuttedTextNative = cuttedTextNative.substr(0, cuttedTextNative.length() - cutChars);
            std::stringstream sstrText2;
            sstrText2 << cuttedTextNative << "...";
            cuttedText = sstrText2.str();
        }
    }
    return cuttedText;
}

std::string StrToLowerCase(std::string str) {
    std::string lowerCase = str;
    const int length = lowerCase.length();
    for(int i=0; i < length; ++i) {
        lowerCase[i] = std::tolower(lowerCase[i]);
    }
    return lowerCase;
}

// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
std::vector<std::string>& splitstring::split(char delim, int rep) {
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    std::string work = data();
    std::string buf = "";
    unsigned int i = 0;
    while (i < work.length()) {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1) {
            flds.push_back(buf);
            buf = "";
        } else if (buf.length() > 0) {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}

cPlugin *GetScraperPlugin(void) {
    static cPlugin *pScraper = cPluginManager::GetPlugin("scraper2vdr");
    if (!pScraper ) // if it doesn't exit, try tvscraper
        pScraper = cPluginManager::GetPlugin("tvscraper");
    return pScraper;
}

cString GetScreenResolutionIcon(void) {
    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;
    cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);
    cString iconName("");
    switch (screenHeight) {
        case 4320: // 7680 x 4320 = 8K UHD
        case 2160: // 3840 x 2160 = 4K UHD
            iconName = "skinIcons/uhd4k";
            break;
        case 1440: // 2560 x 1440 = QHD
        case 1080: // "hd1080i"; // 'i' is default, 'p' can't be detected currently
            iconName = "skinIcons/hd1080i";
            break;
        case 720: // "hd720p"; // 'i' is not defined in standards
            iconName = "skinIcons/hd720p";
            break;
        case 576: // "sd576i"; // assumed 'i'
            iconName = "skinIcons/sd576i";
            break;
        case 480: // "sd480i"; // assumed 'i'
            iconName = "skinIcons/sd576i";
            break;
        default:
            break;
    }
    return iconName;
}
