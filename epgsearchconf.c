enum eEpgSearchTokens { eEPGtime = 0, eEPGdate, eEPGdatesh, eEPGtime_w, eEPGtime_d, eEPGtimespan, 
                        eEPGtitle, eEPGsubtitle, eEPGt_status, eEPGv_status, eEPGr_status,
                        eEPGstatus, eEPGchnr, eEPGchsh, eEPGchlng, eEPGprogr, eEPGprogrT2S};
enum eEPGModes { eMenuSchedule = 0, eMenuWhatsOnNow, eMenuWhatsOnNext, eMenuWhatsOnElse, 
                 eMenuSearchResults, eMenuFavorites, eMenuTimerconflict};

#include <string>
#include "epgsearchconf.h"

cNopacityEPGSearchConfig::cNopacityEPGSearchConfig(void) {
    ReplaceSchedule = false;
    ShowChannelNr = false;
    ShowProgress = false;
    for (int i=0; i<4; i++) {
        UserSet[i] = false;
        User[i] = "";
    }
    for (int i=0; i<EPGSEARCHCATS; i++)
        for (int j =0; j<EPGSEARCHTOKENS; j++)
            epgSearchConfig[i][j] = -1;
    
}
cNopacityEPGSearchConfig::~cNopacityEPGSearchConfig() {
}

bool cNopacityEPGSearchConfig::CheckEPGSearchAvailable(void) {
    cPlugin *epgSearchPlugin =  NULL;
    epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (epgSearchPlugin) {
        return true;
    }
    return false;
}

bool cNopacityEPGSearchConfig::LoadEpgSearchConf(void) {
    std::string ConfigDir = cPlugin::ConfigDirectory(NULL);
    ConfigDir = ConfigDir.substr(0, ConfigDir.find("/plugins"));
    cString vdrSetup = "setup.conf";
    cString fileName = AddDirectory(ConfigDir.c_str(),  *vdrSetup);
    if (access(fileName, F_OK) == 0) {
        FILE *f = fopen(fileName, "r");
        if (f) {
            char *s;
            cReadLine ReadLine;
            while ((s = ReadLine.Read(f)) != NULL) {
                char *p = strchr(s, '#');
                if (p)
                    *p = 0;
                stripspace(s);
                if (!isempty(s)) {
                    std::string line = s;
                    if (line.find("epgsearch.ReplaceOrgSchedule = 1") == 0) {
                        ReplaceSchedule = true;
                    } else if (line.find("epgsearch.ShowChannelNr = 1") == 0) {
                        ShowChannelNr = true;
                    } else if (line.find("epgsearch.ShowProgress = 1") == 0) {
                        ShowProgress = true;
                    } else {
                        for (int i=0; i<4; i++) {
                            cString strEPGUserMode = cString::sprintf("epgsearch.UserMode%dDescription =", i+1);
                            if (line.find(*strEPGUserMode) == 0) {
                                try {
                                    line = line.substr(line.find("=")+1);
                                    line = line.substr(line.find_first_not_of(" "));
                                    if (line.length() > 0) {
                                        User[i] = line.c_str();
                                        UserSet[i] = true;
                                    }
                                } catch (...){}
                            }       
                        }
                    }
                }
            }
        }
    } else {
        return false;
    }
    return true;
}

bool cNopacityEPGSearchConfig::LoadEpgSearchMenuConf(void) {
    cString ConfigDir = cPlugin::ConfigDirectory("epgsearch");
    cString epgsearchConf = "epgsearchmenu.conf";
    cString fileName = AddDirectory(*ConfigDir,  *epgsearchConf);
    bool foundMenuWhatsOnNow = false;
    bool foundMenuWhatsOnNext = false;
    bool foundMenuWhatsOnElse = false;
    bool foundMenuSchedule = false;
    bool foundMenuSearchResults = false;
    bool foundMenuFavorites = false;
    if (access(fileName, F_OK) == 0) {
        dsyslog("nopacity: config file %s found", *fileName);
        FILE *f = fopen(fileName, "r");
        if (f) {
            char *s;
            cReadLine ReadLine;
            while ((s = ReadLine.Read(f)) != NULL) {
                char *p = strchr(s, '#');
                if (p)
                    *p = 0;
                stripspace(s);
                try {
                    if (!isempty(s)) {
                        std::string line = s;
                        std::string *values = new std::string[MAXITEMTABS];
                        if (line.find("MenuWhatsOnNow") == 0) {
                            values = SplitEpgSearchMenuLine(line.substr(line.find_first_of("=") + 1));
                            SetTokens(values, eMenuWhatsOnNow);
                            foundMenuWhatsOnNow = true;
                        } else if (line.find("MenuWhatsOnNext") == 0) {
                            values = SplitEpgSearchMenuLine(line.substr(line.find_first_of("=") + 1));
                            SetTokens(values, eMenuWhatsOnNext);
                            foundMenuWhatsOnNext = true;
                        } else if (line.find("MenuWhatsOnElse") == 0) {
                            values = SplitEpgSearchMenuLine(line.substr(line.find_first_of("=") + 1));
                            SetTokens(values, eMenuWhatsOnElse);
                            foundMenuWhatsOnElse = true;
                        } else if (line.find("MenuSchedule") == 0) {
                            values = SplitEpgSearchMenuLine(line.substr(line.find_first_of("=") + 1));
                            SetTokens(values, eMenuSchedule);
                            foundMenuSchedule = true;
                        } else if (line.find("MenuSearchResults") == 0) {
                            values = SplitEpgSearchMenuLine(line.substr(line.find_first_of("=") + 1));
                            SetTokens(values, eMenuSearchResults);
                            foundMenuSearchResults = true;
                        } else if (line.find("MenuFavorites") == 0) {
                            values = SplitEpgSearchMenuLine(line.substr(line.find_first_of("=") + 1));
                            SetTokens(values, eMenuFavorites);
                            foundMenuFavorites = true;
                        }
                        delete[] values;
                    }
                } catch (...){}
            }
        }
        if (!foundMenuWhatsOnNow || 
            !foundMenuWhatsOnNext || 
            !foundMenuWhatsOnElse || 
            !foundMenuSchedule || 
            !foundMenuSearchResults || 
            !foundMenuFavorites) 
        {
            dsyslog("nopacity: config file not complete, using default");
            return false;
        }
    } else {
        dsyslog("nopacity: %s not available, using epgsearch default settings", *fileName);
        return false;
    }
    dsyslog("nopacity: config file complete");
    return true;
}

std::string *cNopacityEPGSearchConfig::SplitEpgSearchMenuLine(std::string line) {
    size_t posSeparator = -1;
    bool found = false;
    posSeparator = line.find_first_of("|") + 1;
    std::string *values = new std::string[MAXITEMTABS];
    for (int i=0; i < MAXITEMTABS; i++)
        values[i] = "";
    std::string value;
    int i=0;
    while (posSeparator > 0) {
        found = true;
        try {
            value = line.substr(0, posSeparator);
            value = value.substr(0, value.find_first_of(":"));
            value = value.substr(value.find_first_not_of(" "));
            value = value.substr(0, value.find_last_not_of(" ")+1);
            values[i] = value;
            line = line.substr(posSeparator);
            posSeparator = line.find_first_of("|") + 1;
        } catch (...) {}
        i++;
    }
    if (found) {
        try {
            value = line.substr(0, line.find_first_of(":"));
            value = value.substr(value.find_first_not_of(" "));
            value = value.substr(0, value.find_last_not_of(" ")+1);
            values[i] = value;
        } catch (...) {}
    }
    return values;
}

void cNopacityEPGSearchConfig::SetTokens(std::string *values, eEPGModes mode) { 
    for (int i=0; i<MAXITEMTABS; i++) {
        toLowerCase(values[i]);
        if (values[i].find("%time%") == 0)
            epgSearchConfig[mode][eEPGtime] = i;
        else if (values[i].find("%date%") == 0)
            epgSearchConfig[mode][eEPGdate] = i;
        else if (values[i].find("%datesh%") == 0)
            epgSearchConfig[mode][eEPGdatesh] = i;
        else if (values[i].find("%time_w%") == 0)
            epgSearchConfig[mode][eEPGtime_w] = i;
        else if (values[i].find("%time_d%") == 0)
            epgSearchConfig[mode][eEPGtime_d] = i;
        else if (values[i].find("%timespan%") == 0)
            epgSearchConfig[mode][eEPGtimespan] = i;
        else if (values[i].find("%title%") == 0)
            epgSearchConfig[mode][eEPGtitle] = i;
        else if (values[i].find("%subtitle%") == 0)
            epgSearchConfig[mode][eEPGsubtitle] = i;
        else if (values[i].find("%t_status%") == 0)
            epgSearchConfig[mode][eEPGt_status] = i;
        else if (values[i].find("%v_status%") == 0)
            epgSearchConfig[mode][eEPGv_status] = i;
        else if (values[i].find("%r_status%") == 0)
            epgSearchConfig[mode][eEPGr_status] = i;
        else if (values[i].find("%status%") == 0)
            epgSearchConfig[mode][eEPGstatus] = i;
        else if (values[i].find("%chnr%") == 0)
            epgSearchConfig[mode][eEPGchnr] = i;
        else if (values[i].find("%chsh%") == 0)
            epgSearchConfig[mode][eEPGchsh] = i;
        else if (values[i].find("%chlng%") == 0)
            epgSearchConfig[mode][eEPGchlng] = i;
        else if (values[i].find("%progr%") == 0)
            epgSearchConfig[mode][eEPGprogr] = i;
        else if (values[i].find("%progrt2s%") == 0)
            epgSearchConfig[mode][eEPGprogrT2S] = i;    
    }
    
}

void cNopacityEPGSearchConfig::SetDefaultEPGSearchConf(void) {
    //MenuSchedule
    epgSearchConfig[eMenuSchedule][eEPGtime_w] = 0;
    epgSearchConfig[eMenuSchedule][eEPGtime] = 1;
    epgSearchConfig[eMenuSchedule][eEPGstatus] = 2;
    epgSearchConfig[eMenuSchedule][eEPGtitle] = 3;
    //MenuWhatsOnNow
    int i=0;
    if (ShowChannelNr)
        epgSearchConfig[eMenuWhatsOnNow][eEPGchnr] = i++;
    epgSearchConfig[eMenuWhatsOnNow][eEPGchsh] = i++;
    epgSearchConfig[eMenuWhatsOnNow][eEPGtime] = i++;
    if(ShowProgress)
        epgSearchConfig[eMenuWhatsOnNow][eEPGprogrT2S] = i++;
    else
        epgSearchConfig[eMenuWhatsOnNow][eEPGprogr] = i++;
    epgSearchConfig[eMenuWhatsOnNow][eEPGstatus] = i++;
    epgSearchConfig[eMenuWhatsOnNow][eEPGtitle] = i++;
    //MenuWhatsOnNext
    i=0;
    if (ShowChannelNr)
        epgSearchConfig[eMenuWhatsOnNext][eEPGchnr] = i++;
    epgSearchConfig[eMenuWhatsOnNext][eEPGchsh] = i++;
    epgSearchConfig[eMenuWhatsOnNext][eEPGtime] = i++;
    epgSearchConfig[eMenuWhatsOnNext][eEPGstatus] = i++;
    epgSearchConfig[eMenuWhatsOnNext][eEPGtitle] = i++;
    //MenuWhatsOnElse
    i=0;
    if (ShowChannelNr)
        epgSearchConfig[eMenuWhatsOnElse][eEPGchnr] = i++;
    epgSearchConfig[eMenuWhatsOnElse][eEPGchsh] = i++;
    epgSearchConfig[eMenuWhatsOnElse][eEPGtime] = i++;
    epgSearchConfig[eMenuWhatsOnElse][eEPGstatus] = i++;
    epgSearchConfig[eMenuWhatsOnElse][eEPGtitle] = i++;
    //MenuSearchResults
    i=0;
    if (ShowChannelNr)
        epgSearchConfig[eMenuSearchResults][eEPGchnr] = i++;
    epgSearchConfig[eMenuSearchResults][eEPGchsh] = i++;
    epgSearchConfig[eMenuSearchResults][eEPGdatesh] = i++;
    epgSearchConfig[eMenuSearchResults][eEPGtime] = i++;
    epgSearchConfig[eMenuSearchResults][eEPGstatus] = i++;
    epgSearchConfig[eMenuSearchResults][eEPGtitle] = i++;
    //MenuFavorites
    i=0;
    if (ShowChannelNr)
        epgSearchConfig[eMenuFavorites][eEPGchnr] = i++;
    epgSearchConfig[eMenuFavorites][eEPGchsh] = i++;
    epgSearchConfig[eMenuFavorites][eEPGtime] = i++;
    epgSearchConfig[eMenuFavorites][eEPGtimespan] = i++;
    epgSearchConfig[eMenuFavorites][eEPGstatus] = i++;
    epgSearchConfig[eMenuFavorites][eEPGtitle] = i++;
}

void cNopacityEPGSearchConfig::SetTimerConfilictCont(void) {
    epgSearchConfig[eMenuTimerconflict][eEPGtime_d] = 0;
    epgSearchConfig[eMenuTimerconflict][eEPGchsh] = 1;
    epgSearchConfig[eMenuTimerconflict][eEPGdate] = 2;
    epgSearchConfig[eMenuTimerconflict][eEPGtime] = 3;
    epgSearchConfig[eMenuTimerconflict][eEPGtitle] = 4;
}

void cNopacityEPGSearchConfig::SetDefaultVDRConf(void) {
    //MenuSchedule
    epgSearchConfig[eMenuSchedule][eEPGdate] = 0;
    epgSearchConfig[eMenuSchedule][eEPGtime] = 1;
    epgSearchConfig[eMenuSchedule][eEPGstatus] = 2;
    epgSearchConfig[eMenuSchedule][eEPGtitle] = 3;
    //MenuWhatsOnNow
    epgSearchConfig[eMenuWhatsOnNow][eEPGchnr] = 0;
    epgSearchConfig[eMenuWhatsOnNow][eEPGchlng] = 1;
    epgSearchConfig[eMenuWhatsOnNow][eEPGtime] = 2;
    epgSearchConfig[eMenuWhatsOnNow][eEPGstatus] = 3;
    epgSearchConfig[eMenuWhatsOnNow][eEPGtitle] = 4;
    //MenuWhatsOnNext
    epgSearchConfig[eMenuWhatsOnNext][eEPGchnr] = 0;
    epgSearchConfig[eMenuWhatsOnNext][eEPGchlng] = 1;
    epgSearchConfig[eMenuWhatsOnNext][eEPGtime] = 2;
    epgSearchConfig[eMenuWhatsOnNext][eEPGstatus] = 3;
    epgSearchConfig[eMenuWhatsOnNext][eEPGtitle] = 4;
}


void cNopacityEPGSearchConfig::toLowerCase(std::string &str) {
    const int length = str.length();
    for(int i=0; i < length; ++i) {
        str[i] = std::tolower(str[i]);
    }
}