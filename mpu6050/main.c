/*
Author:               sagor sarker
Microcontroller:      PIC16F877A
IDE:                  mikroC Pro
Programming Language: C
Description:          MPU6050 Accelerometer library using bare metal i2c    
*/

void i2c_init(void)
{
    TRISC.b3 = 1;       /* SDA and SCL as input pin */
    TRISC.b4 = 1;      /* these pins can be configured either i/p or o/p */
    SSPSTAT |= 0x80;  /* Slew rate disabled */
    SSPCON = 0x28;   /* SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)) */
    SSPADD = 49;    /* 100Khz @ 20Mhz Fosc */
}

void i2c_start()
{
    SEN_bit = 1;         /* Start condition enabled */
    while(SEN_bit);     /* wait for start condition to finish */
}

void i2c_stop()
{
    PEN_bit = 1;          /* Stop condition enabled */
    while(PEN_bit);      /* Wait for stop condition to finish */
}

void i2c_restart()
{
    RSEN_bit = 1;         /* Repeated start enabled */
    while(RSEN_bit);     /* wait for condition to finish */
}

void i2c_ack()
{
    ACKDT_bit = 0;         /* Acknowledge data bit, 0 = ACK */
    ACKEN_bit = 1;        /* Ack data enabled */
    while(ACKEN_bit);    /* wait for ack data to send on bus */
}

void i2c_nack()
{
    ACKDT_bit = 1;         /* Acknowledge data bit, 1 = NAK */
    ACKEN_bit = 1;        /* Ack data enabled */
    while(ACKEN_bit);    /* wait for ack data to send on bus */
}

void i2c_wait()
{
    while((SSPCON2 & 0x1F) || (SSPSTAT & 0x04));    /* wait for any pending transfer */
}

void i2c_send(unsigned char dat)
{
    SSPBUF = dat;        /* Move data to SSPBUF */
    while(BF_bit);      /* wait till complete data is sent from buffer */
    i2c_wait();        /* wait for any pending transfer */
}

unsigned char i2c_receive(void)
{
    unsigned char recv_data;
    /* Reception works if transfer is initiated in read mode */
    RCEN_bit = 1;            /* Enable data reception */
    while(!BF_bit);         /* wait for buffer full */
    recv_data = SSPBUF;    /* Read serial buffer and store in recv_data register */
    i2c_wait();           /* wait to check any pending transfer */
    return(recv_data);   /* Return the read data from bus */
}


//specify the register address of MPU-6050
char SMPLRT_DIV=0x19;           //sampler rate division
char CONFIG=0x1A;
char GYRO_CONFIG=0x1B;
char ACCEL_CONFIG=0x1C;
char MOT_THR=0x1F;           //motion detection threshold
char FIFO_EN=0x23;          //
char I2C_MST_CTRL=0x24;
char ACCEL_XOUT_H=0x3B;
char ACCEL_XOUT_L=0x3C;
char ACCEL_YOUT_H=0x3D;
char ACCEL_YOUT_L=0x3E;
char ACCEL_ZOUT_H=0x3F;
char ACCEL_ZOUT_L=0x40;
char temp_OUT_H=0x41;
char temp_OUT_L=0x42;
char GYRO_XOUT_H=0x43;
char GYRO_XOUT_L=0x44;
char GYRO_YOUT_H=0x45;
char GYRO_YOUT_L=0x46;
char GYRO_ZOUT_H=0x47;
char GYRO_ZOUT_L=0x48;
char INT_PIN_CFG=0x37;
char INT_ENABLE=0x38;
char USER_CTRL=0x6A;
char WHO_AM_I=0x75;       //default value is 0x68

//specify MPU-ADDRESS
char mpuadd_w=0xD0;
char mpuadd_r=0xD1;

//funtion to write to mpu
void mpu_write(char deviceadd_w,char reg_add,char dat)
{
    i2c_start();
    i2c_send(deviceadd_w);  //send device address+write bit
    i2c_send(reg_add);     //send specific register address
    i2c_send(dat);
    i2c_stop();
}

//funtion to read fv rom mpu
unsigned char mpu_read(char deviceadd_w,char deviceadd_r,char reg_add)
{
    unsigned char dat;
    i2c_start();
    i2c_send(deviceadd_w);      //send device address+write bit
    i2c_send(reg_add);         //send register address
    i2c_restart();
    i2c_send(deviceadd_r);   //send device address+read bit
    dat=i2c_receive();      //get data from ITG-3200
    i2c_nack();            //send not acknowledged
    i2c_stop();
    return dat;

}

//get MPU-6050 Identification
void mpu_identify()
{
    int id=0;
    char idt[3];
    id=mpu_read(mpuadd_w,mpuadd_r,WHO_AM_I);
    inttostr(id,idt);
    lcd_out(1,1,idt);
    if(id==0x68)
    { 
        lcd_out(2,1,"ID Matched");
    }
    else
    {
        lcd_out(2,1,"ERROR:I2C Failed");
    }
    
    delay_ms(2000);
    lcd_cmd(_LCD_CLEAR);
}

//configuration of MPU-6050
void mpu_setup()
{
    //sets sample rate to 8000/(1+7)=1000Hz
    mpu_write(mpuadd_w,SMPLRT_DIV,0x07);
    
    //disable FSYNC ,256Hz DLPF
    mpu_write(mpuadd_w,CONFIG,0x00);
    
    //disable gyro self test, FS_SEL to 500dps
    mpu_write(mpuadd_w,GYRO_CONFIG,0x08);
    
    //disable acc. self test, AFS_SEL to +-2g
    mpu_write(mpuadd_w,ACCEL_CONFIG,0x00);
    

    //motion threshold of 0mg
        mpu_write(mpuadd_w,MOT_THR,0x00);
    
    //disable FIFO buffer
    mpu_write(mpuadd_w,FIFO_EN,0x00);
    
    //single master controll, 400KHz clock speed
    mpu_write(mpuadd_w,I2C_MST_CTRL,0x0D);
    
    //interrupt configuration(still confused)
    mpu_write(mpuadd_w,INT_PIN_CFG,0x00);
    
    //interrupt enable(still confused)
    mpu_write(mpuadd_w,INT_ENABLE,0x00);
    
    //user controll register
    mpu_write(mpuadd_w,USER_CTRL,0x00);
 
}


int read_ax()
{
    int ax;
    ax=mpu_read(mpuadd_w,mpuadd_r,ACCEL_XOUT_H)<<8;
    ax|=mpu_read(mpuadd_w,mpuadd_r,ACCEL_XOUT_L);
    return ax;
}

int read_ay()
{
    int ay;
    ay=mpu_read(mpuadd_w,mpuadd_r,ACCEL_YOUT_H)<<8;
    ay|=mpu_read(mpuadd_w,mpuadd_r,ACCEL_YOUT_L);
    return ay;
}

int read_az()
{
    int az;
    az=mpu_read(mpuadd_w,mpuadd_r,ACCEL_ZOUT_H)<<8;
    az|=mpu_read(mpuadd_w,mpuadd_r,ACCEL_ZOUT_L);
    return az;
}

int read_gx()
{
    int gx;
    gx=mpu_read(mpuadd_w,mpuadd_r,GYRO_XOUT_H)<<8;
    gx|=mpu_read(mpuadd_w,mpuadd_r,GYRO_XOUT_L);
    return gx;
}

int read_gy()
{
    int gy;
    gy=mpu_read(mpuadd_w,mpuadd_r,GYRO_YOUT_H)<<8;
    gy|=mpu_read(mpuadd_w,mpuadd_r,GYRO_YOUT_L);
    return gy;
}

int read_gz()
{
    int gz;
    gz=mpu_read(mpuadd_w,mpuadd_r,GYRO_ZOUT_H)<<8;
    gz|=mpu_read(mpuadd_w,mpuadd_r,GYRO_ZOUT_L);
    return gz;
}

int read_temp()
{
    int temp;
    temp=mpu_read(mpuadd_w,mpuadd_r,temp_OUT_H)<<8;
    temp|=mpu_read(mpuadd_w,mpuadd_r,temp_OUT_L);
    return temp;
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

int ax,ay,az,gx,gy,gz,temp;
float gxrate,gyrate,gzrate;
char axs[5],ays[5],azs[5],temps[5];

void main() 
{  
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    lcd_out(1,1,"MPU-6050");
    delay_ms(2000);
    Lcd_Cmd(_LCD_CLEAR);

    //initialize i2c protocol
    i2c_init();
    
    //device identification
    mpu_identify();
    
    //device configuration
    mpu_setup();
  
    while(1)
    {
        //read the raw data  from MPU-6050
        ax=read_ax();
        ay=read_ay();
        az=read_az();
        
        gx=read_gx();
        gy=read_gy();
        gz=read_gz();
        
        temp=read_temp();

        inttostr(ax,axs);
        inttostr(ay,ays);
        inttostr(az,azs);
        inttostr(temp,temps);
        
        lcd_cmd(_LCD_CLEAR);
        lcd_out(1,1,axs);
        lcd_out(1,7,ays);
        lcd_out(2,1,azs);
        lcd_out(2,7,temps);
  
    }
 
}
