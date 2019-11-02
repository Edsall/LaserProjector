// See LICENSE file for details
// Copyright 2016 Florian Link (at) gmx.de

#include "Laser.h"
#include "Drawing.h"
#include "Cube.h"
#include "Objects.h"
#include "Logo.h"
#include "RandomLine.h"
#include <DueTimer.h>
#include <SoftwareSerial.h>


#include "LaserShow.h"
#include "ILDA_SD.h"

// Create laser instance (with laser pointer connected to digital pin 5)
Laser laser(duo_control_pin, laser_pin_R, laser_pin_G, laser_pin_B);

SoftwareSerial dmx_controller (RS485_RX_PIN_SLAVE, RS485_TX_PIN_SLAVE);  // receive pin, transmit pin

int commandCode = 1;
bool newCode = true;
bool _auto = true;
int offset = 0;
long timeoutBeatCal = 0;
bool _stopDisplay = false;

const int NUM_SCENES = numIldFiles + 20;

void setup()
{
  laser.init();
  Serial.begin(9600);

  while (!Serial) {};
  Serial.println("Starting laser projector version 1.1");
  randomSeed(analogRead(0));  // initialize the random seed

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), getCodeISR, FALLING);

  Timer3.attachInterrupt(timer3ISR);
  Timer3.start(7000000); // Calls every 7s


  countDown('3');
  laser.setColor(WHITE);

}


void letterEffect()
{
  laser.setColor(WHITE);

  String dyn = "CLASS OF";
  String lu  = "  2019  ";
  int w = Drawing::stringAdvance(dyn);
  int z = Drawing::stringAdvance(lu);
  laser.setScale(3048. / w);
  laser.setOffset(2048, 2048);
  Drawing::drawString(dyn, -w / 2, 0, 40);
  for (int i = 0; i < 35; i++) {

    Drawing::drawString(dyn, -w / 2, 0, 5);

    for (int i = 0; i < 8; i++) {
      if (lu[i] > dyn[i]) dyn[i]++;
      if (lu[i] < dyn[i]) dyn[i]--;
    }
  }
  Drawing::drawString(lu, -z / 2, 0, 40);

  int clip = 0;
  for (int i = 0; i < 60; i++) {
    laser.setClipArea(clip, 0, 4095 - clip, 4095);
    Drawing::drawString(lu, -z / 2, 0, 1);
    clip += 2048 / 50;
  }
  laser.resetClipArea();
}


void congrats() {
  laser.setColor(WHITE);
  String str = "CONGRATS";
  int w = Drawing::stringAdvance(str);
  laser.setScale(3048. / w);
  laser.setOffset(2048, 2048);
  float scale = 0;
  for (int i = 0; i < 70; i++) {
    scale += 0.01;
    laser.setScale(scale);
    Drawing::drawString(str, -w / 2, 0);
  }
}

void cats()
{
  laser.setColor(WHITE);
  String str = "LGHS CATS";
  int w = Drawing::stringAdvance(str);
  laser.setScale(0.5);
  laser.setOffset(1024, 1024);
  int count = 360 / 4;
  int angle = 45;
  for (int i = 0; i < count; i++) {
    Matrix3 world;
    world = Matrix3::rotateX(angle % 360);
    laser.setEnable3D(true);
    laser.setMatrix(world);
    laser.setZDist(2000);
    Drawing::drawString(str, -w / 2, -500, 1);
    angle += 8;
  }
  laser.setEnable3D(false);
}

void laserShow()
{
  laser.setColor(WHITE);
  String str = "LGHS";
  int w = Drawing::stringAdvance(str);
  int count = 360 / 4;
  int angle = 0;
  laser.setScale(0.5);
  for (int i = 0; i < count; i++) {
    Matrix3 world;
    laser.setEnable3D(true);
    world = Matrix3::rotateX(angle % 360);
    laser.setMatrix(world);
    laser.setZDist(4000);
    laser.setOffset(1024, 1024 + 600);
    Drawing::drawString(str, -w / 2, -500, 1);
    world = Matrix3::rotateY(angle % 360);
    laser.setMatrix(world);
    laser.setOffset(1024, 1024 - 600);
    Drawing::drawString("2019", -w / 2, -500, 1);
    angle += 8;
  }
  laser.setEnable3D(false);
}

// electric globe
void globe(int count) {
  laser.setColor(WHITE);
  laser.setScale(1);
  laser.setOffset(2048, 2048);
  for (int i = 0; i < count; i++) {
    laser.on();
    int pos = random(360) / 5 * 5;
    int diff1 = random(35);
    int diff2 = random(35);
    int diff3 = random(35);
    for (int r = 0; r <= 360; r += 5)
    {
  laser.setColor(WHITE);
      laser.sendto(SIN(r) / 16, COS(r) / 16);
      if (r == pos)   {
        laser.setColor(validColors[random(NUM_VALID_COLORS)]);
        laser.sendto(SIN(r + diff1) / 32, COS(r + diff2) / 32);
        laser.sendto(SIN(r + diff2) / 64, COS(r + diff3) / 64);
        laser.sendto(0, 0);
        laser.sendto(SIN(r + diff3) / 64, COS(r + diff3) / 64);
        laser.sendto(SIN(r + diff2) / 32, COS(r + diff1) / 32);
        laser.sendto(SIN(r) / 16, COS(r) / 16);
      }
    }
  }
}

// draw a circle using sin/cos
void circle() {
  const int scale = 12;
  laser.sendto(SIN(0) / scale, COS(0) / scale);
  laser.on();
  for (int r = 5; r <= 360; r += 5)
  {
    //Serial.println(SIN(r)/scale);
    laser.sendto(SIN(r) / scale, COS(r) / scale);
  }
  laser.off();
}

// Draw circle and count down from 9 to 1
void countDown(char startDigit) {
  laser.setScale(1);
  laser.setOffset(2048, 2048);
  int center = Drawing::advance('9');
  for (char j = startDigit; j > '0'; j--) {
    float scale = 0.0;
    float step = 0.01;
    for (int i = 0; i < 40; i++) {
      laser.setScale(1.4);
      laser.setColor(WHITE);
      circle();
      laser.setColor(validColors[j % 7]);
      laser.setScale(scale);
      Drawing::drawLetter(j, -center / 3, -center * 2 / 3 + 100);
      scale += step;
      step += 0.002;
    }
  }
}

// simple fixed logo
void drawLogoSimple()
{
  laser.setColor(WHITE);
  long centerX, centerY, w, h;
  Drawing::calcObjectBox(draw_logo, sizeof(draw_logo) / 4, centerX, centerY, w, h);
  laser.setScale(4096 / (float)h);
  laser.setOffset(2048, 2048);
  for (int i = 0; i < 100; i++) {
    Drawing::drawObject(draw_logo, sizeof(draw_logo) / 4, -centerX, -centerY);
  }
}

// logo with drawing effect
void drawLogo()
{
  laser.setColor(WHITE);
  long centerX, centerY, w, h;
  Drawing::calcObjectBox(draw_logo, sizeof(draw_logo) / 4, centerX, centerY, w, h);

  int count = 200;
  laser.setScale(4096 / (float)h);
  laser.setOffset(2048, 2048);

  long maxMove = 0;
  for (int i = 0; i < count; i++) {
    laser.setMaxMove(maxMove);
    maxMove += 400;
    Drawing::drawObject(draw_logo, sizeof(draw_logo) / 4, -centerX, -centerY);
    if (laser.maxMoveReached()) {
      long x, y;
      laser.getMaxMoveFinalPosition(x, y);
      laser.resetMaxMove();
      laser.off();
      laser.sendtoRaw(x, y);
      laser.on();
      laser.sendtoRaw(2047, 0);
    }
  }
  laser.resetMaxMove();
  long pos = 0;
  while (pos < 4095) {
    laser.setClipArea(pos, 0, 4095, 4095);
    pos += 100;
    Drawing::drawObject(draw_logo, sizeof(draw_logo) / 4, -centerX, -centerY);
  }
  laser.resetClipArea();
}

// rotating arduino logo
void drawArduino2DRotate()
{
  laser.setColor(RED);
  long centerX, centerY, hX, hY, w, h;
  Drawing::calcObjectBox(draw_arduino, sizeof(draw_arduino) / 4, centerX, centerY, w, h);
  int angle = 0;
  int x = -centerX + 100;
  for (; x < 4396;) {
    laser.setOffset(x, 2048);
    laser.setScale(1.);
    Drawing::drawObjectRotated(draw_arduino, sizeof(draw_arduino) / 4, centerX, centerY, angle % 360);
    angle += 8;
    x += 40;
  }
}

void drawPlane()
{
  laser.setColor(MAGENTA);
  int count = 180;
  float scale = 1;
  laser.setScale(1);
  laser.setOffset(0, 0);
  long x = 4096;
  long y = 4096;
  for (int i = 0; i < count; i++) {
    laser.setScale(scale);
    laser.setOffset(x, y);
    Drawing::drawObject(draw_plane, sizeof(draw_plane) / 4);
    x -= 20 * scale;
    y -= 20;
    scale += 0.01;
  }
}

void drawBike()
{
  laser.setColor(YELLOW);
  int count = 140;
  laser.setScale(3);
  laser.setOffset(0, 0);
  long x = -2500;
  long y = 1000;
  for (int i = 0; i < count; i++) {
    laser.setOffset(x, y);
    Drawing::drawObject(draw_bike, sizeof(draw_bike) / 4);
    x += 50;
  }
}

void drawArduino3D()
{
  laser.setColor(YELLOW);
  laser.setScale(1.);
  laser.setOffset(0, 0);
  long centerX, centerY, w, h;
  Drawing::calcObjectBox(draw_arduino, sizeof(draw_arduino) / 4, centerX, centerY, w, h);
  int count = 360 / 4;
  int angle = 0;
  for (int i = 0; i < count; i++) {
    Drawing::drawObjectRotated3D(draw_arduino, sizeof(draw_arduino) / 4, centerX, centerY, angle % 360, 0, 1000);
    angle += 8;
  }
}

void losGatosCats()
{
  laser.setColor(WHITE);
  int w1 = Drawing::stringAdvance("LOS");
  int w2 = Drawing::stringAdvance("GATOS");
  int w3 = Drawing::stringAdvance("CATS");
  long centerX, centerY, w, h;
  Drawing::calcObjectBox(draw_question, sizeof(draw_question) / 4, centerX, centerY, w, h);

  laser.setOffset(2048, 2048);
  laser.setScale(0.5);
  for (int i = 0; i < 50; i++) Drawing::drawString("LOS", -w1 / 2, SIN((i * 10) % 360) / 100., 1);
  laser.setScale(0.5);
  for (int i = 0; i < 50; i++) Drawing::drawString("GATOS", -w2 / 2 + SIN((i * 10) % 360) / 100., 0, 1);
  laser.setScale(1.);
  for (int i = 0; i < 120; i++) Drawing::drawString("CATS", -w3 / 2 + SIN((i * 4) % 360) / 100., COS((i * 4) % 360) / 100., 1);
  float scale = 0;
  // for (int i = 0; i < 200; i++) {
  //   laser.setScale(scale);
  //   Drawing::drawObject(draw_question, sizeof(draw_question) / 4, -centerX, -centerY);
  //   scale += 0.02;
  // }
}

void beatCal()
{
  laser.setColor(WHITE);
  int w1 = Drawing::stringAdvance("BEAT");
  int w2 = Drawing::stringAdvance("CAL");
  //int w3 = Drawing::stringAdvance("GO STANFORD");
  long centerX, centerY, w, h;
  Drawing::calcObjectBox(draw_question, sizeof(draw_question) / 4, centerX, centerY, w, h);

  laser.setOffset(2048, 2048);
  for (int j=0; j<2; j++) {
      laser.setScale(.75);
      for (int i = 0; i < 75; i++) Drawing::drawString("BEAT", -w1 / 2, SIN((i * 10) % 360) / 100., 1);
      laser.setScale(0.75);
      for (int i = 0; i < 200; i++) Drawing::drawString("CAL", -w2 / 2 + SIN((i * 10) % 360) / 100., 0, 1);
  }
  //laser.setScale(1.);
  //for (int i = 0; i < 120; i++) Drawing::drawString("GO STANFORD", -w3 / 2 + SIN((i * 4) % 360) / 100., COS((i * 4) % 360) / 100., 1);
  //float scale = 0;
  // for (int i = 0; i < 200; i++) {
  //   laser.setScale(scale);
  //   Drawing::drawObject(draw_question, sizeof(draw_question) / 4, -centerX, -centerY);
  //   scale += 0.02;
  // }
}

void whatNext()
{
  laser.setColor(WHITE);
  int w1 = Drawing::stringAdvance("WHAT");
  int w2 = Drawing::stringAdvance("NEXT");
  //int w3 = Drawing::stringAdvance("CATS");
  long centerX, centerY, w, h;
  Drawing::calcObjectBox(draw_question, sizeof(draw_question) / 4, centerX, centerY, w, h);

  laser.setOffset(2048, 2048);
  laser.setScale(0.5);
  for (int i = 0; i < 50; i++) Drawing::drawString("WHAT", -w1 / 2, SIN((i * 10) % 360) / 100., 1);
  // laser.setScale(0.5);
  // for (int i = 0; i < 50; i++) Drawing::drawString("NEXT", -w2 / 2 + SIN((i * 10) % 360) / 100., 0, 1);
  laser.setScale(1.);
  for (int i = 0; i < 120; i++) Drawing::drawString("NEXT", -w2 / 2 + SIN((i * 4) % 360) / 100., COS((i * 4) % 360) / 100., 1);
  float scale = 0;
  for (int i = 0; i < 200; i++) {
    laser.setScale(scale);
    Drawing::drawObject(draw_question, sizeof(draw_question) / 4, -centerX, -centerY);
    scale += 0.02;
  }
    laser.setScale(4);
    for (int i =0 ; i<100; i++) {
        Drawing::drawObject(draw_question, sizeof(draw_question) / 4, -centerX, -centerY);
    }
}

// arduino + heart
void drawWeLove()
{
  laser.setColor(RED);
  int w1 = Drawing::stringAdvance("2019 SENIORS");
  long centerX, centerY, w, h;
  Drawing::calcObjectBox(draw_heart, sizeof(draw_heart) / 4, centerX, centerY, w, h);

  laser.setOffset(2048, 2048);
  long maxMove = 0;
  for (int i = 0; i < 300; i++) {
    laser.setMaxMove(maxMove);
    maxMove += 200;
    laser.setScale(0.4);
    laser.setColor(WHITE);
    Drawing::drawString("2019 SENIORS", -w1 / 2, SIN((i * 10) % 360) / 100.);
    if (i > 100) {
      laser.setColor(RED);
      laser.resetMaxMove();
      laser.setScale(2 + SIN((i * 10) % 360) / 10000.);
      Drawing::drawObject(draw_heart, sizeof(draw_heart) / 4, -centerX, -centerY);
    }
    if (i > 150) {
      maxMove -= 400;
    }
  }
  laser.resetMaxMove();
}

// currently unused, some more objects
void drawObjects()
{
  laser.setColor(WHITE);
  int count = 100;
  laser.setScale(2);
  laser.setOffset(0, 0);

  for (int i = 0; i < count; i++) Drawing::drawObject(draw_island, sizeof(draw_island) / 4);
  for (int i = 0; i < count; i++) Drawing::drawObject(draw_glasses, sizeof(draw_glasses) / 4);
  for (int i = 0; i < count; i++) Drawing::drawObject(draw_smoking, sizeof(draw_smoking) / 4);
}

// draws text as scroller from right to left
void drawScroller(String s, float scale = 0.5, int offsetY = 2048, int speed = 100)
{
  laser.setColor(WHITE);
  int charW = Drawing::advance('I'); // worst case: smallest char
  int maxChar = (4096. / (charW * scale) );
  // some senseful max buffer, don't use a very small scale...
  char buffer[100];
  for (int j = 0; j < maxChar; j++) {
    buffer[j] = ' ';
  }
  laser.setOffset(0, offsetY);
  laser.setScale(scale);
  int scrollX = 0;
  for (int c = 0; c < s.length() + maxChar; c++) {
    int currentScroll = Drawing::advance(buffer[0]);
    while (scrollX < currentScroll) {
      long time = millis();
      int x = -scrollX;;
      int y = 0;
      bool somethingDrawn = false;
      for (int i = 0; i < maxChar; i++) {
        if (buffer[i] != ' ') {
          somethingDrawn = true;
        }
        x += Drawing::drawLetter(buffer[i], x, y);
        if (x > 4096 / scale) {
          break;
        }
      }
      if (!somethingDrawn) {
        scrollX = currentScroll;
      }
      scrollX += speed / scale;
      long elapsed = millis() - time;
      if (elapsed < 50) {
        delay(50 - elapsed);
      }

    }
    scrollX -= currentScroll;
    for (int k = 0; k < maxChar - 1; k++) {
      buffer[k] = buffer[k + 1];
    }
    if (c < s.length()) {
      buffer[maxChar - 1] = s[c];
    } else {
      buffer[maxChar - 1] = ' ';
    }
  }
}

void drawFile(char fileName[]) {
  // fileName = "ARISTCAT.ILD";
  char FQFileName[20] = "ILDA/";
  unsigned long timeout;

  strcat(FQFileName, fileName);
  Ilda_sd sd(sd_chip_select_pin, FQFileName, laser);

  laser.setColor(0xF);
  laser.on();
  //Serial.println("header done");
  //sd.showHeader();
  timeout = millis() + 7000;

  while(!newCode) {

    for (int i = 0; i < sd.total_frames; i++) {
      sd.readFrame();
      sd.readHeader();
      if (_stopDisplay) {
        break;
      }
    }
    sd.restart();
    sd.readHeader();
  }

}


void random_lines(int numIter) {
    //

    laser.setOffset(0, 0);
    laser.setScale(1.0);

    RandomLine line1(2048,2048,2048,2048);
    RandomLine line2(2048,2048,2048,2048);
    RandomLine line3(2048,2048,2048,2048);
    RandomLine line4(2048,2048,2048,2048);

    for (int i=0; i< numIter; i++) {
        laser.setColor(line1.color);
        laser.drawline(line1.x1Pos, line1.y1Pos, line1.x2Pos, line1.y2Pos);
        line1.update();

        laser.setColor(line2.color);
        laser.drawline(line2.x1Pos, line2.y1Pos, line2.x2Pos, line2.y2Pos);
        line2.update();

        laser.setColor(line3.color);
        laser.drawline(line3.x1Pos, line3.y1Pos, line3.x2Pos, line3.y2Pos);
        line3.update();

        laser.setColor(line4.color);
        laser.drawline(line4.x1Pos, line4.y1Pos, line4.x2Pos, line4.y2Pos);
        line4.update();
    }
    laser.setColor(0xF);
}

void random_triangle(int numIter) {
    //

    laser.setOffset(0, 0);
    laser.setScale(1.0);

    RandomLine line1(2048,2048,2048,2048);
    RandomLine line2(2048,2048,2048,2048);
    RandomLine line3(2048,2048,2048,2048);

    while (!newCode) {
        for (int i=0; i< numIter; i++) {

            laser.setColor(line1.color);
            laser.drawline(line1.x1Pos, line1.y1Pos, line1.x2Pos, line1.y2Pos);
            laser.setColor(line2.color);
            laser.drawline(line1.x2Pos, line1.y2Pos, line2.x1Pos, line2.y1Pos);
            laser.setColor(BLUE);
            laser.drawline(line2.x1Pos, line2.y1Pos, line1.x1Pos, line1.y1Pos);

            laser.setColor(line1.color);
            laser.drawline(line2.x2Pos, line2.y2Pos, line3.x1Pos, line3.y1Pos);
            laser.setColor(line3.color);
            laser.drawline(line3.x1Pos, line3.y1Pos, line3.x2Pos, line3.y2Pos);
            laser.setColor(RED);
            laser.drawline(line3.x2Pos, line3.y2Pos, line2.x2Pos, line2.y2Pos);


            line1.update();
            line2.update();
            line3.update();

        }
    }
    laser.setColor(0xF);
}

void random_mesh(int numIter) {
    //

    laser.setOffset(0, 0);
    laser.setScale(1.0);

    RandomLine line1(2048,2048,2048,2048);
    RandomLine line2(2048,2048,2048,2048);
    RandomLine line3(2048,2048,2048,2048);
    while (!newCode) {
        for (int i=0; i< numIter; i++) {


            laser.setColor(line1.color);
            laser.setColor(line2.color);
            laser.setColor(line1.color);
            laser.setColor(line3.color);
            laser.setColor(GREEN);
            laser.drawline(line1.x1Pos, line1.y1Pos, line1.x2Pos, line1.y2Pos);
            laser.setColor(BLUE);
            laser.drawline(line1.x2Pos, line1.y2Pos, line2.x1Pos, line2.y1Pos);
            laser.setColor(WHITE);
            laser.drawline(line2.x1Pos, line2.y1Pos, line2.x2Pos, line2.y2Pos);
            laser.setColor(MAGENTA);
            laser.drawline(line2.x2Pos, line2.y2Pos, line3.x1Pos, line3.y1Pos);
            laser.setColor(RED);
            laser.drawline(line3.x1Pos, line3.y1Pos, line1.x1Pos, line1.y1Pos);

            laser.setColor(BLUE);
            laser.drawline(line1.x1Pos, line1.y1Pos, line2.x2Pos, line2.y2Pos);
            laser.setColor(RED);
            laser.drawline(line2.x2Pos, line2.y2Pos, line1.x2Pos, line1.y2Pos);
            laser.setColor(WHITE);
            laser.drawline(line1.x2Pos, line1.y2Pos, line3.x1Pos, line3.y1Pos);
            laser.setColor(GREEN);
            laser.drawline(line3.x1Pos, line3.y1Pos, line2.x1Pos, line2.y1Pos);
            laser.setColor(MAGENTA);
            laser.drawline(line2.x1Pos, line2.y1Pos, line1.x1Pos, line1.y1Pos);


            line1.update();
            line2.update();
            line3.update();


        }
    }
    laser.setColor(0xF);
}

/*
// Memory use check

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
*/

void sweep() {
    int range = 4095;
    laser.setOffset(0, 0);
    laser.setScale(1.0);

    for (int l=0; l<5; l++){
        for (int i=0; i< range; i+=32) {
            laser.drawline(i, 0, i, range);
        }
    }
    for (int l=0; l<5; l++){
        for (int i=0; i< range; i+=32) {
            laser.drawline(0, i, range, i);
        }
    }
}

void sweep2() {
    int range = 4095;
    int _timeDelta = 200;
    int _timeout1 = 0;
    for (int i=0; i< 15; i++) {
        for (int j=0; j< range; j+=8) {
            while (micros()< _timeout1) {};
            delay(1);
            laser.off();
            delay(1);
            analogWrite(DAC0, 0);
            analogWrite(DAC1, j);
            _timeout1 = micros() + _timeDelta;
            while (micros()< _timeout1) {};
            delay(10);
            laser.on();
            delay(10);
            analogWrite(DAC0, range);
            analogWrite(DAC1, j);
            _timeout1 = micros() + _timeDelta;

        }
    }

}

void laserOff() {
    laser.off();
}

void loop() {

  if (newCode) {
    Serial.print("code:");
    Serial.println(commandCode);
  }

    launchPattern();

}

void callibration() {
    laser.setColor(WHITE);
    laser.setOffset(2048, 2048);
    laser.setScale(1.0);
    while (!newCode) {
        circle();
        laser.drawline(-2047, -2047, 2047, -2047);
        laser.drawline(2047, -2047, 2047, 2047);
        laser.drawline(2047, 2047, -2047, 2047);
        laser.drawline(-2047, 2047, -2047, -2047);
        laser.off();
    }
}

void launchPattern(){

    newCode = false;
    _stopDisplay = false;
    if (newCode) {
        Serial.print("launch");
        Serial.println(commandCode);
    }
    switch(commandCode) {
        case 0:
            offset = 0;
            laserOff();
            break;
        case 1:
            callibration();
            break;

        case 2:
            countDown('9');
            break;

        case 3:
            laserShow();
            break;

        case 4:
            drawPlane();
            break;

        case 5:
            drawScroller(String("CONGRATULATIONS TO CLASS OF 2019"),0.5,2048,100);
            break;

        case 6:
            drawWeLove();
            break;

        case 7:
            cats();
            break;

        case 8:
            losGatosCats();
            break;

        case 9:
            rotateCube(400);
            break;

        case 10:
            drawBike();
            break;

        case 11:
            globe(200);
            break;

        case 12:
            drawArduino3D();
            break;

        case 13:
            drawScroller(String("WAY TO GO SENIORS"),0.25,2048,50);
            break;

        case 14:
            congrats();
            break;

        case 15:
            random_mesh(3000);
            break;

        case 16:
            random_lines(2000);
            break;

        case 17:
            random_triangle(2000);
            break;

        case 18:
            random_mesh(2000);
            break;

        case 19:
            if (millis() > timeoutBeatCal) {
                timeoutBeatCal = millis() + 15 * 60 * 1000;
                beatCal();
            } else {
                whatNext();
            }

        case 20:
            letterEffect();
            break;

        case 21:
            random_lines(2000);
            if (!_auto) { offset=30;}
            break;

        default:
            if (commandCode >= 22) {
                selectDrawFile(commandCode-22-offset);
            }
            break;
    }
}

void selectDrawFile(int value){

    if (value < numIldFiles && value >= 0) {
        drawFile(ildFileNames[value]);
        } else {
        Serial.println("File number exceeded");
        }
}

void getCodeISR(){
    //
    // read code from arduino
    //

    /*
    int tempCommandCode = 0;
    if (digitalRead(ARD_SRDY_PIN)) {
        for (int i=0; i< NUM_CONTROL_BITS; i++) {
            bitWrite(tempCommandCode, i, digitalRead(arduinoComPins[i]);
        }
        if (digitalRead(ARD_SRDY_PIN)) {
            commandCode = tempCommandCode;
        }
    }
    return commandCode;
    */
    for (int i=0; i< NUM_CONTROL_PINS; i++) {
        bitWrite(commandCode, i, digitalRead(arduinoComPins[i]));
    }

    if (commandCode==0) {
        _stopDisplay = true;
    }
    if (commandCode==2) {
        _auto = true;
        offset = 0;
    } else {
        _auto = false;
    }
    // Serial.print("ISR ");
    // Serial.println(commandCode);
    newCode = true;
}

void timer3ISR() {
  if (_auto) {
        commandCode = random(NUM_SCENES-1) + 1;  // don't select 0
        newCode = true;
  }
  // Serial.println("Timer ISR");
}