/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"

int P=0,I=0,D=0,p_err=0,Kp=100,Ki=0,Kd=50,count_zero=0,counter_ADC=0,prev_state;


int rread(void);
int calError(int,int,int,int);
int calPID(int);
uint8 button;

/**
 * @file    main.c
 * @brief   
 * @details  ** You should enable global interrupt for operating properly. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/


//battery level//
/*int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    ADC_Battery_Start();        
    int16 adcresult =0;
    float volts = 0.0;

    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board

    for(;;)
    {
        
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16();
            volts = ADC_Battery_CountsTo_Volts(adcresult);                  // convert value to Volts
        
            // If you want to print value
            printf("%d %f\r\n",adcresult, volts);
        }
        CyDelay(500);
        
    }
 }   
//*/

/*//ultra sonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        //If you want to print out the value  
        printf("distance = %5.0f\r\n", Ultra_GetDistance());
        CyDelay(1000);
    }
}   
//*/

int main(){
    #if 0
    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
    sensor_isr_StartEx(sensor_isr_handler);
    CyDelay(5000);
    reflectance_start();     
    int16 adcresult =0;
    float volts = 0.0;

    IR_led_Write(1);
    for(;;)
    {
        button = SW1_Read();
        printf("button %d\n",button);
        counter_ADC++;
        if(counter_ADC==10000){            
            ADC_Battery_Start();   
            ADC_Battery_StartConvert();
            if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
                adcresult = ADC_Battery_GetResult16();
                volts = 1.5*ADC_Battery_CountsTo_Volts(adcresult);                  // convert value to Volts
            
                printf("%d %f\r\n",adcresult, volts);
                if(volts<4.0){
                    motor_stop();
                    return 0;
                }
            }
            counter_ADC=0;
            ADC_Battery_StopConvert();
            ADC_Battery_Stop();
        }
        reflectance_read(&ref);
        reflectance_digital(&dig); 
        /*printf("%d %d %d %d \r\n", ref.l3, ref.l1, ref.r1, ref.r3);       //print out each period of reflectance sensors
             //print out 0 or 1 according to results of reflectance period
        printf("%d %d %d %d \r\n", dig.l3, dig.l1, dig.r1, dig.r3);        //print out 0 or 1 according to results of reflectance period
        */
        int err = calError(dig.l3, dig.l1, dig.r1, dig.r3);
        int motorSpeed = calPID(err);
        
        int rightMotorSpeed = 235 - motorSpeed;
        int leftMotorSpeed = 235 + motorSpeed;
        
        if(err==9){
           rightMotorSpeed = 255;
           leftMotorSpeed = 255;
        }
        
        if(rightMotorSpeed > 255)
            rightMotorSpeed = 255;
        if(rightMotorSpeed < 0)
            rightMotorSpeed = 0;
        if(leftMotorSpeed > 255)
            leftMotorSpeed = 255;
        if(leftMotorSpeed < 0)
            leftMotorSpeed = 0;
        
        motor_turn(leftMotorSpeed,rightMotorSpeed,0);
        
        if(count_zero>1000){
            motor_stop();
            return 0;
        }
        else
            motor_start();
        
        CyDelay(1);
    }
    #endif
    #if 1
        struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
    sensor_isr_StartEx(sensor_isr_handler);
    reflectance_start();     
    int16 adcresult =0;
    float volts = 0.0;
    
    int stateCounter=0;
    int prev_line_state=0;
    
    IR_led_Write(1);
    uint8 button = SW1_Read();
    while (button == 1) {
        button = SW1_Read();
    }
    button = 1;
    
    motor_start();
    do {
        reflectance_read(&ref);
        reflectance_digital(&dig);
        motor_forward(50,0);
        printf("l3 %d r3 %d\n",dig.l3,dig.r3);
        CyDelay(100);
    } while(dig.l3==1&&dig.r3==1);
    motor_stop();
    
    while (button == 1) {
        button = SW1_Read();
    }
    for(;;)
    {
        counter_ADC++;
        if(counter_ADC==10000){            
            ADC_Battery_Start();   
            ADC_Battery_StartConvert();
            if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
                adcresult = ADC_Battery_GetResult16();
                volts = 1.5*ADC_Battery_CountsTo_Volts(adcresult);                  // convert value to Volts
            
                printf("%d %f\r\n",adcresult, volts);
                if(volts<4.0){
                    counter_ADC=0;
                    ADC_Battery_StopConvert();
                    ADC_Battery_Stop();
                    motor_stop();
                    sensor_isr_Stop();
                    UART_1_Stop();
                    return 0;
                }
            }
            counter_ADC=0;
            ADC_Battery_StopConvert();
            ADC_Battery_Stop();
        }
        reflectance_read(&ref);
        reflectance_digital(&dig); 
        /*printf("%d %d %d %d \r\n", ref.l3, ref.l1, ref.r1, ref.r3);       //print out each period of reflectance sensors
             //print out 0 or 1 according to results of reflectance period
        printf("%d %d %d %d \r\n", dig.l3, dig.l1, dig.r1, dig.r3);        //print out 0 or 1 according to results of reflectance period
        */
        
        if(dig.l1==0&&dig.r1==0){
            motor_turn(255,255,0);
        }
        if(dig.l1==0&&dig.r1==1){
            motor_turn(85,255,0);     
            prev_state=-1;
        }
        if(dig.l1==1&&dig.r1==0){
            motor_turn(255,85,0);     
            prev_state=1;        
        }
        
        if(dig.l1==1&&dig.r1==1){
            if(prev_state==-1)
                motor_turn(0,255,0);
            if(prev_state==1)
                motor_turn(255,0,0);
            count_zero++;
        }else{
            count_zero=0;
        }
       
        if(dig.l3==0&&dig.r3==0&&dig.l1==0&&dig.r1==0&&prev_line_state==0){
            stateCounter++;
            prev_line_state=1;
            Beep(100, 200);
        }else if(dig.l3==0&&dig.r3==0&&dig.l1==0&&dig.r1==0&&prev_line_state==1){
        }else{
            prev_line_state=0;
        }
        if(count_zero>1000||stateCounter==3){
            motor_stop();
            return 0;
        }
        else
            motor_start();
        
        
        CyDelay(1);
    }
    #endif
    return 0;
}

/*
int main(){
    CyGlobalIntEnable; 
    UART_1_Start();
    CyDelay(1000);
    motor_start();
    //motor_forward(255,2000);
    motor_forward(255,2000);
    motor_turn_90(150,335);
    motor_forward(255,2000);
    motor_turn_90(150,335);
    motor_forward(255,2000);
    motor_turn_90(150,750);
    motor_forward(255,2000);
    motor_stop();
    return 0;
}
*/
/*//nunchuk//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    nunchuk_start();
    nunchuk_init();
    
    for(;;)
    {    
        nunchuk_read();
    }
}   
//*/


/*//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    unsigned int IR_val; 
    
    for(;;)
    {
       IR_val = get_IR();
       printf("%x\r\n\n",IR_val);
    }    
 }   
//*/


/*//Ambient light sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    I2C_Start();
    
    I2C_write(0x29,0x80,0x00);          // set to power down
    I2C_write(0x29,0x80,0x03);          // set to power on
    
    for(;;)
    {    
        uint8 Data0Low,Data0High,Data1Low,Data1High;
        Data0Low = I2C_read(0x29,CH0_L);
        Data0High = I2C_read(0x29,CH0_H);
        Data1Low = I2C_read(0x29,CH1_L);
        Data1High = I2C_read(0x29,CH1_H);
        
        uint8 CH0, CH1;
        CH0 = convert_raw(Data0Low,Data0High);      // combine Data0
        CH1 = convert_raw(Data1Low,Data1High);      // combine Data1

        double Ch0 = CH0;
        double Ch1 = CH1;
        
        double data = 0;
        data = getLux(Ch0,Ch1);
        
        // If you want to print out data
        //printf("%lf\r\n",data);    
    }    
 }   
//*/


/*//accelerometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A;
    int16 X_AXIS_A, Y_AXIS_A, Z_AXIS_A;
    
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        //print out accelerometer output
        X_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_A);
        X_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_A);
        X_AXIS_A = convert_raw(X_L_A, X_H_A);
        
        Y_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_A);
        Y_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_A);
        Y_AXIS_A = convert_raw(Y_L_A, Y_H_A);
        
        Z_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_A);
        Z_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_A);
        Z_AXIS_A = convert_raw(Z_L_A, Z_H_A);
        
        printf("ACCEL: %d %d %d %d %d %d \r\n", X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A);
        value_convert_accel(X_AXIS_A, Y_AXIS_A, Z_AXIS_A);
        printf("\n");
        
        CyDelay(50);
    }
}   
//*/


/*//reflectance//
int main()
{
    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
    for(;;)
    {
        reflectance_read(&ref);
        printf("%d %d %d %d \r\n", ref.l3, ref.l1, ref.r1, ref.r3);       //print out each period of reflectance sensors
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%d %d %d %d \r\n", dig.l3, dig.l1, dig.r1, dig.r3);        //print out 0 or 1 according to results of reflectance period
        
        CyDelay(500);
    }
}   
//*/

 /* //motor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();

    motor_start();              // motor start

    motor_forward(100,2000);     // moving forward
    motor_turn(200,50,2000);     // turn
    motor_turn(50,200,2000);     // turn
    motor_backward(100,2000);    // movinb backward
       
    motor_stop();               // motor stop
    
    for(;;)
    {

    }
}
//*/
    

/*//gyroscope//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_G, X_H_G, Y_L_G, Y_H_G, Z_L_G, Z_H_G;
    int16 X_AXIS_G, Y_AXIS_G, Z_AXIS_G;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps    
    
    for(;;)
    {
        //print out gyroscope output
        X_L_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_L);
        X_H_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_H);
        X_AXIS_G = convert_raw(X_H_G, X_L_G);
        
        
        Y_L_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_L);
        Y_H_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_H);
        Y_AXIS_G = convert_raw(Y_H_G, Y_L_G);
        
        
        Z_L_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_L);
        Z_H_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_H);
        Z_AXIS_G = convert_raw(Z_H_G, Z_L_G);
     
        // If you want to print value
        printf("%d %d %d \r\n", X_AXIS_G, Y_AXIS_G, Z_AXIS_G);
        CyDelay(50);
    }
}   
//*/


/*//magnetometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
   
    uint8 X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M;
    int16 X_AXIS, Y_AXIS, Z_AXIS;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        X_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_M);
        X_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_M);
        X_AXIS = convert_raw(X_L_M, X_H_M);
        
        Y_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_M);
        Y_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_M);
        Y_AXIS = convert_raw(Y_L_M, Y_H_M);
        
        Z_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_M);
        Z_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_M);
        Z_AXIS = convert_raw(Z_L_M, Z_H_M);
        
        heading(X_AXIS, Y_AXIS);
        printf("MAGNET: %d %d %d %d %d %d \r\n", X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M);
        printf("%d %d %d \r\n", X_AXIS,Y_AXIS, Z_AXIS);
        CyDelay(50);      
    }
}   
//*/


#if 0
int rread(void)
{
    SC0_SetDriveMode(PIN_DM_STRONG);
    SC0_Write(1);
    CyDelayUs(10);
    SC0_SetDriveMode(PIN_DM_DIG_HIZ);
    Timer_1_Start();
    uint16_t start = Timer_1_ReadCounter();
    uint16_t end = 0;
    while(!(Timer_1_ReadStatusRegister() & Timer_1_STATUS_TC)) {
        if(SC0_Read() == 0 && end == 0) {
            end = Timer_1_ReadCounter();
        }
    }
    Timer_1_Stop();
    
    return (start - end);
}
#endif

/* Don't remove the functions below */
int _write(int file, char *ptr, int len)
{
    (void)file; /* Parameter is not used, suppress unused argument warning */
	int n;
	for(n = 0; n < len; n++) {
        if(*ptr == '\n') UART_1_PutChar('\r');
		UART_1_PutChar(*ptr++);
	}
	return len;
}

int _read (int file, char *ptr, int count)
{
    int chs = 0;
    char ch;
 
    (void)file; /* Parameter is not used, suppress unused argument warning */
    while(count > 0) {
        ch = UART_1_GetChar();
        if(ch != 0) {
            UART_1_PutChar(ch);
            chs++;
            if(ch == '\r') {
                ch = '\n';
                UART_1_PutChar(ch);
            }
            *ptr++ = ch;
            count--;
            if(ch == '\n') break;
        }
    }
    return chs;
}

int calPID(int error){
    P = error;
    I = I + error;
    D = error - p_err;
    p_err = error;
    return Kp*P + Ki*I + Kd*D;
}

int calError(int l3,int l1,int r1,int r3){
    int err = l3*1 + l1*2+r1*4+r3*8;
    if(err!=15){
        count_zero=0;
    }
    switch(err){
        case 7:
        return 3;
        case 3:
        return 5;
        case 11:
        return 1;
        case 9:
        return 0;
        case 13:
        return -1;
        case 12:
        return -5;
        case 14:
        return -3;
        case 15:
        count_zero++;
        return p_err;
        default:
        return 0;
    }
}
/* [] END OF FILE */
