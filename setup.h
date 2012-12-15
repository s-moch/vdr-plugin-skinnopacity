#ifndef __NOPACITY_SETUP_H
#define __NOPACITY_SETUP_H

class cNopacitySetup : public cMenuSetupPage {
	public:
		cNopacitySetup(void);
		virtual ~cNopacitySetup();		
	private:
		cNopacityConfig tmpNopacityConfig;
		cStringList fontNames;
		void Setup(void);
	protected:
		virtual eOSState ProcessKey(eKeys Key);
		virtual void Store(void);

};


class cMenuSetupSubMenu : public cOsdMenu {
	protected:
		cNopacityConfig *tmpNopacityConfig;
		virtual eOSState ProcessKey(eKeys Key);
		virtual void Set(void) = 0;
		cOsdItem *InfoItem(const char *label, const char *value);
	public:
		cMenuSetupSubMenu(const char *Title, cNopacityConfig *data);
};

class cNopacitySetupMenuDisplay : public cMenuSetupSubMenu {
	protected:
		const char *useSubtitleRerunTexts[3];
		const char *scrollSpeed[4];
		void Set(void);
	public:
		cNopacitySetupMenuDisplay(cNopacityConfig *data);
};

class cNopacitySetupChannelDisplay : public cMenuSetupSubMenu {
	protected:
		void Set(void);
	public:
		cNopacitySetupChannelDisplay(cNopacityConfig *data);
};

class cNopacitySetupReplayDisplay : public cMenuSetupSubMenu {
	protected:
		void Set(void);
	public:
		cNopacitySetupReplayDisplay(cNopacityConfig *data);
};

class cNopacitySetupTrackDisplay : public cMenuSetupSubMenu {
	protected:
		void Set(void);
	public:
		cNopacitySetupTrackDisplay(cNopacityConfig *data);
};

class cNopacitySetupMessageDisplay : public cMenuSetupSubMenu {
	protected:
		void Set(void);
	public:
		cNopacitySetupMessageDisplay(cNopacityConfig *data);
};

class cNopacitySetupVolumeDisplay : public cMenuSetupSubMenu {
	protected:
		void Set(void);
	public:
		cNopacitySetupVolumeDisplay(cNopacityConfig *data);
};
#endif //__NOPACITY_SETUP_H