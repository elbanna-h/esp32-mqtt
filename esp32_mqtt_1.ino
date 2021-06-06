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
MIIDWjCCAkKgAwIBAgIVAPEF8OqjtgKxCJLn5wAfINh/PxlaMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTA1MjgxNDQy
MjRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDvLUqGtYX0hcvRLc1F
xK+4dKOCMuZ7aYXuTcswr3//eqE3p+K7RdI0TqopNYsnBBARDyxyVFlkUft2UxAt
0GZXKdvlvKRWS0sQ8AbDj3cx1qKcCUmsNrXI+f9CIcbJSMmr9Vf35q8tPStHY+kv
mowZyf3+DbJliQE9vnQC8GfjehQuQt6qHVWX6btpRPdomMvUih5muWoDAqVk8nnN
dAauF5AjM3/YZGZMjyOQuJh4nzdOROhOdM3iJZoXiQ0mCVDLLCzOCgXW/cNM++e9
9agKPcVb2lIoU4J6rM6CrwI59zFCip8V7KfplAy67b+Kde0BvAbQ7V/NXxvmYBjQ
H4PRAgMBAAGjYDBeMB8GA1UdIwQYMBaAFJWtQZEEHlnSIFRyfzTQ5n5R8EtJMB0G
A1UdDgQWBBQB++5mDon9itDblpGigP4eBQlehTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAVnIjg1irRCWn+Vtoh3imrk9/
y36jyHeWFEt+3pLNFHAdADZyX/+8GC2hOq4+LOvItSxBVnhqvHGDje67rIi36vb/
mQWGAFaiiqTr8znkDxu+HiU1L4mACkGd855DK0ArB5LOfeLWf1AFw9BAKj6qQjpD
IHx7DsZ6NHtbJCBqGF5lG6wgqEz6NK1r8pZOdp602fhMShJkjDQnsgTurxZ2hcSa
ybrWMTH82t4TtUcybH50OFPDuZj4iVdGHWWERoRbSLMCiNH7nVq80GgKA0qY+SCg
14bAEmbgr8sN6Gj7RrneKfJ9aOyEPzbCreuhuxhbgSOdZPtJAd8gf0XhA2OBWA==
-----END CERTIFICATE-----)EOF";


const char* private_pem_key = \

R"EOF(-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA7y1KhrWF9IXL0S3NRcSvuHSjgjLme2mF7k3LMK9//3qhN6fi
u0XSNE6qKTWLJwQQEQ8sclRZZFH7dlMQLdBmVynb5bykVktLEPAGw493MdainAlJ
rDa1yPn/QiHGyUjJq/VX9+avLT0rR2PpL5qMGcn9/g2yZYkBPb50AvBn43oULkLe
qh1Vl+m7aUT3aJjL1IoeZrlqAwKlZPJ5zXQGrheQIzN/2GRmTI8jkLiYeJ83TkTo
TnTN4iWaF4kNJglQyywszgoF1v3DTPvnvfWoCj3FW9pSKFOCeqzOgq8COfcxQoqf
Feyn6ZQMuu2/inXtAbwG0O1fzV8b5mAY0B+D0QIDAQABAoIBAQCBjavfufW8VLjp
LoEROI/iPzPOM5oK4UbNpU/ZqEaR8S/ggMzcmd5ZvP6I4drlIFs/klOF/lCJearX
Ffkeh9LXhfH5yCvQHmqj2xAe39g1o8hi8tj4Mo5XNathfAA8SXQL9BJZUpgyzf5i
ZikKQxF+ap5s/ahTPVnxwvb+HJB5czvnhLFyRmm447v9U6Bvvst8eue1pwfwKKM7
bc/Fq+pepx8+qNMv4a05UQmoZQ8jnGinhGqFvRO71GYxAJsf7hcWqQvBw8+3mA3p
6gGR4CFNWRnyviNJj7IkDXkuyUJn1whJt5ggTAd6WjtbAdNQbXx55NLowXA3YRJu
/sdRERoBAoGBAPfsA39DkyEcZJp0tnkehWHdetzfaN1gPXGHZMtWKpboodmK4fK1
OZcwapJvJdV1oG7f/ZiW016DDs0PQZTQjDUGY2v8j0JxnsFRljUKN7ugbboJzusN
GLy59nyCVPH70xx1oxNt5UVB4o9a1+/LBFfC5tRhOHw+fRz2BoJq1lmhAoGBAPb4
VTdpxYinERqwJsA07ES1aScdu75dYJ07VMKKCffZAa5ylm+3eNumljZxm7UU3lgE
1tHCuYhJqa8aEkNN23/QslKQP2jNdLfDBMAyFPFqJhFWAp504fj6SL2FxJ5m2LaX
cIHFLS6sCWZRi6qQS+k0stJEaAst5MYMDg1i0twxAoGBAI0dWpQ6q2lCVwUPf7mL
JNKilUdZJsAzAmY3cajHf/Brs4w2oAtdGc2zskVtXHSWwl25wwfyeZcnM+qGT4bb
+aJI45hYplZkg793p+7CxRxX4XzuDkf+KWFq3CDcsmsWMoDVeKAbgk+cg8+jHpwP
y4YEmwyx1cvqaZ5tbyEWnLFhAoGBAOCOOcWRudcfuPCgPAvF0xJCNKMpaNyVQNSV
7uKX6qpduSap79kSZKSrWqeUMvS6TdSQSdceC948xDl/Jh3mO++CjbF+kPk03qx7
eOVcZCiB0K0wnF8BfboJsHwKlhVFBEU5e0WzR/rI2B8dhDWI0Ezx6qo/Iq8PgpUE
vv4y3fDxAoGAOVpSYjEswuV6/bP/lS0fKgIJRnB01LmKcY7AxloiEdnf5QyBa1dL
hixyvHrD260h22QbUT9N2eu4ZA2jFwXDTAxB1dRn2OJIPgRkh675ocyeLEkpSt2Y
m4/7akPb51V7Zf5IE0O8vCXw9LPQaGlV66tqtbutpFbQu2ROE8nQGqY=
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
      pubSubClient.connect("Esp32_1");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
