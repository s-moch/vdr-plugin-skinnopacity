#include "imageloader.h"
#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>

using namespace Magick;

cImageLoader::cImageLoader() {
    InitializeMagick(NULL);
}

cImageLoader::~cImageLoader() {
}

bool cImageLoader::LoadLogo(const char *logo, int width = config.logoWidth, int height = config.logoHeight) {
    if ((width == 0)||(height==0))
        return false;
    std::string logoLower = logo;
    toLowerCase(logoLower);
    bool success = false;
    if (config.logoPathSet) {
        success = LoadImage(logoLower.c_str(), config.logoPath, config.logoExtension);
    }
    if (!success) {
        success = LoadImage(logoLower.c_str(), config.logoPathDefault, config.logoExtension);
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadIcon(const char *cIcon, int size) {
	if (size==0)
        return false;
    bool success = false;
    if (config.iconPathSet) {
        cString iconThemePath = cString::sprintf("%s%s/", *config.iconPath, Setup.OSDTheme);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), config.iconPath, "png");
        }
    }
    if (!success) {
        cString iconThemePath = cString::sprintf("%s%s/", *config.iconPathDefault, Setup.OSDTheme);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), config.iconPathDefault, "png");
        }
    }
    if (!success)
        return false;
    buffer.sample(Geometry(size, size));
    return true;
}

bool cImageLoader::LoadIcon(const char *cIcon, int width, int height) {
  try {
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (config.iconPathSet) {
        cString iconThemePath = cString::sprintf("%s%s/", *config.iconPath, Setup.OSDTheme);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), config.iconPath, "png");
        }
    }
    if (!success) {
        cString iconThemePath = cString::sprintf("%s%s/", *config.iconPathDefault, Setup.OSDTheme);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), config.iconPathDefault, "png");
        }
    }
    if (!success)
        return false;
    buffer.sample(Geometry(width, height));
    return true;
  }
  catch (...) {
    return false;
  }
}

bool cImageLoader::LoadEPGImage(int eventID) {
    int width = config.epgImageWidth;
    int height = config.epgImageHeight;
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (config.epgImagePathSet) {
        success = LoadImage(cString::sprintf("%d", eventID), config.epgImagePath, "jpg");
    }
    if (!success) {
        success = LoadImage(cString::sprintf("%d", eventID), config.epgImagePathDefault, "jpg");
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadRecordingImage(cString Path) {
    int width = config.epgImageWidth;
    int height = config.epgImageHeight;
    if ((width == 0)||(height==0))
        return false;
    cString recImage("");
    if (FirstImageInFolder(Path, "jpg", &recImage)) {
        recImage = cString::sprintf("/%s", *recImage);
        if (!LoadImage(*recImage, Path, "jpg"))
            return false;
        buffer.sample( Geometry(width, height));
        return true;
    }
    return false;
}

void cImageLoader::DrawBackground(tColor back, tColor blend, int width, int height) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    Image tmp(Geometry(width, height), Blend);
    double arguments[9] = {0.0,(double)height,0.0,-1*(double)width,0.0,0.0,1.5*(double)width,0.0,1.0};
    tmp.sparseColor(MatteChannel, BarycentricColorInterpolate, 9, arguments);
    Image tmp2(Geometry(width, height), Back);
    tmp.composite(tmp2, 0, 0, OverlayCompositeOp);
    buffer = tmp;
}

void cImageLoader::DrawBackground2(tColor back, tColor blend, int width, int height) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    Image tmp(Geometry(width, height), Blend);
    double arguments[9] = {0.0,(double)height,0.0,-0.5*(double)width,0.0,0.0,0.75*(double)width,0.0,1.0};
    tmp.sparseColor(MatteChannel, BarycentricColorInterpolate, 9, arguments);
    Image tmp2(Geometry(width, height), Back);
    tmp.composite(tmp2, 0, 0, OverlayCompositeOp);
    buffer = tmp;
}

cImage cImageLoader::GetImage() {
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

Color cImageLoader::Argb2Color(tColor col) {
    tIndex alpha = (col & 0xFF000000) >> 24;
    tIndex red = (col & 0x00FF0000) >> 16;
    tIndex green = (col & 0x0000FF00) >> 8;
    tIndex blue = (col & 0x000000FF);
    Color color(MaxRGB*red/255, MaxRGB*green/255, MaxRGB*blue/255, MaxRGB*(0xFF-alpha)/255);
    return color;
}

void cImageLoader::toLowerCase(std::string &str) {
    const int length = str.length();
    for(int i=0; i < length; ++i) {
        str[i] = std::tolower(str[i]);
    }
}

bool cImageLoader::LoadImage(cString FileName, cString Path, cString Extension) {
    try {
        cString File = cString::sprintf("%s%s.%s", *Path, *FileName, *Extension);
        dsyslog("nopacity: trying to load: %s", *File);
        buffer.read(*File);
        dsyslog("nopacity: %s sucessfully loaded", *File);
    } catch (...) {     
        return false;
    }
    return true;
}

bool cImageLoader::FirstImageInFolder(cString Path, cString Extension, cString *recImage) {
    DIR *folder;
    struct dirent *file;
    folder = opendir(Path);
    while (file = readdir(folder)) {
        if (endswith(file->d_name, *Extension)) {
            std::string fileName = file->d_name;
            if (fileName.length() > 4)
                fileName = fileName.substr(0, fileName.length() - 4);
            else
                return false;
            *recImage = fileName.c_str();
            return true;
        }
    }
    return false;
}