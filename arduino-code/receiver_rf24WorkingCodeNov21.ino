/*
   Created by Michael Shiloh, Modified by Pranav & Ahsen: Fall 2024
   This code has commented out part of the transmitter. For the transmitter's code, please check the other code file in the repo.

   Using the nRF24L01 radio module to communicate
   between two Arduinos with much increased reliability following
   various tutorials, conversations, and studying the nRF24L01 datasheet
   and the library reference.

   Transmitter is
   https://github.com/michaelshiloh/resourcesForClasses/tree/master/kicad/Arduino_Shield_RC_Controller

  Receiver is
  https://github.com/michaelshiloh/resourcesForClasses/blob/master/kicad/nRF_servo_Mega

   This file contains code for both transmitter and receiver.
   Transmitter at the top, receiver at the bottom.
   One of them is commented out, so you need to comment in or out
   the correct section. You don't need to make changes to this 
   part of the code, just to comment in or out depending on
   whether you are programming your transmitter or receiver

   You need to set the correct address for your robot.

   Search for the phrase CHANGEHERE to see where to 
   comment or uncomment or make changes.

   These sketches require the RF24 library by TMRh20
   Documentation here: https://nrf24.github.io/RF24/index.html

   change log

   11 Oct 2023 - ms - initial entry based on
                  rf24PerformingRobotsTemplate
   26 Oct 2023 - ms - revised for new board: nRF_Servo_Mega rev 2
   28 Oct 2023 - ms - add demo of NeoMatrix, servo, and Music Maker Shield
	 20 Nov 2023 - as - fixed the bug which allowed counting beyond the limits
   22 Nov 2023 - ms - display radio custom address byte and channel
   12 Nov 2024 - ms - changed names for channel and address allocation for Fall 2024                  
                      https://github.com/michaelshiloh/resourcesForClasses/blob/master/kicad/nRF_servo_Mega    
                      https://github.com/michaelshiloh/resourcesForClasses/blob/master/kicad/nRFControlPanel
*/


// Common code
//

// Common pin usage
// Note there are additional pins unique to transmitter or receiver
//

// nRF24L01 uses SPI which is fixed
// on pins 11, 12, and 13 on the Uno
// and on pins 50, 51, and 52 on the Mega

// It also requires two other signals
// (CE = Chip Enable, CSN = Chip Select Not)
// Which can be any pins:

// CHANGEHERE
// For the transmitter
// const int NRF_CE_PIN = A4, NRF_CSN_PIN = A5;

// CHANGEHERE
// for the receiver
const int NRF_CE_PIN = A11, NRF_CSN_PIN = A15;

// nRF 24L01 pin   name
//          1      GND
//          2      3.3V
//          3      CE
//          4      CSN
//          5      SCLK
//          6      MOSI/COPI
//          7      MISO/CIPO

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);  // CE, CSN

//#include <printf.h>  // for debugging

// See note in rf24Handshaking about address selection
//

// Channel and address allocation:
// Rama and Hind Y: Channel 30, addr = 0x76
// Ahsen and Pranav: Channel 40, addr = 0x73
// Sara & Toomie:  Channel 50, addr = 0x7C
// Avinash and Vahagn: Channel 60, addr = 0xC6
// Hind A & Javeria:  Channel 70, addr = 0xC3
// Mbebo and Aaron: Channel 80, addr = 0xCC
// Linh and Luke: Channel 90, addr = 0x33

// CHANGEHERE
const byte CUSTOM_ADDRESS_BYTE = 0x73;  // change as per the above assignment
const int CUSTOM_CHANNEL_NUMBER = 40;   // change as per the above assignment

// Do not make changes here
const byte xmtrAddress[] = { CUSTOM_ADDRESS_BYTE, CUSTOM_ADDRESS_BYTE, 0xC7, 0xE6, 0xCC };
const byte rcvrAddress[] = { CUSTOM_ADDRESS_BYTE, CUSTOM_ADDRESS_BYTE, 0xC7, 0xE6, 0x66 };

const int RF24_POWER_LEVEL = RF24_PA_LOW;

// global variables
uint8_t pipeNum;
unsigned int totalTransmitFailures = 0;

struct DataStruct {
  uint8_t stateNumber;
};
DataStruct data;

void setupRF24Common() {

  // RF24 setup
  if (!radio.begin()) {
    Serial.println(F("radio  initialization failed"));
    while (1)
      ;
  } else {
    Serial.println(F("radio successfully initialized"));
  }

  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(CUSTOM_CHANNEL_NUMBER);
  radio.setPALevel(RF24_POWER_LEVEL);
}

// CHANGEHERE
/*
// Transmitter code

// Transmitter pin usage
const int LCD_RS_PIN = 3, LCD_EN_PIN = 2, LCD_D4_PIN = 4, LCD_D5_PIN = 5, LCD_D6_PIN = 6, LCD_D7_PIN = 7;
const int SW1_PIN = 8, SW2_PIN = 9, SW3_PIN = 10, SW4_PIN = A3, SW5_PIN = A2;

// LCD library code
#include <LiquidCrystal.h>

// initialize the library with the relevant pins
LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);


const int NUM_OF_STATES = 6;
char* theStates[] = { "0 robot wakes",
                      "1 robot hello",
                      "2 robot explains",
                      "3 robot pleads",
                      "4 robot weeps",
                      "5 robot goodbye" };

void updateLCD() {

  lcd.clear();
  lcd.print(theStates[data.stateNumber]);
  lcd.setCursor(0, 1);  // column, line (from 0)
  lcd.print("not transmitted yet");
}

void countDown() {
  data.stateNumber = (data.stateNumber > 0) ? (data.stateNumber - 1) : 0;
  updateLCD();
}

void countUp() {
  if (++data.stateNumber >= NUM_OF_STATES) {
    data.stateNumber = NUM_OF_STATES - 1;
  }
  updateLCD();
}


void spare1() {}
void spare2() {}

void rf24SendData() {

  radio.stopListening();  // go into transmit mode
  // The write() function will block
  // until the message is successfully acknowledged by the receiver
  // or the timeout/retransmit maxima are reached.
  int retval = radio.write(&data, sizeof(data));

  lcd.clear();
  lcd.setCursor(0, 0);  // column, line (from 0)
  lcd.print("transmitting");
  lcd.setCursor(14, 0);  // column, line (from 0)
  lcd.print(data.stateNumber);

  Serial.print(F(" ... "));
  if (retval) {
    Serial.println(F("success"));
    lcd.setCursor(0, 1);  // column, line (from 0)
    lcd.print("success");
  } else {
    totalTransmitFailures++;
    Serial.print(F("failure, total failures = "));
    Serial.println(totalTransmitFailures);

    lcd.setCursor(0, 1);  // column, line (from 0)
    lcd.print("error, total=");
    lcd.setCursor(13, 1);  // column, line (from 0)
    lcd.print(totalTransmitFailures);
  }
}

class Button {
  int pinNumber;
  bool previousState;
  void (*buttonFunction)();
public:

  // Constructor
  Button(int pn, void* bf) {
    pinNumber = pn;
    buttonFunction = bf;
    previousState = 1;
  }

  // update the button
  void update() {
    bool currentState = digitalRead(pinNumber);
    if (currentState == LOW && previousState == HIGH) {
      Serial.print("button on pin ");
      Serial.print(pinNumber);
      Serial.println();
      buttonFunction();
    }
    previousState = currentState;
  }
};

const int NUMBUTTONS = 5;
Button theButtons[] = {
  Button(SW1_PIN, countDown),
  Button(SW2_PIN, rf24SendData),
  Button(SW3_PIN, countUp),
  Button(SW4_PIN, spare1),
  Button(SW5_PIN, spare2),
};

void setupRF24() {

  setupRF24Common();

  // Set us as a transmitter
  radio.openWritingPipe(xmtrAddress);
  radio.openReadingPipe(1, rcvrAddress);

  // radio.printPrettyDetails();
  Serial.println(F("I am a transmitter"));

  data.stateNumber = 0;
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Setting up LCD"));

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  // Print a message to the LCD.
  lcd.print("Radio setup");

  // Display the address in hex
  lcd.setCursor(0, 1);
  lcd.print("addr 0x");
  lcd.setCursor(7, 1);
  char s[5];
  sprintf(s, "%02x", CUSTOM_ADDRESS_BYTE);
  lcd.print(s);

  // Display the channel number
  lcd.setCursor(10, 1);
  lcd.print("ch");
  lcd.setCursor(13, 1);
  lcd.print(CUSTOM_CHANNEL_NUMBER);

  Serial.println(F("Setting up radio"));
  setupRF24();

  // If setupRF24 returned then the radio is set up
  lcd.setCursor(0, 0);
  lcd.print("Radio OK state=");
  lcd.print(theStates[data.stateNumber]);

  // Initialize the switches
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SW3_PIN, INPUT_PULLUP);
  pinMode(SW4_PIN, INPUT_PULLUP);
  pinMode(SW5_PIN, INPUT_PULLUP);
}



void loop() {
  for (int i = 0; i < NUMBUTTONS; i++) {
    theButtons[i].update();
  }
  delay(50);  // for testing
}


void clearData() {
  // set all fields to 0
  data.stateNumber = 0;
}

// End of transmitter code
// CHANGEHERE
*/

// Receiver Code
// CHANGEHERE

// Additional libraries for music maker shield
#include <Adafruit_VS1053.h>
#include <SD.h>

// Servo library
#include <Servo.h>

// Additional libraries for graphics on the Neo Pixel Matrix
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

// Additional pin usage for receiver

// Adafruit music maker shield
#define SHIELD_RESET -1  // VS1053 reset pin (unused!)
#define SHIELD_CS 7      // VS1053 chip select pin (output)
#define SHIELD_DCS 6     // VS1053 Data/command select pin (output)
#define CARDCS 4         // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3  // VS1053 Data request, ideally an Interrupt pin
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

// Connectors for NeoPixels and Servo Motors are labeled
// M1 - M6 which is not very useful. Here are the pin
// assignments:
// M1 = 19 // neo pixel
// M2 = 21 //left servo
// M3 = 20
// M4 = 16
// M5 = 18
// M6 = 17 //right servo

#define LED_PIN 19
#define LED_COUNT_1 16
#define LED_COUNT_2 16

Adafruit_NeoPixel ring1(LED_COUNT_1, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring2(LED_COUNT_2, LED_PIN, NEO_GRB + NEO_KHZ800);

Servo LeftWing;
Servo RightWing;

int LeftWingPin = 21;
int RightWingPin = 17;

int mappedLeftAngle;
int mappedRightAngle;

// Function prototype
void moveServosSlowly(Servo &leftServo, Servo &rightServo, int startAngleLeft, int endAngleLeft, int startAngleRight, int endAngleRight);

void setup() {
  Serial.begin(9600);
  // printf_begin();

  // Set up all the attached hardware
  setupMusicMakerShield();
  setupServoMotors();
  setupNeoPixels();

  setupRF24();

  // Brief flash to show we're done with setup()
  flashNeoPixels();
}

void setupRF24() {
  setupRF24Common();

  // Set us as a receiver
  radio.openWritingPipe(rcvrAddress);
  radio.openReadingPipe(1, xmtrAddress);

  // radio.printPrettyDetails();
  Serial.println(F("I am a receiver"));
}

void setupMusicMakerShield() {
  if (!musicPlayer.begin()) {  // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1)
      ;
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD card failed or not present"));
    while (1)
      ;  // don't do anything more
  }

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20, 20);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
}

void setupServoMotors() {
  // Set up the wings 
  LeftWing.attach(LeftWingPin);
  RightWing.attach(RightWingPin);

  // resting position
  int leftAngle = 100;
  int rightAngle = 26;

  mappedLeftAngle = map(leftAngle, 0, 270, 0, 180);
  mappedRightAngle = map(rightAngle, 0, 270, 0, 180);

  LeftWing.write(mappedLeftAngle);
  RightWing.write(mappedRightAngle);
}

void setupNeoPixels() {
  ring1.begin();           
  ring1.show();            
  ring1.setBrightness(15); 
  ring2.begin();           
  ring2.show();            
  ring2.setBrightness(15); 
}

void flashNeoPixels() {
  for(int i = 0; i < ring1.numPixels(); i++){
          // ring.setPixelColor(i, random(255), random(255), random(255), 0);
          ring1.setPixelColor(i, 255, 255, 255, 0);
          ring1.show();
          ring2.setPixelColor(i, 255, 255, 255, 0);
          ring2.show();
          delay(50);
        }
        delay(150);
        for(int i = ring1.numPixels()-1; i >= 0; i--){
          ring1.setPixelColor(i, 0, 0, 0, 0);
          ring1.show();
          ring2.setPixelColor(i, 0, 0, 0, 0);
          ring2.show();
          delay(50);
        }
}

void scaredEyes() {
  for(int i = 0; i < ring1.numPixels(); i++){
          // ring.setPixelColor(i, random(255), random(255), random(255), 0);
          ring1.setPixelColor(i, 138,43,226, 0);
          ring1.show();
          ring2.setPixelColor(i, 138,43,226, 0);
          ring2.show();
          delay(50);
        }
}

void scaredBlinkingEyes() {

    int counter = 0;
    while (counter != 10) { // Infinite blinking loop
    // Turn all LEDs to purple
    for (int i = 0; i < ring1.numPixels(); i++) {
      ring1.setPixelColor(i, 138, 43, 226); // Purple
      ring2.setPixelColor(i, 138, 43, 226); // Purple
    }
    ring1.show();
    ring2.show();
    delay(500); // Keep LEDs on for 500ms

    // Turn all LEDs off
    for (int i = 0; i < ring1.numPixels(); i++) {
      ring1.setPixelColor(i, 0, 0, 0); // Off
      ring2.setPixelColor(i, 0, 0, 0); // Off
    }
    ring1.show();
    ring2.show();
    delay(500); // Keep LEDs off for 500ms
    counter++;
  }
}

void angryEyes() {
  for(int i = 0; i < ring1.numPixels(); i++){
          // ring.setPixelColor(i, random(255), random(255), random(255), 0);
          ring1.setPixelColor(i, 255, 0, 0, 0);
          ring1.show();
          ring2.setPixelColor(i, 255, 0, 0, 0);
          ring2.show();
          delay(50);
        }
}

void whiteEyes() {
  ring1.begin();                  
  ring1.setBrightness(40); 
  ring2.begin();           
  ring2.setBrightness(40); 

  // Set all LEDs to white
  for (int i = 0; i < ring1.numPixels(); i++) {
    ring1.setPixelColor(i, 255, 255, 255); // Full white
    ring2.setPixelColor(i, 255, 255, 255); // Full white
  }
  ring1.show();     
  ring2.show();
}

void closingEyes() {
  for(int i = ring1.numPixels()-1; i >= 0; i--){
          ring1.setPixelColor(i, 0, 0, 0, 0);
          ring1.show();
          ring2.setPixelColor(i, 0, 0, 0, 0);
          ring2.show();
          delay(50);
        }
}

void eyesShut() {
  // Turn all LEDs off
    for (int i = 0; i < ring1.numPixels(); i++) {
      ring1.setPixelColor(i, 0, 0, 0); // Off
      ring2.setPixelColor(i, 0, 0, 0); // Off
    }
    ring1.show();
    ring2.show();
}


void moveServosSlowly(Servo &leftServo, Servo &rightServo, int startAngleLeft, int endAngleLeft, int startAngleRight, int endAngleRight) {
  int maxSteps = max(abs(endAngleLeft - startAngleLeft), abs(endAngleRight - startAngleRight));
  
  for (int step = 0; step <= maxSteps; ++step) {
    int currentAngleLeft = startAngleLeft + (endAngleLeft - startAngleLeft) * step / maxSteps;
    int currentAngleRight = startAngleRight + (endAngleRight - startAngleRight) * step / maxSteps;
    
    leftServo.write(map(currentAngleLeft, 0, 270, 0, 180));
    rightServo.write(map(currentAngleRight, 0, 270, 0, 180));
    
    delay(50);  // Adjust delay for slower or faster movement
  }
  
  // Ensure final positions are reached
  leftServo.write(map(endAngleLeft, 0, 270, 0, 180));
  rightServo.write(map(endAngleRight, 0, 270, 0, 180));
}

int targetLeftAngle = 100; 
int targetRightAngle = 26;

void loop() {
  // If there is data, read it,
  // and do the needfull
  // Become a receiver
  radio.startListening();
  if (radio.available(&pipeNum)) {
    radio.read(&data, sizeof(data));
    Serial.print(F("message received Data = "));
    Serial.print(data.stateNumber);
    Serial.println();

    // Resting angles (hiding behind the robot)
    int leftAngle = 100;
    int rightAngle = 26;

    switch (data.stateNumber) {
      case 0:
        Serial.print(F("1 - Mrs. Peacock enters the scene"));
        musicPlayer.startPlayingFile("/track001.mp3");
        flashNeoPixels();
        moveServosSlowly(LeftWing, RightWing, leftAngle, 30, rightAngle, 108); //tail opens
        whiteEyes();
        break;
      
      case 1:
        Serial.println(F("2 - Mrs. Peacock: What's all this party about"));
        musicPlayer.startPlayingFile("/track002.mp3");
        // // // slow tail retract
        // leftAngle = 30;
        // rightAngle = 108;
        // moveServosSlowly(LeftWing, RightWing, leftAngle, targetLeftAngle, rightAngle, targetRightAngle);
        break;
      
      case 2:
        Serial.println(F("2.5 - Gasp"));
        musicPlayer.startPlayingFile("/gasp0001.mp3");
        moveServosSlowly(LeftWing, RightWing, 30, leftAngle, 108, rightAngle); //tail retracts
        scaredEyes();
        break;

      case 3:
        Serial.println(F("3 - Get me out of here this instant!"));
        musicPlayer.playFullFile("gasp0001.mp3"); //blocker function - only proceeds after the full file is played
        musicPlayer.startPlayingFile("/track003.mp3"); //non-blocker function
        break;
      
      case 4: //added
        Serial.println(F("3.5 - Suspenseful music"));
        musicPlayer.startPlayingFile("/bgmus001.mp3"); //suspicious music
        break;

      case 5: //added
        Serial.println(F("3.9 - gasps!"));
        musicPlayer.playFullFile("gasp0001.mp3");
        break;


      case 6:
        Serial.println(F("4 - He's dead!"));
        musicPlayer.playFullFile("gasp0001.mp3");
        musicPlayer.startPlayingFile("/track004.mp3");
        break;
        
        //tail opens quickly
        leftAngle = 30; // 30 is the angle - wings spread out, and 100 is the angle where it's vertical
        rightAngle = 108; // 150 is the angle - wings spread out, and 26 is the angle where it's vertical
        mappedLeftAngle = map(leftAngle, 0, 270, 0, 180);
        mappedRightAngle = map(rightAngle, 0, 270, 0, 180);
        LeftWing.write(mappedLeftAngle);
        RightWing.write(mappedRightAngle);

        scaredBlinkingEyes();
        scaredEyes();
        delay(10000);
        eyesShut();
        // moveServosSlowly(LeftWing, RightWing, 30, leftAngle, 108, rightAngle); //tail retracts and going into the background
        break;
      
      case 7:
        Serial.println(F("5 - Why was I called to this madhouse"));
        musicPlayer.startPlayingFile("/track005.mp3");
        moveServosSlowly(LeftWing, RightWing, 30, leftAngle, 108, rightAngle); //tail retracts
        scaredEyes();
        break;

      case 8:
        Serial.println(F("6 - Buttler take me somewhere safe"));
        musicPlayer.startPlayingFile("/track006.mp3");
        break;
        
      case 9:
        Serial.println(F("7 - Stop standing about then and take me"));
        musicPlayer.startPlayingFile("/track007.mp3");
        break;

      case 10:
        Serial.println(F("8 - Wretched Prison - How do I leave?"));
        whiteEyes();
        musicPlayer.playFullFile("/track008.mp3");
        musicPlayer.startPlayingFile("/bgmus001.mp3"); //suspicious music
        break;

      case 11:
        Serial.println(F("9 - Are you hinting at something here?"));
        musicPlayer.startPlayingFile("/track009.mp3"); 
        
        //tail opens quickly
        leftAngle = 30; // 30 is the angle - wings spread out, and 100 is the angle where it's vertical
        rightAngle = 108; // 150 is the angle - wings spread out, and 26 is the angle where it's vertical
        mappedLeftAngle = map(leftAngle, 0, 270, 0, 180);
        mappedRightAngle = map(rightAngle, 0, 270, 0, 180);
        LeftWing.write(mappedLeftAngle);
        RightWing.write(mappedRightAngle);

        angryEyes();
        break;

      case 12:
        Serial.println(F("10 - Those payments..., you simpleton"));
        musicPlayer.startPlayingFile("/track010.mp3");
        break;

      case 13:
        Serial.println(F("11 - Back breaking work assigned to you - manslaughter?"));
        musicPlayer.startPlayingFile("/track011.mp3");
        moveServosSlowly(LeftWing, RightWing, 30, leftAngle, 108, rightAngle); //tail retracts
        whiteEyes();
        break;

      case 14:
        Serial.println(F("12 - Come up with 100 other reasons - why you're the one"));
        musicPlayer.startPlayingFile("/track012.mp3");
        break;

      case 15:
        Serial.println(F("13 - Overheating? Doctor - suspicous"));
        musicPlayer.startPlayingFile("/track013.mp3");
        
        //tail opens quickly
        leftAngle = 30; // 30 is the angle - wings spread out, and 100 is the angle where it's vertical
        rightAngle = 108; // 150 is the angle - wings spread out, and 26 is the angle where it's vertical
        mappedLeftAngle = map(leftAngle, 0, 270, 0, 180);
        mappedRightAngle = map(rightAngle, 0, 270, 0, 180);
        LeftWing.write(mappedLeftAngle);
        RightWing.write(mappedRightAngle);

        angryEyes();
        break;

      case 16:
        Serial.println(F("14 - Overcompensating? Oh darling"));
        musicPlayer.startPlayingFile("/track014.mp3");
        whiteEyes();
        delay(18000);
        moveServosSlowly(LeftWing, RightWing, 30, leftAngle, 108, rightAngle); //tail retracts
        break;

      case 17:
        Serial.println(F("15 - You dare malign me with"));
        musicPlayer.startPlayingFile("/track015.mp3");
        
        //tail opens quickly
        leftAngle = 30; // 30 is the angle - wings spread out, and 100 is the angle where it's vertical
        rightAngle = 108; // 150 is the angle - wings spread out, and 26 is the angle where it's vertical
        mappedLeftAngle = map(leftAngle, 0, 270, 0, 180);
        mappedRightAngle = map(rightAngle, 0, 270, 0, 180);
        LeftWing.write(mappedLeftAngle);
        RightWing.write(mappedRightAngle);

        angryEyes();
        delay(5000);
        closingEyes();
        break;

      case 18:
        Serial.println(F("15.5 - Suspenseful music"));
        musicPlayer.startPlayingFile("/bgmus001.mp3"); //suspicious music
        break;

      case 19:
        Serial.println(F("15.9 - gasps!"));
        musicPlayer.playFullFile("gasp0001.mp3");
        break;

      case 20:
        Serial.println(F("16 - How dare you? I'm a victim"));
        musicPlayer.startPlayingFile("/track016.mp3");
        moveServosSlowly(LeftWing, RightWing, 30, leftAngle, 108, rightAngle); //tail retracts
        whiteEyes();
        closingEyes();
        whiteEyes();
        break;

      case 21:
        Serial.println(F("17 - That's right, detective hawk - rookie?"));
        musicPlayer.startPlayingFile("/track017.mp3");
        break;

      case 22:
        Serial.println(F("18 - What are you saying?"));
        musicPlayer.playFullFile("/gasp0001.mp3");
        scaredEyes();
        musicPlayer.startPlayingFile("/track018.mp3");
        delay(2000);
        whiteEyes();
        break;

      case 23:
        Serial.println(F("18.5 - gasps!"));
        musicPlayer.playFullFile("gasp0001.mp3");
        break;

      case 24:
        Serial.println(F("24 - closing - Do you seriously think we'll buy"));
        musicPlayer.startPlayingFile("/track019.mp3");
        moveServosSlowly(LeftWing, RightWing, leftAngle, 30, rightAngle, 108); //tail opens
        break;

      default:
        Serial.println(F("Invalid option"));
    }

  }
}  // end of loop()
// end of receiver code
// CHANGEHERE
