#define MQTT_LOG_ENABLED 1
#include "RootCAs.h"
#include "Secrets.h"
#include <Arduino.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include <DallasTemperature.h>
#include <MQTT.h>
#include <OneWire.h>
#include <PID_v1.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <esp_wpa2.h>
#include <heltec.h>

// Use this on TCDwifi
#define TIMESERVER_HACK

// waterproof temp
#define Temp1_Pin 2
#define Temp2_Pin 12
OneWire oneWire(Temp1_Pin);
OneWire twoWire(Temp2_Pin);

DallasTemperature sensor1(&oneWire);
DallasTemperature sensor2(&twoWire);
float Celcius = 0;
float Fahrenheit = 0;

const int tempPin = 0;
const int alcAPin = 15;
const int alcDPin = 22;

// 4096 bytes
double RawValue = 0;
double Voltage = 0;
double tempC = 0;
double tempF = 0;

// PID init
double set_temp = 20;
double Output = 0;

// Figure out values
/*
double Kp=9.1, Ki=0.3, Kd=1.8;
PID myPID(&tempC, &Output, &set_temp, Kp, Ki, Kd, DIRECT);
*/

WiFiClientSecure *wifiClient;
MQTTClient *mqttClient;
CloudIoTCoreDevice *device;
CloudIoTCoreMqtt *mqtt;

const char *deviceId = "esp32";

void setup() {
    // pinMode(tempPin, INPUT);
    pinMode(alcAPin, INPUT);
    pinMode(alcDPin, INPUT);
	pinMode(Temp1_Pin, INPUT);
	pinMode(Temp2_Pin, INPUT);

    Serial.begin(9600);

    // myPID.SetMode(AUTOMATIC);

    Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/,
                 true /*Serial Enable*/);
    Heltec.display->flipScreenVertically();
    Heltec.display->setFont(ArialMT_Plain_10);

    /* Serial.setDebugOutput(true); */
    /* esp_log_level_set("*", ESP_LOG_VERBOSE); */

    WiFi.mode(WIFI_STA);

    // To connect to TCDwifi
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY,
                                       strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY,
                                       strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD,
                                       strlen(EAP_PASSWORD));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
    esp_wifi_sta_wpa2_ent_enable(&config);

    WiFi.begin("TCDwifi");

    // To connect to hotspot
    /* WiFi.begin("Luke’s iPhone", "password"); */
}

bool connected = false, timeSetup = false, cloudSetup = false;

String spinner() {
    unsigned long d = millis() % 1000;
    const char syms[8] = {'|', '/', '-', '\\', '|', '/', '-', '\\'};
    int i = (int)(d * 8.f / 1000.f) % 8;
    return String(syms[i]);
}

void displayWiFiStatus() {
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    String s;
    connected = WiFi.status() == WL_CONNECTED;

    switch (WiFi.status()) {
    case WL_CONNECTED:
        s = String("Connected @ ") + WiFi.localIP().toString();

        Heltec.display->drawString(0, 0, s);

        break;
    case WL_NO_SSID_AVAIL:
        Heltec.display->drawString(0, 0, "No SSIDs found");
        break;
    case WL_CONNECTION_LOST:
        Heltec.display->drawString(0, 0, "Connection lost");
        break;
    case WL_CONNECT_FAILED:
        Heltec.display->drawString(0, 0, "Failed to connect");
        break;
    case WL_IDLE_STATUS:
    default:
        s = String("Connecting to ") + WiFi.SSID() + " " + String(spinner());
        Heltec.display->drawString(0, 0, s);
        break;
    }
}

String lastMsgTopic = "No messages yet", lastMsgPayload = "";
void messageReceived(String &topic, String &payload) {
    lastMsgTopic = topic;
    lastMsgPayload = payload;
    Serial.println("Message received: " + topic + " - " + payload);
}

// Makes a http(s) request
String http(String host, String url) {
    Serial.println(String("Connecting to ") + host + url);
    if (wifiClient->connect(host.c_str(), 443)) {
        wifiClient->print(String("GET ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" + "User-Agent: ESP32\r\n" +
                          "Connection: close\r\n\r\n");
        size_t length = -1;
        while (wifiClient->connected()) {
            String header = wifiClient->readStringUntil('\n');
            header.toLowerCase();
            if (header.startsWith("content-length: ")) {
                length = header.substring(header.indexOf(' ') + 1).toInt();
            }
            if (header == "\r")
                break;
        }
        if (length == -1) {
            Serial.println("Couldn't read content-length");
            return "";
        }
        char data[length];
        wifiClient->readBytes(data, length);
        return String(data);
    } else {
        char errStr[128];
        wifiClient->lastError(errStr, 128);
        Serial.println(String("Connection unsuccessful: ") + errStr);
        return "";
    }
}

// Needed for Google Cloud IoT Core
// FIXME: the connection drops every other time the token is refreshed
// mqtt: SERVER: The connection was closed because there is another active
// connection with the same device ID.
String getJwt() {
    unsigned long iat = time(nullptr);
    Serial.println(String("Refreshing JWT @ ") + iat);
    return device->createJWT(iat, 3000);
}

float ambientTemp, beerTemp;

// reading 2 liquid temp sensors
void watertemp() {
    sensor1.requestTemperatures();
    ambientTemp = sensor1.getTempCByIndex(0);
    Fahrenheit = sensor1.toFahrenheit(ambientTemp);
    Serial.print("Pin2 Sensor 1 C  ");
    Serial.print(ambientTemp);
    Serial.print(" F  ");
    Serial.println(Fahrenheit);

    sensor2.requestTemperatures();
    beerTemp = sensor2.getTempCByIndex(0);
    Fahrenheit = sensor2.toFahrenheit(beerTemp);
    Serial.print("Pin4 Sensor 2");
    Serial.print(" C  ");
    Serial.print(beerTemp);
    Serial.print(" F  ");
    Serial.println(Fahrenheit);
}

bool syncTime() {
#ifdef TIMESERVER_HACK
    // Because TCDwifi blocks the NTP port...
    // use google to get the time
    int epochTime = http("europe-west1-hombrew-iot.cloudfunctions.net", "/unix-epoch-gmt").toInt();
    Serial.println(String("Epoch time: ") + epochTime);
    timeval epoch = {epochTime, 0};
    const timeval *tv = &epoch;
    timezone utc = {0, 0};
    const timezone *tz = &utc;
    settimeofday(tv, tz);
#else
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("Waiting on time sync...");
    while (time(nullptr) < 1510644967) {
        delay(10);
    }
    Serial.println("Time synced");
#endif

    return true;
}

unsigned long lastMillis;

void loop() {
    unsigned long delta = millis() - lastMillis;
    bool every10Secs = false;
    if (delta > 10000) {
        lastMillis = millis();
        every10Secs = true;
    }

    Heltec.display->clear();

	watertemp();

    displayWiFiStatus();

    // Sync the time
    if (!timeSetup && connected) {

        const IPAddress dnsServer = IPAddress(134, 226, 251, 100); // for tcd wifi
        /* const IPAddress dnsServer = IPAddress(1,1,1,1); */

        // Need to manually set the dns server for some reason. DHCP fails to do
        // so
        WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(),
                dnsServer);
        wifiClient = new WiFiClientSecure();

        timeSetup = syncTime();
    }

    if (timeSetup && !cloudSetup && connected) {
        /* wifiClient->setCACert(googleRootCert); */

        // Setup Google Cloud IoT
        device =
            new CloudIoTCoreDevice("hombrew-iot", "europe-west1", "homebrew",
                                   deviceId, ESP32_PRIVATE_KEY);

        mqttClient = new MQTTClient(512);
        mqttClient->setOptions(180, true,
                               1000); // keepAlive, cleanSession, timeout
        mqtt = new CloudIoTCoreMqtt(mqttClient, wifiClient, device,
                                    true); // Use HTTP port!
        mqtt->setUseLts(true);
        mqtt->setLogConnect(false); // Otherwise we get those esp32-connected
                                    // messages in pub/sub
        mqtt->startMQTT();

        cloudSetup = true;
    }

    if (cloudSetup) {
        mqtt->loop();

        if (!mqttClient->connected()) {
            Serial.println("MQTT client not connected, reconnecting");
            mqttClient->disconnect();
            mqtt->mqttConnect();
        }

        if (every10Secs) {
            String payload =
                String("{ \"ambientTemp\":") + String(ambientTemp) + ",\n";
            payload += "\"beerTemp\": " + String(beerTemp) + "\n";
            payload += "}";
            mqtt->publishTelemetry(payload);
            Serial.println("Sending:" + payload);
        }
    }

    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->drawString(0, 11, lastMsgTopic);
    Heltec.display->drawString(0, 22, lastMsgPayload);

    // temp sensor
    tempC = celsius(analogRead(tempPin));

    Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
    Heltec.display->drawString(63, 33, "Current Ambient Temp:");
    Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
    Heltec.display->drawString(63, 44, String(tempC));

    Heltec.display->setTextAlignment(TEXT_ALIGN_RIGHT);
    Heltec.display->drawString(10, 128, String(millis()));
    // write the buffer to the display
    Heltec.display->display();

    // PID
    /*
    myPID.Compute();
    Serial.print(Output);
    */

    // alcohol sensor
    // needs to be tested
    /*
    double R0 = 0.26;
    double sensor_volt;
    double RS_gas; // Get value of RS in a GAS
    float ratio; // Get ratio RS_GAS/RS_air
    double BAC;
    double sensorValue = analogRead(alcAPin);
    sensor_volt=(double)sensorValue/4095.0*5.0;
    RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL
    Serial.print("value: ");
    Serial.println(sensorValue);
   ///-Replace the name "R0" with the value of R0 in the demo of First Test -
    ratio = RS_gas/R0;  // ratio = RS/R0
    Serial.print("volt: ");
    Serial.println(sensor_volt);
    BAC = 0.1896*pow(ratio, 2) - 8.6178*ratio/10 + 1.0792;   //BAC in mg/L
    Serial.print("BAC = ");
    Serial.println(BAC*0.0001);  //convert to g/dL
    Serial.print("\n\n");
    */
}

double celsius(double RawValue) {
    // fix equation ~500 instead of 330
    Voltage = (RawValue * 330.0) / 4095.0;
    tempC = (Voltage - 50.0) * 0.1; // adjust offset of 50

    tempF = (tempC * 1.8) + 32.0; // conver to F

    /* Serial.print("Raw Value = " ); // shows pre-scaled value */
    /* Serial.print(RawValue); */
    /* Serial.print("\t milli volts = "); // shows the voltage measured */
    /* Serial.print(Voltage,0); // */
    /* Serial.print("\t Temperature in C = "); */
    /* Serial.print(tempC,1); */
    /* Serial.print("\t Temperature in F = "); */
    /* Serial.println(tempF,1); */

    return tempC;
}

// finding base reading R0 for alcohol sensor
double findR0(double sensorValue) {
    double RS;
    double sensor_volt;
    for (int i = 0; i < 100; i++) {
        sensorValue = sensorValue + analogRead(alcAPin);
    }

    sensorValue = sensorValue / 100.0; // get average of reading
    Serial.print("Pin val: ");
    Serial.println(sensorValue, DEC);
    sensor_volt = sensorValue / 4095.0 * 5.0;
    RS = (5.0 - sensor_volt) / sensor_volt; //
    return RS;
}
