
#include <Arduino.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "HX711.h"

#define PIR_PIN 25

HX711 loadcell;

const int LOADCELL_DOUT_PIN = 32;
const int LOADCELL_SCK_PIN = 33;

// 2. Adjustment settings
const long LOADCELL_OFFSET = 200;
const long LOADCELL_DIVIDER = 207385;

float weight = 0; 
int val  = 0;
int pirState = LOW;
int count = 0;

//uint8_t count = 0;
// This example downloads the URL "http://arduino.cc/"
char ssid[50] = "test_network"; // your network SSID (name)
char pass[50] = "test"; // your network password (use for WPA, or use
// as key for WEP)

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

void nvs_access() {
// Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
  err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
  Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
  Serial.printf("Done\n");
  Serial.printf("Retrieving SSID/PASSWD\n");
  size_t ssid_len;
  size_t pass_len;
  err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
  err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
  switch (err) {
  case ESP_OK:
  Serial.printf("Done\n");
  //Serial.printf("SSID = %s\n", ssid);
  //Serial.printf("PASSWD = %s\n", pass);
  break;
  case ESP_ERR_NVS_NOT_FOUND:
  Serial.printf("The value is not initialized yet!\n");
  break;
  default:
  Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  // Retrieve SSID/PASSWD from flash before anything else
  nvs_access();
  // We start by connecting to a WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  Serial.println();
  pinMode(PIR_PIN, INPUT);
  loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  loadcell.set_scale(LOADCELL_DIVIDER);
  loadcell.tare();
  delay(1000);
}

void loop() {
  int err = 0;
  val = digitalRead(PIR_PIN);
  if (val == HIGH){
    if (pirState == LOW) {
      count++;
      // we have just turned on
      Serial.println();
      Serial.println("Motion detected!");
      Serial.println(count);
      // We only want to print on the output change, not state
      pirState = HIGH;
      delay(2000);
      weight = ((float)((int)(loadcell.get_units(10)*100)))/100;
      Serial.print("Weight: ");
      Serial.println(weight);
  WiFiClient c;
  HttpClient http(c);
  std::string t = "/?var=" + std::to_string(count) + "_" + std::to_string(weight);
  //std::string t ="/?var=Humidity:";
  //Serial.print(t.c_str());
  Serial.println();
  err = http.get("52.53.186.179", 5000, t.c_str(), NULL);
  if (err == 0) {
  Serial.println("startedRequest ok");
  err = http.responseStatusCode();
  if (err >= 0) {
  Serial.print("Got status code: ");
  Serial.println(err);
  // Usually you'd check that the response code is 200 or a
  // similar "success" code (200-299) before carrying on,
  // but we'll print out whatever response we get
  err = http.skipResponseHeaders();
  if (err >= 0) {
  int bodyLen = http.contentLength();
  Serial.print("Content length is: ");
  Serial.println(bodyLen);
  Serial.println();
  Serial.println("Body returned follows:");
  // Now we've got to the body, so we can print it out
  unsigned long timeoutStart = millis();
  char c;
  // Whilst we haven't timed out & haven't reached the end of the body
  while ((http.connected() || http.available()) &&
  ((millis() - timeoutStart) < kNetworkTimeout)) {
  if (http.available()) {
  c = http.read();
  // Print out this character
  Serial.print(c);
  bodyLen--;
  // We read something, reset the timeout counter
  timeoutStart = millis();
  } else {
  // We haven't got any data, so let's pause to allow some to
  // arrive
  delay(kNetworkDelay);
  }
  }
  } else {
  Serial.print("Failed to skip response headers: ");
  Serial.println(err);
  }
  } else {
  Serial.print("Getting response failed: ");
  Serial.println(err);
  }
  } else {
  Serial.print("Connect failed: ");
  Serial.println(err);
  }
  
  http.stop();
    }
  } else {
    if (pirState == HIGH){
      // we have just turned of
      //Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }

  // And just stop, now that we've tried a download
  //while (1)
  //  ;
  //delay(2000);
}



