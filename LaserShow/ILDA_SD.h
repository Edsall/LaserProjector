// See LICENSE file for details
// Copyright 2016 Florian Link (at) gmx.de
#ifndef ILDA_SD_H
#define ILDA_SD_H
#include <SD.h>

#include "Arduino.h"
#include "Laser.h"

union headerType {
  struct fields {
    char ilda_string [4] ;
    char reserved1 [3];
    byte format;
    char frame[8];
    char company[8];
    short unsigned int number_records;
    short unsigned int palette;
    short unsigned int total_frames;
    byte projector;
    byte reserved2;

  } fields;
  char data[32];
};

union recordType0 {
  struct fields {
    short x;
    short y;
    short z;
    char status;
    char colorIndex;
  } fields;
  char data[8];
};

union recordType1 {
  struct fields {
    short x;
    short y;
    char status;
    char colorIndex;
  } fields;
  char data[6];
};

const unsigned short ildaColorMap[] PROGMEM = {4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 6, 6, 6, 6, 6,
6, 6, 6, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 3, 3, 3,
3, 3, 3, 1, 1, 1, 1, 1,
1, 1, 1, 1, 5, 5, 5, 5,
5, 5, 5, 5, 7, 7, 7, 7,
7, 7, 4, 4, 4, 4, 4};

class Ilda_sd
{
  public:
    //! The ilda_sd is initialized with the ilda_sdPin,
    //! which selects the digital pin that turns the ilda_sd pointer on/off.
    Ilda_sd(int chipSelect, char fileName[16], Laser &laser);

    void init();
    void readHeader();
    char* getFileName();
    void showRecord(recordType0 recordFormat0);
    void showRecord(recordType1 recordFormat1);
    void showHeader();
    void getRecord();
    void openFile(String text);
    void closeFile();
    void readFrame();
    void restart();

    headerType header;
    recordType0 _record;
    int number_records;
    int total_frames;


  private:
    void header_print(char array[], unsigned int len);
    void header_println(char array[], unsigned int len);
    void array_to_string(char array[], unsigned int len, char buffer[]);

    File _dataFile;
    char _fileName[16];
    int _chipSelect;

    unsigned long _timeout1, _timeDelta;
    Laser *_laser;
};

#endif
