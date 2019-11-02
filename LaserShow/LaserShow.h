#ifndef LASERSHOW_H
#define LASERSHOW_H

#define PARALLEL_COM  // use parallel communication

#define laser_pin_R 47
#define laser_pin_G 49
#define laser_pin_B 51
#define duo_control_pin 53

#define sd_chip_select_pin 4

#define interruptPin 7

#ifdef PARALLEL_COM

#define NUM_CONTROL_PINS 6
byte arduinoComPins[NUM_CONTROL_PINS] = {33,31,29,27,25,23}; // pins connected to arduino

#else

#define DMX_RX_PIN 23  // note these overlap with above
#define DMX_TX_PIN 25  // note these overlap with above

#endif

#define numIldFiles 48
char ildFileNames [numIldFiles] [13] = {{"30STOON2.ILD"}, {"ANIM8TST.ILD"},{"ATOMIC3.ILD"}, {"BALLET1.ILD"},
                                        {"BIRD2.ILD"}, {"BONDGIRL.ILD"}, {"BOXA.ILD"}, {"BOXER0.ILD"},
                                        {"BOXINGKR.ILD"}, {"BRIANZ2.ILD"}, {"BRKDANS3.ILD"}, {"BRKDNCA.ILD"},
                                        {"CANGOOSE.ILD"}, {"CATWALK0.ILD"}, {"CATWALK1.ILD"},{"CATWALK3.ILD"},
                                        {"CATWALK4.ILD"}, {"CELLFONE.ILD"}, {"CHIMPWLK.ILD"}, {"DANCEGLS.ILD"},
                                        {"DANCER4.ILD"}, {"DANCY.ILD"},  {"EVOMORF.ILD"},{"GETUPNDC.ILD"},
                                        {"GOODLUCK.ILD"}, {"HORSE1.ILD"}, {"HORSERDR.ILD"}, {"HOTSTUFF.ILD"},
                                        {"LANDING1.ILD"}, {"LINDSEYS.ILD"}, {"MFDNCRS1.ILD"},
                                        {"MANDANCE.ILD"}, {"OLDTOON.ILD"}, {"PANTHER.ILD"}, {"ROOSTER.ILD"},
                                        {"RUNNER.ILD"}, {"SEXYCTWK.ILD"}, {"SEXYDANS.ILD"}, {"SHAKEYAS.ILD"},
                                        {"SHOPPING.ILD"}, {"SMOOTH.ILD"}, {"SOCCER1.ILD"}, {"SOCCER2.ILD"},
                                        {"SOCCER3.ILD"}, {"STAIRWAY.ILD"}, {"TAKEOFF9.ILD"}, {"TRIQETRA.ILD"},
                                        {"SPACCRET.ILD"}};


/* killed files

glitch  - slow questionable


*/
#endif