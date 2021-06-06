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


const char* ssid = "islam";
const char* password = "12345678";

const char* awsEndpoint = "a261cxy6nskbn1-ats.iot.us-east-1.amazonaws.com";

unsigned long lastPublish;


const char* certificate_pem_crt = \

R"EOF(-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVANP6wSLN9NjXoLsEibTMB7rOwijpMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTA1MjkyMDUx
MDdaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC7n5y7jZ4GOKJjMlFV
V+5tMhZidqxKFzAnXJTRCwcNfrkcCfL7MgSbDq2RWVM/sFSIAW54zrWZ59P5A3ei
SKV0AgQowPbjVQrAraHFtKs8dnIyhxHrtrsxrWuYG05J132q+1vWKC0GsLuHU5CP
irb15cUxF6uq7pXaiuaROkAuawnW4r+VygUo1P33gDoGNSBscf2Hzsy6cIKJn7u9
CsJ0uNBdj1L+GKv3PVEntlgblvo29NLofAMZRXqpT4sMEkv/gsoVvN75OokLPh2b
2MlJwEX5X/OtJje2u90vj/V2FNjGX2mZCeB5N6ak92mnvAnHUlVe6vvm9u5UpMQL
fOKTAgMBAAGjYDBeMB8GA1UdIwQYMBaAFE8Y3bz77OT7NEGAm0YaJ+269qf1MB0G
A1UdDgQWBBQ7ju6PJ2yxjhuI+IOW431/ZpeipTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAGFJeB5OG7nyFa9XB71zK+IZg
krJvm+0fLmHoPuTfgTQKhcqTe0+PsOB/0XoDlnQo9QG6nffj8PofxPdzs1dyK5Lu
LH7S1WvuLuIl2A8uoSgKqpiAOfz1AIIaOi2A3qe1/6bRGEQhaWKqCBKsOr+Q1xmK
jnECDw/q2xU+dW3+mKJ0loWeSzM3PX/6fIqJTMbJM56vTrGoZz7pJjLPwUE/lanC
c2Hq29Y0r0cJveWpYpcXeRkFI5QLp+p4c1BSgMyb56N/UxCJydBwBDR2Vx7MDmSU
zzc25GyKicSotTG8g6h5Pv8vX9XBM52smQ2BvJZLAFBtaFerJdN5OSB86M/gcg==
-----END CERTIFICATE-----)EOF";


const char* private_pem_key = \

R"EOF(-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAu5+cu42eBjiiYzJRVVfubTIWYnasShcwJ1yU0QsHDX65HAny
+zIEmw6tkVlTP7BUiAFueM61mefT+QN3okildAIEKMD241UKwK2hxbSrPHZyMocR
67a7Ma1rmBtOSdd9qvtb1igtBrC7h1OQj4q29eXFMRerqu6V2ormkTpALmsJ1uK/
lcoFKNT994A6BjUgbHH9h87MunCCiZ+7vQrCdLjQXY9S/hir9z1RJ7ZYG5b6NvTS
6HwDGUV6qU+LDBJL/4LKFbze+TqJCz4dm9jJScBF+V/zrSY3trvdL4/1dhTYxl9p
mQngeTempPdpp7wJx1JVXur75vbuVKTEC3zikwIDAQABAoIBACbsafuO2aW4wheK
LM1jvJbyuSocLIoCXX8M+eKUQxZNn+FtPvdV3Ra96Mwql9ayohbFFUICwDH5okQa
sAr+vzRd5rb9sTpgPibS3qjBXnTgrbhFxTX9ZsBbleKsZ4VyKyP5mYfINe8ZYw4k
C+17fZFt5YbVvktry7LW9hZS21XFi7YQre0K+mWpI6UnSe79Ls/KFP+VFVUpQ0H7
rBrH+OHJ3lyEhH+HfuV9gana5qdibDZunLqvod69YFQbbdkdoZcHAPRTZmNne7Pq
TZmy4vQSf9CicPjc0zAsmB0jp0ZhJaCECqoXDGWXYRulMsOlMV0mEcmkXlyDxRlj
rhZhR+ECgYEA7pQPrTHlm9HMR09+W8WGxZKR59lRK0mqidWnq7cPoFA0DsqbIGyh
QBNCgVvSlN6NgI6YCeih8sRqSRZO+GKfcafu7Rc9/zOn8jz11BjRZUY3l6CL8Car
xsvXYkJdRvbrVVBayzE22rnd4M8wtjvUR6eOWFfIQLcDdkqD9Qm2n70CgYEAyVMC
mLjU+3EG4Z5jCxkoG33lt/TylF5Y59irDNBat0+ffZgFbQNE/Tt9Wcmx5MlUuUio
V9mVl0DpXuL+ID8JW/LxoeIrhfElPyYjgRhfMnmtg0hKm0z6ZwhFGwQsf3YKe6wQ
Tughh85+U19pMCEd9432xYZ0cwSHXBNNVJRGyI8CgYBfafODDquHT7Zk5pPgW/T0
WxPjJ3Rg3YtIqJvv/rxPHwRSzhCQPSGYIJu+ZFypkQACZATGIbVzA76GEFfw2nBt
4f2Z8Guo/p2yMVMXSf2khRqCl4CfwLd+P9OZxnTLhsDDU+mbzcjkXOUxnWsb1n2l
UouJJUI/L3JBArgXbZUN+QKBgG1dA48U8NW6O4O6d5Ei1E/C31RD4rbIXcFsSNcI
JA9qWHhwaZojaYmqlFh/IXzoBXc1SgWW9BUbNOrPoj7AH4Rsf+hxpTT4n0QjoqvR
In2X7bLgRW8BrRDlRCz94nJHHEC8YouKvBQ7DxKZRNnZU58T5la8yGtmrWVOMvbc
KWtvAoGATLBJM4Upmz/F9hFUdgCuzBXTWD+l7PsjRWdIK3A40SiX7Zxm2jUnY0oA
mbS2v/AAAI4dPyf61RvIafagXHXBEl/EB/F5JmN4y7ITc6WeNExf7JWiriiczrjW
VKXL0M2AHWD0OS43RJffNnQz7GMPdJWO/t4DKvUaHf9YDiSUAhU=
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

  if (millis() - lastPublish > 3000) {

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
      pubSubClient.connect("Esp32_4");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
