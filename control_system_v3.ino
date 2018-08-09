#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

#define MAXDO 30
#define MAXCLK 31

#define ultrasonicEchoPin 24
#define ultrasonicTrigPin 25

#pragma region // GLOBAL VARIABLE INITIALIZATION

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple1(MAXCLK, 32, MAXDO);
Adafruit_MAX31855 thermocouple2(MAXCLK, 33, MAXDO);
Adafruit_MAX31855 thermocouple3(MAXCLK, 34, MAXDO);
Adafruit_MAX31855 thermocouple4(MAXCLK, 35, MAXDO);
Adafruit_MAX31855 thermocouple5(MAXCLK, 36, MAXDO);
Adafruit_MAX31855 thermocouple6(MAXCLK, 37, MAXDO);
Adafruit_MAX31855 thermocouple7(MAXCLK, 38, MAXDO);
Adafruit_MAX31855 thermocouple8(MAXCLK, 39, MAXDO);
Adafruit_MAX31855 thermocouple9(MAXCLK, 40, MAXDO);
Adafruit_MAX31855 thermocouple10(MAXCLK, 41, MAXDO);
Adafruit_MAX31855 thermocouple11(MAXCLK, 42, MAXDO);
Adafruit_MAX31855 thermocouple12(MAXCLK, 43, MAXDO);
Adafruit_MAX31855 thermocouple13(MAXCLK, 44, MAXDO);
Adafruit_MAX31855 thermocouple14(MAXCLK, 45, MAXDO);
Adafruit_MAX31855 thermocouple15(MAXCLK, 46, MAXDO);
Adafruit_MAX31855 thermocouple16(MAXCLK, 47, MAXDO);
Adafruit_MAX31855 thermocouple17(MAXCLK, 48, MAXDO);
Adafruit_MAX31855 thermocouple18(MAXCLK, 49, MAXDO);
Adafruit_MAX31855 thermocouple19(MAXCLK, 50, MAXDO);
Adafruit_MAX31855 thermocouple20(MAXCLK, 51, MAXDO);
String tc[20];

LiquidCrystal_I2C lcd(0x3F, 20, 4); // set the LCD address to 0x3F for a 20 chars and 4 line display

//initializing keypad
const byte ROWS = 4; //four rows for keypad membrane
const byte COLS = 3; //four columns for keypad membrane

//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] =
    {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'*', '0', '#'}};

byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6};    //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int flowReadPin = 8;         // Reads Pin [8], output voltage caused by the current flow sesnsor
float flowReadValue = 0;     //flow sensor value, 0-5V but microcontroller reads it as 0-1023
float flowReadDisplayed = 0; //converted ^ to a 0-50.00 L/min to output to display
int flowReadPin2 = 9;         // Reads Pin [9], output voltage caused by the current flow sesnsor
float flowReadValue2 = 0;     //flow sensor value, 0-5V but microcontroller reads it as 0-1023
float flowReadDisplayed2 = 0; //converted ^ to a 0-50.00 L/min to output to display

const int numReadings = 40;    //determine the sizes of flow readings in the index
int flowReadings[numReadings]; //the readings from the analog input
int readIndex = 0;             //number of readings in the index above
float total = 0;               //add value of all the readings in the index
float avgflowReadValue = 0;    //the total divided by number of readings
int flowReadings2[numReadings]; //the readings from the analog input
int readIndex2 = 0;             //number of readings in the index above
float total2 = 0;               //add value of all the readings in the index
float avgflowReadValue2 = 0;    //the total divided by number of readings

int setPointPin = 11;    //signal to Pin [11] to control flow, 0-5V
float setPoint = 0;      //input signal 0-50 L/min
float setPointVolt = 0;  //converted ^ to 0-5V
float setPointValue = 0; //converted ^ to 0-255 for analog signal to flowmeter
int setPointPin2 = 12;    //signal to Pin [12] to control flow, 0-5V
float setPoint2 = 0;      //input signal 0-50 L/min
float setPointVolt2 = 0;  //converted ^ to 0-5V
float setPointValue2 = 0; //converted ^ to 0-255 for analog signal to flowmeter

int motorspeedPin = 13;    //signal to Pin [13] to control motor speed
float motorspeed = 0;      //input signal 0-13 RPM
float motorspeedValue = 0; //converted ^ to 0-255 for analog signal to motor driver

int motorspeedPin2 = 14;    //signal to Pin [14] to control motor speed
float motorspeed2 = 0;      //input signal 0-13 RPM
float motorspeedValue2 = 0; //converted ^ to 0-255 for analog signal to motor driver

float microVoltage = 0; //measured voltage from the +5V of microcontroller board

const int pressureTransducerPin = 12;
const int pressureTransducerPin2 = 13;

unsigned long ultrasonicCounter = 0;

int displayedMeasurement = 0;
// 0: pressure
// 1: temperature

#pragma endregion

#pragma region // HELPER METHODS

void getFlowSetpoint(float &sp, float &spVal, int index) { //function gets a value for setpoint of flow

    lcd.clear();                   //clears screen
    lcd.setCursor(0, 0);           //sets cursor to starting position
    lcd.print("Enter flow rate " + (String)index + ":"); //prints "Enter flow rate:" on the first row

    lcd.setCursor(0, 1);     //moves cursor to the start of row 2
    lcd.print("00.0 L/min"); //prints "00.0 L/min" on the 2nd row
    lcd.setCursor(0, 1);     //moves cursor back to (0,1)
    lcd.cursor();            //sets a visible cursor to show position

    float flowInput = 0;        //creates the "flowInput" variable
    int i = 0;                  //creates a variable that will be incremented
    char key = keypad.getKey(); //creates variable "key" and sets it to whatever is inputted
    delay(50);

    while (key != '#' && i < 3)
    {

        switch (key) //waits for "key" to define a case
        {
        case NO_KEY: //if no key is inputted it will keep looking until one is inputted
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4': //case when a number is inputted
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (i == 0) //takes in the 1st value
            {
                flowInput = 10 * (key - '0'); //stores tens value in "flowInput"
                if (flowInput > 50)           //compares "flowInput" to max flowrate which is 50 L/min
                {
                    lcd.setCursor(0, 1);
                    lcd.print("Invalid           "); //if greater tha 50 prints "Invalid"
                    lcd.noCursor();
                    delay(1000);
                    flowInput = 0; //resets "flowinput"
                    i = 0;         //resets "i"
                    lcd.cursor();
                    lcd.setCursor(0, 1);
                    lcd.print("00.0 L/min"); //resets screen
                    lcd.setCursor(0, 1);
                }
                else
                {
                    lcd.print(key); //if "flowInput" is less than 50 it will print the key value
                    i++;            //increments "i", i =1
                }
            }

            else if (i == 1)
            {
                flowInput = flowInput + (key - '0'); //stores ones value in "flowInput"
                if (flowInput > 50)                  //compares "flowInput" to max flowrate which is 50 L/min ex. 53
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
                    lcd.print(key);      //if "flowInput" is less than 50 it will print the key value
                    lcd.setCursor(3, 1); //moves cursor to avoid the decimal place
                    i++;                 //increments "i", i =2
                }
            }

            else if (i == 2)
            {
                flowInput = flowInput + 0.1 * (key - '0'); //stores tenths value in "flowInput"

                if (flowInput > 50) //compares "flowInput" to max flowrate which is 50 L/min ex. 50.3
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
                    lcd.print(key); //if "flowInput" is less than 50 it will print the key value
                    lcd.noCursor(); //removes cursor to show no more inputs are needed
                    i++;            //increments "i", i=3. When i=3 program breaks out
                }
            }
            break;

        case '*': //if '*' is inputted program will reset
            flowInput = 0;
            i = 0;
            lcd.setCursor(0, 1);
            lcd.print("00.0 L/min");
            lcd.setCursor(0, 1);
            lcd.cursor();
            break;
        }

        key = keypad.getKey(); //programs takes in inputted key and then goes into switch
    }

    sp = flowInput; //once # of i=3 program will return 'flowInput' for 'getFlowSetpoint'

    spVal = sp * (255.0 / 50.0); //converting the inputted setPoint of 0-50L/min to 0-255 for the analogWrite
    // There was an error between what the user was asking for and the sent value
    // This function comes from a linear fit of the arduino read vs the actual voltage
    // The linear fit is (arduino) = 0.9649(actual) - 0.009. The R^2 = 1.
    spVal += 0.009;
    spVal /= 0.9649;
}

void getMotorSpeed(float &ms, float &msVal, int index) {//function gets value for motor speed, look at getFlowSetpoint() for ex.
    lcd.setCursor(0, 2);             //moves cursor to the start of row 3
    lcd.print("Enter motor speed " + (String)index + ":"); //prints "Enter motor speed:" on the 3rd row

    lcd.setCursor(0, 3); //moves cursor to the start of row 4
    lcd.print("00.0 RPM");
    lcd.setCursor(0, 3);       //moves cursor back to the start of row 4
    lcd.cursor();              //sets a visible cursor
    float motorspeedInput = 0; //creates variable 'motorspeedInput'
    int j = 0;                 //creates increment variable 'j'
    char key = keypad.getKey();
    delay(50);
    while (key != '#' && j < 3)
    {

        switch (key)
        {
        case NO_KEY:

            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (j == 0)
            {
                motorspeedInput = 10 * (key - '0');
                if (motorspeedInput > 13) //max motor speed is 13 RPM
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
                motorspeedInput = motorspeedInput + (key - '0');
                if (motorspeedInput > 13)
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
                motorspeedInput = motorspeedInput + 0.1 * (key - '0');
                if (motorspeedInput > 13)
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
            lcd.setCursor(0, 3);
            lcd.print("00.0 RPM");
            lcd.setCursor(0, 3);
            lcd.cursor();
            break;
        }

        key = keypad.getKey();
    }

    ms = motorspeedInput;
    msVal = 255.0 * (ms / 13.0); //convert 0-13 inputted motor speed to 0-255 for analogWrite
}

// prints the given information to the LCD; improves code readability and cuts down on repetition
void print_lcd(String measurement, String reading, String units, int row) {
    lcd.setCursor(0, row);
    lcd.print(measurement + ": ");
    lcd.print(reading + " ");
    lcd.print(units);
}

void readFlow(int index) {
    if (index == 1) {
        flowReadValue = analogRead(flowReadPin); // reads the input pin and gets a value between 0-1023

        total = total - flowReadings[readIndex];
        flowReadings[readIndex] = flowReadValue; //Creating an index that saves values of the flow to average
        total = total + flowReadValue;
        readIndex++;
        if (readIndex >= numReadings)
        {
            readIndex = 0;
        }
        avgflowReadValue = total / numReadings;
        flowReadDisplayed = avgflowReadValue * microVoltage * 10 / 1024.0; //converting the value of 0-1023 to 0-50 L/min
        // There was an error between what the controller was reporting and the actual value
        // This function comes from a linear fit of the arduino read vs the actual voltage
        // The linear fit is (arduino) = 10.145(actual) - 0.1642. The R^2 = 1.
        // To get from the inverse of the linear fit to the correct voltage, multiplied by 10
        flowReadDisplayed += 0.1642;
        flowReadDisplayed /= 10.145;
        flowReadDisplayed *= 10;
    }
    else if (index == 2) {
        flowReadValue2 = analogRead(flowReadPin); // reads the input pin and gets a value between 0-1023

        total2 = total2 - flowReadings2[readIndex2];
        flowReadings2[readIndex2] = flowReadValue2; //Creating an index that saves values of the flow to average
        total2 = total2 + flowReadValue2;
        readIndex2++;
        if (readIndex2 >= numReadings)
        {
            readIndex2 = 0;
        }
        avgflowReadValue2 = total2 / numReadings;
        flowReadDisplayed2 = avgflowReadValue2 * microVoltage * 10 / 1024.0; //converting the value of 0-1023 to 0-50 L/min
        // There was an error between what the controller was reporting and the actual value
        // This function comes from a linear fit of the arduino read vs the actual voltage
        // The linear fit is (arduino) = 10.145(actual) - 0.1642. The R^2 = 1.
        // To get from the inverse of the linear fit to the correct voltage, multiplied by 10
        flowReadDisplayed2 += 0.1642;
        flowReadDisplayed2 /= 10.145;
        flowReadDisplayed2 *= 10;
    }

}

// reads from Omega pressure transducer on given pin
float readPressure(int pin) {
    float pressureReading = analogRead(pin);
    pressureReading *= 5 / 4.848;    // adjustment for voltage not being exactly 5V
    pressureReading *= (12.5 / 512); // converts to Pa; 12.5 Pa per 2.5V
    return pressureReading;
}

int readDistance(int trigPin, int echoPin) {
    /* The following trigPin/echoPin cycle is used to determine the
    distance of the nearest object by bouncing soundwaves off of it. */
    digitalWrite(trigPin, LOW);
    //Serial.println("trigPin: " + String(trigPin));
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    int duration = pulseIn(echoPin, HIGH);
    //Serial.println("duration: " + String(duration));

    //Calculate the distance (in cm) based on the speed of sound.
    return (int)(duration / 58.2);
}

#pragma endregion

#pragma region // PROGRAM FLOW METHODS

void setup() {
    // debug
    Serial.begin(115200);
    // Serial.println("Hello Computer");

    lcd.init();       //initialize lcd
    lcd.backlight();  //turns on backlight
    lcd.begin(20, 4); // set up the LCD's number of columns and rows
    lcd.noAutoscroll();

    keypad.setDebounceTime(10);
    TCCR2B = TCCR2B & 0b11111000 | 0x01; // sets timer 1 frequency to 31372.55 Hz
    // important because the higher frequency is better for our low-pass filter
    

    pinMode(setPointPin, OUTPUT);   //sets degital Pin [11] as an output
    pinMode(setPointPin2, OUTPUT);
    pinMode(motorspeedPin, OUTPUT); //sets digital Pin [13] as an output
    pinMode(motorspeedPin2, OUTPUT);

    lcd.print("Welcome!"); //prints "Welcome!"
    // delay(3000);

    pinMode(ultrasonicTrigPin, OUTPUT);
    pinMode(ultrasonicEchoPin, INPUT);

    microVoltage = 4.951; //max output voltage of microcontroller
}

void running() {
    lcd.noCursor();
    lcd.clear();
    
    print_lcd("S.P.s", String(setPoint, 1) + "," + String(setPoint2, 1), "L/m", 0);

    print_lcd("M.S.s", String(motorspeed, 1) + "," + String(motorspeed2, 1), "RPM", 2);
    
    // JSON serial communication
    // Serial.println("{");
    // Serial.println("    \"SP\":[" + String(setPoint, 1) + "," + String(setPoint2,1) + "],");
    // Serial.println("    \"MS\":[" + String(motorspeed, 1) + "," + String(motorspeed2, 1) + "]");
    // Serial.println("}");
    // Serial.println("=====");
    // Serial.println("{");

    char key = keypad.getKey();
    delay(50);
    unsigned long counter = 0;
    unsigned long timer = millis();

    while (key != '#') //while '#' is not press motor and flow controller will operate
    {
        analogWrite(motorspeedPin, motorspeedValue); //sending a pwm from motorspeedPin that will be converted to an analog signal that will control the motor
        analogWrite(motorspeedPin2, motorspeedValue2);
        analogWrite(setPointPin, setPointValue);     //sending a pwm from setPointPin that will be converted to an analog signal that will set the flow
        analogWrite(setPointPin2, setPointValue2);

        readFlow(1);
        readFlow(2);
        print_lcd("F.R.s", String(flowReadDisplayed, 1) + "," + String(flowReadDisplayed2, 1), "L/m", 1);

        key = keypad.getKey();
        //Serial.println(String(thermocouple1.readCelsius(), 1));

        // displays a different measurement on the bottom row based on * key push
        // collects measurements
        if (key == NO_KEY)
            key = keypad.getKey();
        String pressure = String(readPressure(pressureTransducerPin), 1);
        String pressure2 = String(readPressure(pressureTransducerPin2), 1);
        String temperature = tc[0];
        String distance = String(readDistance(ultrasonicTrigPin, ultrasonicEchoPin));
        String weight = "0";
        tc[0] = String(thermocouple1.readCelsius());
        if (key == NO_KEY)
            key = keypad.getKey();
        if (counter % 1 == 0) {
            // Serial.println("Starting collection");
            tc[1] = String(thermocouple2.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[2] = String(thermocouple3.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[3] = String(thermocouple4.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[4] = String(thermocouple5.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[5] = String(thermocouple6.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[6] = String(thermocouple7.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[7] = String(thermocouple8.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[8] = String(thermocouple9.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[9] = String(thermocouple10.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[10] = String(thermocouple11.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[11] = String(thermocouple12.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[12] = String(thermocouple13.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[13] = String(thermocouple14.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[14] = String(thermocouple15.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[15] = String(thermocouple16.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[16] = String(thermocouple17.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[17] = String(thermocouple18.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[18] = String(thermocouple19.readCelsius());
            if (key == NO_KEY)
                key = keypad.getKey();
            tc[19] = String(thermocouple20.readCelsius());
            // print data to be plotted
            String output = "";
            for (int i = 0; i < 19; i++) {
                output += tc[i] + " ";
            }
            output += tc[19] + "~";
            output += pressure + " " + pressure2 + "~";
            output += distance + "~";
            output += weight;
            Serial.println(output);
            // Serial.println("Finished collection");
            // print out JSON data to serial monitor
            // if (counter != 0)
            //     Serial.println(",");
            // Serial.println("    \"read" + String(int(counter/10)) + "\":{");
            // Serial.println("        \"TC\": [");
            // for (int i = 0; i < 19; i++) {
            //     Serial.println("            " + tc[i] + ",");
            // }
            // Serial.println("            " + tc[19]);
            // Serial.println("        ],");
            // Serial.println("        \"P\": [");
            // Serial.println("            " + pressure  + ",");
            // Serial.println("            " + pressure2);
            // Serial.println("        ],");
            // Serial.println("        \"D\":" + distance + ",");
            // Serial.println("        \"W\":" + weight);
            // Serial.print("    }");
        }

        if (key == NO_KEY)
            key = keypad.getKey();

        switch(displayedMeasurement) {
            case 0: // pressure
                print_lcd("Pressure", pressure, "Pa  ", 3);
                break;
            case 1: // temperature
                print_lcd("Temperature", temperature, "C ", 3);
                break;
            case 2: // distance
                print_lcd("Distance", distance, "cm    ", 3);
                break;
            case 3:
                print_lcd("Weight", weight, "lbs   ", 3);
        }
        if (key == NO_KEY) {
            key = keypad.getKey();
        }
        switch (key) //nothing will happen if other keys are pressed
        {
            case NO_KEY:
                break;
            case '*':
                displayedMeasurement++;
                displayedMeasurement %= 4;
            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '#':
                break;
        }
        counter++;
        // Serial.println((String)(millis() - timer));
        while (millis() - timer < 2000) ;
        timer = millis();
        // Serial.println("Loop done");
    }

}

void reset() { //resets motorspeed and setpoint to 0 to restart program
    motorspeedValue = 0;
    setPointValue = 0;
    analogWrite(motorspeedPin, motorspeedValue);
    analogWrite(setPointPin, setPointValue);
}

void loop() {
    getFlowSetpoint(setPoint, setPointValue, 1);   //takes in the input flow and puts out the setpoint signal
    lcd.clear();
    getFlowSetpoint(setPoint2, setPointValue2, 2);
    getMotorSpeed(motorspeed, motorspeedValue, 1);  // screen requests user to enter desired motor speed
    lcd.clear();
    getMotorSpeed(motorspeed2, motorspeedValue2, 2);  

    running();          //runs the components
    // Serial.println();
    // Serial.println("}");
    // Serial.println("==========");

    reset(); //resets program
}

#pragma endregion
