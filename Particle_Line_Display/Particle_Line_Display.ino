#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET     LED_BUILTIN //Reset Pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int particleAmount = 15;
const float Speed = 0.8;
const float Radius = 2;
const float linkRadius = 24;
const bool fillState = 1;
const int df = 0;

static const unsigned char PROGMEM logo[] = {
0xf0, 0x00, 0x0f, 0xc1, 0xf0, 0x00, 0x1f, 0x83, 0xf0, 0x00, 0x3f, 0x07, 0xf0, 0x00, 0x7e, 0x0f, 
0xf0, 0x00, 0xfc, 0x1f, 0xf0, 0x01, 0xf8, 0x3f, 0xf0, 0x03, 0xf0, 0x7f, 0xf0, 0x07, 0xe0, 0xff, 
0xf0, 0x0f, 0xc1, 0xff, 0xf0, 0x1f, 0x83, 0xff, 0xf0, 0x3f, 0x07, 0xef, 0xf0, 0x7e, 0x0f, 0xcf, 
0xf0, 0xfc, 0x1f, 0x8f, 0xf1, 0xfc, 0x3f, 0x0f, 0xf3, 0xfe, 0x7e, 0x0f, 0xf7, 0xff, 0xfc, 0x0f, 
0xff, 0xdf, 0xf8, 0x0f, 0xff, 0x8f, 0xf0, 0x0f, 0xff, 0x07, 0xe0, 0x0f, 0xfe, 0x0f, 0xf0, 0x0f, 
0xfc, 0x1f, 0xf8, 0x0f, 0xf8, 0x3f, 0xfc, 0x0f, 0xf0, 0x7e, 0x7e, 0x0f, 0xe0, 0xfc, 0x3f, 0x0f, 
0xc1, 0xf8, 0x1f, 0x8f, 0x83, 0xf0, 0x0f, 0xcf, 0x07, 0xe0, 0x07, 0xef, 0x0f, 0xc0, 0x03, 0xff, 
0x1f, 0x80, 0x01, 0xff, 0x3f, 0x00, 0x00, 0xff, 0x7e, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x3f
};

class Mypoint {
  double x;
  double y;
  double tx;
  double ty;
  double angle;
  public:
    void setPos(double posX, double posY, double tanX, double tanY) {
      x = posX;
      y = posY;
      tx = tanX;
      ty = tanY;
    }
    void setAng(double ang) {
      angle = (PI/180)*ang;
    }
    double getX() { return x; }
    double getY() { return y; }
    double getTX() { return tx; }
    double getTY() { return ty; }
    double getAng() { return angle; }
};
Mypoint dot[particleAmount];

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.display();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  logoImage(logo, 32, 32);
  display.clearDisplay();
  display.display();

  for(int i=0; i<particleAmount; i++) {
    dot[i].setAng(random(360));
    dot[i].setPos(
      random(SCREEN_WIDTH),
      random(SCREEN_HEIGHT),
      cos(dot[i].getAng())*Speed,
      sin(dot[i].getAng())*Speed
    );
  }
}

void loop() {
    render();
}

void logoImage(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width()-w)/2,
    (display.height()-h)/2,
    bitmap, w, h, 2);
  display.display();
  delay(2000);
}

void render() {
  display.clearDisplay();

  for(int i=0; i<particleAmount; i++) {
    double x = dot[i].getX();
    double y = dot[i].getY();
    double tx = dot[i].getTX();
    double ty = dot[i].getTY();
    double ang = dot[i].getAng();

    double nxt = cos(ang)*Speed;
    double nyt = sin(ang)*Speed;
    
    if((x+Radius) >= SCREEN_WIDTH || (x-Radius) <= 0) { tx *= -1; }
    if((y+Radius) >= SCREEN_HEIGHT || (y-Radius) <= 0) { ty *= -1; }

    x += nxt*tx;
    y += nyt*ty;
    
    if((x+Radius) >= SCREEN_WIDTH)    { x = SCREEN_WIDTH-Radius; }
    if((y+Radius) >= SCREEN_HEIGHT)   { y = SCREEN_HEIGHT-Radius; }

    if((x-Radius) < 0) { x = Radius; }
    if((y-Radius) < 0) { y = Radius; }
    
    dot[i].setPos(x, y, tx, ty);
    if(fillState) { display.fillCircle(x, y, Radius-0.5, WHITE); }
    else          { display.drawCircle(x, y, Radius-0.5, WHITE); }
  }
  for(int i=0; i<particleAmount; i++) {
    for(int j=i+1; j<particleAmount; j++) {
      float distance = dist(dot[i].getX(), dot[i].getY(), dot[j].getX(), dot[j].getY());
      if(distance <= linkRadius) {
        display.drawLine(
          dot[i].getX(), dot[i].getY(),
          dot[j].getX(), dot[j].getY(),
          1
        );
      }
    }
  }
  display.display();
  delay(df);
}

float dist(float x1, float y1, float x2, float y2) { return sqrt( pow(x2-x1, 2) + pow(y1-y2, 2) ); }
