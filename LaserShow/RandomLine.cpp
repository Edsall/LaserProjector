#include "RandomLine.h"
#include "Laser.h"

RandomLine::RandomLine(int x1, int y1, int x2, int y2)
{
    Serial.println("Randomline initialized");
    x1Pos = x1;
    y1Pos = y1;
    x2Pos = x2;
    y2Pos = y2;
    selectNewTarget(x1Pos, y1Pos, _x1Pos, _y1Pos, _targetX1, _targetY1, _stepX1, _stepY1);
    // selectNewTarget(x2Pos, y2Pos, _x2Pos, _y2Pos, _targetX2, _targetY2, _stepX2, _stepY2);
    color = selectNewColor();


}

void RandomLine::selectNewTarget(int xPos, int yPos, int& _xPos, int& _yPos,
                                int& _newTargetX, int& _newTargetY, int& _newStepX, int& _newStepY)
{
    int _deltaX;
    int _deltaY;

    _xPos = xPos << 12;
    _yPos = yPos << 12;

    _newTargetX = random(4095) << 12;
    _newTargetY = random(4095) << 12;

    _deltaX = _newTargetX-_xPos;
    _deltaY = _newTargetY-_yPos;

    if (abs(_deltaX) > abs(_deltaY)) {
        _newStepX = (_deltaX)/(abs(_deltaX) >> 16);
        _newStepY = (_deltaY)/(abs(_deltaX) >> 16);
    } else {                                    
        _newStepX = (_deltaX)/(abs(_deltaY) >> 16);
        _newStepY = (_deltaY)/(abs(_deltaY) >> 16);
    }


}
    // draw random line of different colors
    //
void RandomLine::update()
{

    if (pointsClose()) {
        color = selectNewColor();
    }

    _x1Pos += _stepX1;
    _y1Pos += _stepY1;
    _x2Pos += _stepX2;
    _y2Pos += _stepY2;
    
    x1Pos = ((_x1Pos) >> 12);
    y1Pos = ((_y1Pos) >> 12);
    x2Pos = ((_x2Pos) >> 12);
    y2Pos = ((_y2Pos) >> 12);
    
    // if ((x1Pos == (_targetX1 >> 12)) && (y1Pos == (_targetY1 >> 12))) {
    if ((abs(_x1Pos - _targetX1) >> 16 == 0) && (abs(_y1Pos-_targetY1) >> 16 == 0)) {
        selectNewTarget(x1Pos, y1Pos, _x1Pos, _y1Pos, _targetX1, _targetY1, _stepX1, _stepY1);
    }

    //if ((x2Pos == (_targetX2 >> 12)) && (y2Pos == (_targetY2 >> 12))) {
    if ((abs(_x2Pos-_targetX2) >> 16 == 0) && (abs(_y2Pos-_targetY2) >> 16 == 0)) {
        selectNewTarget(x2Pos, y2Pos, _x2Pos, _y2Pos, _targetX2, _targetY2, _stepX2, _stepY2);
    }

}

bool RandomLine::pointsClose()
{
    bool close = false;

    if (abs(x1Pos-x2Pos) > RANDOMLINE_FLIP_X ) {
        return false;
    }

    if (abs(y1Pos-y2Pos) > RANDOMLINE_FLIP_Y ) {
        return false;
    }
    return true;
}

byte RandomLine::selectNewColor()
{
    byte color =  validColors[random(NUM_VALID_COLORS)];
    return color;
}

