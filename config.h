#ifndef __NOPACITY_CONFIG_H
#define __NOPACITY_CONFIG_H

enum eDisplayType {
    dtFlat = 0,
    dtBlending,
    dtGraphical
};

class cNopacityConfig {
    private:
        std::map<std::string, int> conf;
        std::map<std::string, std::map<std::string, int> > themeConfigDefaults;
        std::map<std::string, std::map<std::string, int> > themeConfigSetup;
        void LoadThemeConfig(cString confFile, cString theme);
        std::pair<std::string, int> ReadThemeConfigLine(const char *line);
        cString checkSlashAtEnd(std::string path);
    public:
        cNopacityConfig();
        ~cNopacityConfig();
        int GetValue(std::string name);
        int *GetValueRef(std::string name);
        std::map<std::string, int>::const_iterator GetStart(void) { return conf.begin(); };
        std::map<std::string, int>::const_iterator GetEnd(void) { return conf.end(); };
        bool SetupParse(const char *Name, const char *Value);
        void SetThemeConfigSetupValue(std::string themeName, std::string key, int value);
        void SetLogoPath(cString path);
        void SetIconPath(cString path);
        void SetEpgImagePath(cString path);
        bool logoPathSet;
        bool epgImagePathSet;
        bool iconPathSet;
        cString logoPathDefault;
        cString iconPathDefault;
        cString epgImagePathDefault;
        void LoadDefaults(void);
        void LoadThemeSpecificConfigs(void);
        void SetThemeSpecificDefaults(void);
        void SetThemeSetup(void);
        void SetPathes(void);
        void DumpConfig(void);
        void DumpThemeConfig(void);
        void SetFontName();
        const char *fontDefaultName;
        char *fontName;
        cString logoPath;
        cString logoExtension;
        cString iconPath;
        cString epgImagePath;
        bool mainMenuEntry;
};

#endif //__NOPACITY_CONFIG_H