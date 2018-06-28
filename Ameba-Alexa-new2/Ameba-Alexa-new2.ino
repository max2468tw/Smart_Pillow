/*
  This sketch shows how to use Amazon Alexa service on Ameba.
  You need these items:
      Ameba RTL8195AM x1
      Realtek Audio Shield (ALC5680) x1      
      Speaker x1

  This sketch requre I2S interface.
  According to Teensy Audio Shield pinout, we connect pins as below:

      Ameba                Audio Shield
      GPIOC_5(IRQ_VOICE)   BUTTON
      GPIOE_5(LED)         LED      
      GPIOC_0(I2S_WS)      LRCLK
      GPIOC_1(I2S_CLK)     BCLK
      GPIOC_2(I2S_SDTX)    TX      
      3.3V                 3.3V and VOL
      GND                  GND

  Audio shield also needs conntect to Powered Spekaer.
  
  A button to trigger and listen voice command from user.
  This button is on  Realtek Audio Shield.

  You need pre-configure AVS account.
  For more information, please refer www.amebaiot.com
*/

#include <WiFi.h>
#include <Alexa.h>
#include <FlashMemory.h>

char ssid[] = "wifi";     // your network SSID (name)
char pass[] = "06051230";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

char avs_refresh_token[]     = "Atzr|IwEBIHXi4IP0hNrMm-nHC_NhG_iqTyEUEeBV39UuCIc64rLSa8xkk9XkwYMA6gAnaa0CXQsasmWr4Hqf40Eed55DfjjyhqkW6DxW3RocicMaHx-0AifgouNh-wvBH9lUXmP8Wq56HDXZ6tYfvJ8uPShuPPANncmZZLJCAjqM47z3GHlEzF3OB5yrkr-hVbCf4c1QuWvjhTgp5MzyNXB7pjC7F1XM07jBwG20zhOXtkrPFmVOQZtieOSwAoGqJQ17zOXE1RbQEf4FENE1H_kK_zmoMQRzDQOdlMAfv-8CXCwih1n4Z3YXDIyLnKx9fXbjFhBcfyX-DM5y-cj9SGSvoaotrCbSBd4MuhDYFBfFx0ulLlBN6FnrYYnL6hGNuMkZk-S0gHFueDX1CWGKsltSBfzeJy7SiU9aqYOlHxAEVPoPM2W64Wph-PPy6PT6So6QP4odIik";
char avs_client_id[]         = "amzn1.application-oa2-client.6097d413377b4d3dbd06483ae05b4cb0";
char avs_client_secret[]     = "0802f43a7613c445af051f3187c0a1844bcb2cdfd2fefa7f5f40a271a15f4e3d";
char avs_http2_host[]        = "avs-alexa-na.amazon.com";

void setup() {
  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to wifi");

  WiFi.disablePowerSave();

  // this sketch use huge code space, so we can't use default flash memory address
  //FlashMemory.begin(0x1FF000, 0x1000);

  
  setup_alexa();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) 
    {
      delay(1000);
    }
    Serial.println("Connected to wifi");
  }

  delay(100);
}

void setup_alexa() {
  Alexa.setAvsRefreshToken(avs_refresh_token, sizeof(avs_refresh_token)-1);
  Alexa.setAvsClientId(avs_client_id, sizeof(avs_client_id)-1);
  Alexa.setAvsClientSecret(avs_client_secret, sizeof(avs_client_secret)-1);
  Alexa.setAvsHttp2Host(avs_http2_host, sizeof(avs_http2_host)-1);

  Alexa.begin();
}

