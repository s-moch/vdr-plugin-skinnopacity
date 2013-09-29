#include "rssreader.h"

static size_t WriteXMLMemoryCallback(void* pointer, size_t size, size_t nmemb, void* xmlData) {
    size_t realsize = size * nmemb;
    struct XMLMemoryStruct* memStruct = (struct XMLMemoryStruct*)xmlData;
    if (memStruct->memory)
        memStruct->memory = (char*)realloc(memStruct->memory, memStruct->size + realsize + 1);
    else
        memStruct->memory = (char*)malloc(memStruct->size + realsize + 1);
    if (memStruct->memory) {
        memcpy (&(memStruct->memory[memStruct->size]), pointer, realsize);
        memStruct->size += realsize;
        memStruct->memory[memStruct->size] = 0;
    }
    return realsize;
}

cRssReader::cRssReader(cOsd *osd, cFont *font, cPoint position, cPoint size) {
    this->osd = osd;
    this->font = font;
    pixmap = NULL;

    x = position.X();
    y = position.Y();
    
    width = size.X();
    height = size.Y();
    
    xmlData.memory = 0;
    xmlData.size = 0;
    
    useProxy = false;
    httpproxy = "";
    
    separator = " +++ ";
    currentElement = 0;
    switchToNextMessage = false;
}

cRssReader::~cRssReader() {
    osd->DestroyPixmap(pixmap);
}

int cRssReader::readRssURL(const char *url) {
    CURL* my_curl_handle;
    long code;
    if (xmlData.memory)
      free(xmlData.memory);
    xmlData.memory = 0;
    xmlData.size = 0;
    if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
        esyslog("nopacity: Error, something went wrong with curl_global_init()");
        return -1;
    }
    my_curl_handle = curl_easy_init();
    if (!my_curl_handle) {
        esyslog("nopacity: Error, unable to get handle from curl_easy_init()");
        return -1;
    }
    if (useProxy) {
      curl_easy_setopt(my_curl_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
      curl_easy_setopt(my_curl_handle, CURLOPT_PROXY, httpproxy.c_str());             // Specify HTTP proxy
    }
    curl_easy_setopt(my_curl_handle, CURLOPT_URL, url);                               // Specify URL to get   
    curl_easy_setopt(my_curl_handle, CURLOPT_FOLLOWLOCATION, 0);                      // don't follow redirects
    curl_easy_setopt(my_curl_handle, CURLOPT_WRITEFUNCTION, WriteXMLMemoryCallback);  // Send all data to this function
    curl_easy_setopt(my_curl_handle, CURLOPT_WRITEDATA, (void*)&xmlData);             // Pass our 'data' struct to the callback function
    curl_easy_setopt(my_curl_handle, CURLOPT_MAXFILESIZE, 1024*1024);                 // Set maximum file size to get (bytes)
    curl_easy_setopt(my_curl_handle, CURLOPT_NOPROGRESS, 1);                          // No progress meter
    curl_easy_setopt(my_curl_handle, CURLOPT_NOSIGNAL, 1);                            // No signaling
    curl_easy_setopt(my_curl_handle, CURLOPT_TIMEOUT, 30);                            // Set timeout to 30 seconds
    curl_easy_setopt(my_curl_handle, CURLOPT_USERAGENT, NOPACITY_USERAGENT);          // Some servers don't like requests that are made without a user-agent field

    if (curl_easy_perform(my_curl_handle) != 0) {
        curl_easy_cleanup(my_curl_handle);  // Cleanup curl stuff
        if (xmlData.memory) {
            free(xmlData.memory);
            xmlData.memory = 0;
            xmlData.size = 0;
        }
        esyslog("nopacity: Error, download of '%s' failed", url);
        return -1;
    }
    curl_easy_getinfo(my_curl_handle, CURLINFO_HTTP_CODE, &code); 
    if (code == 404) {
        if (xmlData.memory)
            free(xmlData.memory);
        xmlData.memory = 0;
        xmlData.size = 0;
    }
    curl_easy_cleanup(my_curl_handle);     // Cleanup curl stuff
   
    return xmlData.size;
}

void cRssReader::traverseTree(xmlNode * a_node, bool foundItem) {
    xmlNode *cur_node = NULL;
    xmlChar *node_content;
    bool foundItemAct = false;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"item"))){
                foundItemAct = true;
                saveItem();
            } else {
                if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"title")) && foundItem){
                    node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
                    if (node_content)
                        title = (const char *)node_content;
                    else
                        title = "";
                    xmlFree(node_content);
                }
                if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"description")) && foundItem){
                    node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
                    if (node_content)
                        content = (const char *)node_content;
                    else 
                        content = "";
                    xmlFree(node_content);
                }
            }
        }
        traverseTree(cur_node->children, foundItemAct);
    }
    
}

void cRssReader::saveItem(void) {
    if (!title.length())
        return;
    RssElement rssElement;
    rssElement.title = title;
    rssElement.content = content;
    int width = font->Width(title.c_str()) + font->Width(": ") + font->Width(content.c_str()) + font->Width(separator.c_str());
    rssElement.width = width;
    rssElements.push_back(rssElement);
}

void cRssReader::saveRss(void) {
    xmlNode *root_element = NULL;
    root_element = xmlDocGetRootElement(doc);
    traverseTree(root_element, false);
    saveItem();
    xmlFreeDoc(doc);     
}

void cRssReader::createPixmap(void) {
    if (pixmap)
        osd->DestroyPixmap(pixmap);
    pixmap = osd->CreatePixmap(7, cRect(x, y, width, height), cRect(0, 0, rssElements[currentElement].width, height));
    pixmap->Fill(clrTransparent);
    pixmap->SetAlpha(0);
}

void cRssReader::drawText(void) {
    int currentX = 5;
    int textY = (height - font->Height()) / 2;
    cString text = cString::sprintf("%s: ", rssElements[currentElement].title.c_str());
    pixmap->DrawText(cPoint(currentX, textY), *text, Theme.Color(clrRSSFeedTitle), clrTransparent, font);
    currentX += font->Width(*text);
    text = cString::sprintf("%s%s", rssElements[currentElement].content.c_str(), separator.c_str());
    pixmap->DrawText(cPoint(currentX, textY), *text, Theme.Color(clrRSSFeedText), clrTransparent, font);
}

void cRssReader::DoSleep(int duration) {
    int sleepSlice = 10;
    for (int i = 0; Running() && (i*sleepSlice < duration); i++)
        cCondWait::SleepMs(sleepSlice);
}

void cRssReader::Action(void) {

    int success = readRssURL(feedUrl.c_str());
    if (success < 1)
        return;
    if (!strstr(xmlData.memory, "rss"))
        return;
    doc = NULL;
    doc = xmlReadMemory(xmlData.memory, strlen(xmlData.memory), "noname.xml", NULL, 0);
    if (doc == NULL) {
        return;
    }
    saveRss();
    //debugRSS();
    int numElements = rssElements.size();
    int scrollDelay = config.rssScrollDelay * 1000;
    int drawPortX;
    int FrameTime = config.rssScrollFrameTime;
    int maxX;
    bool doSleep;

    createPixmap();
    drawText();
    fadeInOut(true);
    DoSleep(scrollDelay);
    doSleep = false;
    maxX = pixmap->DrawPort().Width() - pixmap->ViewPort().Width();
    
    while (Running()) {
        if (doSleep) {
            fadeInOut(true);
            DoSleep(scrollDelay);
            doSleep = false;
        }
        uint64_t Now = cTimeMs::Now();
        cPixmap::Lock();
        drawPortX = pixmap->DrawPort().X();
        drawPortX -= 1;
        cPixmap::Unlock();
        if ((abs(drawPortX) > maxX) || switchToNextMessage) {
            if (!switchToNextMessage)
                DoSleep(scrollDelay);
            else
                switchToNextMessage = false;
            fadeInOut(false);
            currentElement = (currentElement + 1)%numElements;
            createPixmap();
            drawText();
            maxX = pixmap->DrawPort().Width() - pixmap->ViewPort().Width();
            drawPortX = 0;
            doSleep = true;
        }
        cPixmap::Lock();
        if (Running())
            pixmap->SetDrawPortPoint(cPoint(drawPortX, 0));
        cPixmap::Unlock();
        int Delta = cTimeMs::Now() - Now;
        if (Running())
            osd->Flush();
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
    }

}

void cRssReader::fadeInOut(bool fadeIn) {
    int frameTime = 50;
    int alpha = (fadeIn)?0:225;
    for (int i=0; i<10; i++) {
        if (Running()) {
            if (fadeIn)
                alpha += 26;
            else 
                alpha -= 26;
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
            cPixmap::Lock();
            pixmap->SetAlpha(alpha);
            cPixmap::Unlock();
            osd->Flush();
        }
        DoSleep(frameTime);       
    }
}

void cRssReader::debugRSS(void) {
    for (std::vector<RssElement>::iterator it = rssElements.begin(); it!=rssElements.end(); ++it) {
        esyslog("nopacity: title %s", it->title.c_str());
        esyslog("nopacity: content %s", it->content.c_str());
    }
    
}

cRssStandaloneTicker::cRssStandaloneTicker(cImageCache *imgCache) {
    this->imgCache = imgCache;
    osdLeft = cOsd::OsdLeft();
    osdWidth = cOsd::OsdWidth();
    osdHeight = config.rssFeedHeightStandalone * cOsd::OsdHeight() / 100;
    if (config.rssFeedStandalonePos == 0)
        osdTop = cOsd::OsdHeight() - osdHeight - 5;
    else
        osdTop = cOsd::OsdTop() + 5;
    
    osd = CreateOsd(osdLeft, osdTop, osdWidth, osdHeight);
    font = cFont::CreateFont(config.fontName, (osdHeight / 2) + 3 + config.fontRssFeedStandalone);
    pixmapBackground = osd->CreatePixmap(1, cRect(0, 0, osdWidth, osdHeight));
    pixmapFeed = osd->CreatePixmap(2, cRect(0, 0, osdWidth, osdHeight));
    pixmapIcon = osd->CreatePixmap(3, cRect(0, 0, osdHeight, osdHeight));
    currentFeed = 0;
}

cRssStandaloneTicker::~cRssStandaloneTicker() {
    if (rssReader) {
        rssReader->Stop();
        while (rssReader->Active())
            cCondWait::SleepMs(10);
        delete rssReader;
        rssReader = NULL;
    }
    osd->DestroyPixmap(pixmapFeed);
    osd->DestroyPixmap(pixmapBackground);
    osd->DestroyPixmap(pixmapIcon);
    delete font;
    delete osd;
}

void cRssStandaloneTicker::SetFeed(std::string feedName) {
    pixmapBackground->Fill(clrBlack);
    pixmapFeed->Fill(clrTransparent);
    pixmapIcon->Fill(clrTransparent);
    
    int feedNameLength = font->Width(feedName.c_str());
    labelWidth = 2 + osdHeight + 2 + feedNameLength + 6;
    pixmapFeed->Fill(Theme.Color(clrRSSFeedBorder));
    if (config.doBlending) {
        cImage imgBack = imgCache->GetBackground(Theme.Color(clrRSSFeedHeaderBack), Theme.Color(clrRSSFeedHeaderBackBlend), labelWidth, osdHeight - 4);
        pixmapFeed->DrawImage(cPoint(2,2), imgBack);
        cImage imgBack2 = imgCache->GetBackground(Theme.Color(clrRSSFeedBack), Theme.Color(clrRSSFeedBackBlend), osdWidth - labelWidth - 2, osdHeight - 4);
        pixmapFeed->DrawImage(cPoint(labelWidth,2), imgBack2);
    } else {
        pixmapFeed->DrawRectangle(cRect(2, 2, labelWidth, osdHeight - 4), Theme.Color(clrRSSFeedHeaderBack));
        pixmapFeed->DrawRectangle(cRect(labelWidth, 2, osdWidth - labelWidth - 2, osdHeight - 4), Theme.Color(clrRSSFeedBack));
    }
    pixmapFeed->DrawText(cPoint(osdHeight + 2, (osdHeight - font->Height()) / 2), feedName.c_str(), Theme.Color(clrRSSFeedHeaderText), clrTransparent, font);
    pixmapIcon->Fill(clrTransparent);
    
    cImage *imgIcon = imgCache->GetSkinIcon("skinIcons/rssStandalone", osdHeight-4, osdHeight-4);
    if (imgIcon)
        pixmapIcon->DrawImage(cPoint(2,2), *imgIcon);
    osd->Flush();
}

void cRssStandaloneTicker::Start(void) {
    rssReader = new cRssReader(osd, font, cPoint(labelWidth,0), cPoint(osdWidth, osdHeight));
    rssReader->SetFeed(config.rssFeeds[config.rssFeed[currentFeed]].url);
    rssReader->Start();
}

void cRssStandaloneTicker::SwitchNextRssMessage(void) {
    if (rssReader) {
        rssReader->SwitchNextMessage();
    }
}

void cRssStandaloneTicker::SwitchNextRssFeed(void) {
    if (rssReader) {
        rssReader->Stop();
        while (rssReader->Active())
            cCondWait::SleepMs(10);
        delete rssReader;
        rssReader = NULL;
    }
    SetNextFeed();
    int feedNum = (config.rssFeed[currentFeed]==0)?0:(config.rssFeed[currentFeed]-1);
    SetFeed(config.rssFeeds[feedNum].name);
    Start();
}

void cRssStandaloneTicker::SetNextFeed(void) {
    int nextFeed = 0;
    for (int i = 1; i<6; i++) {
        nextFeed = (currentFeed + i)%5;
        if ((nextFeed == 0)||(config.rssFeed[nextFeed] > 0)) {
            currentFeed = nextFeed;
            break;
        }    
    }
} 