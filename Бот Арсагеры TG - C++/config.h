#pragma once
#define CURL_STATICLIB
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using std::string;
using std::cout;
using std::endl;

class requests;

string chat_id = "-1002584000983"; // -1002351013699
string bot_token = "8189937701:AAEs0Zgi97TtPb1XO60t6yx6T9DxBpnZY24"; // в GITHUB скрыт 


string URL_Arsagera = "https://arsagera.ru/api/v1/funds/fa/fund-metrics/";

string URL_ClockWorld = "http://worldclockapi.com/api/json/utc/now";
string URL_WorldTime = "https://worldtimeapi.org/api/timezone/UTC/";

string URL_TelegramBOT = "https://api.telegram.org/bot";
