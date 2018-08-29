
#include <Wire.h>


/*
   Defines
*/

#define APDS9190_I2C_SLAVE_ADDRESS  0x39

#define APDS_DISABLE_PS  0
#define APDS_ENABLE_PS  1

#define APDS_DISABLE_ALS  0
#define APDS_ENABLE_ALS_WITH_INT  1
#define APDS_ENABLE_ALS_NO_INT  2

#define APDS_DISABLE_GESTURE  0
#define APDS_ENABLE_GESTURE  1

#define APDS_ALS_POLL_SLOW  0 // 1 Hz (1s)
#define APDS_ALS_POLL_MEDIUM  1 // 10 Hz (100ms)
#define APDS_ALS_POLL_FAST  2 // 20 Hz (50ms)

#define APDS_ALS_CALIBRATION  0
#define APDS_PS_CALIBRATION  1

#define APDS9190_ENABLE_REG  0x80
#define APDS9190_ATIME_REG  0x81
#define APDS9190_WTIME_REG  0x83
#define APDS9190_AILTL_REG  0x84
#define APDS9190_AILTH_REG  0x85
#define APDS9190_AIHTL_REG  0x86
#define APDS9190_AIHTH_REG  0x87
#define APDS9190_PITLO_REG  0x89
#define APDS9190_PITHI_REG  0x8B
#define APDS9190_PERS_REG  0x8C
#define APDS9190_CONFIG_REG  0x8D
#define APDS9190_PPULSE_REG  0x8E
#define APDS9190_CONTROL_REG  0x8F

#define APDS9190_STATUS_REG  0x93


#define APDS9190_PDATA_REG  0x9C

#define APDS9190_TEST2_REG  0xC3  // use for soft_reset


/* Register Value define : ENABLE */
#define APDS9190_PWR_DOWN  0x00  /* PON = 0 */
#define APDS9190_PWR_ON  0x01  /* PON = 1 */
#define APDS9190_ALS_ENABLE  0x02  /* AEN */
#define APDS9190_PS_ENABLE  0x04  /* PEN */
#define APDS9190_WAIT_ENABLE  0x08  /* WEN */
#define APDS9190_ALS_INT_ENABLE  0x10  /* AIEN */
#define APDS9190_PS_INT_ENABLE  0x20  /* PIEN */
#define APDS9190_GESTURE_ENABLE  0x40  /* GEN */

/* Register Value define : CONTROL */
#define APDS9190_PDRIVE_100mA 0x00
#define APDS9190_PDRIVE_50mA  0x40
#define APDS9190_PDRIVE_25mA  0x80
#define APDS9190_PDRIVE_12_5mA  0xC0

#define APDS9190_PGAIN_1X  0x00
#define APDS9190_PGAIN_2X  0x04
#define APDS9190_PGAIN_4X  0x08
#define APDS9190_PGAIN_8X  0x0C

#define APDS9190_AGAIN_1X  0x00
#define APDS9190_AGAIN_4X  0x01
#define APDS9190_AGAIN_16X  0x02
#define APDS9190_AGAIN_64X  0x03

#define CMD_FORCE_INT  0xE4
#define CMD_CLR_PS_INT  0xE5
#define CMD_CLR_ALS_INT  0xE6
#define CMD_CLR_ALL_INT  0xE7


/* Register Value define : ATIME */
#define APDS9190_100MS_ADC_TIME 0xD6  /* 100.8ms integration time */
#define APDS9190_50MS_ADC_TIME  0xEB  /* 50.4ms integration time */
#define APDS9190_27MS_ADC_TIME  0xF6  /* 24ms integration time */

/* Register Value define : PERS */
#define APDS9190_PPERS_0  0x00  /* Every proximity ADC cycle */
#define APDS9190_PPERS_1  0x10  /* 1 consecutive proximity value out of range */
#define APDS9190_PPERS_2  0x20  /* 2 consecutive proximity value out of range */
#define APDS9190_PPERS_3  0x30  /* 3 consecutive proximity value out of range */
#define APDS9190_PPERS_4  0x40  /* 4 consecutive proximity value out of range */
#define APDS9190_PPERS_5  0x50  /* 5 consecutive proximity value out of range */
#define APDS9190_PPERS_6  0x60  /* 6 consecutive proximity value out of range */
#define APDS9190_PPERS_7  0x70  /* 7 consecutive proximity value out of range */
#define APDS9190_PPERS_8  0x80  /* 8 consecutive proximity value out of range */
#define APDS9190_PPERS_9  0x90  /* 9 consecutive proximity value out of range */
#define APDS9190_PPERS_10  0xA0  /* 10 consecutive proximity value out of range */
#define APDS9190_PPERS_11  0xB0  /* 11 consecutive proximity value out of range */
#define APDS9190_PPERS_12  0xC0  /* 12 consecutive proximity value out of range */
#define APDS9190_PPERS_13  0xD0  /* 13 consecutive proximity value out of range */
#define APDS9190_PPERS_14  0xE0  /* 14 consecutive proximity value out of range */
#define APDS9190_PPERS_15  0xF0  /* 15 consecutive proximity value out of range */

#define APDS9190_APERS_0  0x00  /* Every ADC cycle */
#define APDS9190_APERS_1  0x01  /* 1 consecutive als value out of range */
#define APDS9190_APERS_2  0x02  /* 2 consecutive als value out of range */
#define APDS9190_APERS_3  0x03  /* 3 consecutive als value out of range */
#define APDS9190_APERS_5  0x04  /* 5 consecutive als value out of range */
#define APDS9190_APERS_10  0x05  /* 10 consecutive als value out of range */
#define APDS9190_APERS_15  0x06  /* 15 consecutive als value out of range */
#define APDS9190_APERS_20  0x07  /* 20 consecutive als value out of range */
#define APDS9190_APERS_25  0x08  /* 25 consecutive als value out of range */
#define APDS9190_APERS_30  0x09  /* 30 consecutive als value out of range */
#define APDS9190_APERS_35  0x0A  /* 35 consecutive als value out of range */
#define APDS9190_APERS_40  0x0B  /* 40 consecutive als value out of range */
#define APDS9190_APERS_45  0x0C  /* 45 consecutive als value out of range */
#define APDS9190_APERS_50  0x0D  /* 50 consecutive als value out of range */
#define APDS9190_APERS_55  0x0E  /* 55 consecutive als value out of range */
#define APDS9190_APERS_60  0x0F  /* 60 consecutive als value out of range */

/* Register Value define : STATUS */
#define APDS9190_STATUS_ASAT  0x80  /* ALS Saturation */
#define APDS9190_STATUS_PSAT  0x40  /* PS Saturation - analog saturated, not a proximity detection */
#define APDS9190_STATUS_PINT  0x20  /* PS Interrupt status */
#define APDS9190_STATUS_AINT  0x10  /* ALS Interrupt status */
#define APDS9190_STATUS_PVALID  0x02  /* PS data valid status */
#define APDS9190_STATUS_AVALID  0x01  /* ALS data valid status */

#define APDS9190_PPULSE_FOR_PS    8
#define APDS9190_PPULSE_LEN_FOR_PS  APDS9190_PULSE_LEN_16US
#define APDS9190_PDRVIE_FOR_PS    APDS9190_PDRIVE_100mA
#define APDS9190_PGAIN_FOR_PS   APDS9190_PGAIN_4X

#define APDS9190_PS_DETECTION_THRESHOLD 50
#define APDS9190_PS_HSYTERESIS_THRESHOLD  30

#define APDS9190_NEAR_THRESHOLD_LOW 0
#define APDS9190_FAR_THRESHOLD_HIGH 255

#define APDS9190_ALS_CALIBRATED_LUX 300
#define APDS9190_ALS_CALIBRATED_CCT 5000

#define APDS9190_PS_CALIBRATED_XTALK_BASELINE  10
#define APDS9190_PS_CALIBRATED_XTALK  20

#define APDS9190_ALS_THRESHOLD_HSYTERESIS  20 /* 20 = 20% */



typedef enum
{
  APDS9190_ALS_RES_24MS  = 0,    /* 24ms integration time */
  APDS9190_ALS_RES_50MS  = 1,    /* 50ms integration time */

  APDS9190_ALS_RES_100MS = 2     /* 100ms integration time */
}
APDS9190_als_res_e;

typedef enum
{
  APDS9190_ALS_GAIN_1X    = 0,    /* 1x AGAIN */
  APDS9190_ALS_GAIN_4X    = 1,    /* 4x AGAIN */
  APDS9190_ALS_GAIN_16X   = 2,    /* 16x AGAIN */
  APDS9190_ALS_GAIN_64X   = 3     /* 64x AGAIN */
}
APDS9190_als_gain_e;



struct APDS9190_data {
  unsigned int enable;
  unsigned int atime;
  unsigned int wtime;
  unsigned int ailt;
  unsigned int aiht;
  unsigned int pilt;
  unsigned int piht;
  unsigned int pers;
  unsigned int _config;
  unsigned int ppulse;
  unsigned int control;

  unsigned int pdata;

  unsigned int enable_ps_sensor;
  unsigned int enable_als_sensor;
  unsigned int enable_gesture_sensor;

  /* PS parameters */
  unsigned int ps_threshold;
  unsigned int ps_hysteresis_threshold;   /* always lower than ps_threshold */
  unsigned int ps_detection;    /* 0 = near-to-far; 1 = far-to-near */
  unsigned int ps_data;       /* to store PS data */
  unsigned int ps_ppulse;


  /* ALS parameters */
  unsigned int als_threshold_l;   /* low threshold */
  unsigned int als_threshold_h;   /* high threshold */
  unsigned int als_data;      /* to store ALS data */
  int als_prev_lux;         /* to store previous lux value */

  unsigned int als_gain;    /* needed for Lux calculation */
  unsigned int als_poll_delay;  /* needed for light sensor polling : micro-second (us) */
  unsigned int als_atime_index;   /* storage for als integratiion time */
  unsigned int als_again_index;   /* storage for als GAIN */

  unsigned int lux;     /* lux */

};

struct APDS9190_data global_data;




char dummyc[100];
volatile int isr_flag = 0;
int ch0data, ch1data;
int pdata;
int timing;
const int INFRARED_MOSFET_PIN = 4;

#include "UPS_ADXL345.h"
ADXL345 adxl;
int displayOrientationAndGetZaxis();

void setup()
{
  Serial.begin(9600);
  Serial.println("init");

  pinMode(INFRARED_MOSFET_PIN, OUTPUT);
  digitalWrite(INFRARED_MOSFET_PIN, HIGH);

  Wire.begin(); // join i2c bus (address optional for master)
  //attachInterrupt(0, i2c_isr, FALLING);

  //init standard APDS-9190 sensor
  i2c_smbus_write_byte_data(0x82, 0xFF); //ptime
  APDS9190_set_ppulse(4);
  APDS9190_set_control(42);
  i2c_smbus_write_byte_data(APDS9190_ENABLE_REG, 0x07); //PS and ALS
  i2c_smbus_write_byte_data(APDS9190_ATIME_REG, 0xFF); //ATIME


  timing = i2c_smbus_read_byte_data(0x81);

  adxl.powerOn();
  Serial.println(timing, HEX);
}

void loop()
{
  unsigned char adc_data[10];
  if (isr_flag == 1)
  {
    isr_flag = 0;
    APDS9190_clear_interrupt(0xe7);
  }
  //read APDS-9190 sensor
  i2c_smbus_read_i2c_block_data(0x94, 4, adc_data);

  ch0data = (adc_data[1] << 8) & 0xFF00;
  ch0data |= (adc_data[0] & 0xFF);
  ch1data = (adc_data[3] << 8) & 0xFF00;
  ch1data |= (adc_data[2] & 0xFF);

  i2c_smbus_read_i2c_block_data(0x98, 2, adc_data);
  pdata = (adc_data[1] << 8) & 0xFF00;
  pdata |= (adc_data[0] & 0xFF);

  sprintf(dummyc, "Ch0: %d, Ch1: %d, PS: %d", ch0data, ch1data, pdata);
  Serial.println(dummyc);


  //timing = i2c_smbus_read_byte_data(0x81);
  //Serial.println(timing, HEX);

  delay(20);

  displayOrientationAndGetZaxis();

  delay(20);

}

void i2c_isr()
{
  //Serial.println("i2c_isr");
  isr_flag = 1;
}

static int i2c_smbus_write_byte(int value)
{
  Wire.beginTransmission(APDS9190_I2C_SLAVE_ADDRESS);
  Wire.write(value);
  Wire.endTransmission();

  return 0;
}

static int i2c_smbus_write_byte_data(int reg, int value)
{
  Wire.beginTransmission(APDS9190_I2C_SLAVE_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();

  return 0;
}


static int i2c_smbus_write_word_data(int reg, int value)
{
  Wire.beginTransmission(APDS9190_I2C_SLAVE_ADDRESS);
  Wire.write(reg);
  Wire.write(value & 0xFF);
  Wire.write((value >> 8) & 0xFF);
  Wire.endTransmission();

  return 0;
}

static int i2c_smbus_read_byte_data(int reg)
{
  int value = -1;

  Wire.beginTransmission(APDS9190_I2C_SLAVE_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();

  //  Wire.beginTransmission(APDS9190_I2C_SLAVE_ADDRESS);
  Wire.requestFrom(APDS9190_I2C_SLAVE_ADDRESS, 1);

  while (Wire.available())
  {
    value = Wire.read();
  }
  //  Wire.endTransmission();
  return value;
}

static int i2c_smbus_read_i2c_block_data(int reg, int bytes_read, unsigned char *recv_data)
{
  int value = -1;
  int i = 0;

  if (!recv_data) return -1;

  Wire.beginTransmission(APDS9190_I2C_SLAVE_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.beginTransmission(APDS9190_I2C_SLAVE_ADDRESS);
  Wire.requestFrom(APDS9190_I2C_SLAVE_ADDRESS, bytes_read);
Wire.endTransmission();
  while (Wire.available())
  {
    if (i >= bytes_read) return i;
    recv_data[i++] = Wire.read();
  }

  return i;
}


static int APDS9190_clear_interrupt(int command)
{
  return i2c_smbus_write_byte(command);
}

static int APDS9190_set_enable(int enable)
{
  return i2c_smbus_write_byte_data(APDS9190_ENABLE_REG, enable);
}

static int APDS9190_set_atime(int atime)
{
  return i2c_smbus_write_byte_data(APDS9190_ATIME_REG, atime);
}

static int APDS9190_set_wtime(int wtime)
{
  return i2c_smbus_write_byte_data(APDS9190_WTIME_REG, wtime);
}

static int APDS9190_set_ailt(int threshold)
{
  return i2c_smbus_write_word_data(APDS9190_AILTL_REG, threshold);
}

static int APDS9190_set_aiht(int threshold)
{
  return i2c_smbus_write_word_data(APDS9190_AIHTL_REG, threshold);
}

static int APDS9190_set_pilt(int threshold)
{
  return i2c_smbus_write_byte_data(APDS9190_PITLO_REG, threshold);
}

static int APDS9190_set_piht(int threshold)
{
  return i2c_smbus_write_byte_data(APDS9190_PITHI_REG, threshold);
}

static int APDS9190_set_pers(int pers)
{
  return i2c_smbus_write_byte_data(APDS9190_PERS_REG, pers);
}

static int APDS9190_set_config(int config)
{
  return i2c_smbus_write_byte_data(APDS9190_CONFIG_REG, config);
}

static int APDS9190_set_ppulse(int ppulse)
{
  return i2c_smbus_write_byte_data(APDS9190_PPULSE_REG, ppulse);
}

static int APDS9190_set_control(int control)
{
  return i2c_smbus_write_byte_data(APDS9190_CONTROL_REG, control);
}


int displayOrientationAndGetZaxis()
{
  int x, y, z;
   adxl.readAccel(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

  char str[50] = "";
  sprintf(str, "{\"cor\":\"%d,%d,%d\"}", x, y, z);

  Serial.println(str);
  return y;
}
