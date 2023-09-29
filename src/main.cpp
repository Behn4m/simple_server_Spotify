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

#include <mDNS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// #include <FS.h>
#include<SPIFFS.h>
#include "SpotifyClient.h"
#include "settings.h"

#include"ESPmDNS.h"
// Statements like:
// #pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
// C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles. Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution. You can double
// click on these messages and jump to the line in question.
//
// see https://stackoverflow.com/questions/5966594/how-can-i-use-pragma-message-so-that-the-message-points-to-the-filelineno
//
#define Stringize(L) #L
#define MakeString(M, L) M(L)
#define $Line MakeString(Stringize, __LINE__)
#define Reminder __FILE__ "(" $Line ") : Reminder: "
#ifdef LOAD_SD_LIBRARY
#pragma message(Reminder "Comment out the line with LOAD_SD_LIBRARY /JPEGDecoder/src/User_config.h !")
#endif

const char *host = "api.spotify.com";
const int httpsPort = 443;

int BUFFER_WIDTH = 240;
int BUFFER_HEIGHT = 160;
// Limited to 4 colors due to memory constraints
int BITS_PER_PIXEL = 2; // 2^2 =  4 colors

SpotifyClient client(clientId, clientSecret, redirectUri);
SpotifyData data;
SpotifyAuth auth;

String currentImageUrl = "";
uint32_t lastDrawingUpdate = 0;
uint16_t counter = 0;
long lastUpdate = 0;
uint32_t lastTouchMillis = 0;
boolean isDownloadingCover = false;


String formatTime(uint32_t time);
void saveRefreshToken(String refreshToken);
String loadRefreshToken();


void setup()
{
    Serial.begin(115200);
   
    boolean mounted = SPIFFS.begin();

    if (!mounted)
    {
        Serial.println("FS not formatted. Doing that now");
        SPIFFS.format();
        Serial.println("FS formatted...");
        SPIFFS.begin();
    }


    Serial.println();
    Serial.print("connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    if (!MDNS.begin("ESP32"))
    { // Start the mDNS responder for ESP32.local
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");

    String code = "";
    String grantType = "";
    String refreshToken = loadRefreshToken();
    if (refreshToken == "")
    {
        Serial.println("No refresh token found. Requesting through browser");

        Serial.println("Open browser at http://" + espotifierNodeName + ".local");

        code = client.startConfigPortal();
        grantType = "authorization_code";
    }
    else
    {
        Serial.println("Using refresh token found on the FS");
        code = refreshToken;
        grantType = "refresh_token";
    }
    client.getToken(&auth, grantType, code);
    Serial.printf("Refresh token: %s\nAccess Token: %s\n", auth.refreshToken.c_str(), auth.accessToken.c_str());
    if (auth.refreshToken != "")
    {
        saveRefreshToken(auth.refreshToken);
    }
    // client.setDrawingCallback(&drawSongInfoCallback);
}

void loop()
{

    // if (millis() - lastUpdate > 1000)
    // {
    //     uint16_t responseCode = client.update(&data, &auth);
    //     Serial.printf("HREF: %s\n", data.image300Href.c_str());
    //     lastUpdate = millis();
    //     Serial.printf("--------Response Code: %d\n", responseCode);
    //     Serial.printf("--------Free mem: %d\n", ESP.getFreeHeap());
    //     if (responseCode == 401)
    //     {
    //         client.getToken(&auth, "refresh_token", auth.refreshToken);
    //         if (auth.refreshToken != "")
    //         {
    //             saveRefreshToken(auth.refreshToken);
    //         }
    //     }
    //     if (responseCode == 200)
    //     {

    //         String selectedImageHref = data.image300Href;
    //         selectedImageHref.replace("https", "http");

    //         if (selectedImageHref != currentImageUrl)
    //         {

    //             Serial.println("New Image. Downloading it");

    //             isDownloadingCover = true;
    //             client.downloadFile(selectedImageHref, "/cover.jpg");
    //             isDownloadingCover = false;
    //             currentImageUrl = selectedImageHref;
                
    //         }
    //     }
    //     if (responseCode == 400)
    //     {
        
    //     }
    // }
   

    // if (touchController.isTouched(500) && millis() - lastTouchMillis > 1000)
    // {
        
    //     //lastTouchPoint = p;
    //     lastTouchMillis = millis();
    //     String command = "";
    //     String method = "";
    //     if (p.y > 160)
    //     {
    //         if (p.x < 80)
    //         {
    //             method = "POST";
    //             command = "previous";
    //         }
    //         else if (p.x >= 80 && p.x <= 160)
    //         {
    //             method = "PUT";
    //             command = "play";
    //             if (data.isPlaying)
    //             {
    //                 command = "pause";
    //             }
    //             data.isPlaying = !data.isPlaying;
    //         }
    //         else if (p.x > 160)
    //         {
    //             method = "POST";
    //             command = "next";
    //         }
    //         uint16_t responseCode = client.playerCommand(&auth, method, command);
    //         Serial.print("playerCommand response =");
    //         Serial.println(responseCode);
    //     }
    // }
}





String formatTime(uint32_t time)
{
    char time_str[10];
    uint8_t minutes = time / (1000 * 60);
    uint8_t seconds = (time / 1000) % 60;
    sprintf(time_str, "%2d:%02d", minutes, seconds);
    return String(time_str);
}

void saveRefreshToken(String refreshToken)
{

    File f = SPIFFS.open("/refreshToken.txt", "w+");
    if (!f)
    {
        Serial.println("Failed to open config file");
        return;
    }
    f.println(refreshToken);
    f.close();
}

String loadRefreshToken()
{
    Serial.println("Loading config");
    File f = SPIFFS.open("/refreshToken.txt", "r");
    if (!f)
    {
        Serial.println("Failed to open config file");
        return "";
    }
    while (f.available())
    {
        // Lets read line by line from the file
        String token = f.readStringUntil('\r');
        Serial.printf("Refresh Token: %s\n", token.c_str());
        f.close();
        return token;
    }
    return "";
}