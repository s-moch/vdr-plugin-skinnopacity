#include "menuitem.h"
#include <string>
#include <sstream>
#include <algorithm>


// cNopacityMenuItem  -------------

cNopacityMenuItem::cNopacityMenuItem(cOsd *osd, const char *text, bool cur, bool sel) {
	this->osd = osd;
	hasIcon = false;
	iconDrawn = false;
	Text = text;
	selectable = sel;
	current = cur;
	itemTabs = NULL;
	tabWidth = NULL;
	pixmapIcon = NULL;
}

cNopacityMenuItem::~cNopacityMenuItem(void) {
	delete [] itemTabs;
	delete [] tabWidth;
	osd->DestroyPixmap(pixmap);
	if (pixmapIcon) {
		osd->DestroyPixmap(pixmapIcon);
	}
}

void cNopacityMenuItem::CreatePixmap(int top, int space, int index, int width, int height) {
	pixmap = osd->CreatePixmap(3, cRect(space, top + index * (height + space), width, height));
	this->width = width;
	this->height = height;
}

void cNopacityMenuItem::CreatePixmapIcon(int top, int space, int index, int itemHeight, int iconWidth, int iconHeight) {
	pixmapIcon = osd->CreatePixmap(4, cRect(space, top + index * (itemHeight + space), iconWidth, iconHeight));
	pixmapIcon->Fill(clrTransparent);
	hasIcon = true;
}

void cNopacityMenuItem::SetTabs(cString *tabs, int *tabWidths, int numtabs) {
	if (itemTabs)
		delete [] itemTabs;
	if (tabWidth)
		delete [] tabWidth;
	itemTabs = tabs;
	tabWidth = tabWidths;
	numTabs = numtabs;
}

void cNopacityMenuItem::SetBackgrounds(int *handleBackgrounds) {
	this->handleBackgrounds = handleBackgrounds;
}

void cNopacityMenuItem::DrawDelimiter(const char *del, const char *icon, int handleBgrd) {
	pixmap->Fill(Theme.Color(clrMenuItem));
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);
	cImageLoader imgLoader;
	if (!iconDrawn) {
		if (imgLoader.LoadIcon(icon, config.iconHeight)) {
			pixmapIcon->DrawImage(cPoint(1, (height - config.iconHeight) / 2), imgLoader.GetImage());
		}
		iconDrawn = true;
	}
	std::string delimiter = del;
	delimiter.erase(delimiter.find_last_not_of("-")+1);
	int x = config.iconHeight + 3;
	int y = (height - font->Height()) / 2;
	pixmap->DrawText(cPoint(x, y), delimiter.c_str(), Theme.Color(clrMenuFontMenuItemSep), clrTransparent, font);
}

// cNopacityMainMenuItem  -------------
cNopacityMainMenuItem::cNopacityMainMenuItem(cOsd *osd, const char *text, bool cur, bool sel) : cNopacityMenuItem (osd, text, cur, sel) {
}

cNopacityMainMenuItem::~cNopacityMainMenuItem(void) {
}

void cNopacityMainMenuItem::SplitMenuItem() {

	std::string text = skipspace(Text);
	bool found = false;
	bool doBreak = false;
	size_t i = 0;
	for (; i < text.length(); i++) {
		char s = text.at(i);
		if (found) {
			if (!(s >= '0' && s <= '9')) {
				doBreak = true;
			}			
		}
		if (s >= '0' && s <= '9') {
			found = true;
		}
		if (doBreak)
			break;
		if (i>4)
			break;
	}
	if (found) {
		menuNumber = skipspace(text.substr(0,i).c_str());
		menuEntry = skipspace(text.substr(i).c_str());
	} else {
		menuNumber = "";
		menuEntry = text.c_str();		
	}
}

std::string cNopacityMainMenuItem::items[6] = {"Schedule", "Channels", "Timers", "Recordings", "Setup", "Commands"};

cString cNopacityMainMenuItem::GetIconName() {
	std::string element = *menuEntry;
	for (int i=0; i<6; i++) {
		std::string s = trVDR(items[i].c_str());
		if (s == element)
			return items[i].c_str();
	}
	return menuEntry;
}

void cNopacityMainMenuItem::Render() {
	
	pixmap->Fill(Theme.Color(clrMenuBorder));
	int handleBgrd = (current)?handleBackgrounds[3]:handleBackgrounds[2];
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);
	SplitMenuItem();
	cString cIcon = GetIconName();
	if (!iconDrawn) {
		cImageLoader imgLoader;
		if (imgLoader.LoadIcon(*cIcon, config.iconHeight)) {
			pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
		}
		iconDrawn = true;
	}
	
	int x = config.iconHeight;
	int numberTotalWidth = font->Width("xxx");
	int numberWidth = font->Width(*menuNumber);
	pixmap->DrawText(cPoint(x + (numberTotalWidth - numberWidth)/2, (height - font->Height())/2), *menuNumber, clrFont, clrTransparent, font);
	x += numberTotalWidth;

	int textWidth = font->Width(*menuEntry);
	if (textWidth <= width - x) {
		pixmap->DrawText(cPoint(x, (height - font->Height())/2), *menuEntry, clrFont, clrTransparent, font);
	} else {
		cTextWrapper menuText;
		menuText.Set(*menuEntry, font, width - x);
		//max. 2 lines
		pixmap->DrawText(cPoint(x, (height/2 - font->Height())  / 2 ), menuText.GetLine(0), clrFont, clrTransparent, font);
		pixmap->DrawText(cPoint(x, (height/2 - font->Height())  / 2 + height/2), menuText.GetLine(1), clrFont, clrTransparent, font);
	}
}

// cNopacityScheduleMenuItem  -------------

cNopacityScheduleMenuItem::cNopacityScheduleMenuItem(cOsd *osd, const char *text, bool cur, bool sel, eMenuSubCategory subCat) : cNopacityMenuItem (osd, text, cur, sel) {
	subCategory = subCat;
}

cNopacityScheduleMenuItem::~cNopacityScheduleMenuItem(void) {
}

void cNopacityScheduleMenuItem::Render() {
	int logoWidth = config.menuItemLogoWidth;
	int logoHeight = config.menuItemLogoHeight;
	std::stringstream sstrDateTime;
	std::string strTitle;
	std::string strSubtitle;
	std::string strChannelName;
	std::string strProgressbar;
	eEPGModes mode;
	bool hasLogo = false;
	int left = 5;
	std::string delimiterType = "daydelimiter";
	bool hasProgressBar = false;
	int handleBgrd = (current)?handleBackgrounds[5]:handleBackgrounds[4];
	
	pixmap->Fill(Theme.Color(clrMenuBorder));
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);
	
	switch (subCategory) {
		case mcSubSchedule:
			mode = eMenuSchedule;
			break;
		case mcSubScheduleWhatsOnNow:
			mode = eMenuWhatsOnNow;
			hasLogo = true;
			delimiterType = "Channelseparator";
			break;
		case mcSubScheduleWhatsOnNext:
			mode = eMenuWhatsOnNext;
			hasLogo = true;
			delimiterType = "Channelseparator";
			break;
		case mcSubScheduleSearchResults:
			mode = eMenuSearchResults;
			hasLogo = true;
			delimiterType = "Channelseparator";
			break;
		case mcSubScheduleWhatsOnElse:
			mode = eMenuWhatsOnElse;
			hasLogo = true;
			break;
		case mcSubScheduleFavorites:
			mode = eMenuFavorites;
			hasLogo = true;
			break;
		case mcSubScheduleTimerconflict:
			mode = eMenuTimerconflict;
			hasLogo = true;
			break;
		default:
			mode = eMenuSchedule;
			break;
	}
	
	if (hasLogo)
		left = logoWidth + 10;
        	
	if (selectable) {
		//Build Date & Time & Status
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGtime_d] > -1)
			sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtime_d]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGtime_w] > -1) 
			sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtime_w]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGdate] > -1)
			sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGdate]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGdatesh] > -1)
			sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGdatesh]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGtime] > -1)
			sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtime]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGtimespan] > -1)
			sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtimespan]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGstatus] > -1)
			sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGstatus]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGt_status] > -1)
			sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGt_status]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGv_status] > -1)
			sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGv_status]] << " ";
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGr_status] > -1)
			sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGr_status]] << " ";
		//Build title and subtitle
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGtitle] > -1) {
			strTitle = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtitle]];
			if (config.epgSearchConf->epgSearchConfig[mode][eEPGsubtitle] > -1) {
				strSubtitle = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGsubtitle]];
			} else {
				size_t delimiter = strTitle.find("~");
				if (delimiter != std::string::npos) {
					strSubtitle = strTitle.substr(delimiter+2);
					strTitle = strTitle.substr(0, delimiter);
				} else {
					strSubtitle = "";
				}
			}
		}
		if (font->Width(strTitle.c_str()) > (width - left)) {
			cTextWrapper twTitle;
			std::stringstream sstrTitle;
			twTitle.Set(strTitle.c_str(), font, width - left);
			sstrTitle << twTitle.GetLine(0) << "...";
			strTitle = sstrTitle.str();
		}
		if (fontSmall->Width(strSubtitle.c_str()) > (width - left)) {
			cTextWrapper twSubtitle;
			std::stringstream sstrSubtitle;
			twSubtitle.Set(strSubtitle.c_str(), fontSmall, width - left);
			sstrSubtitle << twSubtitle.GetLine(0) << "...";
			strSubtitle = sstrSubtitle.str();
		}
		//Build Channel Name
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGchlng] > -1)
			strChannelName = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGchlng]];
		else if (config.epgSearchConf->epgSearchConfig[mode][eEPGchsh] > -1)
			strChannelName = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGchsh]];
		else
			strChannelName = "";
		//Build Progressbar
		if (config.epgSearchConf->epgSearchConfig[mode][eEPGprogrT2S] > -1) {
			strProgressbar = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGprogrT2S]];
			hasProgressBar = true;
		}

		//DISPLAY
		int titleY = (height - font->Height())/2 - 2;
		if (strSubtitle.length() == 0)
			titleY = font->Height() + (height - 2*font->Height())/2;
		pixmap->DrawText(cPoint(left, 3), sstrDateTime.str().c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, font);
		pixmap->DrawText(cPoint(left, titleY), strTitle.c_str(), Theme.Color(clrMenuFontMenuItemTitle), clrTransparent, font);
		pixmap->DrawText(cPoint(left, titleY + font->Height() - 2), strSubtitle.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, fontSmall);
		if (hasIcon) {
			if (!iconDrawn) {
				cImageLoader imgLoader;
				if (imgLoader.LoadLogo(strChannelName.c_str(), logoWidth, logoHeight)) {
					pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
				} else {
					cTextWrapper channel;
					channel.Set(strChannelName.c_str(), font, logoWidth);
					int lines = channel.Lines();
					int lineHeight = height / 3;
					int heightChannel = lines * lineHeight;
					int y = (heightChannel>height)?0:(height-heightChannel)/2;
					for (int line = 0; line < lines; line++) {
						pixmapIcon->DrawText(cPoint((logoWidth - font->Width(channel.GetLine(line)))/2, y+lineHeight*line), channel.GetLine(line), Theme.Color(clrMenuFontMenuItemHigh), clrTransparent, font);
					}	
				}
				iconDrawn = true;
			}
		}
		if (hasProgressBar) {
			DrawRemaining(strProgressbar.c_str(), left, height*7/8, width - left - 10);
		}
		
	} else {
		DrawDelimiter(*itemTabs[1], delimiterType.c_str(), handleBgrd);
	}
}

void cNopacityScheduleMenuItem::DrawRemaining(cString remaining, int x, int y, int width) {
	int seen = 0;
	int total = 8;
	if (*remaining) {
		const char *p = *remaining;
		p++;
		while (*p) {
			if (*p == '|') {
				seen++;
				p++;
			} else
				break;
        }
	}
	double percentSeen = (double)seen/total;
	pixmap->DrawEllipse(cRect(x, y, 7, 7), Theme.Color(clrProgressBarBack));
	pixmap->DrawEllipse(cRect(x+width, y, 7, 7), Theme.Color(clrProgressBarBack));
	pixmap->DrawRectangle(cRect(x+4, y, width-1, 7), Theme.Color(clrProgressBarBack));
	pixmap->DrawEllipse(cRect(x+1, y+1, 5, 5), Theme.Color(clrProgressBar));
	if (percentSeen > 0.0)
		pixmap->DrawEllipse(cRect(x+(width*percentSeen), y+1, 5, 5), Theme.Color(clrProgressBar));
	pixmap->DrawRectangle(cRect(x+4, y+1, (width-1)*percentSeen, 5), Theme.Color(clrProgressBar));
}

// cNopacityChannelMenuItem  -------------

cNopacityChannelMenuItem::cNopacityChannelMenuItem(cOsd *osd, const char *text, bool cur, bool sel) : cNopacityMenuItem (osd, text, cur, sel) {
}

cNopacityChannelMenuItem::~cNopacityChannelMenuItem(void) {
}

void cNopacityChannelMenuItem::Render() {
	
	int handleBgrd = (current)?handleBackgrounds[5]:handleBackgrounds[4];
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	cString channelNumber = *itemTabs[0];
	cString channelName = *itemTabs[1];
	if (selectable) {							//Channels
		pixmap->Fill(Theme.Color(clrMenuBorder));
		int logoWidth = config.menuItemLogoWidth;
		int logoHeight = config.menuItemLogoHeight;
		//eliminate strange wareagle icons :-/
		std::string name = *channelName;
		if (!isalnum(name.at(0))) {
			if (name.length() > 3)
				name = name.substr(4);
		}
		name.erase(name.find_last_not_of(" ")+1);
		pixmap->DrawImage(cPoint(1, 1), handleBgrd);
		if (!iconDrawn) {
			cImageLoader imgLoader;
			if (imgLoader.LoadLogo(name.c_str(), logoWidth, logoHeight)) {
				pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
			}
			iconDrawn = true;
		}
		cTextWrapper channel;
		channel.Set(*cString::sprintf("%s %s", *channelNumber, *channelName), font, width - logoWidth - 6);
		int lineHeight = font->Height();
		int lines = channel.Lines();
		int heightChannel = lines * lineHeight;
		int y = (heightChannel>height)?0:(height-heightChannel)/2;
		for (int line = 0; line < lines; line++) {
			pixmap->DrawText(cPoint(logoWidth + 10, y+lineHeight*line), channel.GetLine(line), clrFont, clrTransparent, font);
		}
	} else {									//Channelseparators
		DrawDelimiter(*itemTabs[1], "Channelseparator", handleBgrd);
	}
}

// cNopacityDefaultMenuItem  -------------

cNopacityDefaultMenuItem::cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool cur, bool sel) : cNopacityMenuItem (osd, text, cur, sel) {
}

cNopacityDefaultMenuItem::~cNopacityDefaultMenuItem(void) {
}

void cNopacityDefaultMenuItem::Render() {
	pixmap->Fill(Theme.Color(clrMenuBorder));
	int handleBgrd = (current)?handleBackgrounds[1]:handleBackgrounds[0];
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);
	int colWidth = 0;
	int colTextWidth = 0; 
	cString itemText("");
	for (int i=0; i<numTabs; i++) {
		if (tabWidth[i] > 0) {
			colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
			colTextWidth = font->Width(*itemTabs[i]);
			if (colTextWidth > colWidth) {
				cTextWrapper itemTextWrapped;
				itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
				itemText = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
			} else {
				itemText = itemTabs[i];
			}
			pixmap->DrawText(cPoint(tabWidth[i], (height - font->Height()) / 2), *itemText, clrFont, clrTransparent, font);
		} else
			break;
	}
}

// cNopacityTrackMenuItem  -------------

cNopacityTrackMenuItem::cNopacityTrackMenuItem(cOsd *osd, const char *text, bool cur) : cNopacityMenuItem (osd, text, cur, true) {
}

cNopacityTrackMenuItem::~cNopacityTrackMenuItem(void) {
}

void cNopacityTrackMenuItem::Render() {
	pixmap->Fill(Theme.Color(clrMenuBorder));
	int handleBgrd = (current)?handleBackgrounds[1]:handleBackgrounds[0];
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);
	pixmap->DrawText(cPoint(5, (height - font->Height())/2), Text, Theme.Color(clrTracksFontButtons), clrTransparent, font);
}
