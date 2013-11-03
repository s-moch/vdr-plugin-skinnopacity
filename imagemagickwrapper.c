#include <string>
#include <sstream>
#include "imagemagickwrapper.h"

cImageMagickWrapper::cImageMagickWrapper() {
    InitializeMagick(NULL);
}

cImageMagickWrapper::~cImageMagickWrapper() {
}

cImage *cImageMagickWrapper::CreateImage() {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    cImage *image = new cImage(cSize(w, h));
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
    for (int iy = 0; iy < h; ++iy) {
        for (int ix = 0; ix < w; ++ix) {
            tColor col = (~int(pixels->opacity * 255 / MaxRGB) << 24) 
            | (int(pixels->green * 255 / MaxRGB) << 8) 
            | (int(pixels->red * 255 / MaxRGB) << 16) 
            | (int(pixels->blue * 255 / MaxRGB) );
            image->SetPixel(cPoint(ix, iy), col);
            ++pixels;
        }
    }
    return image;
}

cImage cImageMagickWrapper::CreateImageCopy() {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    cImage image (cSize(w, h));
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
    for (int iy = 0; iy < h; ++iy) {
        for (int ix = 0; ix < w; ++ix) {
            tColor col = (~int(pixels->opacity * 255 / MaxRGB) << 24) 
            | (int(pixels->green * 255 / MaxRGB) << 8) 
            | (int(pixels->red * 255 / MaxRGB) << 16) 
            | (int(pixels->blue * 255 / MaxRGB) );
            image.SetPixel(cPoint(ix, iy), col);
            ++pixels;
        }
    }
    return image;
}

bool cImageMagickWrapper::LoadImage(std::string FileName, std::string Path, std::string Extension) {
    try {
        std::stringstream sstrImgFile;
        sstrImgFile << Path << FileName << "." << Extension;
        std::string imgFile = sstrImgFile.str();
        if (config.GetValue("debugImageLoading"))
            dsyslog("nopacity: trying to load: %s", imgFile.c_str());
        buffer.read(imgFile.c_str());
        if (config.GetValue("debugImageLoading"))
            dsyslog("nopacity: %s sucessfully loaded", imgFile.c_str());
    } catch (...) {     
        return false;
    }
    return true;
}

bool cImageMagickWrapper::LoadImage(const char *fullpath) {
    if ((fullpath == NULL) || (strlen(fullpath) < 5))
        return false;
    try {
        if (config.GetValue("debugImageLoading"))
            dsyslog("nopacity: trying to load: %s", fullpath);
        buffer.read(fullpath);
        if (config.GetValue("debugImageLoading"))
            dsyslog("nopacity: %s sucessfully loaded", fullpath);
    } catch (...) {     
        return false;
    }
    return true;
}

Color cImageMagickWrapper::Argb2Color(tColor col) {
    tIndex alpha = (col & 0xFF000000) >> 24;
    tIndex red = (col & 0x00FF0000) >> 16;
    tIndex green = (col & 0x0000FF00) >> 8;
    tIndex blue = (col & 0x000000FF);
    Color color(MaxRGB*red/255, MaxRGB*green/255, MaxRGB*blue/255, MaxRGB*(0xFF-alpha)/255);
    return color;
}

void cImageMagickWrapper::CreateBackground(tColor back, tColor blend, int width, int height, bool mirror) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    Image tmp(Geometry(width, height), Blend);
    double arguments[9] = {0.0,(double)height,0.0,-1*(double)width,0.0,0.0,1.5*(double)width,0.0,1.0};
    tmp.sparseColor(MatteChannel, BarycentricColorInterpolate, 9, arguments);
    Image tmp2(Geometry(width, height), Back);
    tmp.composite(tmp2, 0, 0, OverlayCompositeOp);
    if (mirror)
        tmp.flop();
    buffer = tmp;
}

void cImageMagickWrapper::CreateBackgroundReverse(tColor back, tColor blend, int width, int height) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    Image tmp(Geometry(width, height), Blend);
    double arguments[9] = {0.0,(double)height,0.0,-0.5*(double)width,0.0,0.0,(double)width,0.0,1.0};
    tmp.sparseColor(MatteChannel, BarycentricColorInterpolate, 9, arguments);
    Image tmp2(Geometry(width, height), Back);
    tmp.composite(tmp2, 0, 0, OverlayCompositeOp);
    buffer = tmp;
}

