#include <PID_v1.h>
#include <Wire.h>

#include "Arduino.h"
#include "heltec.h"


const int tempPin = 0;
const int alcAPin = 15;
const int alcDPin = 22;

//4096 bytes
double RawValue= 0;
double Voltage = 0;
double tempC = 0;
double tempF = 0;

//PID init
double set_temp = 20;
double Output = 0;


//Figure out values
/*
double Kp=9.1, Ki=0.3, Kd=1.8;
PID myPID(&tempC, &Output, &set_temp, Kp, Ki, Kd, DIRECT);
*/
 
void setup(){
pinMode(tempPin, INPUT);
pinMode(alcAPin, INPUT);
pinMode(alcDPin, INPUT);

Serial.begin(9600); 

//myPID.SetMode(AUTOMATIC);

Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
Heltec.display->flipScreenVertically();
Heltec.display->setFont(ArialMT_Plain_10);


}

 
void loop(){
  Heltec.display->clear();

  //temp sensor
  tempC = celsius(analogRead(tempPin));

  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->drawString(63, 22, "Current Ambient Temp:");
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->drawString(63, 11, String(tempC));

  Heltec.display->setTextAlignment(TEXT_ALIGN_RIGHT);
  Heltec.display->drawString(10, 128, String(millis()));
  // write the buffer to the display
  Heltec.display->display();
  delay(1000);


  //PID
  /*
  myPID.Compute();
  Serial.print(Output);
  */
  
  //alcohol sensor
  //needs to be tested
  /*
  double R0 = 0.26;
  double sensor_volt;
  double RS_gas; // Get value of RS in a GAS
  float ratio; // Get ratio RS_GAS/RS_air
  double BAC;
  double sensorValue = analogRead(alcAPin);
  sensor_volt=(double)sensorValue/4095.0*5.0;
  RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL
  Serial.print("value: ");
  Serial.println(sensorValue);
 ///-Replace the name "R0" with the value of R0 in the demo of First Test -
  ratio = RS_gas/R0;  // ratio = RS/R0  
  Serial.print("volt: ");
  Serial.println(sensor_volt); 
  BAC = 0.1896*pow(ratio, 2) - 8.6178*ratio/10 + 1.0792;   //BAC in mg/L
  Serial.print("BAC = ");
  Serial.println(BAC*0.0001);  //convert to g/dL
  Serial.print("\n\n");
  */
}

double celsius(double RawValue){
  //fix equation ~500 instead of 330
  Voltage = (RawValue*330.0)/4095.0;
  tempC = (Voltage -50.0)*0.1;  //adjust offset of 50
  
  tempF = (tempC * 1.8) + 32.0; // conver to F
  
  Serial.print("Raw Value = " ); // shows pre-scaled value
  Serial.print(RawValue);
  Serial.print("\t milli volts = "); // shows the voltage measured
  Serial.print(Voltage,0); //
  Serial.print("\t Temperature in C = ");
  Serial.print(tempC,1);
  Serial.print("\t Temperature in F = ");
  Serial.println(tempF,1);
  
  return tempC;
}


//finding base reading R0 for alcohol sensor
double findR0(double sensorValue) {
    double RS;
    double sensor_volt;
    for(int i = 0 ; i < 100 ; i++)
    {
        sensorValue = sensorValue + analogRead(alcAPin);
    }
    
    sensorValue = sensorValue/100.0;     //get average of reading
    Serial.print("Pin val: ");
    Serial.println(sensorValue, DEC);
    sensor_volt = sensorValue/4095.0*5.0;
    RS = (5.0-sensor_volt)/sensor_volt; // 
    return RS;
}
