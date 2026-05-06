#include "esp_camera.h"
#include <WiFi.h>

#define CAMERA_MODEL_AI_THINKER
#include "C:\Users\ASUS\Downloads\camera_pins.h\camera_pins.h.ino"

const char* ssid = "TT_3FB8";
const char* password = "9co1hhe9h3";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting RAW RGB565 camera server...");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 8000000;
  config.pixel_format = PIXFORMAT_RGB565;
config.frame_size = FRAMESIZE_QQVGA; // 160x120
config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 1;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_brightness(s, 2);
  s->set_contrast(s, 1);
  s->set_saturation(s, 1);

  Serial.println("Camera init OK");

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("Open: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/capture.raw");

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  String request = client.readStringUntil('\r');

  while (client.available()) {
    client.read();
  }

  if (!request.startsWith("GET /capture.raw")) {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Connection: close");
    client.println();
    client.stop();
    return;
  }

  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    client.println("HTTP/1.1 500 Internal Server Error");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Capture failed");
    client.stop();
    return;
  }

  size_t imageSize = fb->len;

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/octet-stream");
  client.println("X-Width: 96");
  client.println("X-Height: 96");
  client.println("X-Format: RGB565");
  client.print("Content-Length: ");
  client.println(imageSize);
  client.println("Connection: close");
  client.println();

  size_t sent = 0;
  while (sent < imageSize) {
    size_t chunkSize = min((size_t)1024, imageSize - sent);
    client.write(fb->buf + sent, chunkSize);
    sent += chunkSize;
    delay(1);
  }

  esp_camera_fb_return(fb);

  client.flush();
  client.stop();

  Serial.print("Sent bytes: ");
  Serial.println(imageSize);
}
