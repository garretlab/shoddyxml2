#include <WiFi.h>
#include <rssClass.h>

const char *ssid = "YOURSSID";
const char *password = "YOURPASSWORD";

const char *url = "http://rss.cnn.com/rss/edition.rss";
const char *tag = "title";

rssClass reader;
const int rssArticleSize = 128;
const int rssNumArticle = 20;
const int rssGetInterval = 1800;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.printf("Connecting to WiFi ... ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.printf("done.\n");

  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
}

void loop() {
  struct tm timeInfo;
  char header[32];

  getLocalTime(&timeInfo);
  sprintf(header, "News at %04d/%02d/%02d %02d:%02d",
          timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min);
  Serial.printf("\n%s\n", header);

  int n = reader.getArticles(url, tag, rssArticleSize, rssNumArticle);
  for (int i = 0; i < n; i++) {
    Serial.printf("%2d: %s\n", i, reader.itemData[i]);
  }
  delay(rssGetInterval*1000);
}
