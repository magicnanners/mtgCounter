/*
  This is the code for my MTG life counter project utilizing an Arduino Nano and an SSD1306 OLED
  Project started 1/1/25
  Version 1.0: 
  -Basic functionality impltemented
  -Basic display functions implemented

  TODO:
  -Add menu to select game parameters
  -Have game reset after game over
  -Tidy up UI implementation and make it cleaner
  -Add additional button to cycle current column left instead of just right
  -Add functionality to the MTG Logo as it currently does nothing. Plan to add secrets and such
  -Implement selection highlight flashing if possible
    Perhaps using time since on and time since off rather than standard delay?
  -Cleanup code and documentation
*/

//Dependencies
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Begin Definitions

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
#define BUTTON_ROW_PIN 2
#define BUTTON_COL_PIN 3
#define BUTTON_DOWN_PIN 4
#define BUTTON_UP_PIN 5

//End Defintitions

//Begin Variables

//Booleans for debounce
bool buttonRowClicked = false;
bool buttonColClicked = false;
bool buttonDownClicked = false;
bool buttonUpClicked = false;

//Delay time for flashing indicator of what item is selected. Not implemented
int highlightDelay = 300;
//Variables to track damages
int playerLife;
int poisonDmg;
int c2Dmg;
int c3Dmg;
int c4Dmg;

//Setup variables for menu
int currentRow = 0;
int currentCol = 0;
int currentSelection = 0;
int maxRow = 1;
int maxCol = 2;

//Boolean for determining game over
bool isGameOver()
{  
  if(playerLife == 0 || c2Dmg >= 21 || c3Dmg >= 21 || c4Dmg >= 21 || poisonDmg >= 10)
  {
    return true;
  }
  else 
  {
    return false;
  }
}

//End Variables

//Begin Bitmaps

// 'icon_heart', 16x16px
const unsigned char bitmap_icon_heart [] PROGMEM = {
	0x00, 0x00, 0x3c, 0x3c, 0x7e, 0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x7f, 0xfe, 0x3f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00
};
// 'icon_liquid', 16x16px
const unsigned char bitmap_icon_liquid [] PROGMEM = {
	0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0e, 0x70, 0x1c, 0x38, 0x18, 0x18, 0x33, 0x0c, 0x37, 0x8c, 
	0x67, 0x86, 0x63, 0x06, 0x60, 0x06, 0x60, 0x06, 0x30, 0x0c, 0x3c, 0x3c, 0x1f, 0xf8, 0x07, 0xe0
};
// 'icon_skull', 16x16px
const unsigned char bitmap_icon_skull [] PROGMEM = {
	0x0f, 0xf0, 0x3f, 0xfc, 0x7f, 0xfe, 0x7f, 0xfe, 0xf3, 0xcf, 0xe1, 0x87, 0xe1, 0x87, 0xf3, 0xcf, 
	0x7f, 0xfe, 0x3f, 0xfc, 0x7f, 0xfe, 0x7f, 0xfe, 0x6d, 0xb6, 0x6d, 0xb6, 0x6d, 0xb6, 0x00, 0x00
};
// 'MTGLogo', 16x16px
const unsigned char bitmap_MTGLogo [] PROGMEM = {
	0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x05, 0x40, 0x05, 0x40, 0x15, 0x50, 0x15, 0x50, 0x1f, 0xf0, 
	0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x03, 0x80, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00
};

// End Bitmaps


void updateSelection()
{
  //A Way to update current selection via columns and rows
  //There is likely a smarter way to achieve this but this works for now.
  //You're welcome to try and improve it, I just wanted to get this done haha.
  if (currentRow == 0 && currentCol == 0)
  {
    //Set current selection to player life
    currentSelection = 0;
  }
  else if(currentRow == 1 && currentCol == 0)
  {
    //Set current selection to Commander 2 Damage
    currentSelection = 1;
  }
 
  else if(currentRow == 1 && currentCol == 1)
  {
    //Set current selection to Commander 3 Damage
    currentSelection = 2;
  }
  else if(currentRow == 1 && currentCol == 2)
  {
    //Set current selection to Commander 4 Damage
    currentSelection = 3;
  }
  else if(currentRow == 0 && currentCol == 2)
  {
    //Set current selection to MTG Logo
    currentSelection = 4;
  }
  else if(currentRow == 0 && currentCol == 1)
  {
    //Set Current Selection to Poison Damage
    currentSelection = 5;
  }
}

//Update functions for different screen modes

//Display main game screen
void displayMainGame()
{
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(2, 2);
      display.print("P2: ");
      display.print(c2Dmg);
      display.print("|");
      display.setCursor(45, 2);
      display.print("P3: ");
      display.print(c3Dmg);
      display.print("|");
      display.setCursor(85, 2);
      display.print("P4: ");
      display.print(c4Dmg);
      display.setTextSize(2);
      display.setCursor(22, 18);
      display.print(playerLife);
      display.drawBitmap(1,15, bitmap_icon_heart,16,16,SSD1306_WHITE); 
      display.drawBitmap(55,15, bitmap_icon_liquid,16,16, SSD1306_WHITE);
      display.setCursor(75,18);
      display.print(poisonDmg);
      display.drawBitmap(100,15,bitmap_MTGLogo,16,16,SSD1306_WHITE);
      updateHighlight();
      display.display();
}
//Display game over screen
void displayGameOver()
{
  display.clearDisplay();
  display.drawBitmap(48,2,bitmap_icon_skull,16,16,SSD1306_WHITE);
  display.setCursor(8, 16);
  display.setTextSize(2);
  display.print("Game Over :(");
  display.display();
}


//Function to update the screen
void updateDisplay()
{
  if (isGameOver() == false)
  {
    displayMainGame();
  }
  else if (isGameOver() == true)
  {
    delay(400);
    displayGameOver();
  }
}

//Logic for displaying the correct highlightBox
void updateHighlight ()
{
  
  if (currentSelection == 0)
  {
    //Highlight player life
    display.drawRect(0,16,45,16,SSD1306_WHITE);
  }
  else if(currentSelection == 1)
  {
    //Highlight Commander 2 Damage
    display.drawRect(0,0,40,10,SSD1306_WHITE);
  }
  else if(currentSelection == 2)
  {
    //Highlight Commander 3 Damage
    display.drawRect(45,0,40,10,SSD1306_WHITE);
  }
  else if(currentSelection == 3)
  {
    //Highlight Commander 4 Damage
    display.drawRect(85,0,40,10,SSD1306_WHITE);
  }
  else if(currentSelection == 4)
  {
    //Highlight MTG Logo
    display.drawRect(100,16,16,16,SSD1306_WHITE);
  }
  else if(currentSelection == 5)
  {
    //Highlight Poison Damage
    display.drawRect(55,16,40,16,SSD1306_WHITE);
  }
}

//Logic for updating the correct value based on selection and 
void updateValue(int currentSelection, bool increase)
{
  if (currentSelection == 0)
  {
    //Update Player Life
    if(increase == true)
    {
      playerLife++;
    }
    else
    {
      playerLife--;
    }
  }
  else if(currentSelection == 1)
  {
    //Update Commander 2 Damage
    if(increase == true)
    {
      c2Dmg++;
      playerLife--;
    }
    else
    {
      c2Dmg--;
      playerLife++;
    }
  }
  else if(currentSelection ==2)
  {
    //Update Commander 3 Damage
    if(increase == true)
    {
      c3Dmg++;
      playerLife--;
    }
    else
    {
      c3Dmg--;
      playerLife++;
    }
  }
  else if(currentSelection == 3)
  {
    //Update Commander 4 Damage
    if(increase == true)
    {
      c4Dmg++;
      playerLife--;
    }
    else
    {
      c4Dmg--;
      playerLife++;
    }
  }
  else if(currentSelection == 4)
  {
    //Do nothing... for now ;)
  }
  else if(currentSelection == 5)
  {
    //Update Poison Damage
    if(increase == true)
    {
      poisonDmg++;
      playerLife--;
    }
    else
    {
      poisonDmg--;
      playerLife++;
    }
  } 
}

void setup() {
  
  // Intialize serial
    Serial.begin(9600);

  //Setup buttons
  pinMode(BUTTON_ROW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_COL_PIN, INPUT_PULLUP);
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
  updateDisplay();
}

void loop(){
  // put your main code here, to run repeatedly
  
  //If up button pressed
 if((digitalRead(BUTTON_UP_PIN) == LOW) && (buttonUpClicked == false))
 {
   buttonUpClicked = true;
   updateValue(currentSelection, true);
 }

 if((digitalRead(BUTTON_UP_PIN) == HIGH) && (buttonUpClicked == true))
 {
   buttonUpClicked = false;
 }

//If down button pressed
 if((digitalRead(BUTTON_DOWN_PIN) == LOW) && (buttonDownClicked == false))
 {
   buttonDownClicked = true;
   updateValue(currentSelection, false);
   
 }

 if((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (buttonDownClicked == true))
 {
   buttonDownClicked = false;

 }

  //If row button pressed
 if((digitalRead(BUTTON_ROW_PIN) == LOW) && (buttonRowClicked == false))
 {
   buttonRowClicked = true;
   //Note: This is ugly but it works will have to tweak this for instances with more than 2 rows
   if(currentRow == 0)
   {currentRow = 1;}
   else if (currentRow == 1)
   {currentRow = 0;}
   updateSelection();
 }

 if((digitalRead(BUTTON_ROW_PIN) == HIGH) && (buttonRowClicked == true))
 {
   buttonRowClicked = false;
 }
 if((digitalRead(BUTTON_COL_PIN) == LOW) && (buttonColClicked == false))
 { 
   buttonColClicked = true;
   currentCol++;
   if(currentCol > maxCol)
   {currentCol =0;}
   updateSelection();
   
 }
 if((digitalRead(BUTTON_COL_PIN) == HIGH) && (buttonColClicked == true))
 {
   buttonColClicked = false;
 }

//Update display at end of main loop
updateDisplay();

}
