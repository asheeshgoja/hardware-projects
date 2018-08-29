
#include <Wire.h>


const int WTIME = 0xff; // 2.7ms – minimum Wait time
const int PTIME = 0xff; // 2.7ms – minimum Prox integration time
const int PPCOUNT = 1; // Minimum prox pulse count
const int APDS9190_I2C_ADDR = 0x39;
const int INFRARED_MOSFET_PIN = 4;

bool wireWriteDataByte(uint8_t reg, uint8_t val);
int wireReadDataBlock(   uint8_t reg, uint8_t *val, unsigned int len);
uint16_t Read_Word(uint8_t reg);
bool wireWriteByte(uint8_t val);


void setup() {

  pinMode(INFRARED_MOSFET_PIN, OUTPUT);
  digitalWrite(INFRARED_MOSFET_PIN, HIGH);


  // put your setup code here, to run once:
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(9600);

  // put your main code here, to run repeatedly:
  Wire.beginTransmission(APDS9190_I2C_ADDR); // transmit to device #112 (0x70)

  //WriteRegData(0x00, 0); //Disable and Powerdown
  if ( !wireWriteDataByte(0x00, 0) ) {
    Serial.println("WriteRegData  0x00, 0 failed!");
    return false;
  }

  //WriteRegData (0x02, PTIME);
  if ( !wireWriteDataByte(0x02, PTIME) ) {
    Serial.println("WriteRegData 0x02, PTIME failed!");
    return false;
  }


  //WriteRegData (0x03, WTIME);
  if ( !wireWriteDataByte(0x03, WTIME) ) {
    Serial.println("WriteRegData 0x03, WTIME failed!");
    return false;
  }


  //WriteRegData (0x0e, PPCOUNT);
  if ( !wireWriteDataByte(0x0e, PPCOUNT) ) {
    Serial.println("WriteRegData failed!");
    return false;
  }


  uint8_t PDRIVE, PDIODE, PGAIN, AGAIN;
  PDRIVE = 0; //100mA of LED Power
  PDIODE = 0x20; // Ch1 Diode
  PGAIN = 0; //1x Prox gain

  //WriteRegData (0xf, PDRIVE | PDIODE | PGAIN | AGAIN);
  if ( !wireWriteDataByte(0x0f, PDRIVE | PDIODE | PGAIN | AGAIN) ) {
    Serial.println("WriteRegData failed!");
    return false;
  }


  uint8_t WEN, PEN, PON;
  WEN = 8; // Enable Wait
  PEN = 4; // Enable Prox
  PON = 1; // Enable Power On

  //WriteRegData (0, WEN | PEN | PON); // WriteRegData(0,0x0f);
  if ( !wireWriteDataByte(0x00, WEN | PEN | PON) ) {
    Serial.println("WriteRegData failed!");
    return false;
  }

  delay(12); //Wait for 12 ms
  uint16_t Prox_data;
  Prox_data = Read_Word(0x18);

  //Wire.endTransmission();
  Serial.print(Prox_data);
  Serial.println(": init success :) ");

}

void loop() {

}

//void  WriteRegData(uint8 reg, uint8 data)
//{
//  m_I2CBus.WriteI2C(0x39, 0x80 | reg, 1, &data);
//}

uint16_t Read_Word(uint8_t reg)
{
  uint8_t barr[2];
  memset(barr, 0, 2);

  //m_I2CBus.ReadI2C(0x39, 0xA0 | reg, barr, 2);
  wireReadDataBlock(0xA0 | reg, barr, 2);
  Serial.print(barr[0]); Serial.println(barr[1]);
  return (uint16_t)(barr[0] + 256 * barr[1]);

}

bool wireWriteDataByte(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(APDS9190_I2C_ADDR);
  Wire.write(reg);
  Wire.write(val);
  if ( Wire.endTransmission() != 0 ) {
    return false;
  }

  return true;
}

int wireReadDataBlock(   uint8_t reg, uint8_t *val, unsigned int len)
{
  unsigned char i = 0;

  /* Indicate which register we want to read from */
  if (!wireWriteByte(reg)) {
    return -1;
  }

  /* Read block data */
  Wire.requestFrom(APDS9190_I2C_ADDR, len);
  while (Wire.available()) {
    if (i >= len) {
      return -1;
    }
    val[i] = Wire.read();
    Serial.print("wire read :"); Serial.println(val[i]);
    i++;
  }
  return i;
}

bool wireWriteByte(uint8_t val)
{
  Wire.beginTransmission(APDS9190_I2C_ADDR);
  Wire.write(val);
  if ( Wire.endTransmission() != 0 ) {
    return false;
  }
  return true;
}


