#include "ILDA_SD.h"

#include <SPI.h>
#include <SD.h>
#define EMBEDDED_DAC
// const int chipSelect = 4;

Ilda_sd::Ilda_sd(int chipSelect, char *fileName, Laser &laser)
{
  _laser = &laser;
  _chipSelect = chipSelect;
  init();
  Serial.print("Using file:");
  Serial.println(fileName);

  _timeDelta = 200;
  _timeout1 = 0;

  openFile(fileName);
  readHeader();
}

void Ilda_sd::init()
{

  if (!SD.begin(_chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void Ilda_sd::openFile(String text) {
  _dataFile = SD.open(text);
}

void Ilda_sd::closeFile() {
  _dataFile.close();
}

void Ilda_sd::restart() {
  _dataFile.seek(0);
}


void Ilda_sd::header_print(char array[], unsigned int len)
{
  char buffer[16];
  for (unsigned int i = 0; i < len; i++)
  {
    buffer[i] = array[i];
  }
  buffer[len + 1] = '\0';
  Serial.write(buffer, len);
}
void Ilda_sd::header_println(char array[], unsigned int len)
{
  header_print(array, len);
  Serial.println("");
}


void Ilda_sd::showHeader() {

  Serial.print("String:");
  header_println(header.fields.ilda_string, sizeof(header.fields.ilda_string));
  Serial.print("Reserved1:");
  header_println(header.fields.reserved1, sizeof(header.fields.reserved1));
  Serial.print("Format:");
  Serial.println(header.fields.format, HEX);
  Serial.print("Frame:");
  header_println(header.fields.frame, sizeof(header.fields.frame));
  Serial.print("Company:");
  header_println(header.fields.company, sizeof(header.fields.company));
  Serial.print("Number of records:");
  Serial.println(number_records);
  Serial.print("Palette:");
  Serial.println(__builtin_bswap16(header.fields.palette), HEX);
  Serial.print("Total Frames:");
  Serial.println(total_frames);
  Serial.print("Projector:");
  Serial.println(header.fields.projector, HEX);
  Serial.print("Reserved2:");
  Serial.println(header.fields.reserved2, HEX);
}


void Ilda_sd::readHeader() {

  _dataFile.read(header.data, 32);

  number_records = __builtin_bswap16(header.fields.number_records);
  total_frames = __builtin_bswap16(header.fields.total_frames);
}


char* Ilda_sd::getFileName() {
  return _fileName;
}

void Ilda_sd::showRecord(recordType1 recordFormat1) {
  Serial.print("X:");
  Serial.println(__builtin_bswap16(recordFormat1.fields.x));
  Serial.print("Y:");
  Serial.println(__builtin_bswap16(recordFormat1.fields.y));
  Serial.print("Status:");
  Serial.println(recordFormat1.fields.status, HEX);
  Serial.print("Color:");
  Serial.println(recordFormat1.fields.colorIndex, HEX);
}

void Ilda_sd::showRecord(recordType0 recordFormat0) {
  Serial.print("X:");
  Serial.print(recordFormat0.fields.x);
  Serial.print(" Y:");
  Serial.print(recordFormat0.fields.y);
  Serial.print(" Z:");
  Serial.print(recordFormat0.fields.z);
  Serial.print(" Status:");
  Serial.print(recordFormat0.fields.status, HEX);
  Serial.print(" Color:");
  Serial.println(recordFormat0.fields.colorIndex, HEX);
}

void Ilda_sd::getRecord() {
  recordType0 rawRecord;
  _dataFile.read(&rawRecord, 8);
  _record.fields.x = (short)__builtin_bswap16(rawRecord.fields.x);
  _record.fields.y = (short)__builtin_bswap16(rawRecord.fields.y);
  _record.fields.z = (short)__builtin_bswap16(rawRecord.fields.z);
  _record.fields.status = rawRecord.fields.status;

    // check for colorIndex having values out of range
  if (rawRecord.fields.colorIndex > 63) {
    _record.fields.colorIndex = 56;
  } else {
    _record.fields.colorIndex = rawRecord.fields.colorIndex;
  }
}

void Ilda_sd::array_to_string(char array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    buffer[i] = array[i];
  }
  buffer[len + 1] = '\0';
}

void Ilda_sd::readFrame() {
    int count = 0;
  for (int i = 0; i < number_records; i++)  {
    getRecord();
#ifdef EMBEDDED_DAC

    while (micros()< _timeout1) {};
    if (_record.fields.status & 0x40) {
        _laser->off();
    } else {
        _laser->on();
    }
    _laser->setColor(ildaColorMap[_record.fields.colorIndex]);

    analogWrite(DAC0, ((_record.fields.x + 32768) >> 4) );
    analogWrite(DAC1, ((_record.fields.y + 32768) >> 4) );
    _timeout1 = micros() + _timeDelta;
    //delayMicroseconds(200);
#else

    //Serial.print("[");
    //Serial.print(i);
    //Serial.print("]") ;
    //showRecord(_record);
#endif
  }
  //Serial.println(count);
  //Serial.println("Done with frame");
}
