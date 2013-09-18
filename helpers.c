#include <string>
#include <sstream>

static cOsd *CreateOsd(int Left, int Top, int Width, int Height) {
    cOsd *osd = cOsdProvider::NewOsd(Left, Top);
    if (osd) {
        tArea Area = { 0, 0, Width, Height,  32 };
        if (osd->SetAreas(&Area, 1) == oeOk) {  
            return osd;
        }
    }
    return NULL;
}

static void DrawBlendedBackground(cPixmap *pixmap, tColor color, tColor colorBlending, bool fromTop) {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    pixmap->Fill(color);
    int numSteps = 16;
    int alphaStep = 0x0F;
    int alpha = 0x00;
    int step, begin, end;
    bool cont = true;
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
    for (int i = begin; cont; i = i + step) {
        clr = AlphaBlend(color, colorBlending, alpha);
        pixmap->DrawRectangle(cRect(0,i,width,1), clr);
        alpha += alphaStep;
        if (i == end) 
            cont = false;
    }
}

static void DrawRoundedCorners(cPixmap *p, int radius, int x, int y, int width, int height) {
    if (radius > 2) {
        p->DrawEllipse(cRect(x, y, radius, radius), clrTransparent, -2);
        p->DrawEllipse(cRect(x + width - radius, y , radius, radius), clrTransparent, -1);
        p->DrawEllipse(cRect(x, y + height - radius, radius, radius), clrTransparent, -3);
        p->DrawEllipse(cRect(x + width - radius, y + height - radius, radius, radius), clrTransparent, -4);
    }
}

static void DrawRoundedCornersWithBorder(cPixmap *p, tColor borderColor, int radius, int width, int height, cPixmap *pBack = NULL) {
    if (radius < 3)
        return;
    p->DrawEllipse(cRect(0,0,radius,radius), borderColor, -2);
    p->DrawEllipse(cRect(-1,-1,radius,radius), clrTransparent, -2);
    if (pBack)
        pBack->DrawEllipse(cRect(-1,-1,radius,radius), clrTransparent, -2);

    p->DrawEllipse(cRect(width-radius,0,radius,radius), borderColor, -1);
    p->DrawEllipse(cRect(width-radius+1,-1,radius,radius), clrTransparent, -1);
    if (pBack)
        pBack->DrawEllipse(cRect(width-radius+1,-1,radius,radius), clrTransparent, -1);

    p->DrawEllipse(cRect(0,height-radius,radius,radius), borderColor, -3);
    p->DrawEllipse(cRect(-1,height-radius+1,radius,radius), clrTransparent, -3);
    if (pBack)
        pBack->DrawEllipse(cRect(-1,height-radius+1,radius,radius), clrTransparent, -3);

    p->DrawEllipse(cRect(width-radius,height-radius,radius,radius), borderColor, -4);
    p->DrawEllipse(cRect(width-radius+1,height-radius+1,radius,radius), clrTransparent, -4);
    if (pBack)
        pBack->DrawEllipse(cRect(width-radius+1,height-radius+1,radius,radius), clrTransparent, -4);
    
}

static int Minimum(int a, int b, int c, int d, int e, int f) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    if (d < min) min = d;
    if (e < min) min = e;
    if (f < min) min = f;
    return min;
}

static std::string CutText(std::string text, int width, const cFont *font) {
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

class splitstring : public std::string {
    std::vector<std::string> flds;
public:
    splitstring(const char *s) : std::string(s) { };
    std::vector<std::string>& split(char delim, int rep=0);
};

// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
std::vector<std::string>& splitstring::split(char delim, int rep) {
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    std::string work = data();
    std::string buf = "";
    int i = 0;
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