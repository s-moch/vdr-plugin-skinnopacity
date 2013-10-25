#include "imageloader.h"
#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>

using namespace Magick;

cImageLoader::cImageLoader() : cImageMagickWrapper() {
}

cImageLoader::~cImageLoader() {
}

cImage cImageLoader::GetImage() {
    return CreateImageCopy();
}

bool cImageLoader::LoadLogo(const char *logo, int width = config.GetValue("logoWidthOriginal"), int height = config.GetValue("logoHeightOriginal")) {
    if ((width == 0)||(height==0))
        return false;
    std::string logoLower = StrToLowerCase(logo);
    bool success = false;
    if (config.logoPathSet) {
        //theme dependend logo
        cString logoPath = cString::sprintf("%s%s/", *config.logoPath, Setup.OSDTheme);
        success = LoadImage(logoLower.c_str(), *logoPath, *config.logoExtension);
        if (!success)
            success = LoadImage(logoLower.c_str(), *config.logoPath, *config.logoExtension);
    }
    if (!success) {
        //theme dependend logo
        cString logoPath = cString::sprintf("%s%s/", *config.logoPathDefault, Setup.OSDTheme);
        success = LoadImage(logoLower.c_str(), *logoPath, *config.logoExtension);
        if (!success)
            success = LoadImage(logoLower.c_str(), *config.logoPathDefault, *config.logoExtension);
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadEPGImage(int eventID) {
    int width = config.GetValue("epgImageWidth");
    int height = config.GetValue("epgImageHeight");
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (config.epgImagePathSet) {
        success = LoadImage(*cString::sprintf("%d", eventID), *config.epgImagePath, "jpg");
    }
    if (!success) {
        success = LoadImage(*cString::sprintf("%d", eventID), *config.epgImagePathDefault, "jpg");
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadAdditionalEPGImage(cString name) {
    int width = config.GetValue("epgImageWidthLarge");
    int height = config.GetValue("epgImageHeightLarge");
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (config.epgImagePathSet) {
        success = LoadImage(*name, *config.epgImagePath, "jpg");
    }
    if (!success) {
        success = LoadImage(*name, *config.epgImagePathDefault, "jpg");
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadRecordingImage(cString Path) {
    int width = config.GetValue("epgImageWidth");
    int height = config.GetValue("epgImageHeight");
    if ((width == 0)||(height==0))
        return false;
    cString recImage("");
    if (FirstImageInFolder(Path, "jpg", &recImage)) {
        recImage = cString::sprintf("/%s", *recImage);
        if (!LoadImage(*recImage, *Path, "jpg"))
            return false;
        buffer.sample( Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::LoadAdditionalRecordingImage(cString path, cString name) {
    int width = config.GetValue("epgImageWidthLarge");
    int height = config.GetValue("epgImageHeightLarge");
    if ((width == 0)||(height==0))
        return false;
    if (LoadImage(*name, *path, "jpg")) {
        buffer.sample( Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::LoadPoster(const char *poster, int width, int height) {
    if (LoadImage(poster)) {
        buffer.sample(Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::SearchRecordingPoster(cString recPath, cString &found) {
    cString manualPoster = cString::sprintf("%s/cover_vdr.jpg", *recPath);
    if (FileSize(*manualPoster) != -1) {
        found = manualPoster;
        return true;
    }
    manualPoster = cString::sprintf("%s/../../../cover_vdr.jpg", *recPath);
    if (FileSize(*manualPoster) != -1) {
        found = manualPoster;
        return true;
    }
    manualPoster = cString::sprintf("%s/../../cover_vdr.jpg", *recPath);
    if (FileSize(*manualPoster) != -1) {
        found = manualPoster;
        return true;
    }
    return false;    
}

bool cImageLoader::FirstImageInFolder(cString Path, cString Extension, cString *recImage) {
    DIR *folder;
    struct dirent *file;
    folder = opendir(Path);
    while (file = readdir(folder)) {
        if (endswith(file->d_name, *Extension)) {
            std::string fileName = file->d_name;
            if (!fileName.compare(fileName.size()-8, 8, "_vdr.jpg"))
                continue;
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