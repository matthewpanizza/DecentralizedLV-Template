/* 
 * Project DecentralizedLV-Template
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "DecentralizedLV-Boards/DecentralizedLV-Boards.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

CAN_Controller canController;               //Create an instance of the CAN Controller
LV_CANMessage canMessage;                   //Custom CAN bus message object for receiving CAN messages
DashController_CAN dc(DASH_CONTROL_ADDR);   //Create a Dash Controller that transmits on 0x99

uint64_t timerSnapshot = 0;

void setup(){
    Serial.begin(9600);                         //Start a serial session so you can see the output

    canController.begin(CAN_500KBPS, S3);       //Start CAN bus transmission at 500kbps CAN on a P2 with a MCP2515 that has its chip select on S3.
    canController.addFilter(dc.boardAddress);   //Allow incoming messages from Power Controller

    RGB.control(true);                          //Enable control of the RGB LED for debugging

    pinMode(A0, INPUT);                         //Use INPUT, INPUT_PULLUP, INPUT_PULLDOWN for reading GPIO Inputs (for Buttons, Switches, etc.) 
    pinMode(A1, INPUT);                         //Use INPUT for analog read values (for voltage dividers, sensors, etc.)
    pinMode(A2, OUTPUT);                        //Use OUTPUT for PWM output (for dimming MOSFETs)
    pinMode(A3, OUTPUT);                        //Use OUTPUT for binary GPIO pins (for MOSFETs, LEDs, etc.)
}

void loop(){
    if(canController.receive(canMessage)){  //Check if we received a message
        dc.receiveCANData(canMessage);      //Process the message. This receiveCANData has a filter built in for which CAN ID this was received on.

        //Now that we've received the CAN bus data from the Dashboard Controller, you can just access the fields to get the data!
        Serial.printlnf("Received the right turn signal value of %d from the Dash Controller!", dc.rightTurnPWM);

        //Set the onboard RGB LED to a color based on the data we received. Function takes (R, G, B) arguments.
        RGB.color(dc.rightTurnPWM, dc.leftTurnPWM, dc.headlight * 255);
    }

    bool buttonPressed = digitalRead(A0);   //Read if this pin is low (below 0.6V) or high (above 2.5V). In between is unknown.
    uint16_t voltageRead = analogRead(A1);  //Read voltage at this pin. 3.3V reads 1023, 0V reads 0. Scales linearly.
    analogWrite(A2, 128);                   //Set A2 to output 50% duty cycle
    digitalWrite(A3, HIGH);                 //Set A3 to 3.3V

    if(System.millis() - timerSnapshot > 5000){     //Non-blocking loop example
        timerSnapshot = System.millis();            //Update the snapshot of the clock so it doesn't print again until 5 seconds from now
        Serial.print("Printing every 5 seconds!");
    }
}    
