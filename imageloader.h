#ifndef __NOPACITY_IMAGELOADER_H
#define __NOPACITY_IMAGELOADER_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>

using namespace Magick;
 
class cImageLoader {
public:
    cImageLoader();
    ~cImageLoader();
    cImage GetImage();
    bool LoadLogo(const char *logo, int width, int height);
    bool LoadIcon(const char *cIcon, int size);
    bool LoadIcon(const char *cIcon, int width, int height, bool preserveAspect = true);
    bool LoadEPGImage(int eventID);
    bool LoadAdditionalEPGImage(cString name);
    bool LoadRecordingImage(cString Path);
    bool LoadAdditionalRecordingImage(cString path, cString name);
    void DrawBackground(tColor back, tColor blend, int width, int height);
    void DrawBackground2(tColor back, tColor blend, int width, int height);
private:
    Image buffer;
    Color Argb2Color(tColor col);
    void toLowerCase(std::string &str);
    bool LoadImage(cString FileName, cString Path, cString Extension);
	bool FirstImageInFolder(cString Path, cString Extension, cString *recImage);
};

#endif //__NOPACITY_IMAGELOADER_H
