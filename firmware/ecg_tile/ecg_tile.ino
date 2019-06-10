#include <Adafruit_NeoPixel.h>

const int LED_PIN = 5;

typedef struct {
  float r;
  float g;
  float b;
} RGBColor;

typedef struct {
  float h;
  float s;
  float v;
} HSVColor;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

// Adapted from https://stackoverflow.com/a/6930407
HSVColor rgb_to_hsv(int r, int g, int b) {
  RGBColor in = { r, g, b };
  float min_chan = min(min(in.r, in.g), in.b);
  float max_chan = max(min(in.r, in.g), in.b);

  HSVColor out;
  out.v = max_chan;
  float delta = max_chan - min_chan;

  if (delta < 0.00001) {
    out.s = 0;
    out.h = 0; // Undefined, maybe nan?
    return out;
  }

  if (max_chan > 0.0) {
    // Avoid divide-by-zero
    out.s = (delta / max_chan);
  } else {
    // If max is 0, then r = g = b = 0
    // s = 0, h is undefined
    out.s = 0.0f;
    out.h = 0.0f; // Actually undefined
    return out;
  }

  if (in.r >= max_chan) {
    out.h = (in.g - in.b) / delta; // Between yellow & magenta
  } else {
    if (in.g >= max_chan) {
      out.h = 2.0f + (in.b - in.r) / delta; // Between cyan & yellow
    } else {
      out.h = 4.0f + (in.r - in.g) / delta; // Between magenta & cyan
    }
  }

  out.h *= 60.0f; // Degrees

  if(out.h < 0.0f) {
    out.h += 360.0f;
  }

  return out;
}

RGBColor hsv_to_rgb(int h, int s, int v) {
  HSVColor in = { h, s, v };
  RGBColor out;

  if (in.s <= 0.0f) {
    out.r = in.v;
    out.g = in.v;
    out.b = in.v;
    return out;
  }

  float hh = in.h;
  if(hh >= 360.0) {
    hh = 0.0;
  }

  hh /= 60.0;
  long i = (long)hh;
  float ff = hh - i;
  float p = in.v * (1.0 - in.s);
  float q = in.v * (1.0 - (in.s * ff));
  float t = in.v * (1.0 - (in.s * (1.0 - ff)));

  switch(i) {
    case 0:
      out.r = in.v;
      out.g = t;
      out.b = p;
      break;
    case 1:
      out.r = q;
      out.g = in.v;
      out.b = p;
      break;
    case 2:
      out.r = p;
      out.g = in.v;
      out.b = t;
      break;
    case 3:
      out.r = p;
      out.g = q;
      out.b = in.v;
      break;
    case 4:
      out.r = t;
      out.g = p;
      out.b = in.v;
      break;
    case 5:
    default:
      out.r = in.v;
      out.g = p;
      out.b = q;
      break;
  }

  return out;
}

RGBColor get_palette_color(int i) {
  return hsv_to_rgb(i, 0xff, 0xff);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); // Setup for leads off detection LO -

  // Initialize all pixels to off
  strip.begin();
  strip.setBrightness(50);
  strip.show();

  Serial.println("Brooklyn Fashion and Design Accelerator");
  Serial.println("ECG Tile");
}

void loop() {
  if(digitalRead(2) || digitalRead(3)) {
    // Turn off LEDs because no one is touching the tile
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
  } else {
    RGBColor color = get_palette_color(analogRead(A0) >> 2);
    strip.setPixelColor(0, strip.Color(color.r, color.g, color.b));
    strip.show();
  }
}

