
/*
Author:               sagor sarker
Microcontroller:      PIC16F877A
IDE:                  mikroC Pro
Programming Language: C
Description:          ITG-3200 gyroscope sensor library  
*/

//specify the register address of ITG-3200(gyro)
char WHO_AM_I=0x00;
char SMPLR_DIV=0x15;
char DLPF=0x16;
char GYRO_XOUT_H=0x1D;
char GYRO_XOUT_L=0x1E;
char GYRO_YOUT_H=0x1F;
char GYRO_YOUT_L=0x20;
char GYRO_ZOUT_H=0x21;
char GYRO_ZOUT_L=0x22;

//specify gyro address
char gyroadd_w=0xD0;       //device address+write(pin 9 is logic low)
char gyroadd_r=0xD1;      //device address+read(pin 9 is logic low)

//funtion to write to gyro
void gyro_write(char deviceadd_w,char reg_add,char dat)
{
  i2c1_start();
  while(!I2C1_Is_Idle()); //wait untill the bus is free
  i2c1_wr(deviceadd_w);  //send device address+write bit
  while(!I2C1_Is_Idle());
  i2c1_wr(reg_add);     //send specific register address
  while(!I2C1_Is_Idle());
  i2c1_wr(dat);        //
  while(!I2C1_Is_Idle());
  i2c1_stop();
  while(!I2C1_Is_Idle());
}

//funtion to read from gyro
unsigned char gyro_read(char deviceadd_w,char deviceadd_r,char reg_add)
{
  unsigned char dat;
  i2c1_start();
  while(!I2C1_Is_Idle());
  i2c1_wr(deviceadd_w);  //send device address+write bit
  while(!I2C1_Is_Idle());
  i2c1_wr(reg_add);
  while(!I2C1_Is_Idle());
  i2c1_repeated_start();
  while(!I2C1_Is_Idle());
  i2c1_wr(deviceadd_r);   //send device address+read bit
  while(!I2C1_Is_Idle());
  dat=i2c1_rd(0);         //get data from ITG-3200
  while(!I2C1_Is_Idle());
  i2c1_stop();
  while(!I2C1_Is_Idle());
  return dat;

}

//read X-axis value
int readx()
{
 int dat;
 dat  =gyro_read(gyroadd_w,gyroadd_r,GYRO_XOUT_H)<<8;
 dat |=gyro_read(gyroadd_w,gyroadd_r,GYRO_XOUT_L);
 return dat;
}

//read Y-axis value
int ready()
{
 int dat;
 dat  =gyro_read(gyroadd_w,gyroadd_r,GYRO_YOUT_H)<<8;
 dat |=gyro_read(gyroadd_w,gyroadd_r,GYRO_YOUT_L);
 return dat;
}

//read Z-axis value
int readz()
{
 int dat;
 dat  =gyro_read(gyroadd_w,gyroadd_r,GYRO_ZOUT_H)<<8;
 dat |=gyro_read(gyroadd_w,gyroadd_r,GYRO_ZOUT_L);
 return dat;
}

// LCD module connections
sbit LCD_RS at RB2_bit;
sbit LCD_EN at RB3_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;

sbit LCD_RS_Direction at TRISB2_bit;
sbit LCD_EN_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;
// End LCD module connections


int id=0;
char idt[7];
int xdata,ydata,zdata;
char xval[7],yval[7],zval[7];

void main()
{
  Lcd_Init();
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_CURSOR_OFF);
  lcd_out(1,1,"gyroscope");
  delay_ms(2000);
  Lcd_Cmd(_LCD_CLEAR);

  //initialize i2c protocol
  I2C1_Init(100000);

  //get gyro identification
  id=gyro_read(gyroadd_w,gyroadd_r,WHO_AM_I);
  inttostr(id,idt);
  lcd_out(2,8,idt);
  delay_ms(100);
  
  //gyro configuration
  gyro_write(gyroadd_w,DLPF,0x1E);    //DLPF is set to 0b00011011
  gyro_write(gyroadd_w,SMPLR_DIV,0x09);  //F.sample to 100hz
  delay_ms(100);
 while(1)
 {
   //read gyroscope data
   xdata=readx();
   ydata=ready();
   zdata=readz();

   inttostr(xdata,xval);
   inttostr(ydata,yval);
   inttostr(zdata,zval);

   lcd_out(1,1,xval);
   lcd_out(1,8,yval);
   lcd_out(2,1,zval);

 }

}