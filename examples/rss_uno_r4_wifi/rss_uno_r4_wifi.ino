#include <WiFi.h>
#include "rssClass.h"
#include "arduino_secrets.h"
#include <garretlab_MisakiLEDMatrix.h>

garretlab_MisakiLEDMatrix matrix;

const char *url = "https://www.nhk.or.jp/rss/news/cat0.xml";
const char *tag = "title";

rssClass reader;
const int rssArticleSize = 128;
const int rssNumArticle = 20;
const int rssGetInterval = 10000;

void setup() {
  int status = WL_IDLE_STATUS;

  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    // Connect to WPA/WPA2 network.
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.print("Connecting to WiFi ... ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("done.\n");

  matrix.begin();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(120);
}

void loop() {
  char header[32];
  int n;
  static int first = 1;
  static unsigned long lastRead = millis();

  if (first || (millis() - lastRead > rssGetInterval)) {
    n = reader.getArticles(url, tag, rssArticleSize, rssNumArticle);
    for (int i = 0; i < n; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(reader.itemData[i]);
    }
    Serial.println("");
    first = 0;
    lastRead = millis();
  }

  matrix.beginDraw();
  matrix.clear();
  matrix.textFont(Font_Misaki);
  matrix.beginText(0, 0, 0xFFFFFF);
  for (int i = 0; i < n; i++) {
    matrix.println(reader.itemData[i]);
    matrix.println("     ");
  }
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}
