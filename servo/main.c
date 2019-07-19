
/*
Author:               sagor sarker
Microcontroller:      PIC16F877A
IDE:                  mikroC Pro
Programming Language: C
Description:          driving servo motor using bare metal timer-pwm   
*/

//servo, sensor & button pin direction
sbit srv1_pin at RC0_bit;
sbit srv2_pin at RC1_bit;
sbit srv3_pin at RC2_bit;
sbit srv4_pin at RC3_bit;
sbit button1_press at RD0_bit;
sbit button2_press at RD1_bit;
sbit button3_press at RD2_bit;
sbit button4_press at RD3_bit;

//variable declaration for counting timer values
unsigned int ovr0,ovr1;
unsigned long count1;
unsigned long count2,tmr1_count;
double count0,tmr0_count;

//variable declaration for servo pulse
unsigned long servo1_time;
unsigned long servo2_time;
unsigned long servo3_time;
unsigned long servo4_time;


void interrupt()                    // interrupt routine
{
    if(t0if_bit==1)                 //timer0 interrupt flag bit high
    {
        ovr0++;                     //timer0 overflow increment
        t0if_bit=0;                 //flag bit cleared
    }
    else if(tmr1if_bit==1)          //timer1 interrupt flag bit high
    {
        ovr1++;                     //timer1 overflow increment
        tmr1if_bit=0;               //flag bit cleared
    }
}

void init_routine()
{
    trisc=0;                    //declaration of portc as servo outgoing
    portc=0;
    trisd=255;                  //portd as input for sensor & button
    portd=255;

    option_reg= 0b10011000;     //timer0 configuration for internal instruction cycle,H2L transition,wdt,1:1 prescaler
    tmr0=0; ovr0=0;

    t1con= 0b00000100;            //timer1 configuration
    tmr1on_bit= 1;                //start timer1 for time delay
    tmr1l=0; tmr1h=0; ovr1=0;

    gie_bit=1;             //global interrupt enabled for timer0 interrupt
    peie_bit=1;            //peripheral interrupt enabled for timer1 interrupt

    t0ie_bit=1;            //tmr0 interrupt enabled
    t0if_bit=0;            //tmr0 flag bit cleared

    tmr1ie_bit=1;          //tmr1 interrupt enabled
    tmr1if_bit=0;          //tmr1 flag bit cleared

    servo1_time=7500;      //assigning the initial time values for definite servo angle in microsecond
    servo2_time=7500;
    servo3_time=7500;
    servo4_time=7250;
    tmr1l=0; tmr1h=0; ovr1=0; count1=0; count2=0; tmr1_count=0;
}

void move_arm()
{
    count1 = ((long)tmr1h * 256)   + ((long)tmr1l);      //counting timer1 value
    count2 = ((long)ovr1  * 65536) + ((long)count1);
    tmr1_count=((long)count2);                          //total timer1 increment

    if(tmr1_count>= servo1_time)         //rotate zero degree
    {
        srv1_pin=0;
    }
    if(tmr1_count>= servo2_time)        //rotate 90 degree
    {
        srv2_pin=0;
    }
    if(tmr1_count>= servo3_time)        //rotate 180 degree
    {
        srv3_pin=0;
    }
    if(tmr1_count>= servo4_time)       //rotate 90 degree
    {
        srv4_pin=0;
    }
    if(tmr1_count>=100000)    //creat a pulse of 20ms
    {
        srv1_pin=srv2_pin=srv3_pin=srv4_pin=1;                          //servo pins repeats to high
        tmr1l=0; tmr1h=0; ovr1=0; count1=0; count2=0; tmr1_count=0;     //all timer1 count to zero
    }
}

void main()
{
    init_routine();

    while(1)
    { 
        if(button1_press==1)
        {
            servo1_time=10500;
            servo2_time=10500;
            servo3_time=10500;
            servo4_time=10500;
        }
        else if(button2_press==1)
        {
            servo1_time=7500;
            servo2_time=4500;
            servo3_time=4500;
            servo4_time=7500;
        }
        else if(button3_press==1)
        {
            servo1_time=7500;
            servo2_time=7500;
            servo3_time=7500;
            servo4_time=7250;
        }
        else if(button4_press==1)
        {
            servo1_time=10500;
            servo2_time=4500;
            servo3_time=4500;
            servo4_time=10500;
        }
        /*  else
        {
            servo1_time=7500;
            servo2_time=7500;
            servo3_time=7500;
            servo4_time=7250;
        }*/
        move_arm();
    }

}