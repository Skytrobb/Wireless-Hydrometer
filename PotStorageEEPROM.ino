/* This program will take the reading of a potentiometer
 *  and store it in EEPROM on an Arduino board.
 *  Right now the program is setup to display serial data to a bluetooth monitor,
 *  to change it to display on a computer serial monitor change every isntance
 *  of 'BTMonitor' to 'Serial.'
 */

/*
 * -------PIO pin assignments-------
 * HC-06 Bluetooth
 * TX to arduino pin 4
 * RX to arduino pin 2
 * 
 * Potentiometer
 * resistance value to arduino analog pin A0;
 * ----------------------------------
 */

#include <EEPROM.h>
#include <SoftwareSerial.h>
SoftwareSerial BTMonitor(4,2);

#define potPin A0
#define PotReadings 10 // this controls how many readings will be taken from the potentiometer

bool FirstLoop = true;
int readings[PotReadings];
int EEvalue;
int addr = 0;
int SensorReading;
int SensorStoreVal;
int NumofReadings;
int index = 0;
int Average;
int readIndex;
char A;

void setup() {
  BTMonitor.begin(9600);

}

void loop() {
   
  if (BTMonitor.available() > 0) { // if there is serial connection do the followings
    BTMonitor.println("connected"); // display "connected" to the BT monitor so its possible to
                                    //see if the monitor is receiving any data
    A = (BTMonitor.read());
      if(A == 'w') {
        if(FirstLoop) { // check to see if this is the first time executing the command
       NumofReadings = EEPROM.read(1023); // recall how many addresses are being used
       FirstLoop = false;
       BTMonitor.println("first reading");
        }
         for (readIndex = 0; readIndex < PotReadings; readIndex++) {
         readings[readIndex] = analogRead(potPin); //store potentiometer reading in array
        }
         Average = (AverageOfArray(readings, PotReadings)); // call the average function and store it in Average
  
        SensorStoreVal = Average / 4;// divide that value by 4. (EEPROM can only store values between 0-255)
        
        EEPROM.write(NumofReadings, SensorStoreVal); //write the current sensor reading to EEPROM 
                                            // address 'NumofReadings'
        NumofReadings++; //increment our reading count to avoid writing over data
        EEPROM.write(1023, NumofReadings); //store number of readings in EEPROM so we can begin
                                           // storing data from where the program left off.
      }
      if (A == 'r') {
        for(index = 0; index < NumofReadings; index++) {
          BTMonitor.println(EEPROM.read(index));
        }
      }
  }


}

int AverageOfArray (int arr[], int ArrSize) { // this function returns the average of array 'arr[]' of length 'ArrSize'

int Index = 0;
int SumofArray = 0;
int Ave;

 for (Index = 0; Index < ArrSize; Index++) {
         SumofArray = (arr[Index] + SumofArray);
      }
  Ave = (SumofArray/ArrSize);

  return Ave;
}
