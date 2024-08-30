#include "config.h"
#include "helpers.h"
#include "imageloader.h"
#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>
#include <filesystem>

using namespace Magick;

cImageLoader::cImageLoader() : cImageMagickWrapper() {
}

cImageLoader::~cImageLoader() {
}

cImage cImageLoader::GetImage() {
    return CreateImageCopy();
}

bool cImageLoader::LoadLogo(const char *logo, int width, int height ) {
    if ((width == 0)||(height==0))
        return false;
    std::string logoLower = StrToLowerCase(logo);
    bool success = false;
    for (int i = 0; i <= 1; i++) {
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
        if (success)
            break;
        if (i == 0) {
            for (size_t i = 0; i < logoLower.length(); i++) {
                if (logoLower[i] == '/') {
                    logoLower[i] = '~';
                }
            }
        }
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadEPGImage(int eventID, int w, int h) {
    int width = (w) ?  w : config.GetValue("epgImageWidth");
    int height = (h) ?  h : config.GetValue("epgImageHeight");
    if ((width == 0) || (height == 0))
        return false;

    bool success = false;
    if (config.epgImagePathSet) {
        success = LoadImage(*cString::sprintf("%d", eventID), *config.epgImagePath, "jpg");
    }
    if (!success) {
        success = LoadImage(*cString::sprintf("%d", eventID), *config.epgImagePathDefault, "jpg");
    }
    if (!success && config.epgImagePathSet) {
        success = LoadImage(*cString::sprintf("%d_0", eventID), *config.epgImagePath, "jpg");
    }
    if (!success) {
        success = LoadImage(*cString::sprintf("%d_0", eventID), *config.epgImagePathDefault, "jpg");
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
    if ((width == 0) || (height == 0))
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

bool cImageLoader::LoadRecordingImage(cString Path, int w, int h) {
    int width = (w) ?  w : config.GetValue("epgImageWidth");
    int height = (h) ?  h : config.GetValue("epgImageHeight");
    if ((width == 0) || (height == 0))
        return false;

    cString recImage("");
    if (FirstImageInFolder(Path, ".jpg", &recImage)) {
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
    if ((width == 0) || (height == 0))
        return false;

    if (LoadImage(*name, *path, "jpg")) {
        buffer.sample( Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::LoadPoster(const char *poster, int width, int height, bool scale) {
    if ((width == 0) || (height==0))
        return false;
    if (LoadImage(poster)) {
        if (scale)
            buffer.sample(Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::SearchRecordingPoster(cString recPath, cString &found, bool asFolder) {
    cString manualPoster = cString::sprintf("%s/%s", *recPath, config.posterFileName);
    // first, search the recording for a manual poster
    if (FileSize(*manualPoster) != -1) {
        found = manualPoster;
        return true;
    }
    if (asFolder) return false;
    // next, search the folder in which the recording resides with its mate recordings;
    // this could be episodes of a series or a category like "movies"
    std::filesystem::path path = *cString::sprintf("%s/../../%s", *recPath, config.posterFileName);
    manualPoster = path.lexically_normal().c_str();
    if (FileSize(*manualPoster) != -1) {
        found = manualPoster;
        return true;
    }
    // finally, search the next higher folder for a potential category poster, like for
    // all the series
    path = *cString::sprintf("%s/../../../%s", *recPath, config.posterFileName);
    manualPoster = path.lexically_normal().c_str();
    if (FileSize(*manualPoster) != -1) {
        found = manualPoster;
        return true;
    }
    return false;
}

bool cImageLoader::SearchRecordingImage(cString recPath, cString &found) {
    cString manualPoster;
    if (FirstImageInFolder(recPath, ".jpg", &manualPoster)) {
        found = cString::sprintf("%s/%s.jpg", *recPath, *manualPoster);
        return true;
    }
    return false;
}

bool cImageLoader::FirstImageInFolder(cString Path, cString Extension, cString *recImage) {
    DIR *folder = NULL;
    struct dirent *file;
    folder = opendir(Path);
    if (!folder)
        return false;
    while (file = readdir(folder)) {
        if (endswith(file->d_name, *Extension)) {
            std::string fileName = file->d_name;
            int length = fileName.length();
            fileName = fileName.substr(0, length - strlen(*Extension));
            *recImage = fileName.c_str();
            return true;
        }
    }
    return false;
}
