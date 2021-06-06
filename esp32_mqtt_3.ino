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
MIIDWTCCAkGgAwIBAgIUdl5603c78y0lFqAiOvDds5gwaGEwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDUyOTIwMzkz
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM2d+Z+IFVhUHsVzkCir
cJgA1TSFRmmgvpm1LPZPmBlyh90NIjmVTFxyRlCYyoYOHVyODkneqVL479ZskBnc
Sy2T4tvfDeQIUaq1z2A5zcMdIyLnnD5T6YC+dOYGs9tn2P9ftuQI6FOpQIusLrP9
jXOdK4kUmy6QsSOHj6ZgUBKzT43LvFKotD/LOF1MCNt0PSsAiZNP+jAqWFnqCj52
mGHdnsqYboHsrnnzXbsBh+8cHtPGNMjRl3zB88ZnA5Fet80H5hWLSVisF0eW0rVy
8WAytO7Ajt1duQRaOlTJM8VgOTWnzReJFf98P7CC1yxOYUuM+i0v26stTnV9Aaek
bScCAwEAAaNgMF4wHwYDVR0jBBgwFoAUoMTGTmpWKcxdyAIIY9DRTT7gg1AwHQYD
VR0OBBYEFN9KF8Mg9NetPuMOsAANy8U2/auOMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBsK5rPeKFQ+RavnDdQ6AJoOM0e
iXwPEWExe91Fv/Z1Scgn+50+pUUAhlYghJbKXsFUJbNZq/6GJoRU6ClAAD+qLW+7
HpmJqqcvcuV9NxFRPJuIyWFHz/K5R3KIijdcUm9IKQQBoytNhIH72WaVaV0/Gt3Z
xEwW1IaQ+/uOO15wA1zIlPg2IB7Ljx50V75TKBFB8h8+G7tff0Z08FmoHuC5YTGa
1oooD1lIwb6sBUrVmIdeev2ajSoWLIWKY4H0/L82ph/0vlhFlfMmRZdxiHiu4/yU
aKlVYw9I74UiFcDp7KXxXsCLF4Z1Nzoj4hpPUXbaXJVIbMr85lkLdqsUbtV0
-----END CERTIFICATE-----)EOF";


const char* private_pem_key = \

R"EOF(-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAzZ35n4gVWFQexXOQKKtwmADVNIVGaaC+mbUs9k+YGXKH3Q0i
OZVMXHJGUJjKhg4dXI4OSd6pUvjv1myQGdxLLZPi298N5AhRqrXPYDnNwx0jIuec
PlPpgL505gaz22fY/1+25AjoU6lAi6wus/2Nc50riRSbLpCxI4ePpmBQErNPjcu8
Uqi0P8s4XUwI23Q9KwCJk0/6MCpYWeoKPnaYYd2eyphugeyuefNduwGH7xwe08Y0
yNGXfMHzxmcDkV63zQfmFYtJWKwXR5bStXLxYDK07sCO3V25BFo6VMkzxWA5NafN
F4kV/3w/sILXLE5hS4z6LS/bqy1OdX0Bp6RtJwIDAQABAoIBABxybbW3kxRsVvGE
Yn5t8Z7mBplEkxBMD/KeEUFmk6l5ofhgN8r1r+HeYYIHEYXocAF9Gdb4ej6jeDMX
Q/x/8Nq2ah13R6mzyPCwRKeQd7o1zsWme4RYAacxMmUcqkeBn5D9QgVcgiAI+4gX
Y0K9opNLPFmzwyB3HjrHJ4WiL5HWgy8BdTSUCQtCDrgekNJ5U7GCLoDRqjlOHI9k
dozKZliZHXCoMmBo33uf1vQVQx4zsh6fBVDMllxeGv7llcEpPyK+M3X/8zm5bBq9
tzGB08kkgPNCvC8WOwXdIim+sDsUR7EE1AbmX1CUJeh897mp6KJQcCdkvojOOqcy
OfUzalECgYEA/YwnDANm4TlCHUXstFlQRTUP7ZFawQBK1whhUL6Z0R3VNW6IY5yp
eYy/CmkFcmI9jyg0tx/5VI3JnFANUdytxxrn3iTxPulfN1ohDYqT+JGyHSYtxPCj
xYUr8yc3cYHrRgThd5gXE/D3x2G/Ta3CnzjeJv/AG1n5UVopuacLpEkCgYEAz5si
hdghR+g1iHc/dI2ZclUmOdZNkTGrq72HildRxvvEtGmnUh0L3uH8fXKsg63S8qaJ
5crl5OLt20J+8ert3IIzu5a4cVRf8AtzLKr/WSvHtl4q2I4aB3vmtBS6OB6ucP5y
VfUO1+BI8DqUb7LYmnQ68DUBlU0H9WMD94XLpe8CgYB+aXkyWSPbGPQoqitJA9qs
lrcZSEZKQb4OlgQHAPCpvUkG+dQQbhRlINJC+3eWvvpYPMCDqPJjwMmLILJzZSZT
RcvjCuIo7qvlag9es//wh5uZQubA8QWGPk5aTewrsI2//ED5op+m9pZjFw4NSZvm
TLEeGpcKTAKtF9oD0RBR2QKBgB9kgAXpNq8e5+lozsmbF9b4FT2tsDECZ8kicbpz
lcQXEhmN2GVMSx064uk9S+Tlhf53PGbpFXZU8k6zcWq3cZxke9oj4mQQtCkiMhpR
CSe8vQhQmP4CDuGO1MXqVHZj/VDMyRFCayotmhnpYcS6MJK6fKcR/NlMuqVagaEO
NEs1AoGBANP9ZY9gJyYScWfrLM6BmUQSz2R1b+DduBfQMTagaTR9F9yfS/4by1DC
eC/08jg/5pF14GIcj1NScTzDOL2wNdIXBZjL+7vdZwrh6Yq5bQlBi7Vd5OcKJ49x
vWqkXEJMaC9+95/3dIxrskq8Jplozp6+iqmX/SQExaUuCBHHtMYx
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
      pubSubClient.connect("Esp32_3");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
