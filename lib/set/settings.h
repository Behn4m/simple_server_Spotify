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
#include<Arduino.h>
#include <TZ.h>
// WiFi settings
const char* ssid = "Hardware12";
const char* password = "87654321";

// Spotify settings
String clientId = "2c2dadbd46244f2cb9f71251bc004caa";
String clientSecret = "d80bd7e8c1b04bfcacdb5ecf6a155215";
// These next two lines have to match. A client clientSpotifierHostName makes the
// node listen on http://ESP32.local. Spotify needs an exact match of the redirect url
String espotifierNodeName = "deskhub";
// String redirectUri = "http%3A%2F%2F" + espotifierNodeName + ".local%2Fcallback%2F";
String redirectUri ="http%3A%2F%2Fdeskhub.local%2Fcallback%2f";
#define TIMEZONE TZ_Europe_Zurich




// UI Settings
#define SHOW_TOUCH_POINT_MILLIS 1500

// Hardware settings
#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8

#define TOUCH_CS D3
#define TOUCH_IRQ  D4

// Display settings
#define MINI_BLACK 0
#define MINI_WHITE 1
#define MINI_YELLOW 2
#define MINI_GRAY 3

