#ifndef __NOPACITY_RSSREADER_H
#define __NOPACITY_RSSREADER_H

#include <curl/curl.h>
#include <curl/easy.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <vector>

#define NOPACITY_USERAGENT "libcurl-agent/1.0"

struct XMLMemoryStruct {
   char   *memory;
   size_t  size;
};
 
struct RssElement {
   std::string  title;
   std::string  content;
   int          width;
};
 
class cRssReader : public cThread{
public:
    cRssReader(cOsd *osd, cFont *font, cPoint position, cPoint size);
    ~cRssReader();
    virtual void Action(void);
    void Stop(void) {Cancel(-1);};
    void SetFeed(std::string feedUrl) {this->feedUrl = feedUrl;};
    void SwitchNextMessage(void) {switchToNextMessage = true;};
private:
    cOsd *osd;
    cFont *font;
    cPixmap *pixmap;
    std::string feedUrl;
    int x, y;
    int width, height;
    void createPixmap(void);
    void drawText(void);
    int readRssURL(const char *url);
    void saveRss(void);
    void traverseTree(xmlNode * a_node, bool foundItem);
    void saveItem(void);
    void fadeInOut(bool fadeIn);
    XMLMemoryStruct xmlData;
    xmlDoc *doc;
    std::string title, content;
    std::vector<RssElement> rssElements;
    int currentElement;
    bool useProxy;
    std::string httpproxy;
    std::string separator;
    void DoSleep(int duration);
    bool switchToNextMessage;
};

#endif //__NOPACITY_RSSREADER_H