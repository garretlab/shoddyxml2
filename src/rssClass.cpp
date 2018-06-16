#include "rssClass.h"
#include <string.h>

rssClass::rssClass() {
}

int rssClass::getArticles(const char *url, const char *targetTag, const int maxItemDataSize, const int maxItemNum) {
  int c;
  int port = 0;
  char protocol[8], server[32];

  clearItemData();
  resetStatus();
  itemData = new char *[maxItemNum];
  for (int i = 0; i < maxItemNum; i++) {
    itemData[i] = new char[maxItemDataSize];
  }
  this->maxItemNum = maxItemNum;
  this->maxItemDataSize = maxItemDataSize;

  sscanf(url, "%7[^:]://%31[^:/]:%6d/", protocol, server, &port);

  if (strcmp(protocol, "https") == 0) {
    client = new WiFiClientSecure();
    if (!port) {
      port = 443;
    }
  } else if (strcmp(protocol, "http") == 0) {
    client = new WiFiClient();
    if (!port) {
      port = 80;
    }
  } else {
    return 0;
  }

  this->targetTag = strdup(targetTag);

  if (client->connect(server, port) == 1) {
    client->print("GET ");
    client->print(url);
    client->println(" HTTP/1.1");
    client->print("Host: ");
    client->println(server);
    client->println("Connection: close");
    client->println();
    client->flush();

    skipHeaders();
    init();
    while ((c = getChar()) != EOF) {
      parse(c);
    }
    client->stop();

  }

  free(this->targetTag);
  delete client;
  client = NULL;
  return itemNum;
}

void rssClass::clearItemData() {
  if (itemData) {
    for (int i = 0; i < maxItemNum; i++) {
      delete itemData[i];
    }
    delete[] itemData;
    itemData = (char **)NULL;
  }
}

int rssClass::getChar() {
  static unsigned long lastRead = 0;
  if (client->connected()) {
    if (client->available()) {
      lastRead = millis();
      return client->read();
    } else {
      if ((millis() - lastRead) > CONNECTION_TIMEOUT_MILLIS) {
        return EOF;
      } else {
        return 0;
      }
    }
  } else {
    return EOF;
  }
}

void rssClass::foundXMLDecl() {
}

void rssClass::foundPI(char *s) {
}

void rssClass::foundSTag(char *s, int numAttributes, attribute_t attributes[]) {
  if (strcmp(s, "item") == 0) {
    itemDepth++;
  }

  if (strcmp(s, targetTag) == 0) {
    lastTagMatches = 1;
  } else {
    lastTagMatches = 0;
  }
}

void rssClass::foundETag(char *s) {
  if (itemNum < maxItemNum) {
    if ((itemDepth == 1) && (strcmp(s, targetTag) == 0)) {
      itemData[itemNum++][bufPos] = '\0';
      bufPos = 0;
    }
    if (strcmp(s, "item") == 0) {
      itemDepth--;
    }
  }
}

void rssClass::foundEmptyElemTag(char *s, int numAttributes, attribute_t attributes[]) {
}

void rssClass::foundSection(char *s) {
}

void rssClass::foundCharacter(char c) {
  if ((itemDepth == 1) && (lastTagMatches == 1)) {
    if (bufPos < (maxItemDataSize - 1)) {
      itemData[itemNum][bufPos++] = c;
    }
  }
}

void rssClass::foundElement(char *s) {
}

void rssClass::resetStatus() {
  itemDepth = 0;
  lastTagMatches = 0;
  itemNum = 0;
  bufPos = 0;
}

void rssClass::skipHeaders() {
  while (client->connected()) {
    String line = client->readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
}