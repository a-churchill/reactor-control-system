#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <Key.h>
#include <Keypad.h>
#include <Wire.h> 

LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 20 chars and 4 line display

//initializing keypad
const byte ROWS = 4; //four rows for keypad membrane
const byte COLS = 3; //four columns for keypad membrane

//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = 
{
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int flowReadPin = 14;     // Reads Pin [14], output voltage caused by the current flow sesnsor
float flowReadValue = 0;  //flow sensor value, 0-5V but microcontroller reads it as 0-1023        
float flowReadDisplayed = 0; //converted ^ to a 0-50.00 L/min to output to display

const int numReadings = 10;  //determine the sizes of flow readings in the index
int flowReadings[numReadings];  //the readings from the analog input
int readIndex = 0;    //number of readings in the index above
float total = 0;      //add value of all the readings in the index
float avgflowReadValue = 0;   //the total divided by number of readings

int setPointPin = 12;    //signal to Pin [12] to control flow, 0-5V 
float setPoint = 0;        //input signal 0-50 L/min
float setPointVolt = 0;         //converted ^ to 0-5V
float setPointValue = 0;        //converted ^ to 0-255 for analog signal to flowmeter 

int motorspeedPin = 13;    //signal to Pin [13] to control motor speed
float motorspeed = 0;        //input signal 0-13 RPM
float motorspeedValue = 0;   //converted ^ to 0-255 for analog signal to motor driver

float microVoltage = 0;  //measured voltage from the +5V of microcontroller board

void setup() 
{
  Serial.begin(9600);
  lcd.init();           //initialize lcd
  lcd.backlight();      //turns on backlight
  lcd.begin(20, 4);     // set up the LCD's number of columns and rows
  lcd.noAutoscroll();
  
  keypad.setDebounceTime(10);

  pinMode(setPointPin, OUTPUT); //sets degital Pin [12] as an output
  pinMode(motorspeedPin, OUTPUT); //sets digital Pin [13] as an output
  
  lcd.print("Welcome!");      //prints "Welcome!"
  delay(2000); 
  
  microVoltage = 4.951; //max output voltage of microcontroller
}


void flowrateEntry(){
  
  lcd.clear();            //clears screen
  lcd.setCursor(0, 0);    //sets cursor to starting position
  lcd.print("Enter flow rate:");  //prints "Enter flow rate:" on the first row

  lcd.setCursor(0, 1);    //moves cursor to the start of row 2
  lcd.print("00.0 L/min");  //prints "00.0 L/min" on the 2nd row
  lcd.setCursor(0, 1);    //moves cursor back to (0,1)
  lcd.cursor();           //sets a visible cursor to show position

}

float GetFlow()     //function gets a value for setpoint of flow
{
  float flowInput = 0;  //creates the "flowInput" variable
  int i = 0;    //creates a variable that will be incremented
  char key = keypad.getKey();   //creates variable "key" and sets it to whatever is inputted
  delay(50);

 
  while(key != '#' && i < 3) 
  {

    switch (key)      //waits for "key" to define a case
    {
      case NO_KEY:    //if no key is inputted it will keep looking until one is inputted
        break;

        case '0': case '1': case '2': case '3': case '4':     //case when a number is inputted
        case '5': case '6': case '7': case '8': case '9':
          if (i == 0)                     //takes in the 1st value
          {
            flowInput = 10*(key - '0');   //stores tens value in "flowInput"
            if (flowInput > 50)           //compares "flowInput" to max flowrate which is 50 L/min
            {
              lcd.setCursor(0, 1);
              lcd.print("Invalid           ");    //if greater tha 50 prints "Invalid"
              lcd.noCursor();
              delay(1000);
              flowInput = 0;                      //resets "flowinput" 
              i = 0;                              //resets "i"
              lcd.cursor();
              lcd.setCursor(0, 1);
              lcd.print("00.0 L/min");            //resets screen
              lcd.setCursor(0, 1);
            }
            else {
              lcd.print(key);                     //if "flowInput" is less than 50 it will print the key value
              i++;                                //increments "i", i =1
            } 
          }
          
          else if (i == 1)            
          {
            flowInput = flowInput + (key - '0');  //stores ones value in "flowInput"
            if (flowInput > 50)                   //compares "flowInput" to max flowrate which is 50 L/min ex. 53
            {
              lcd.setCursor(0, 1);
              lcd.print("Invalid           ");
              lcd.noCursor();
              delay(1000);
              flowInput = 0;
              i = 0;
              lcd.cursor();
              lcd.setCursor(0, 1);
              lcd.print("00.0 L/min");
              lcd.setCursor(0, 1);
            }
            else 
            {
              lcd.print(key);                 //if "flowInput" is less than 50 it will print the key value
              lcd.setCursor(3, 1);            //moves cursor to avoid the decimal place
              i++;                            //increments "i", i =2
            }
            
          }

          else if (i == 2) 
          {
            flowInput = flowInput + 0.1 * (key -'0');  //stores tenths value in "flowInput"
            
            if (flowInput > 50)                        //compares "flowInput" to max flowrate which is 50 L/min ex. 50.3
            {
              lcd.setCursor(0, 1);
              lcd.print("Invalid           ");
              lcd.noCursor();
              delay(1000);
              flowInput = 0;
              i = 0;
              lcd.cursor();
              lcd.setCursor(0, 1);
              lcd.print("00.0 L/min");
              lcd.setCursor(0, 1);
            }
            else {
              lcd.print(key);                           //if "flowInput" is less than 50 it will print the key value
              lcd.noCursor();                           //removes cursor to show no more inputs are needed
              i++;                                      //increments "i", i=3. When i=3 program breaks out
          }
          }
          break;

       case '*':                                      //if '*' is inputted program will reset 
          flowInput = 0;
          i = 0;
          lcd.setCursor(0, 1);
          lcd.print("00.0 L/min");
          lcd.setCursor(0, 1);
          lcd.cursor();
          break;
    }

    key = keypad.getKey();    //programs takes in inputted key and then goes into switch 
 
  }

 return flowInput;      //once # of i=3 program will return 'flowInput' for 'GetFlow'
  
}


void setPointOutput()     //global function that convertes
{
  setPoint = GetFlow();               //calls 'GetFlow' for setpoint value
  setPointValue = setPoint * (255.0/50.0); //converting the inputted setPoint of 0-50L/min to 0-255 for the analogWrite
}

void motorspeedEntry()
{
  lcd.setCursor(0, 2);              //moves cursor to the start of row 3
  lcd.print("Enter motor speed:");  //prints "Enter motor speed:" on the 3rd row

  lcd.setCursor(0,3);               //moves cursor to the start of row 4
  lcd.print("00.0 RPM");
  lcd.setCursor(0,3);               //moves cursor back to the start of row 4
  lcd.cursor();                     //sets a visible cursor
}

float GetMotorSpeed()           //function gets value for motor speed, look at GetFlow() for ex.
{
  float motorspeedInput = 0;    //creates variable 'motorspeedInput'
  int j = 0;                    //creates increment variable 'j'
  char key = keypad.getKey();
  delay(50);
  while(key != '#' && j < 3)
  {
    
    switch (key)
    {
      case NO_KEY:
      
        break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
         if (j == 0) 
         {
            motorspeedInput = 10*(key - '0');
            if(motorspeedInput > 13)          //max motor speed is 13 RPM
            {
              lcd.setCursor(0, 3);
              lcd.print("Invalid             ");
              lcd.noCursor();
              delay(1000);
              motorspeedInput = 0;
              j = 0;
              lcd.cursor();
              lcd.setCursor(0, 3);
              lcd.print("00.0 RPM");
              lcd.setCursor(0, 3);
            }
            else
            {
              lcd.print(key);
              j++;
            }
          }
          
          else if (j == 1) 
          {
            motorspeedInput = motorspeedInput +(key - '0');
            if(motorspeedInput > 13)
            {
              lcd.setCursor(0, 3);
              lcd.print("Invalid             ");
              lcd.noCursor();
              delay(1000);
              motorspeedInput = 0;
              j = 0;
              lcd.cursor();
              lcd.setCursor(0, 3);
              lcd.print("00.0 RPM");
              lcd.setCursor(0, 3);
            }
            else
            {
            lcd.print(key);
            lcd.setCursor(3, 3);
            j++;
          }
          }
          else if (j == 2) 
          {
            motorspeedInput = motorspeedInput + 0.1*(key -'0');
            if(motorspeedInput > 13)
            {
              lcd.setCursor(0, 3);
              lcd.print("Invalid             ");
              lcd.noCursor();
              delay(1000);
              motorspeedInput = 0;
              j = 0;
              lcd.cursor();
              lcd.setCursor(0, 3);
              lcd.print("00.0 RPM");
              lcd.setCursor(0, 3);
            }
            else
            {
            lcd.print(key);
            j++;
            lcd.noCursor();
            }

          }
          break;

       case '*':
          motorspeedInput = 0;
          j = 0;
          lcd.setCursor(0,3);
          lcd.print("00.0 RPM");
          lcd.setCursor(0,3);
          lcd.cursor();
          break;
    }
    
    key = keypad.getKey();
 
  }

  return motorspeedInput; 
}

void motorspeedOutput ()
{
  motorspeed = GetMotorSpeed();         //gets 'GetMotorSpeed' for motorspeed value, between 0-13
  motorspeedValue = 255.0 * (motorspeed/13.0); //convert 0-13 inputted motor speed to 0-255 for analogWrite 
}

void readFlow()
{
  flowReadValue = analogRead(flowReadPin);    // reads the input pin and gets a value between 0-1023 
  
  total = total - flowReadings[readIndex];     
  flowReadings[readIndex] = flowReadValue;      //Creating an index that saves values of the flow to average
  total = total + flowReadings[readIndex];
  readIndex = readIndex +1;
  if (readIndex >= numReadings)
  {
   readIndex =0; 
  }
  avgflowReadValue = total / numReadings;
  flowReadDisplayed = avgflowReadValue * 50.0/1023.0; //converting the value of 0-1023 to 0-50 L/min
  
  lcd.setCursor(0, 1);
  lcd.print("FlowRate:");
  lcd.print(flowReadDisplayed, 2);  //prints current flowrate, to 2 decimal places
  lcd.print("L/min");
  delay (1000);
}

void Running ()   
{
  lcd.noCursor();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setpoint:");   
  lcd.print(setPoint, 1);   //prints inputted setpoint value
  lcd.print("L/min");

  lcd.setCursor(0, 2);
  lcd.print("Motor Speed:");
  lcd.print(motorspeed, 1);   //prints inputted motorspeed value
  lcd.print(" RPM");
  
  char key = keypad.getKey();
  delay(50);
  
  while (key != '#')    //while '#' is not press motor and flow controller will operate
  {
    analogWrite(motorspeedPin, motorspeedValue); //sending a pwm from motorspeedPin that will be converted to an analog signal that will control the motor
    analogWrite(setPointPin, setPointValue);    //sending a pwm from setPointPin that will be converted to an analog signal that will set the flow
    
    readFlow();
    
    switch(key)     //nothing will happen if other keys are pressed
    {
      case NO_KEY: case '0': case '1': case '2': case '3': case '4':    
      case '5': case '6': case '7': case '8': case '9': case '*':
        break;
    }
    
    key = keypad.getKey();

  }
}

void Reset()    //resets motorspeed and setpoint to 0 to restart program
{
  motorspeedValue = 0;
  setPointValue = 0;
  analogWrite(motorspeedPin, motorspeedValue);
  analogWrite(setPointPin, setPointValue);
}

void loop() 
{
  flowrateEntry();  //screen requests user to enter desired flowrate 
  setPointOutput(); //takes in the input flow and puts out the setpoint signal
  motorspeedEntry();// screen requests user to enter desired motor speed
  motorspeedOutput(); //takes in the input motor speed and puts out the motor speed signal
  Running(); //runs the components

  Reset(); //resets program
} 

