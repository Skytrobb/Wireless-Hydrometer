/* This program will take the reading of a potentiometer
    and store it in EEPROM on an Arduino board.
    Right now the program is setup to display serial data to a bluetooth monitor,
    to change it to display on a computer serial monitor change every instance
    of 'BTMonitor' to 'Serial.'

    latest version includes a serial output for exporting EEPROM
    data to a text file

    4/4 - added a sleep timer to decrease power consumption.
         .

*/

/*
   -------PIO pin assignments-------
   HC-06 Bluetooth
   TX to arduino pin 4
   RX to arduino pin 2

   Potentiometer
   resistance value to arduino analog pin A0;
   ----------------------------------
*/
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <LowPower.h>
SoftwareSerial BTMonitor(4, 2);

#define potPin A0
#define PotReadings 10 // this controls how many readings will be taken from the potentiometer
#define delaytime 1000 //this defines how long the delay will be
#define delaytimeramount 30  //this defines how many times the delaytime will happen
// this is so the program can check once every (delaytime) if bluetooth is connected or not

#define SleepMax 4 //sleep happens in 8 second intervals. SleepMax will dictate how many intervals must happen before a reading is taken

int delaytimercount = 0;
int NumofReadings;
int EEvalue;
int addr = 0;
int SensorReading;
int index = 0;
char A;
bool FirstLoop = true;
int SleepCount = 0;  //SleepCount indicates how many times the board has gone to sleep

void setup() {
  Serial.begin(9600);
  BTMonitor.begin(9600);
  if (FirstLoop) { // check to see if this is the first time executing the command
    NumofReadings = EEPROM.read(1023); // recall how many addresses are being used
    //the number of readings is stored in address 1023
    FirstLoop = false;
    BTMonitor.println("EEPROM has been checked");
  }


}

void loop() {


  EEvalue = analogRead(A0);

  if (BTMonitor.available() > 0) { // if there is serial connection do the followings
    BTMonitor.println("connected"); // display "connected" to the BT monitor so its possible to
    //see if the monitor is receiving any data
    A = (BTMonitor.read());
    delaytimercount = 0; //reset the delay timer if there is a BT stream
    if (A == 'w') {
      takeReading();
    }
    if (A == 'r') {
      for (index = 0; index < NumofReadings; index++) {
        BTMonitor.println(EEPROM.read(index));
      }
    }
    if (A == 'e') {
      for (index = 0; index < NumofReadings; index++) { //sends the EEPROM data to serial. The python script will read this data and export it to a text file
        Serial.println(EEPROM.read(index));
      }
    }
  }

  if (delaytimercount < delaytimeramount) { // if there hasn't been communication through bluetooth for 30 seconds then enter sleep mode
    delay(delaytime);
    delaytimercount++;

  }
  else {
    while (BTMonitor.available() == 0) { //while there is no data coming from the BT stream
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //power down the board for 8 seconds
      if (SleepCount < SleepMax) { //upon waking up, increment the sleep count
        SleepCount++;
      }
      else {
        takeReading(); //once sleepcount has reached the correct value, take a reading.
        SleepCount = 0;
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
  Ave = (SumofArray / ArrSize);

  return Ave;
}

void takeReading() {

  int readIndex;
  int Average;
  int readings[PotReadings];
  int SensorStoreVal;

  for (readIndex = 0; readIndex < PotReadings; readIndex++) {
    readings[readIndex] = analogRead(potPin); //store potentiometer reading in array
  }
  Average = (AverageOfArray(readings, PotReadings)); // call the average function and store it in Average

  SensorStoreVal = Average / 4;// divide that value by 4. (EEPROM can only store values between 0-255)

  EEPROM.write(NumofReadings, SensorStoreVal); //write the current sensor reading to EEPROM
  // the address will be determined by the findStorageAddress
  NumofReadings++; //increment our reading count to avoid writing over data
  EEPROM.write(findStorageAddress(NumofReadings), NumofReadings); //store number of readings in EEPROM so we can begin
  // storing data from where the program left off.

}
int findStorageAddress(int CurrentReadingCount) { //4 EEPROM addresses are required to keep track of all of the readings
  // without this function, all data is erased after the 255th EEPROM entry
  int CurrentReadingCount;
  int EEPROMStorageAddress;

  if (CurrentReadingCount < 255) {
    EEPROMStorageAddress = 1021;
  }
  else if (CurrentReadingCount >= 255 && CurrentReadingCount < 510) {
    EEPROMStorageAddress = 1022;
  }
  else if (CurrentReadingCount >= 510 && CurrentReadingCount < 765) {
    EEPROMStorageAddress = 1023
  }
  else if (CurrentReadingCount >= 765 && CurrentReadingCount < 1020) {
    EEPROMStorageAddress = 1024;
  }

  return EEPROMStorageAddress;

}

