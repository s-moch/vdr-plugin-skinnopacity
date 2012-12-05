#ifndef __NOPACITY_EPGSEARCHCONFIG_H
#define __NOPACITY_EPGSEARCHCONFIG_H

#define EPGSEARCHCATS 7
#define EPGSEARCHTOKENS 17

class cNopacityEPGSearchConfig {
	private:
		bool ReplaceSchedule;
		bool ShowChannelNr;
		bool ShowProgress;
		int MaxTabs;
		std::string *SplitEpgSearchMenuLine(std::string line);
		void SetTokens(std::string *values, eEPGModes mode);
		void toLowerCase(std::string &str);
	public:
		cNopacityEPGSearchConfig(void);
		virtual ~cNopacityEPGSearchConfig();
		bool CheckEPGSearchAvailable(void);
		bool ReplaceScheduleActive(void) {return ReplaceSchedule;};
		bool LoadEpgSearchConf(void);
		bool LoadEpgSearchMenuConf(void);
		void SetDefaultEPGSearchConf(void);
		void SetDefaultVDRConf(void);
		void SetTimerConfilictCont(void);
		bool UserSet[4];
		cString User[4];
		int epgSearchConfig[EPGSEARCHCATS][EPGSEARCHTOKENS];
};

#endif //__NOPACITY_EPGSEARCHCONFIG_H