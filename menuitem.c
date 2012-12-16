#include "menuitem.h"
#include <string>
#include <sstream>
#include <algorithm>


// cNopacityMenuItem  -------------

cNopacityMenuItem::cNopacityMenuItem(cOsd *osd, const char *text, bool sel) {
	this->osd = osd;
	hasIcon = false;
	drawn = false;
	Text = text;
	selectable = sel;
	current = false;
	wasCurrent = false;
	scrollable = false;
	itemTabs = NULL;
	tabWidth = NULL;
	pixmapIcon = NULL;
	pixmapTextScroller = NULL;
}

cNopacityMenuItem::~cNopacityMenuItem(void) {
	Cancel(-1);
	while (Active())
		cCondWait::SleepMs(10);
	delete [] itemTabs;
	delete [] tabWidth;
	osd->DestroyPixmap(pixmap);
	if (pixmapIcon) {
		osd->DestroyPixmap(pixmapIcon);
	}
	if (pixmapTextScroller) {
		osd->DestroyPixmap(pixmapTextScroller);
	}
}

void cNopacityMenuItem::SetCurrent(bool cur) {
	wasCurrent = current;
	current = cur;
} 

void cNopacityMenuItem::SetGeometry(int index, int top, int left, int width, int height) {
	this->index = index;
	this->top = top;
	this->left = left;
	this->width = width;
	this->height = height;
}

void cNopacityMenuItem::CreatePixmap() {
	pixmap = osd->CreatePixmap(3, cRect(left, top + index * (height + left), width, height));
}

void cNopacityMenuItem::CreatePixmapIcon(int iconWidth, int iconHeight) {
	pixmapIcon = osd->CreatePixmap(5, cRect(left, top + index * (height + left), iconWidth, iconHeight));
	pixmapIcon->Fill(clrTransparent);
	hasIcon = true;
}

void cNopacityMenuItem::CreatePixmapTextScroller(int totalWidth) {
	pixmapTextScroller = osd->CreatePixmap(4, cRect(left, top + index * (height + left), width, height), cRect(0, 0, totalWidth+10, height));
	pixmapTextScroller->Fill(clrTransparent);
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
	if (!drawn) {
		if (imgLoader.LoadIcon(icon, config.iconHeight)) {
			if (pixmapIcon == NULL) {
				pixmapIcon = osd->CreatePixmap(5, cRect(left, top + index * (height + left), config.menuItemLogoWidth, config.menuItemLogoWidth));
				pixmapIcon->Fill(clrTransparent);
			}
			pixmapIcon->DrawImage(cPoint(1, (height - config.iconHeight) / 2), imgLoader.GetImage());
		}
		drawn = true;
	}
	std::string delimiter = del;
	delimiter.erase(delimiter.find_last_not_of("-")+1);
	int x = config.iconHeight + 3;
	int y = (height - font->Height()) / 2;
	pixmap->DrawText(cPoint(x, y), delimiter.c_str(), Theme.Color(clrMenuFontMenuItemSep), clrTransparent, font);
}

void cNopacityMenuItem::Action(void) {
	int sleepTime = 10;
	for (int i = 0; Running() && (i*sleepTime < config.menuScrollDelay); i++)
		cCondWait::SleepMs(sleepTime);
	cPixmap::Lock();
	if (Running())
		SetTextFull();
	cPixmap::Unlock();
	int drawPortX;
	int FrameTime = config.menuScrollFrameTime;
	int maxX = pixmapTextScroller->DrawPort().Width() - pixmapTextScroller->ViewPort().Width();
	while (Running()) {
		uint64_t Now = cTimeMs::Now();
		cPixmap::Lock();
		drawPortX = pixmapTextScroller->DrawPort().X();
		drawPortX -= 1;
		if (abs(drawPortX) > maxX) {
			for (int i = 0; Running() && (i*sleepTime < config.menuScrollDelay); i++)
				cCondWait::SleepMs(sleepTime);
			drawPortX = 0;
		}
		if (Running())
			pixmapTextScroller->SetDrawPortPoint(cPoint(drawPortX, 0));
		cPixmap::Unlock();
		int Delta = cTimeMs::Now() - Now;
		if (Running())
			osd->Flush();
		if (Running() && (Delta < FrameTime))
			cCondWait::SleepMs(FrameTime - Delta);
	}
}

// cNopacityMainMenuItem  -------------
cNopacityMainMenuItem::cNopacityMainMenuItem(cOsd *osd, const char *text, bool sel) : cNopacityMenuItem (osd, text, sel) {
}

cNopacityMainMenuItem::~cNopacityMainMenuItem(void) {
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

void cNopacityMainMenuItem::CreatePixmapTextScroller(int totalWidth) {
	int drawPortWidth = totalWidth + 10;
	if (hasIcon)
		drawPortWidth += config.iconHeight + 10;
	pixmapTextScroller = osd->CreatePixmap(4, cRect(left, top + index * (height + left), width, height), cRect(0, 0, drawPortWidth, height));
	pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityMainMenuItem::CreateText() {
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
	strEntry = *menuEntry;
}

int cNopacityMainMenuItem::CheckScrollable(bool hasIcon) {
	int spaceLeft = left;
	if (hasIcon)
		spaceLeft += config.iconHeight;
	int totalTextWidth = width - spaceLeft;
	int numberWidth = font->Width("xxx");
	int textWidth = font->Width(*menuEntry);
	if ((numberWidth +  textWidth) > (width - spaceLeft)) {
		scrollable = true;
		totalTextWidth = max(numberWidth + textWidth, totalTextWidth);
		strEntryFull = strEntry.c_str();
		cTextWrapper twEntry;
		std::stringstream sstrEntry;
		twEntry.Set(strEntry.c_str(), font, width - spaceLeft - numberWidth);
		sstrEntry << twEntry.GetLine(0) << "...";
		strEntry = sstrEntry.str();
	}
	return totalTextWidth;
}

void cNopacityMainMenuItem::SetTextFull(void) {
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmapTextScroller->Fill(clrTransparent);
	int x = config.iconHeight;
	int numberTotalWidth = font->Width("xxx");
	int numberWidth = font->Width(*menuNumber);
	pixmapTextScroller->DrawText(cPoint(x + (numberTotalWidth - numberWidth)/2, (height - font->Height())/2), *menuNumber, clrFont, clrTransparent, font);
	x += numberTotalWidth;
	pixmapTextScroller->DrawText(cPoint(x, (height - font->Height())/2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityMainMenuItem::SetTextShort(void) {
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmapTextScroller->Fill(clrTransparent);
	int x = config.iconHeight;
	int numberTotalWidth = font->Width("xxx");
	int numberWidth = font->Width(*menuNumber);
	pixmapTextScroller->DrawText(cPoint(x + (numberTotalWidth - numberWidth)/2, (height - font->Height())/2), *menuNumber, clrFont, clrTransparent, font);
	x += numberTotalWidth;
	pixmapTextScroller->DrawText(cPoint(x, (height - font->Height())/2), strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityMainMenuItem::Render() {
	pixmap->Fill(Theme.Color(clrMenuBorder));
	int handleBgrd = (current)?handleBackgrounds[3]:handleBackgrounds[2];
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);
	if (selectable) {
		cString cIcon = GetIconName();
		if (!drawn) {
			cImageLoader imgLoader;
			if (imgLoader.LoadIcon(*cIcon, config.iconHeight)) {
				pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
			}
			drawn = true;
		}
		SetTextShort();
		if (current && scrollable && !Running() && config.menuScrollSpeed) {
			Start();
		}
		if (wasCurrent && !current && scrollable && Running()) {
			pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
			SetTextShort();
			Cancel(-1);
		}
	} else {
		DrawDelimiter(*itemTabs[1], "Channelseparator", handleBgrd);
	}
}

// cNopacityScheduleMenuItem  -------------

cNopacityScheduleMenuItem::cNopacityScheduleMenuItem(cOsd *osd, const char *text, bool sel, eMenuSubCategory subCat) : cNopacityMenuItem (osd, text, sel) {
	subCategory = subCat;
	strDateTime = "";
	strTitle = "";
	strSubTitle = "";
	strTitleFull = "";
	strSubTitleFull = "";
	strProgressbar = "";
	hasProgressBar = false;
	hasLogo = false;
	delimiterType = "daydelimiter";
}

cNopacityScheduleMenuItem::~cNopacityScheduleMenuItem(void) {
}

void cNopacityScheduleMenuItem::CreatePixmapTextScroller(int totalWidth) {
	int drawPortWidth = totalWidth + 10;
	if (hasIcon)
		drawPortWidth += config.menuItemLogoWidth + 10;
	pixmapTextScroller = osd->CreatePixmap(4, cRect(left, top + index * (height + left), width, height), cRect(0, 0, drawPortWidth, height));
	pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityScheduleMenuItem::SetDisplayMode(void) {
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
}

void cNopacityScheduleMenuItem::CreateText() {
	std::stringstream sstrDateTime;
	//Build Date & Time & Status
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGtime_d] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGtime_d] < MAXITEMTABS))
		sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtime_d]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGtime_w] > -1) 
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGtime_w] < MAXITEMTABS))
		sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtime_w]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGdate] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGdate] < MAXITEMTABS))
		sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGdate]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGdatesh] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGdatesh] < MAXITEMTABS))
		sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGdatesh]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGtime] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGtime] < MAXITEMTABS))
		sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtime]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGtimespan] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGtimespan] < MAXITEMTABS))
		sstrDateTime << *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtimespan]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGstatus] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGstatus] < MAXITEMTABS))
		sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGstatus]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGt_status] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGt_status] < MAXITEMTABS))
		sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGt_status]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGv_status] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGv_status] < MAXITEMTABS))
		sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGv_status]] << " ";
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGr_status] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGr_status] < MAXITEMTABS))
		sstrDateTime <<  *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGr_status]] << " ";
	strDateTime = sstrDateTime.str();
	//Build title and subtitle
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGtitle] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGtitle] < MAXITEMTABS)) {
		strTitle = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGtitle]];
		if ((config.epgSearchConf->epgSearchConfig[mode][eEPGsubtitle] > -1) 
			&&(config.epgSearchConf->epgSearchConfig[mode][eEPGsubtitle] < MAXITEMTABS)) {
			strSubTitle = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGsubtitle]];
		} else {
			size_t delimiter = strTitle.find("~");
			if (delimiter != std::string::npos) {
				strSubTitle = strTitle.substr(delimiter+2);
				strTitle = strTitle.substr(0, delimiter);
			} else {
				strSubTitle = "";
			}
		}
	}
	//Build Channel Name
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGchlng] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGchlng] < MAXITEMTABS))
		strChannelName = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGchlng]];
	else if ((config.epgSearchConf->epgSearchConfig[mode][eEPGchsh] > -1)
			&&(config.epgSearchConf->epgSearchConfig[mode][eEPGchsh] < MAXITEMTABS))
		strChannelName = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGchsh]];
	else
		strChannelName = " ";
	//Build Progressbar
	if ((config.epgSearchConf->epgSearchConfig[mode][eEPGprogrT2S] > -1)
		&&(config.epgSearchConf->epgSearchConfig[mode][eEPGprogrT2S] < MAXITEMTABS)) {
		strProgressbar = *itemTabs[config.epgSearchConf->epgSearchConfig[mode][eEPGprogrT2S]];
		hasProgressBar = true;
	}
}

int cNopacityScheduleMenuItem::CheckScrollable(bool hasIcon) {
	int spaceLeft = left;
	if (hasIcon)
		spaceLeft += config.menuItemLogoWidth;
	int totalTextWidth = width - spaceLeft;
	if (font->Width(strTitle.c_str()) > (width - spaceLeft)) {
		scrollable = true;
		totalTextWidth = max(font->Width(strTitle.c_str()), totalTextWidth);
		strTitleFull = strTitle.c_str();
		strSubTitleFull = strSubTitle.c_str();
		cTextWrapper twTitle;
		std::stringstream sstrTitle;
		twTitle.Set(strTitle.c_str(), font, width - spaceLeft);
		sstrTitle << twTitle.GetLine(0) << "...";
		strTitle = sstrTitle.str();
	}
	if (fontSmall->Width(strSubTitle.c_str()) > (width - spaceLeft)) {
		if (!scrollable) {
			scrollable = true;
			strTitleFull = strTitle.c_str();
			strSubTitleFull = strSubTitle.c_str();
		}
		totalTextWidth = max(fontSmall->Width(strSubTitle.c_str()), totalTextWidth);
		cTextWrapper twSubtitle;
		std::stringstream sstrSubtitle;
		twSubtitle.Set(strSubTitle.c_str(), fontSmall, width - spaceLeft);
		sstrSubtitle << twSubtitle.GetLine(0) << "...";
		strSubTitle = sstrSubtitle.str();
	}
	return totalTextWidth;

}

void cNopacityScheduleMenuItem::SetTextFull(void) {
	pixmapTextScroller->Fill(clrTransparent);
	pixmapTextScroller->DrawText(cPoint(textLeft, 3), strDateTime.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, font);
	pixmapTextScroller->DrawText(cPoint(textLeft, titleY), strTitleFull.c_str(), Theme.Color(clrMenuFontMenuItemTitle), clrTransparent, font);
	pixmapTextScroller->DrawText(cPoint(textLeft, titleY + font->Height() - 2), strSubTitleFull.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::SetTextShort(void) {
	pixmapTextScroller->Fill(clrTransparent);
	pixmapTextScroller->DrawText(cPoint(textLeft, 3), strDateTime.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, font);
	pixmapTextScroller->DrawText(cPoint(textLeft, titleY), strTitle.c_str(), Theme.Color(clrMenuFontMenuItemTitle), clrTransparent, font);
	pixmapTextScroller->DrawText(cPoint(textLeft, titleY + font->Height() - 2), strSubTitle.c_str(), Theme.Color(clrMenuFontMenuItem), clrTransparent, fontSmall);
}

void cNopacityScheduleMenuItem::Render() {
	int logoWidth = config.menuItemLogoWidth;
	int logoHeight = config.menuItemLogoHeight;
	textLeft = 5;
	int handleBgrd = (current)?handleBackgrounds[5]:handleBackgrounds[4];
	
	pixmap->Fill(Theme.Color(clrMenuBorder));
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);

	if (hasLogo)
		textLeft = logoWidth + 10;
        	
	if (selectable) {
		titleY = (height - font->Height())/2 - 2;
		//draw ProgressBar
		if (hasProgressBar) {
			DrawRemaining(strProgressbar.c_str(), textLeft, height*7/8, width - textLeft - 10);
		}
		//draw new?	
		if (!drawn) {
			//draw Icon
			if (hasIcon) {
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
			}
			SetTextShort();
			drawn = true;
		}
		if (current && scrollable && !Running() && config.menuScrollSpeed) {
			Start();
		}
		if (wasCurrent && !current && scrollable && Running()) {
			pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
			SetTextShort();
			Cancel(-1);
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

cNopacityChannelMenuItem::cNopacityChannelMenuItem(cOsd *osd, const char *text, bool sel) : cNopacityMenuItem (osd, text, sel) {
}

cNopacityChannelMenuItem::~cNopacityChannelMenuItem(void) {
}

void cNopacityChannelMenuItem::CreatePixmapTextScroller(int totalWidth) {
	int drawPortWidth = totalWidth + 10;
	if (hasIcon)
		drawPortWidth += config.menuItemLogoWidth + 10;
	pixmapTextScroller = osd->CreatePixmap(4, cRect(left, top + index * (height + left), width, height), cRect(0, 0, drawPortWidth, height));
	pixmapTextScroller->Fill(clrTransparent);
}

void cNopacityChannelMenuItem::CreateText() {
	std::string strChannelNumber = *itemTabs[0];
	std::string strChannelName = *itemTabs[1];
	std::string name = strChannelName.c_str();
	if ((name.length() > 0) && !isalnum(name.at(0))) {
		if (name.length() > 3)
			name = name.substr(4);
	}
	name.erase(name.find_last_not_of(" ")+1);
	std::stringstream sstrEntry;
	sstrEntry << strChannelNumber << " " << name;
	strEntry = sstrEntry.str();
	strLogo = name.c_str();
}

int cNopacityChannelMenuItem::CheckScrollable(bool hasIcon) {
	int spaceLeft = left;
	if (hasIcon)
		spaceLeft += config.menuItemLogoWidth;
	int totalTextWidth = width - spaceLeft;
	if (font->Width(strEntry.c_str()) > (width - spaceLeft)) {
		scrollable = true;
		totalTextWidth = max(font->Width(strEntry.c_str()), totalTextWidth);
		strEntryFull = strEntry.c_str();
		cTextWrapper twEntry;
		std::stringstream sstrEntry;
		twEntry.Set(strEntry.c_str(), font, width - spaceLeft);
		sstrEntry << twEntry.GetLine(0) << "...";
		strEntry = sstrEntry.str();
	}
	return totalTextWidth;
}

void cNopacityChannelMenuItem::SetTextFull(void) {
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmapTextScroller->Fill(clrTransparent);
	pixmapTextScroller->DrawText(cPoint(config.menuItemLogoWidth + 10, (height - font->Height())/2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityChannelMenuItem::SetTextShort(void) {
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmapTextScroller->Fill(clrTransparent);
	pixmapTextScroller->DrawText(cPoint(config.menuItemLogoWidth + 10, (height - font->Height())/2), strEntry.c_str(), clrFont, clrTransparent, font);
}

void cNopacityChannelMenuItem::Render() {
	
	int handleBgrd = (current)?handleBackgrounds[5]:handleBackgrounds[4];
	
	if (selectable) {							//Channels
		pixmap->Fill(Theme.Color(clrMenuBorder));
		int logoWidth = config.menuItemLogoWidth;
		int logoHeight = config.menuItemLogoHeight;
		pixmap->DrawImage(cPoint(1, 1), handleBgrd);
		if (!drawn) {
			cImageLoader imgLoader;
			if (imgLoader.LoadLogo(strLogo.c_str(), logoWidth, logoHeight)) {
				pixmapIcon->DrawImage(cPoint(1, 1), imgLoader.GetImage());
			}
			drawn = true;
		}
		SetTextShort();
		if (current && scrollable && !Running() && config.menuScrollSpeed) {
			Start();
		}
		if (wasCurrent && !current && scrollable && Running()) {
			pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
			SetTextShort();
			Cancel(-1);
		}
	} else {									//Channelseparators
		DrawDelimiter(*itemTabs[1], "Channelseparator", handleBgrd);
	}
}

// cNopacityDefaultMenuItem  -------------

cNopacityDefaultMenuItem::cNopacityDefaultMenuItem(cOsd *osd, const char *text, bool sel) : cNopacityMenuItem (osd, text, sel) {
	scrollCol = -1;
}

cNopacityDefaultMenuItem::~cNopacityDefaultMenuItem(void) {
}

void cNopacityDefaultMenuItem::SetTextFull(void) {
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmapTextScroller->Fill(clrTransparent);
	pixmapTextScroller->DrawText(cPoint(0, (height - font->Height()) / 2), strEntryFull.c_str(), clrFont, clrTransparent, font);
}

void cNopacityDefaultMenuItem::SetTextShort(void) {
	tColor clrFont = (current)?Theme.Color(clrMenuFontMenuItemHigh):Theme.Color(clrMenuFontMenuItem);
	pixmapTextScroller->Fill(clrTransparent);
	pixmapTextScroller->DrawText(cPoint(0, (height - font->Height()) / 2), strEntry.c_str(), clrFont, clrTransparent, font);
}

int cNopacityDefaultMenuItem::CheckScrollable(bool hasIcon) {
	int colWidth = 0;
	int colTextWidth = 0; 
	for (int i=0; i<numTabs; i++) {
		if (tabWidth[i] > 0) {
			colWidth = tabWidth[i+cSkinDisplayMenu::MaxTabs];
			colTextWidth = font->Width(*itemTabs[i]);
			if (colTextWidth > colWidth) {
				scrollable = true;
				scrollCol = i;
				strEntryFull = *itemTabs[i];
				cTextWrapper itemTextWrapped;
				itemTextWrapped.Set(*itemTabs[i], font, colWidth - font->Width("... "));
				strEntry = cString::sprintf("%s... ", itemTextWrapped.GetLine(0));
				break;
			}
		} else
			break;
	}
	if (scrollable) {
		pixmapTextScroller = osd->CreatePixmap(4, cRect(left + tabWidth[scrollCol], top + index * (height + left), tabWidth[scrollCol+numTabs], height), cRect(0, 0, colTextWidth+10, height));
		pixmapTextScroller->Fill(clrTransparent);
	}
	return 0;
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
			if (i != scrollCol) {
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
			} else {
				if (!Running())
					SetTextShort();
			}
		} else
			break;
	}
	if (current && scrollable && !Running() && config.menuScrollSpeed) {
		Start();
	}
	if (wasCurrent && !current && scrollable && Running()) {
		pixmapTextScroller->SetDrawPortPoint(cPoint(0, 0));
		SetTextShort();
		Cancel(-1);
	}
}

// cNopacityTrackMenuItem  -------------

cNopacityTrackMenuItem::cNopacityTrackMenuItem(cOsd *osd, const char *text) : cNopacityMenuItem (osd, text, true) {
}

cNopacityTrackMenuItem::~cNopacityTrackMenuItem(void) {
}

void cNopacityTrackMenuItem::Render() {
	pixmap->Fill(Theme.Color(clrMenuBorder));
	int handleBgrd = (current)?handleBackgrounds[1]:handleBackgrounds[0];
	pixmap->DrawImage(cPoint(1, 1), handleBgrd);
	pixmap->DrawText(cPoint(5, (height - font->Height())/2), Text, Theme.Color(clrTracksFontButtons), clrTransparent, font);
}
