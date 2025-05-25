#ifdef ARDUINO_UNOR4_WIFI 
#include <WiFiSSLClient.h>
#include <WiFiS3.h>
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <WiFiClientSecure.h>
#endif

#include "shoddyxml2.h"

#define CLIENT_TIMEOUT_MILLIS 30000

class rssClass: public shoddyxml {
  public:
    rssClass();

    char **itemData;
    int itemNum;

    int getArticles(const char *url, const char *targetTag, const int maxItemDataSize, const int maxItemNum, const char *rootCA = NULL);
    void clearItemData();

  private:
    virtual int getChar();
    virtual void foundXMLDecl();
    virtual void foundPI(char *s);
    virtual void foundSTag(char *s, int numAttributes, attribute_t attributes[]);
    virtual void foundETag(char *s);
    virtual void foundEmptyElemTag(char *s, int numAttributes, attribute_t attributes[]);
    virtual void foundSection(char *s);
    virtual void foundCharacter(char c);
    virtual void foundElement(char *s);

    Client *client;
    WiFiClient wifiClient;
#ifdef ARDUINO_UNOR4_WIFI 
    WiFiSSLClient sslClient;
#elif ARDUINO_ARCH_ESP32
    WiFiClientSecure sslClient;
#endif

    int bufPos;
    int itemDepth;
    int lastTagMatches;
    char *targetTag;
    int maxItemNum;
    int maxItemDataSize;
    
    void resetStatus();
    void skipHeaders();
};
