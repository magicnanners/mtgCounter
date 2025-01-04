/*
  This is the code for my MTG life counter project utilizing an Arduino Nano and an SSD1306 OLED
  Project started 1/1/25
*/

//Dependencies
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Define Screen Width & Height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Define pins
#define BUTTON_LEFT_PIN 2
#define BUTTON_RIGHT_PIN 3
#define BUTTON_DOWN_PIN 4
#define BUTTON_UP_PIN 5

//Delay time for flashing indicator of what item is selected
int highlightDelay = 300;
int playerLife;
int poisonDmg;
int c2Dmg;
int c3Dmg;
int c4Dmg;

//Function to update current values of life and damage
//TODO: Add GFX for life total and poison DMG
void updateText()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print("P2: ");
  display.print(c2Dmg);
  display.print("|");
  display.print("P3: ");
  display.print(c3Dmg);
  display.print("|");
  display.print("P4: ");
  display.print(c4Dmg);
  display.setTextSize(2);
  display.setCursor(22, 18);
  display.print(playerLife);
  display.display();
}

void setup() {
  
  // Intialize serial
    Serial.begin(9600);

  //Setup buttons
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);

  //Set initial values for damage and life
  playerLife = 40;
  poisonDmg = 0;
  c2Dmg = 0;
  c3Dmg = 0;
  c4Dmg = 0;

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //TODO: add logic for changing life totals and damage
  updateText();


}
