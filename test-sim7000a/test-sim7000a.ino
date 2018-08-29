#include "CellModemModule.h"

void setup() {
  // put your setup code here, to run once:
  setupCellModemModule();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  sendPackageCountMessage();
  delay(1000);
}
