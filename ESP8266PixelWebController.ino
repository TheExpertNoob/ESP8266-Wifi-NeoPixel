#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

#define PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRBW + NEO_KHZ800);

WiFiServer server(80);

void setup() 
{
  WiFi.softAP("NeoPixel Controller", "neopixel");
  server.begin();
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();

  // Match the request
  String val; // We'll use 'val' to keep track
  //Do Rainbow Chaser
  if (req.indexOf("/rainbowc") != -1) {
    val = "Rainbow Chaser";
    theaterChaseRainbow(50); }
  //Do Red Chaser
  else if (req.indexOf("/redc") != -1) {
    val = "Red Chaser";
    theaterChase(strip.Color(127, 0, 0), 50); }
  //Not an option or start page.
  else {
    val = "Nothing Selected."; //All other Requests.
  }
  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML><meta name='viewport' content='width=320'>\r\n";
  s += "<html><head><title>NeoPixel Controller</title></head><body>\r\n";
  s += "<h1>Select Mode Below</h1>\r\n";
  s += "<p><input type=button value='Rainbow Chaser' onmousedown=location.href='rainbowc'><br>\r\n";
  s += "<input type=button value='Red Chaser' onmousedown=location.href='redc'></p>\r\n";
  s += "<p>Current Mode: " + String(val) + "</p></body></html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
