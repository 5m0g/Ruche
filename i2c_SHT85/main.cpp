#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <math.h>
#include <time.h>
#include <DigitalIn.h>
#include <Servo.h>
#include <string.h>

/// Read temperature from STH85
/// control a servo motor 

//--- Hive Gate init ---
Servo trappe(GPIO2); //DK: GPIO2(pwm&adc) p15

void OpenGate();
void CloseGate();


//--- init I2C SHT85 ---
I2C i2c(I2C_SDA, I2C_SCL); //DK: SDA p19, SCL p20

const int addr7bit = 0x44;      // 7 bit I2C address
const int addr8bit = addr7bit << 1; // 8bit I2C address, 0x90

float ReadSHTint(int addr8bit,char type);
float ReadHumid(int addr8bit);

//---GPIO init---
DigitalIn GateControl(GPIO0); //DK: p17

float temp;
float hum;
int main()
{
    
    while (1) {
        //temp=ReadTemp(addr8bit);
        //ThisThread::sleep_for(2ms);
        //hum=ReadHumid(addr8bit);
        //--- Servo + trappe test-----
        //if(GateControl){
        //    OpenGate();
        //}
        //else{CloseGate();}
                        //char temperature[20];
                        //sprintf(temperature, "La température est de : %f \n",temp);  //---- NE MARCHE PAS
                        //puts(temperature);
        OpenGate();
        ThisThread::sleep_for(1s);
        CloseGate();
        ThisThread::sleep_for(1s);

    }
}


//--- Servo ---
void OpenGate(){
    //ajouter remesure de position ??? trappe.read();
    trappe.position(180);
}
void CloseGate(){
    //ajouter remesure de position ? trappe.read();
    trappe.position(0);
}

//--- I2c SHT85 ---
float ReadSHT(int addr8bit,char type){
    char cmd[6];                    // dimention 4*octet pour aceuillir deux fois 16bits de données 
    cmd[0] = 0x24;                  //single shot measurement command
    cmd[1] = 0x00;                  //high repeatability measure setting
    //set command

    i2c.write(addr8bit, cmd, 2);
    ThisThread::sleep_for(1ms);
    i2c.read(addr8bit,cmd,0);
    ThisThread::sleep_for(16ms);
    //fetch  temp readings
    i2c.read(addr8bit, cmd, 6);

    if(strcmp(type,"Temp")==0){
        //concatenate 8bits val to 16 bit
        uint16_t fullVal = (uint16_t)cmd[0]<<8;
        fullVal += (uint16_t)cmd[1];
        //display + conversion
        float tmp = 175*fullVal/(pow(2,16)-1)-45; 
        return tmp;
    }
    else if (strcmp(type,"Humid")==0){
        uint16_t fullVal = (uint16_t)cmd[3]<<8;
        fullVal += (uint16_t)cmd[4];            //last two bytes of meas dedicated to humidity
        //display + conversion
        float hum = 100*fullVal/(pow(2,16)-1);
        return hum;
    }
    
}

float ReadHumid(int addr8bit){
    char cmd[4];                    // dimention 4*octet pour aceuillir deux fois 16bits de données 
    cmd[0] = 0x24;                  //single shot measurement command
    cmd[1] = 0x00;                  //high repeatability measure setting
    //set command
    i2c.write(addr8bit, cmd, 2);
    ThisThread::sleep_for(1ms);
    i2c.read(addr8bit,cmd,0);
    ThisThread::sleep_for(16ms);
    //fetch  temp readings
    i2c.read(addr8bit, cmd, 4);
    //concatenate 8bits val to 16 bit
    uint16_t fullVal = (uint16_t)cmd[2]<<8;
    fullVal += (uint16_t)cmd[3];            //last two bytes of meas dedicated to humidity
    //display + conversion
    float hum = 100*fullVal/(pow(2,16)-1);
    return hum;
}