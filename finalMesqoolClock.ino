
#include <WiFi.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include "esp_wps.h"
#include "time.h"
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#define EEPROM_SIZE 512
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFiClientSecure.h>


#include <Arduino.h>
  uint8_t block1[] = {0x8C, 0x20, 0x4C, 0x2C, 0xCC, 0x00, 0x2C, 0x82, 0xAC, 0x00};
  uint8_t block2[] = {0x04, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF};
uint8_t displayBytes[14];
uint8_t block3[] = {
  0x04, 0x01, // header
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // b[2] to b[7]
  0x00, 0x00, 0x00, 0x00, 0x00,       // b[8] to b[12]
  0x00                                // b[13]
};

// Segment definitions based on your layout
struct SegmentBit {
  const  char* label;
  uint8_t byteIndex;
  uint8_t bitIndex;  // MSB = 7
};

 const SegmentBit segmentMap[] = {
  // Decimal Hour
  {"dha", 4, 7}, {"dhb", 2, 7}, {"dhc", 2, 4}, {"dhd", 2, 6}, {"dhf", 3, 7},
  // Unit Hour
  {"uha", 6, 4}, {"uhb", 7, 4}, {"uhc", 7, 6}, {"uhd", 6, 5}, {"uhe", 5, 6},
  {"uhf", 5, 4}, {"uhg", 6, 6},
  // Decimal Minute
  {"dma", 8, 7}, {"dmb", 8, 4}, {"dmc", 8, 6},
  {"dmd", 11, 5}, {"dme", 12, 6}, {"dmf", 11, 4}, {"dmg", 11, 6},
  // Unit Minute
  {"uma", 9, 7}, {"umb", 9, 4}, {"umg", 9, 6},{"umc", 4, 6},
  {"umd", 10, 5}, {"ume", 10, 6}, {"umf", 10, 4}
};

// Segment patterns for each digit
const char* digitSegments[] = {
  "abcfed", "bc", "abged", "abgcd", "fgbc",
  "afgcd", "afgcde", "abc", "abcdefg", "abcfgd"
};

WiFiClientSecure client;
AsyncWebServer server(80);
///                 send this from the browser 
///// http://192.168.0.100/number?value=42345 or any other value 4234 but <255
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

int fetchedNumber=200;
int timezoneOffsetHoursSaved=0;
int number=188;
int timezoneOffsetHours = 0;  // Time offset in seconds from GMTin
int f=0;
#define MAX_WIFI_ATTEMPTS 10  // Max retries before erasing credentials
const char* fileURL = "https://script.googleusercontent.com/macros/echo?user_content_key=AehSKLgNocE3bW_dciyWfEqXxgQLhq5064NSnflyk8T8QhTtG085EG0ZsfNIpN_UvBrdbcr0fuFRMqI6jXSmiA1LRnT1LXE_JXTIoc52N82awQK9k9RPMTYnbTYO4XJbGPRK0UC5h9aX7z3nqAhrHksXNQOqYYHGbkyxU-l1Ih8dUTX9SxFvfcUQ3R1-VniG9cHq94t-M41njqdi6eAj6Qtwh8EmyF9Fzc19bmMcbv1Ysc61Y8-dxhjDbsN8KX68pfktt2FLtl8vhuMm1pA7PgcZ7uagpS0Mew&lib=MQpkFdC_jwFQ9qeazNSsrTLTFd0BYWbni";

// takes PWm from google account

const char* ipApiUrl = "http://ip-api.com/json";  // IP-based geolocation service

#include "esp_task_wdt.h"
TaskHandle_t WiFiTaskHandle;

#define WATCHDOG_TIMEOUT_SEC 10  // Timeout in seconds
#define ESP_WPS_MODE WPS_TYPE_PBC
const char* ntpServer = "pool.ntp.org";

#define PIN_DATA 2
#define PIN_CLK 3
#define PIN_CS 4


int LED =1;//
int LED1=8;// int LED2=8;
int n=0;
int e=165; ///pwm output led pins
unsigned long bitDuration = 490;       // Bit duration (adjustable)
unsigned long phaseShift = 440;        // Phase shift (adjustable)
const unsigned long sendInterval = 2000; // Transmit interval
int d=0;

  unsigned long previousMillisWiFi = millis();
  const unsigned long intervalWiFi = 5000;
  int attempts = 0;


int s=0;
uint8_t value=0;

int g=0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////


int dh; int uh; int dm; int um;
int hoursOriginal;
///////////////////////////////////////////////////////////////////////////


// int fetchedNumber=200;
//const char* ssid = "AEBC Internet";
//const char* password = "nastasecat";
// const char* fileURL = "https://script.googleusercontent.com/macros/echo?user_content_key=AehSKLgNocE3bW_dciyWfEqXxgQLhq5064NSnflyk8T8QhTtG085EG0ZsfNIpN_UvBrdbcr0fuFRMqI6jXSmiA1LRnT1LXE_JXTIoc52N82awQK9k9RPMTYnbTYO4XJbGPRK0UC5h9aX7z3nqAhrHksXNQOqYYHGbkyxU-l1Ih8dUTX9SxFvfcUQ3R1-VniG9cHq94t-M41njqdi6eAj6Qtwh8EmyF9Fzc19bmMcbv1Ysc61Y8-dxhjDbsN8KX68pfktt2FLtl8vhuMm1pA7PgcZ7uagpS0Mew&lib=MQpkFdC_jwFQ9qeazNSsrTLTFd0BYWbni";


#include "esp_task_wdt.h"
// TaskHandle_t WiFiTaskHandle;

#define WATCHDOG_TIMEOUT_SEC 10  // Timeout in seconds
#define ESP_WPS_MODE WPS_TYPE_PBC

bool transmissionInProgress = false;
bool r=false;
int minute=3;
int  hour=3;
int t=0;int k=0;
uint32_t preservedBits;
Preferences preferences;
String wifiSSID = "";
String wifiPASS = "";
bool wpsSuccess = false;
int timezoneOffset = 0;
// WPS Configuration
#define ESP_WPS_MODE WPS_TYPE_PBC
int DST_Offset;

unsigned long lastSyncTime = 0;
unsigned long syncInterval = 1 * 24 * 60 * 60;  // 10 days in seconds/ 1 day


//////////////////////////////////////////////////////////////////////////
//                            wifi


// // Function to get current epoch timeO
// unsigned long getTime() {
//   time_t now;
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     return 0;
//   }
//   time(&now);
//   return now;
// }

void setSegment(const char* label) {
  for (const  auto& entry : segmentMap) {
    if (strcmp(entry.label, label) == 0) {
      displayBytes[entry.byteIndex] |= (1 << entry.bitIndex);
      return;
    }
  }
}

void showTime(uint8_t dh, uint8_t uh, uint8_t dm, uint8_t um) {
  memset(displayBytes, 0, sizeof(displayBytes));
  displayBytes[0] = 0x40;
  displayBytes[1] = 0x01;

  struct Digit {
    const  char* prefix;
    uint8_t value;
  } digits[] = {
    {"dh", dh}, {"uh", uh}, {"dm", dm}, {"um", um}
  };

  for (const  auto& digit : digits) {
    const  char* pattern = digitSegments[digit.value];
    for (size_t i = 0; pattern[i]; ++i) {
      char label[5];
      snprintf(label, sizeof(label), "%s%c", digit.prefix, pattern[i]);
      setSegment(label);
    }
  }
}



void sendByte(uint8_t value) {
  for (int i = 7; i >= 0; --i) {
    digitalWrite(PIN_DATA, (value & (1 << i)) ? HIGH : LOW);
     digitalWrite(PIN_CLK, LOW);
  delayMicroseconds(14);
  digitalWrite(PIN_CLK, HIGH);
  delayMicroseconds(14);
  }
}

void sendSequence(const  uint8_t* data, size_t length) {
  digitalWrite(PIN_CS, LOW);
  for (size_t i = 0; i < length; ++i) {
    sendByte(data[i]);
  }
  digitalWrite(PIN_CS, HIGH);
}

void clearDecimalHourSegments() {
  // List of all segment labels tied to the decimal hour (dh)
  const char* dhSegments[] = { "dha", "dhb", "dhc", "dhd", "dhf" };

  for (const auto& label : dhSegments) {
    for (const auto& entry : segmentMap) {
      if (strcmp(entry.label, label) == 0) {
        displayBytes[entry.byteIndex] &= ~(1 << entry.bitIndex);  // Clear the bit
        break;
      }
    }
  }
}



void wpsStart() {  // different from finall
  esp_wps_config_t config;
  memset(&config, 0, sizeof(esp_wps_config_t));
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, "ESPRESSIF");
  strcpy(config.factory_info.model_number, CONFIG_IDF_TARGET);
  strcpy(config.factory_info.model_name, "ESPRESSIF IOT");
  strcpy(config.factory_info.device_name, "ESP DEVICE");
  //strcpy(config.pin, "00000000");
  
  if (esp_wifi_wps_enable(&config) == ESP_OK) {
    Serial.println("WPS Enabled");
    esp_wifi_wps_start(0);
  } else {
    Serial.println("WPS Failed");
  }
}

void wpsStop() {
        Serial.println("Debug Stop function wpsStop ");

  esp_err_t err = esp_wifi_wps_disable();
  if (err != ESP_OK) {
    Serial.printf("WPS Disable Failed: 0x%x: %s\n", err, esp_err_to_name(err));
  }
}
// Function to retrieve stored credentials
bool loadCredentials() {
          Serial.println("Debug loadCredentials  ");
  preferences.begin("wifi", true);
  wifiSSID = preferences.getString("ssid", "");
  wifiPASS = preferences.getString("pass", "");
  preferences.end();
  return (wifiSSID.length() > 0 && wifiPASS.length() > 0);
}

// Handle Wi-Fi events
void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("Connected to: " + String(WiFi.SSID()));
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());

      // Store Wi-Fi credentials in NVS
      preferences.begin("wifi", false);
      preferences.putString("ssid", WiFi.SSID());
      preferences.putString("pass", WiFi.psk());
      preferences.end();

      Serial.println("Wi-Fi credentials saved!");

      // Sync time **only if 10 days have passed**
      if (getTime() - lastSyncTime >= syncInterval) {  // HERE 1 DAY TIME CHECK
        configTime(0, 0, ntpServer);
        Serial.println("   Time synchronization started.");
        lastSyncTime = getTime();

      }

      getGeolocation();  // Run geolocation query
            wpsStop(); // Stop WPS after connection

      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected, trying to reconnect...");

// do some blinking if no network
   if(d % 2==0)
     e=140;
    if(d % 2==1)
    e=180;
    d++;

  //e=85; // pwm led power
analogWrite(LED,e); //controls the led intensity
 analogWrite(LED1,e); //controls the led intensity
// analogWrite(LED2,e); //controls the led intensity





      WiFi.reconnect();      

      break;

    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      Serial.println("WPS Successful! Connecting to: " + String(WiFi.SSID()));
      esp_wifi_wps_disable();delay(10);
      WiFi.begin();
      break;



    default:
      break;
  }
}




unsigned long getTime() {
      Serial.println("Debug getTime");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return 0;
  }
  return timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60;
}




// Function to retrieve geolocation via longitude and determine GMT offset
void getGeolocation() {   
  Serial.println("Fetching IP-based geolocation...");

  HTTPClient http;
  http.begin(ipApiUrl);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String response = http.getString();
    StaticJsonDocument<512> responseDoc;
    deserializeJson(responseDoc, response);

    float latitude = responseDoc["lat"];
    float longitude = responseDoc["lon"];
    String country = responseDoc["country"];
    String city = responseDoc["city"];

    Serial.print("Country: ");
    Serial.print(country);
    Serial.print("  City: ");
    Serial.print(city);
    Serial.print("  Latitude: ");
    Serial.print(latitude);
    Serial.print("  Longitude: ");
    Serial.println(longitude);

    // I f GMT offset isn't provided, calculate it based on the city
    timezoneOffsetHours = round(longitude / 15) ;// * 3600; //getGMTOffset(city);
    Serial.print("Calculated Timezone Offset (GMT) in hours: ");
    Serial.print(timezoneOffsetHours);
  } else {
    Serial.print("Geolocation failed, HTTP Response Code: ");
    Serial.println(httpResponseCode);
    Serial.print(" n= ");  Serial.println(n);
    if(n>7)
    ESP.restart();
    n++;

  }
  http.end();
}

// activate DST , summer interval
bool isDSTActive(int month, int day) {

    //  if((day == 1 || day == 10 || day == 28) && minute < 5 && hour == 1) {  /// updtae server time every ten days 
    //   configTime(0, 0, ntpServer);
    //   Serial.println("   Time synchronization started.");}

  // Example: DST starts on the 2nd Sunday of March and ends on the 1st Sunday of November (U.S. rule)
  if ((month > 3 && month < 11) ||  // DST months
      (month == 3 && day >= 8) ||   // DST starts (2nd Sunday in March)
      (month == 11 && day < 8)) {   // DST ends (1st Sunday in November)
    return true;  // DST is active
  }
  return false;  // Standard time
}
// update time with DST in the summer
void updateTimeForDST() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    if (isDSTActive(timeinfo.tm_mon + 1, timeinfo.tm_mday)) {
      timezoneOffsetHours += 1;  // Add one hour for DST
      DST_Offset=1;
    } else {
      timezoneOffsetHours -= 1;  // Revert back to standard time
      DST_Offset=-1;
    }
  }
  Serial.print ("  Month= "); Serial.print(timeinfo.tm_mon + 1); Serial.print (" Day= "); Serial.println( timeinfo.tm_mday );
}
// // save the offset time  timezoneOffsetHours
// void saveTimeOffset( int bb)  {
//       EEPROM.put(300, bb);  // Store at address 0
//       EEPROM.commit();  // Ensure data is written
//     //preferences.putString("TimeOffset", bb);
// }

//  // Read from address 0 AN INTEGER, read the offset time for time zone timezoneOffsetHours
// void loadTimeOffset( void) { 
//    EEPROM.get(300, timezoneOffsetHours);
//     }





void setup() 
{ 
  Serial.begin(115200);
 delay(10);
  Serial.println();
    // EEPROM.begin(EEPROM_SIZE);
    // EEPROM.write(0, 0);  // Reset value at address 0
    // EEPROM.commit();
    // Serial.println("EEPROM value reset.");


  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_CS, OUTPUT);

  digitalWrite(PIN_DATA, HIGH);
  digitalWrite(PIN_CLK, HIGH);
  digitalWrite(PIN_CS, HIGH);

  // Wake up / init
  digitalWrite(PIN_CS, LOW);
  sendByte(0x80);
  digitalWrite(PIN_CS, HIGH);
  delayMicroseconds(20);

  sendSequence(block1, sizeof(block1));
  delay(7);


  sendSequence(block2, sizeof(block2));
  delay(1000);
  pinMode(LED, OUTPUT);   //
  pinMode(LED1, OUTPUT);  //pinMode(LED2, OUTPUT); 




delay(1000);  // Short delay before reconnecting




delay(10);
  Serial.println();

  // Load stored credentials
  preferences.begin("wifi", false);
  String storedSSID = preferences.getString("ssid", "");
  String storedPASS = preferences.getString("pass", "");
    // lastSyncTime = preferences.getULong("lastSync", 0);
  preferences.end();



  WiFi.begin();
// int intervalWiFi=5
   while (WiFi.status() != WL_CONNECTED) {
    if (previousMillisWiFi < intervalWiFi)
    {
      previousMillisWiFi = millis();
      Serial.print(".");


      delay(500);
    }
    else break;

  }
//  while (WiFi.status() != WL_CONNECTED && attempts < MAX_WIFI_ATTEMPTS) {
//     Serial.print(".");
//     delay(500);
//     attempts++;
//   }

  WiFi.onEvent(WiFiEvent);  
  WiFi.mode(WIFI_MODE_STA);



// WiFi.mode(WIFI_MODE_STA);
//   WiFi.onEvent(WiFiEvent);  
  

  Serial.println(wifiSSID);
  Serial.println(wifiPASS);





  if (storedSSID != "" && storedPASS != "") {
    Serial.println("Using stored credentials...");
    WiFi.begin(storedSSID.c_str(), storedPASS.c_str());


  } else {
    Serial.println("Starting WPS WPS WPS WPS WPS WPS WPS WPS WPS WPS WPS WPS ...");
    wpsStart();
  }



Serial.println( wifiSSID );
Serial.println( wifiPASS );
///                 send this from the browser 
///// http://192.168.0.100/number?value=42345 or any other value 4234 but <255
//  IPAddress gateway = WiFi.gatewayIP(); // Get gateway IP
//   Serial.print("Gateway IP: ");
//   Serial.println(gateway);

//   // Dynamically assign local IP based on the gateway's third octet
//   IPAddress local_IP(192, 168, gateway[2], 200);

//   IPAddress subnet(255, 255, 255, 0);
//   if (!WiFi.config(local_IP, gateway, subnet)) {
//     Serial.println("Failed to configure static IP!");
//   }

//   Serial.print("Assigned IP: ");
//   Serial.println(WiFi.localIP());
 // Define an endpoint to receive numbers
  // Print current IP address to Serial Monitor


server.on("/number", HTTP_GET, [](AsyncWebServerRequest *request){
  if (request->hasParam("value")) {
    String value = request->getParam("value")->value();
    number = value.toInt();  // Assign the received number
    Serial.println("Received number: " + String(number));
  }
  request->send(200, "text/plain", "Number received!");
});

  // Start server
  server.begin();
   Serial.print("Current IP: ");
  Serial.println(WiFi.localIP());


//  configTime(timezoneOffset, 0, ntpServer);  // 
  // configTime(timezoneOffsetHours, 0, ntpServer);
  // timeClient.begin();
  // timeClient.setTimeOffset(0); // Adjust for your timezone (PST: UTC-8)
  
  configTime(0, 0, ntpServer);
// configTime(timezoneOffsetHours, 0, "pool.ntp.org", "time.nist.gov");


}



//Loops "Abcd"
void loop() 
{     
  ////////////////////////////////////////////////////  wifi
 ///                 send this from the browser 
///// http://192.168.0.100/number?value=42345 or any other value 4234 but <255

k++;
if(k==60*24*10) { /// syncronize every ten days with the server
    configTime(0, 0, ntpServer);
     timeClient.update();
    k=0; 
}



f=0;
  // Print current IP address to Serial Monitor
  Serial.print("Current IP: ");
  Serial.println(WiFi.localIP());

// if (f==0) // run once LED Server
// {  RunServerLED() ; f=1; }

Serial.println(  WiFi.SSID() );
Serial.println(  WiFi.psk() );

if(timezoneOffsetHoursSaved == 0)  {
   getGeolocation() ;  // get lat si longitude,
   // returns 
   // returns 
   // returns timezoneOffsetHours , for vancouver its -8

}



int time_delay;  // delay to be used in general delay, 1 sec for 7 times when we update MOnth and Day, and 55 sec the rest of it
     
     if(t<7) {   // update DST few times ever day at 2am SO I INQUIRE THE MONTH AND DAY EVERY 1 DAY
      updateTimeForDST() ; // adjust hours and minutes for DST 
      Serial.print(" Time timezoneOffsetHours adjusted ONCE t= ");Serial.println(t);
      time_delay=3000;
         }    
         
    if(t>6)  // do not acces the update DST function to offset timezoneOffsetHours ;; updateTimeForDST() , to keep the network not busy 
      {  timezoneOffsetHours = timezoneOffsetHours + DST_Offset; // adjust according to DST, DST_Offset is set in the updateTimeForDST() function
         time_delay=55999;
      }
t++;

      if(hour == 2 && minute < 2)  // adjust DST every day at 2 am
        t=0;


    // if (timezoneOffset==0)
    //    getTimezoneFromIP();

     Serial.print(" Time timezoneOffsetHours: ");
     Serial.println(timezoneOffsetHours); // Serial.print("     Epoch Time: ");  Serial.println(epochTime);


  // Extract hour and minute as uint8_t
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    hour = (timeinfo.tm_hour+timezoneOffsetHours);//timezoneOffset) % 24;
    minute = timeinfo.tm_min;

        if (hour <0)
          hour+=24;


    Serial.printf("  Current Time:  %02d:%02d ", hour, minute);
  } else {
    Serial.print("    Failed to obtain time, n=");  Serial.println(n);
    if(n>9)
    ESP.restart();
    n++;
     goto rrr; // if dont get te local time , dont wait delay (60000)
  }




   // number comes from the server , in the setup()
   e=number ;//fetchedNumber;
   Serial.print(" PWM for led e= ")  ;  Serial.println(e);



  rrr:
  //e=85; // pwm led power
analogWrite(LED,e); //controls the led intensity
 analogWrite(LED1,e); //controls the led intensity
// analogWrite(LED2,e); //controls the led intensity
d=0;


    if (k < 8 && g==0) {       hour = 22; // trick the LCD to display 0 in case zero is cleared from LCD
 }

  if(hour ==0)
     hour = 12;


  int hours=hour; //plug my hours and minutes from wifi to lcd display
  int minutes=minute;

  hoursOriginal=hour;



   if(hour > 12)
      hours=hours-12;

 Serial.print(hour); Serial.print( ":"); Serial.print(minute);Serial.println( " hour minute ");
   Serial.print(hours); Serial.print( ":"); Serial.print(minutes);Serial.println( " hours minutes ");

    dh=hours/10;
    uh=hours%10;
    dm=minutes/10;
    um=minutes%10;



// displa the ip numbers to adjust later by phone
IPAddress localIP = WiFi.localIP(); // Get assigned IP address

    if (k < 8 && g==0) {  // displa IP only at startup when g=0
      Serial.print (" display te IP now display te IP now display te IP now display te IP now K= "), Serial.println (k); 
      dh = localIP[2];
      uh = localIP[3]/100;;
      dm = (localIP[3]-100*(localIP[3]/100)) / 10;
      um = (localIP[3]-10*(localIP[3]/10)) ; 
      if(k == 7) // when k == 7 do not display any more IP on LCD
       g=1;
    }


    Serial.print(dh ); Serial.print( "   ");  Serial.print(uh );Serial.print( "   ");
    Serial.print(dm ); Serial.print( "   ");;  Serial.println(um );
    Serial.print( "dh  "); Serial.print( "uh  ");
    Serial.print( "dm  ");Serial.println( "um  ");
    // delay(88888);

   showTime(dh, uh, dm, um); // Example: 12:34
  // for (int i = 2; i <= 13; ++i)
  //   block3[i] = displayBytes[i];
if(dh == 0)
clearDecimalHourSegments();

displayBytes[7] |= (1 << 5);  // Set bit 5 of byte 7 , char ":" doua puncte
// displayBytes[6] |= (1 << 7); // Set bit 7 of byte 10 


  sendSequence(displayBytes, sizeof(displayBytes));
   
  delay(time_delay);
//goto jjj;

    

  
}



//////////////////////////////////////////////

