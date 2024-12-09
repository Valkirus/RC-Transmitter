#include <Arduino.h>

#include "Wire.h"
#include "Helpers.h"
#include "Globals.h"
#include "Inputs.h"
#include "NRF.h"
#include "Screen.h"
#include "Page.h"

const bool DEBUG_PRINT = false;
#define UpdateScreenInterval 100
unsigned long drawTimerStart = 0;
unsigned long drawTimerEnd = 0;
bool drewScreen = false;

void setup()
{
  Serial.begin(9600);
  setupScreen();
  setupInputs();
  Wire.begin();
  setupNRF();
  loadValues(); 
}

void loop()
{
  drewScreen = false;
  every(UpdateScreenInterval)
  {
    mpu.update();

    if (previousPage != currentPage)
    {
      currentPage->init();
      previousPage = currentPage;
    }

    
    u8g2.clearBuffer();
    currentPage->loop();    
    u8g2.sendBuffer();
    
    drewScreen = true;
  }

  if (!drewScreen)
    rc_data.joy1_X = getJoyValue(A).x;
    rc_data.joy1_Y = getJoyValue(A).y;
    rc_data.joy2_X = getJoyValue(B).x;
    rc_data.joy2_Y = getJoyValue(B).y;
    rc_data.toggle1 = getSwitchValue(A);
    rc_data.toggle2 = getSwitchValue(B);
    rc_data.toggle3 = getSwitchValue(C);
    rc_data.toggle4 = getSwitchValue(D);
    sendNRFData();
}
