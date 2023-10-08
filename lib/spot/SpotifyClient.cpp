/**The MIT License (MIT)

 Copyright (c) 2018 by ThingPulse Ltd., https://thingpulse.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include "Arduino.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "SpotifyClient.h"
#include "base64.h"
#include <Arduino.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <WebServer.h>
#include <FS.h>
#include <base64.h>
#include "base64.h"
#include "SPIFFS.h"
// #include"settings.h"
static const char digicertRootCaCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";


static const char private_key[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCwYp7epz++0QkH
JioMD7U7BitLgpcYPi8Cid1l7snt6Kp546iQsDBJ3l8xnRtPU7ANEsjT8KxIHmyw
h/NGp94FlOKRw3ahh3yUGtowS9vdHv+S+TAfuj07NjSnKIyv5KnGZJ+fDFl4Q1tT
aQJybY1Z4itirL6/2CGEm8g/iYhLNDBsRMfpDpfXe4URyWiM3Rhf7ztqZdveb9al
3pAJZIDTLWCFQI1MvQjKamkAQkES/gZj0iUZFwbGJPBj54nkuLFLKedw7DbwgrVg
0+n3fQ9b/gQepw5PxQjyobY2DsDgGZV+MFjUmaUTa+XX68SrG4wJ+DwrkdmpHReB
vFi1Hg1hAgMBAAECggEAaTCnZkl/7qBjLexIryC/CBBJyaJ70W1kQ7NMYfniWwui
f0aRxJgOdD81rjTvkINsPp+xPRQO6oOadjzdjImYEuQTqrJTEUnntbu924eh+2D9
Mf2CAanj0mglRnscS9mmljZ0KzoGMX6Z/EhnuS40WiJTlWlH6MlQU/FDnwC6U34y
JKy6/jGryfsx+kGU/NRvKSru6JYJWt5v7sOrymHWD62IT59h3blOiP8GMtYKeQlX
49om9Mo1VTIFASY3lrxmexbY+6FG8YO+tfIe0tTAiGrkb9Pz6tYbaj9FjEWOv4Vc
+3VMBUVdGJjgqvE8fx+/+mHo4Rg69BUPfPSrpEg7sQKBgQDlL85G04VZgrNZgOx6
pTlCCl/NkfNb1OYa0BELqWINoWaWQHnm6lX8YjrUjwRpBF5s7mFhguFjUjp/NW6D
0EEg5BmO0ePJ3dLKSeOA7gMo7y7kAcD/YGToqAaGljkBI+IAWK5Su5yldrECTQKG
YnMKyQ1MWUfCYEwHtPvFvE5aPwKBgQDFBWXekpxHIvt/B41Cl/TftAzE7/f58JjV
MFo/JCh9TDcH6N5TMTRS1/iQrv5M6kJSSrHnq8pqDXOwfHLwxetpk9tr937VRzoL
CuG1Ar7c1AO6ujNnAEmUVC2DppL/ck5mRPWK/kgLwZSaNcZf8sydRgphsW1ogJin
7g0nGbFwXwKBgQCPoZY07Pr1TeP4g8OwWTu5F6dSvdU2CAbtZthH5q98u1n/cAj1
noak1Srpa3foGMTUn9CHu+5kwHPIpUPNeAZZBpq91uxa5pnkDMp3UrLIRJ2uZyr8
4PxcknEEh8DR5hsM/IbDcrCJQglM19ZtQeW3LKkY4BsIxjDf45ymH407IQKBgE/g
Ul6cPfOxQRlNLH4VMVgInSyyxWx1mODFy7DRrgCuh5kTVh+QUVBM8x9lcwAn8V9/
nQT55wR8E603pznqY/jX0xvAqZE6YVPcw4kpZcwNwL1RhEl8GliikBlRzUL3SsW3
q30AfqEViHPE3XpE66PPo6Hb1ymJCVr77iUuC3wtAoGBAIBrOGunv1qZMfqmwAY2
lxlzRgxgSiaev0lTNxDzZkmU/u3dgdTwJ5DDANqPwJc6b8SGYTp9rQ0mbgVHnhIB
jcJQBQkTfq6Z0H6OoTVi7dPs3ibQJFrtkoyvYAbyk36quBmNRjVh6rc8468bhXYr
v/t+MeGJP/0Zw8v/X2CFll96
-----END PRIVATE KEY-----

)EOF";

static const char cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDKzCCAhOgAwIBAgIUBxM3WJf2bP12kAfqhmhhjZWv0ukwDQYJKoZIhvcNAQEL
BQAwJTEjMCEGA1UEAwwaRVNQMzIgSFRUUFMgc2VydmVyIGV4YW1wbGUwHhcNMTgx
MDE3MTEzMjU3WhcNMjgxMDE0MTEzMjU3WjAlMSMwIQYDVQQDDBpFU1AzMiBIVFRQ
UyBzZXJ2ZXIgZXhhbXBsZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB
ALBint6nP77RCQcmKgwPtTsGK0uClxg+LwKJ3WXuye3oqnnjqJCwMEneXzGdG09T
sA0SyNPwrEgebLCH80an3gWU4pHDdqGHfJQa2jBL290e/5L5MB+6PTs2NKcojK/k
qcZkn58MWXhDW1NpAnJtjVniK2Ksvr/YIYSbyD+JiEs0MGxEx+kOl9d7hRHJaIzd
GF/vO2pl295v1qXekAlkgNMtYIVAjUy9CMpqaQBCQRL+BmPSJRkXBsYk8GPnieS4
sUsp53DsNvCCtWDT6fd9D1v+BB6nDk/FCPKhtjYOwOAZlX4wWNSZpRNr5dfrxKsb
jAn4PCuR2akdF4G8WLUeDWECAwEAAaNTMFEwHQYDVR0OBBYEFMnmdJKOEepXrHI/
ivM6mVqJgAX8MB8GA1UdIwQYMBaAFMnmdJKOEepXrHI/ivM6mVqJgAX8MA8GA1Ud
EwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBADiXIGEkSsN0SLSfCF1VNWO3
emBurfOcDq4EGEaxRKAU0814VEmU87btIDx80+z5Dbf+GGHCPrY7odIkxGNn0DJY
W1WcF+DOcbiWoUN6DTkAML0SMnp8aGj9ffx3x+qoggT+vGdWVVA4pgwqZT7Ybntx
bkzcNFW0sqmCv4IN1t4w6L0A87ZwsNwVpre/j6uyBw7s8YoJHDLRFT6g7qgn0tcN
ZufhNISvgWCVJQy/SZjNBHSpnIdCUSJAeTY2mkM4sGxY0Widk8LnjydxZUSxC3Nl
hb6pnMh3jRq4h0+5CZielA4/a+TdrNPv/qok67ot/XJdY3qHCCd8O2b14OVq9jo=
-----END CERTIFICATE-----
)EOF";
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

SpotifyClient::SpotifyClient(String clientId, String clientSecret, String redirectUri)
{
  this->clientId = clientId;
  this->clientSecret = clientSecret;
  this->redirectUri = redirectUri;
}

uint16_t SpotifyClient::update(SpotifyData *data, SpotifyAuth *auth)
{
  this->data = data;

  level = 0;
  isDataCall = true;
  currentParent = "";
  WiFiClientSecure client = WiFiClientSecure();
  JsonStreamingParser parser;
  parser.setListener(this);

  String host = "api.spotify.com";
  const int port = 443;
  String url = "/v1/me/player/currently-playing";
  if (!client.connect(host.c_str(), port))
  {
    Serial.println("connection failed");
    return 0;
  }

  Serial.print("Requesting URL: ");
  // Serial.println(url);
  String request = "GET " + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Authorization: Bearer " + auth->accessToken + "\r\n" +
                   "Connection: close\r\n\r\n";
  // This will send the request to the server
  Serial.println(request);
  client.print(request);

  int retryCounter = 0;
  while (!client.available())
  {
    executeCallback();
    retryCounter++;
    if (retryCounter > 10)
    {
      return 0;
    }
    delay(10);
  }
  uint16_t bufLen = 1024;
  unsigned char buf[bufLen];
  boolean isBody = false;
  char c = ' ';

  int size = 0;
  client.setNoDelay(false);
  // while(client.connected()) {
  uint16_t httpCode = 0;
  while (client.connected() || client.available())
  {
    while ((size = client.available()) > 0)
    {

      if (isBody)
      {
        uint16_t len = min(bufLen, size);
        c = client.readBytes(buf, len);
        for (uint16_t i = 0; i < len; i++)
        {
          parser.parse(buf[i]);
          // Serial.print((char)buf[i]);
        }
      }
      else
      {
        String line = client.readStringUntil('\r');
        Serial.println(line);
        if (line.startsWith("HTTP/1."))
        {
          httpCode = line.substring(9, line.indexOf(' ', 9)).toInt();
          Serial.printf("HTTP Code: %d\n", httpCode);
        }
        if (line == "\r" || line == "\n" || line == "")
        {
          Serial.println("Body starts now");
          isBody = true;
        }
      }
      executeCallback();
    }
  }
  if (httpCode == 200)
  {
    this->data->isPlayerActive = true;
  }
  else if (httpCode == 204)
  {
    this->data->isPlayerActive = false;
  }
  // client.flush();
  // client.stop();
  this->data = nullptr;
  return httpCode;
}

uint16_t SpotifyClient::playerCommand(SpotifyAuth *auth, String method, String command)
{

  level = 0;
  isDataCall = true;
  currentParent = "";
  WiFiClientSecure client = WiFiClientSecure();
  JsonStreamingParser parser;
  parser.setListener(this);

  String host = "api.spotify.com";
  const int port = 443;
  String url = "/v1/me/player/" + command;
  if (!client.connect(host.c_str(), port))
  {
    Serial.println("connection failed");
    return 0;
  }

  Serial.print("Requesting URL: ");
  // Serial.println(url);
  String request = method + " " + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Authorization: Bearer " + auth->accessToken + "\r\n" +
                   "Content-Length: 0\r\n" +
                   "Connection: close\r\n\r\n";
  // This will send the request to the server
  Serial.println(request);
  client.print(request);

  int retryCounter = 0;
  while (!client.available())
  {
    executeCallback();

    retryCounter++;
    if (retryCounter > 10)
    {
      return 0;
    }
    delay(10);
  }
  uint16_t bufLen = 1024;
  unsigned char buf[bufLen];
  boolean isBody = false;
  char c = ' ';

  int size = 0;
  client.setNoDelay(false);
  // while(client.connected()) {
  uint16_t httpCode = 0;
  while (client.connected() || client.available())
  {
    while ((size = client.available()) > 0)
    {
      if (isBody)
      {
        uint16_t len = min(bufLen, size);
        c = client.readBytes(buf, len);
        for (uint16_t i = 0; i < len; i++)
        {
          parser.parse(buf[i]);
          // Serial.print((char)buf[i]);
        }
      }
      else
      {
        String line = client.readStringUntil('\r');
        Serial.println(line);
        if (line.startsWith("HTTP/1."))
        {
          httpCode = line.substring(9, line.indexOf(' ', 9)).toInt();
          Serial.printf("HTTP Code: %d\n", httpCode);
        }
        if (line == "\r" || line == "\n" || line == "")
        {
          Serial.println("Body starts now");
          isBody = true;
        }
      }
    }
    executeCallback();
  }
  return httpCode;
}

void SpotifyClient::getToken(SpotifyAuth *auth, String grantType, String code)
{

  this->auth = auth;
  isDataCall = false;
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClientSecure client;
  // WiFiClient client;
  const char *host = "accounts.spotify.com";

  const int port = 80;
  String url = "/api/token";

  Serial.print("Requesting URL: ");
  // Serial.println(url);
  String codeParam = "code";
  if (grantType == "refresh_token")
  {
    codeParam = "refresh_token";
  }
  // + "&client_id=" + clientId + "&client_secret=" + clientSecret
  base64 base;
  String authorizationRaw = clientId + ":" + clientSecret;
  String authorization = base.encode(authorizationRaw);
  // This will send the request to the server
  String content = "grant_type=" + grantType + "&" + codeParam + "=" + code + "&redirect_uri=" + redirectUri;
  String request = String("POST ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Authorization: Basic " + authorization + "\r\n" +
                   "Content-Length: " + String(content.length()) + "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n" +
                   "Connection: close\r\n\r\n" +
                   content;
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print("request=");
  Serial.println(request);

  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
// connect(const char *host, uint16_t port, const char *CA_cert, const char *cert, const char *private_key)
 client.setCACert(digicertRootCaCert);
  if (!client.connect(host, port))
  {
    Serial.println("connection failed");
    return;
  }
  Serial.println("connectiond");
  client.print(request);
  int retryCounter = 0;
  while (!client.available())
  {
    executeCallback();
    retryCounter++;
    if (retryCounter > 10)
    {
      return;
    }
    delay(10);
  }

  int pos = 0;
  boolean isBody = false;
  char c;

  int size = 0;
  client.setNoDelay(false);
  while (client.connected() || client.available())
  {
    while ((size = client.available()) > 0)
    {
      c = client.read();
      if (c == '{' || c == '[')
      {
        isBody = true;
      }
      if (isBody)
      {
        parser.parse(c);
        Serial.print(c);
      }
      else
      {
        Serial.print(c);
      }
    }
    executeCallback();
  }
  this->data = nullptr;
}

String SpotifyClient::startConfigPortal()
{
  String oneWayCode = "";

  server.on("/", [this]()
            {
    Serial.println(this->clientId);
    Serial.println(this->redirectUri);
    server.sendHeader("Location", String("https://accounts.spotify.com/authorize/?client_id=" 
      + this->clientId 
      + "&response_type=code&redirect_uri=" 
      + this->redirectUri 
      + "&scope=user-read-private%20user-read-currently-playing%20user-read-playback-state%20user-modify-playback-state"), true);
    server.send ( 302, "text/plain", ""); });

  server.on("/callback/", [this, &oneWayCode]()
            {
    if(!server.hasArg("code")) {server.send(500, "text/plain", "BAD ARGS"); return;}
    
    oneWayCode = server.arg("code");
    Serial.printf("Code: %s\n", oneWayCode.c_str());
  
    String message = "<html><head></head><body>Succesfully authentiated This device with Spotify. Restart your device now</body></html>";
  
    server.send ( 200, "text/html", message ); });

  server.begin();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected!");
  }
  else
  {
    Serial.println("WiFi not connected!");
  }

  Serial.println("HTTP server started");

  while (oneWayCode == "")
  {
    server.handleClient();
    yield();
  }
  server.stop();
  return oneWayCode;
}

void SpotifyClient::whitespace(char c)
{
  Serial.println("whitespace");
}

void SpotifyClient::startDocument()
{
  Serial.println("start document");
  level = 0;
}

void SpotifyClient::key(String key)
{
  currentKey = String(key);
  rootPath[level] = key;
  // Serial.println(getRootPath());
}

String SpotifyClient::getRootPath()
{
  String path = "";
  for (uint8_t i = 1; i <= level; i++)
  {
    String currentLevel = rootPath[i];
    if (currentLevel == "")
    {
      break;
    }
    if (i > 1)
    {
      path += ".";
    }
    path += currentLevel;
  }
  return path;
}

void SpotifyClient::value(String value)
{
  if (isDataCall)
  {

    String rootPath = this->getRootPath();
    // Serial.printf("%s = %s\n", rootPath.c_str(), value.c_str());
    // Serial.printf("%s = %s\n", currentKey.c_str(), value.c_str());
    //  progress_ms = 37516 uint32_t progressMs;
    if (currentKey == "progress_ms")
    {
      data->progressMs = value.toInt();
    }
    // duration_ms = 259120 uint32_t durationMs;
    if (currentKey == "duration_ms")
    {
      data->durationMs = value.toInt();
    }
    // name = Lost in My MindString name;
    if (currentKey == "name")
    {
      data->title = value;
    }
    // is_playing = true boolean isPlaying;
    if (currentKey == "is_playing")
    {
      data->isPlaying = (value == "true" ? true : false);
    }
    if (currentKey == "height")
    {
      currentImageHeight = value.toInt();
    }
    if (currentKey == "url")
    {
      Serial.printf("HREF: %s = %s", rootPath.c_str(), value.c_str());

      if (rootPath == "item.album.images.url")
      {
        if (currentImageHeight == 640)
        {
          data->image640Href = value;
        }
        if (currentImageHeight > 250 && currentImageHeight < 350)
        {
          data->image300Href = value;
        }
        if (currentImageHeight == 64)
        {
          data->image64Href = value;
        }
      }
    }
    if (rootPath == "item.album.artists.name")
    {
      data->artistName = value;
    }
  }
  else
  {
    Serial.printf("\n%s=%s\n", currentKey.c_str(), value.c_str());
    // "access_token": "XXX" String accessToken;
    if (currentKey == "access_token")
    {
      auth->accessToken = value;
    }
    // "token_type":"Bearer", String tokenType;
    if (currentKey == "token_type")
    {
      auth->tokenType = value;
    }
    // "expires_in":3600, uint16_t expiresIn;
    if (currentKey == "expires_in")
    {
      auth->expiresIn = value.toInt();
    }
    // "refresh_token":"XX", String refreshToken;
    if (currentKey == "refresh_token")
    {
      auth->refreshToken = value;
    }
    // "scope":"user-modify-playback-state user-read-playback-state user-read-currently-playing user-read-private String scope;
    if (currentKey == "scope")
    {
      auth->scope = value;
    }
  }
}

void SpotifyClient::endArray()
{
}

void SpotifyClient::startObject()
{
  // Serial.println("Starting new object: " + currentKey);
  currentParent = currentKey;
  // rootPath[level] = currentKey;
  level++;

  // level++;*/
}

void SpotifyClient::endObject()
{
  // rootPath[level] = "";
  level--;
  currentParent = "";
}

void SpotifyClient::endDocument()
{
}

void SpotifyClient::startArray()
{
}

void SpotifyClient::executeCallback()
{
  if (drawingCallback != nullptr)
  {
    (*this->drawingCallback)();
  }
}

void SpotifyClient::downloadFile(String url, String filename)
{
  Serial.println("Downloading " + url + " and saving as " + filename);

  // wait for WiFi connection
  // TODO - decide if there's a different action for first call or subsequent calls
  // boolean isFirstCall = true;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");

  // configure server and url
  http.begin(url);

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    // SPIFFS.remove(filename);
    fs::File f = SPIFFS.open(filename, "w+");
    if (!f)
    {
      Serial.println("file open failed");
      return;
    }
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {

      // get lenght of document (is -1 when Server sends no Content-Length header)
      int total = http.getSize();
      int len = total;
      // progressCallback(filename, 0,total, true);
      //  create buffer for read
      uint8_t buff[128] = {0};

      // get tcp stream
      WiFiClient *stream = http.getStreamPtr();

      // read all data from server
      while (http.connected() && (len > 0 || len == -1))
      {
        // get available data size
        size_t size = stream->available();

        if (size)
        {
          // read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

          // write it to Serial
          f.write(buff, c);

          if (len > 0)
          {
            len -= c;
          }
          // progressCallback(filename, total - len,total, false);
          //  isFirstCall = false;
          executeCallback();
        }
        delay(1);
      }

      Serial.println();
      Serial.print("[HTTP] connection closed or file end.\n");
    }
    f.close();
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
