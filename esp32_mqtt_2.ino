#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

String Addresses[] = {
  "dd:33:16:00:02:9d",
  "dd:33:04:13:00:81",
  "dd:33:16:00:03:73",
  "dd:33:04:13:00:a1",
  "dd:33:16:00:03:4a",
  "dd:33:16:00:02:d0"
  };

typedef struct {
  char address[17];
  int rssi;
} BeaconData;

BeaconData buffer[6];
uint8_t myBufferIndex = 0;

int scanTime = 1;
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {

      extern uint8_t myBufferIndex;
      extern BeaconData buffer[];
      
      if(myBufferIndex >= 6) {
        return;
      }
                  
      for (int i = 0; i < 6; i++) {
      
        if (strcmp(advertisedDevice.getAddress().toString().c_str(), Addresses[i].c_str()) == 0){
        
              strcpy (buffer[myBufferIndex].address, advertisedDevice.getAddress().toString().c_str());
        
              if(advertisedDevice.haveRSSI()) {
                buffer[myBufferIndex].rssi = advertisedDevice.getRSSI();
              } else { buffer[myBufferIndex].rssi =  0; }
        
              myBufferIndex++;
        }
      }
   }
};


const char* ssid = "HANY";
const char* password = "12345678";

const char* awsEndpoint = "a261cxy6nskbn1-ats.iot.us-east-1.amazonaws.com";

unsigned long lastPublish;


const char* certificate_pem_crt = \

R"EOF(-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUOK+sEK7jD3wS83WtURWSkir4wSMwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDUyODE0NTgx
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALNXJbizr1QWlU/8hkei
Pf2SYN3BoWi2LrSj/+bGEocTr+xuJLaITBEH9PA+3VfVW6Y0uPO4qkWMevbESG2/
EMDR69KYyrOX7P2mLGcNim2OK8rjGuoWuhPCRVvjfSvQ7tO5VPsGC1+ivAl+tWKA
AyPO3V94UQdUowx3ovePqyuZ4FvjSt0ZRbJqaVPINWvbuI1ByqDdNnPOU1zuRNEt
UHoWcw+7ifto9i3Cr7u/QpxlVBzwtepQ2o7jXjnMacv5CSpubmVlKTNU/zkCiy/Y
YsxK2Ti8qP7lRrdRMc6do/H1Y79NeFym22DX6QfmFi4vvz5X3b9Je8fToJIbrdDr
nMcCAwEAAaNgMF4wHwYDVR0jBBgwFoAUF5xyMBUnL3CXN1P+fBgzGz9Crr4wHQYD
VR0OBBYEFG6X5CbIoTWQq7WWOpz2xzIyJJv1MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQA3/HvPRm/LZtmTrEPv00GvG1py
5TUt26MqC2r7Af8pUwZiOr4i+XrZvMDJH2w9WYU+EUKlJPMer0MtzHQzAA2qgZ6U
j82laQlXmGJleQw+AkGG6Ui4voypgwM6nZ1DjiEkgSCR+jtjP7hpIY3Ipm4cF/D0
2xsnlKzT9Gup7X+JE6xAxWdUIv2RP3kfczDCcAQH1QOvCJzTg8e1+SzIV75XPkyF
yLknkWTyQP/80nmKqSutmVSt+wtDEGR9NGu8AorsQyVIuZl6OD/HoCEOjhMbEMgG
CXHAc/u3Umi/c6g0Jv/dfwDQ1XtFUbQKPutwxRDuuKIDSGUs1+dVQZ15d8+E
-----END CERTIFICATE-----)EOF";


const char* private_pem_key = \

R"EOF(-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAs1cluLOvVBaVT/yGR6I9/ZJg3cGhaLYutKP/5sYShxOv7G4k
tohMEQf08D7dV9VbpjS487iqRYx69sRIbb8QwNHr0pjKs5fs/aYsZw2KbY4ryuMa
6ha6E8JFW+N9K9Du07lU+wYLX6K8CX61YoADI87dX3hRB1SjDHei94+rK5ngW+NK
3RlFsmppU8g1a9u4jUHKoN02c85TXO5E0S1QehZzD7uJ+2j2LcKvu79CnGVUHPC1
6lDajuNeOcxpy/kJKm5uZWUpM1T/OQKLL9hizErZOLyo/uVGt1Exzp2j8fVjv014
XKbbYNfpB+YWLi+/Plfdv0l7x9Ogkhut0OucxwIDAQABAoIBACL1XuYFKK0khdCE
sgwupRTTvswbXYJS2QHfYhezDpOEE+vrgH6QTPuY6HcYWf8wGOBqMI9ZtN8/xgxq
MJAsCXD8upJftEuT1t+Oz+L6sbwvVXwEDxAIhBcr7GdADftHi/Z7jwQrpnXq++qB
54ue1n3Zai+c/XaHWg0pGPxW5g/maAEChQg6GI0NFdLmsjIMydqB+9dNS0LlWfF1
/GxGO1rQLgQnk0gJboRg9nPaQCPsPz+QWKcLW65duvCpPLM9TL55evb5tThv9IwH
5MguxJ8CEQiQurGMgsu4zC4lnvVUwfHvUJTR2A4THyDOzimF6czkVX27tGgTGLqI
722F9NECgYEA6utDmhxSTz5Fwpt0vOtfzFelUT8AMSoFS1Gc+2LYo2I7TD+sVZRj
K4oRp38KtACJcBFgNvd/8ntauK4qwHMoyJ+LV5kV6GKwGCro8JgAehO2V7QHULw5
0Lfbkz82/wFCgjb5sj3Duw9crk3X5QHSU3Tb8oKvOoF8pk6pAY6qymkCgYEAw28X
H3ROJrR54VWz+i4kwR6MqAH0DevZfJ2ayvBPwQLdR3hGrzCkY+cNslAhEbOOt3Y9
q3e3JEeG7+xBtPXHshvdoS3i/3HGfQxeKVm2Bt1HLI9QV6I2WGU58d0tRSdzjUEK
9hyYH0xEhLuUajK3QAFj/Ac24MptfEjnwyeDZ68CgYBprrsz2mfhBGfHu73Ru6XW
PNu/aDNC6t+E2ctggEBriCabD84MlfvjO3LjwlBjWBZ8VyTMtZ1Wzc2rZZcQCv6e
+r9qAhGMANGtnag7jHYydSnD3NapqHPuBLiATqEEKWHtSKQeXpDuV2Pfel+Uw7QG
HX1RZR9Fl7km7B41+CO3eQKBgQCzgP7pCieaD1x/JvvHR8jEhK3GxvyGKOG7Pvtl
ePKYImxib8vkr7XeeOOwqz6khpsIUZd1PeE+dmAvUH5Nmw5Pvp6Do6UC4VrhkpGO
Q3MYvPfZqsytoTVvXZKqX29sPD87N8m8oijrXl7012ILZQfBx0UxwOKb9/I2HLSb
IFjmywKBgQDY3RAWr6WQMTVQJOZghb2CT/48kkzhZR/ySCPx21TsngPpvnwO0Ub+
lQ+Vnwm5EgSupHWSZS+faBVNE43ELvL5Q/qVIAFRzvHQ7X5dGasW9ZyD5hRK0Jdj
jKhdfnroUIB1otxun3a60WUuBHWR8ec4rSaNKuk6CJl7DBmK25I5mQ==
-----END RSA PRIVATE KEY-----)EOF";


const char* rootCA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";


WiFiClientSecure wiFiClient;

void msgReceived(char* topic, byte* payload, unsigned int len);

PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200);

  Serial.print("ssid: "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi.localIP: "); Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void loop() {

  pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();
  delay(1000);

    for(uint8_t i = 0; i < myBufferIndex; i++) {
      for(uint8_t j = i; j < myBufferIndex; ++j){
        if ((j != i) && (strcmp(buffer[i].address, buffer[j].address) == 0)){
          buffer[i].rssi = 1000;
        }
      }
    }

  String myString = "{\"e\":[";
  for(uint8_t i = 0; i < myBufferIndex; i++) {
    if (buffer[i].rssi != 1000) {
      myString += "{";
      myString += "\"m\":\"";
      myString += String(buffer[i].address);
      myString += "\",";
      myString += "\"r\":\"";
      myString += String(buffer[i].rssi);
      myString += "\"}";
      if(i < myBufferIndex-1) {
        myString += ',';
      }
    }
  }

  myString += "],\"wm\":\"";
  myString += String(WiFi.macAddress());
  myString += "\"}";


  pubSubCheckConnect();

  if (millis() - lastPublish > 1000) {

    String msg = String(myString);
    boolean result = pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Result="); Serial.print( (result ? "OK: " : "FAILED: ") );
    Serial.println(msg);

    myBufferIndex = 0;
    
    lastPublish = millis();
  }

  
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("Esp32_2");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
