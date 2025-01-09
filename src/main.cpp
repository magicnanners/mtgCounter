/*
  This is the code for my MTG life counter project utilizing an Arduino Nano and an SSD1306 OLED
  Project started 1/1/25
  Version 1.0: 
  -Basic functionality impltemented
  -Basic display functions implemented

  Version 1.1:
  -Added support for 6  input buttons to prep for PCB & expanded menus
  -Implemented highlighted selection blinking!

  TODO:
  -Add menu to select game parameters
  -Have game reset after game over
  -Tidy up UI implementation and make it cleaner
  -Add functionality to the MTG Logo as it currently does nothing. Plan to add secrets and such
  -Cleanup code and documentation
*/

//Dependencies
#include <Arduino.h>
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
#define BUTTON_DOWN_PIN 2
#define BUTTON_UP_PIN 7
#define BUTTON_LEFT_PIN 3
#define BUTTON_RIGHT_PIN 4
#define BUTTON_DEC_PIN 6
#define BUTTON_INC_PIN 5


//End Defintitions

//Begin Variables

bool debugActive = true;

//Booleans for debounce
bool buttonDownClicked = false;
bool buttonUpClicked = false;
bool buttonLeftClicked = false;
bool buttonRightClicked = false;
bool buttonDecClicked = false;
bool buttonIncClicked = false;

//Variables to track damages
int playerLife;
int poisonDmg;
int c2Dmg;
int c3Dmg;
int c4Dmg;

int numPlayers = 4;
int startingLife = 40;

//Setup variables for menu
int currentRow = 0;
int currentCol = 0;
int currentSelection = 0;
int maxRow;
int maxCol;

/*
  This tracks the current game state. 
  0 = Loading screen
  1 = Setup menu - NOT IMPLEMENTED
  2 = Standard state
  3 = Game Over
*/
int gameState = 0;

//Variables for highlight blinking
unsigned long startMillis;
unsigned long currentMillis;
const long interval = 300;

//Boolean for determining game over
bool isGameOver()
{  
  //Serial.println(F("Checking if game is over."));
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

//Function Declarations
void changeState(int state);
void updateSelection();
void displayMainGame();
void displayGameOver();
void displayLoadingScreen();
void displaySetupScreen();
void updateDisplay();
void updateHighlight();
void updateValue(int currentSelection, bool increase);


void setup() {
  
  // Intialize serial
    Serial.begin(9600);
    Serial.println(F("Begin program"));
    Serial.println(millis());

  //Setup buttons
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_INC_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DEC_PIN, INPUT_PULLUP);
  gameState = 0;
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
  // Show loading screen
  displayLoadingScreen();
  delay(1000); // Pause for 1 second
  startMillis = millis();
  changeState(1);
}

void loop()
{
  // put your main code here, to run repeatedly
  
  //If Increase button pressed
 if((digitalRead(BUTTON_INC_PIN) == LOW) && (buttonIncClicked == false))
 {
   buttonIncClicked = true;
   if(debugActive == true)
   {
    Serial.println(F("Button Inc State"));
   }

   if(gameState == 2)
   {
    updateValue(currentSelection, true);
   }
   else if(gameState == 1 && currentSelection == 0)
   {
    updateValue(currentSelection, true);
   }
   else if(gameState == 1 && currentSelection == 1)
   {
    changeState(2);
   }
 }

 if((digitalRead(BUTTON_INC_PIN) == HIGH) && (buttonIncClicked == true))
 {
   buttonIncClicked = false;
 }

//If down button pressed
 if((digitalRead(BUTTON_DEC_PIN) == LOW) && (buttonDecClicked == false))
 {
   buttonDecClicked = true;
   if(debugActive == true)
   {
      Serial.println(F("Button Dec State:"));
      Serial.println(buttonDecClicked);
      Serial.println(F("Button Down State:"));
      Serial.println(buttonDownClicked);
   }
   if(gameState == 2)
   {
    updateValue(currentSelection, false);
   }
   else if(gameState == 1 && currentSelection == 0)
   {
    updateValue(currentSelection, false);
   }
   else if(gameState == 1 && currentSelection == 1)
   {
    changeState(2);
   }
   //updateValue(currentSelection, false);
   
 }

 if((digitalRead(BUTTON_DEC_PIN) == HIGH) && (buttonDecClicked == true))
 {
   buttonDecClicked = false;
   if(debugActive == true)
   {
    Serial.println(F("Button Dec State:"));
    Serial.println(buttonDecClicked);
    Serial.println(F("Button Down State:"));
    Serial.println(buttonDownClicked);
   }

 }

  //If Down button pressed
 if((digitalRead(BUTTON_DOWN_PIN) == LOW) && (buttonDownClicked == false))
 {
   buttonDownClicked = true;
   if(debugActive == true)
   {
    Serial.println(F("Button Down State:"));
    Serial.println(buttonDownClicked);
   }
   //Note: This is ugly but it works will have to tweak this for instances with more than 2 rows
   if(currentRow == 0)
   {currentRow = 1;}
   else if (currentRow == 1)
   {currentRow = 0;}
   updateSelection();
 }

 if((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (buttonDownClicked == true))
 {
   buttonDownClicked = false;
   if(debugActive == true)
   {
    Serial.println(F("Button Down State:"));
    Serial.println(buttonDownClicked);
   }
 }
 //If Up button pressed
 if((digitalRead(BUTTON_UP_PIN) == LOW) && (buttonUpClicked == false))
 {
   buttonUpClicked = true;
   //Note: This is ugly but it works will have to tweak this for instances with more than 2 rows
  if(currentRow == 0)
  {currentRow = 1;}
  else if (currentRow == 1)
  {currentRow = 0;}
  updateSelection();
 }

 if((digitalRead(BUTTON_UP_PIN) == HIGH) && (buttonUpClicked == true))
 {
   buttonUpClicked = false;
 }
 if((digitalRead(BUTTON_LEFT_PIN) == LOW) && (buttonLeftClicked == false))
 { 
  if(gameState ==2)
  {
   buttonLeftClicked = true;
   currentCol--;
   if(currentCol < 0)
   {currentCol = maxCol;}
   updateSelection();
  }
 }
 if((digitalRead(BUTTON_LEFT_PIN) == HIGH) && (buttonLeftClicked == true))
 {
   buttonLeftClicked = false;
 }
 if((digitalRead(BUTTON_RIGHT_PIN) == LOW) && (buttonRightClicked == false))
 { 
  if(gameState == 2)
  {
   buttonRightClicked = true;
   currentCol++;
   if(currentCol > maxCol)
   {currentCol = 0;}
   updateSelection();
  }

   
 }
 if((digitalRead(BUTTON_RIGHT_PIN) == HIGH) && (buttonRightClicked == true))
 {
   buttonRightClicked = false;
 }

//Update display at end of main loop
updateDisplay();

}

void changeState(int state)
{
  gameState = state;
  //If on setup screen
  if (gameState == 1)
  {
    maxRow = 2;
    maxCol = 0;
    if(debugActive == true)
    {
      Serial.println(F("maxRow: "));
      Serial.print(maxRow);
    }
  }
  //If on main game
  if (gameState == 2)
  {
    maxRow = 1;
    maxCol = 2;
    playerLife = startingLife;
    c2Dmg = 0;
    c3Dmg = 0;
    c4Dmg = 0;
    poisonDmg = 0;
    currentSelection = 0;
    displayLoadingScreen();
    delay(300);
    updateDisplay();
  }
}

void updateSelection()
{
  //A Way to update current selection via columns and rows
  //There is likely a smarter way to achieve this but this works for now.
  //You're welcome to try and improve it, I just wanted to get this done haha.

  if (gameState == 1)
  {
    if(currentRow == 0)
    {
      currentSelection = 0;
    }
    else if (currentRow == 1)
    {
      currentSelection = 1;
    }
    if(debugActive == true)
    {
      Serial.println(F("Current Selection:"));
      Serial.println(currentSelection);
    }
  }


  if(gameState == 2)
  {
    if (currentRow == 0 && currentCol == 0)
    {
      //Set current selection to # players if in setup and player life if main game    
      currentSelection = 0;
    }
    else if(currentRow == 1 && currentCol == 0)
    {
      //Set current selection to starting life if in setup and Commander 2 Damage if in main game
      currentSelection = 1;
    }

    else if(currentRow == 1 && currentCol == 1)
    {
      //Set current selection to confirm if in setup menu and Commander 3 Damage if in main game
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
      currentMillis = millis();
      if ((currentMillis - startMillis >= interval) && (currentMillis - startMillis < interval * 2))
      {
        updateHighlight();
      }
      else if (currentMillis - startMillis > (interval * 2))
      {
        startMillis = currentMillis;
      }
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

void displayLoadingScreen()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(48, 0);
  display.print("Now");
  display.setCursor(26, 16);
  display.print("Loading");
  display.display();
}

void displaySetupScreen()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print("Starting Life: ");
  display.print(startingLife);
  display.setCursor(2, 19);
  display.print("Confirm");
  currentMillis = millis();
  if ((currentMillis - startMillis >= interval) && (currentMillis - startMillis < interval * 2))
  {
    updateHighlight();
  }
  else if (currentMillis - startMillis > (interval * 2))
  {
    startMillis = currentMillis;
  }
  display.display();
}

//Function to update the screen
void updateDisplay()
{
  if (gameState == 1)
  {
    displaySetupScreen();
  }
  if (gameState == 2)
  {
    displayMainGame();
  }
  if (isGameOver() == true)
  {
    delay(400);
    gameState = 3;
    displayGameOver();
  }
}

//Logic for displaying the correct highlightBox
void updateHighlight ()
{
  
  //If on setup screen
  if (gameState == 1)
  {
      if (currentSelection == 0)
      {
        display.drawRect(0, 0, 110, 8, SSD1306_WHITE);
      }
      if (currentSelection == 1) 
      {
        display.drawRect(0,19,50,8, SSD1306_WHITE);
      }
  }

  //If on main game screen
  if (gameState == 2)
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
}

//Logic for updating the correct value based on selection and 
void updateValue(int currentSelection, bool increase)
{ 
  if (gameState == 1)
  {
    if(currentSelection == 0)
    {
      if(increase == true)
      {
        startingLife++;
      }
      else 
      {
        startingLife--;
        if(startingLife < 1)
        {
          startingLife = 1;
        }
      }

    }
  }

  if(gameState == 2)
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
        if(c2Dmg < 0)
        {
          c2Dmg = 0;
        }
        else 
        {
          playerLife++;
        }
        
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
        if(c3Dmg < 0)
        {
          c3Dmg = 0;
        }
        else 
        {
          playerLife++;
        }
        
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
        if(c4Dmg < 0)
        {
          c4Dmg = 0;
        }
        else 
        {
          playerLife++;
        }
        
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
        if(poisonDmg < 0)
        {
          poisonDmg = 0;
        }
        else 
        {
          playerLife++;
        }
      }
    }
  } 
}

