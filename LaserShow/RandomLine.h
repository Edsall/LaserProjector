#ifndef RANDOMLINE_H
#define RANDOMLINE_H

#include "Arduino.h"
#include "Basics.h"

// how close the points need to be to change colors.
#define RANDOMLINE_FLIP_X  100
#define RANDOMLINE_FLIP_Y  100

class RandomLine
{
public:

    // RandomLine is initialized with the starting points
  RandomLine(int x1Pos, int y1Pos, int x2Pos, int y2Pos);

  //! send the laser to the given position, scaled and translated and line clipped.
  void update();


  int x1Pos= 0;
  int y1Pos= 0;
  int x2Pos= 0;
  int y2Pos= 0;
  
  byte color= 0xF;

private:
  //! send X/Y to DAC
  void selectNewTarget(int xPos, int yPos, int& _xPos, int& _yPos,
                    int& _newTargetX, int& _newTargetY, int& _newStepX, int& _newStepY);

  bool pointsClose();

  byte selectNewColor();

  int _x1Pos= 0;
  int _y1Pos= 0;
  int _targetX1= 0;
  int _targetY1= 0;
  int _stepX1= 0;
  int _stepY1= 0;

  int _x2Pos= 0;
  int _y2Pos= 0;
  int _targetX2= 0;
  int _targetY2= 0;
  int _stepX2= 0;
  int _stepY2= 0;


};

#endif
