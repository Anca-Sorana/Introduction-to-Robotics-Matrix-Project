#include "LedControl.h"  // Library for controlling the LED matrix
#include <LiquidCrystal.h>
#include <EEPROM.h>
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const byte LCD_Backlight = 3;

// Pin configuration
const int dinPin = 13;
const int clockPin = 12;
const int loadPin = 10;
const int xPin = A0;      // Joystick X-axis
const int yPin = A1;      // Joystick Y-axis
const int buttonPin = 2;  // Button pin

const int buzzerPin = 11;

// LED matrix configuration
const byte matrixSize = 8;                                 // Size of the LED matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // Initialize LedControl object
byte matrixBrightness = 2;                                 // Brightness level of the matrix

// Player variables
byte xPosPlayer = 0;          //current row for player
byte yPosPlayer = 0;          //current column for player
byte xLastPosPlayer = 0;      //last row for player
byte yLastPosPlayer = 0;      //last column for player
int delayLedPlayer = 300;     // Delay for LED player update
byte xPositionRandom;         // random row for wall
byte yPositionRandom;         // random column for wall
int lastDebounceDelayPlayer;  // last time the player led was on/off

// Joystick thresholds
int minThreshold = 200;
int maxThreshold = 600;
byte moveInterval = 200;  // Interval for LED movement
unsigned long long lastMoved = 0;

// Button variables
byte pressInterval = 50;
int lastPressed = 0;  // last time the button was pressed
byte buttonState = LOW;
int lastReading;    //last value read from the button
int currentMillis;  //current time
int reading;        // current value for button

// Game variables
bool matrixChanged = true;  // Flag for matrix update
bool gameStatus = true;     // Game running status
int nrWalls = 0;            // Current number of walls
int nrWallsMax;             // Max number of walls for the current level
int level = 1;              // begin with level 1
const int levelMax = 3;     // no of maximum levels
byte nrTotalLives = 3;      // total number of lives
byte nrLives = 3;           // number of lives remained in the game
byte nrWallsMaxArray[levelMax] = { 0 };
int score = 0;                    // score for the current game
int gameOverMessageDelay = 2000;  //debounce delay for when the game is finished (either won or lost)
int gameOverStartTime;            //time when the game finnished

// Bomb variables
byte bombActive = false;
int startBomb;
int bombExplodingTime = 2000;
int lastChangedBomb;
int bombLightInterval = 200;
int xPosBomb;
int yPosBomb;
int directions[][2] = { { 0, 0 }, { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };  //possible direction for bomb explosion
int count = 0;                                                                 //no of bomb exploded
int directionsLen = 5;                                                         //length of directions array

// Animation variables
int lastAnimation = 0;
int delayAnimation = 100;
int currentAnimation = 0;

// Matrix state array
byte matrix[matrixSize][matrixSize] = { 0 };

// All menu posibilities
int index = 0;  // index to help us naavigate through menu
const char mainMenuArray[4][15] = { "1. Start Game", "2. Settings", "3. About", "4. Info" };
const char settingsArray[5][20] = { "1.LCD Intensity", "2.Mtx Intensity", "3.Sound", "4.Reset", "5.Exit" };
int lenMenu = 4;
int lenMenuSettings = 5;

// Flags for each menu category
byte gameStarted = false;
byte menuOn = true;
byte settings = false;
byte about = false;
byte lcdBright = false;
byte matrixBright = false;
byte sound = false;
byte info = false;
byte reset = false;
byte resetActive = false;

byte gameStartedIndex = 0;
byte settingsIndex = 1;
byte aboutIndex = 2;
byte infoIndex = 3;
byte lcdBrightIndex = 0;
byte matrixBrightIndex = 1;
byte soundIndex = 2;
byte resetIndex = 3;
byte exitIndex = 4;

int messageBeginDelay = 3000;  // debounce time for begin message

//Variables for About message
int character = 50;        // character index used for scrolling message
int lenAboutMessage = 50;  // length about message
int characterDelay = 500;  // delay for scrolling
int lastCharacterDisplay;  // last time a character was shown (scrolling text)

// Eeprom address
int lcdBrightnessAdress = 0;     //adress in eeprom for lcd brightness
int matrixBrightnessAdress = 1;  //adress in eeprom for matrix brightness
int soundAdress = 2;             //adress in eeprom for sound (on or off)

// Variables used to save the values saved in EEPROM
int lcdBrightnessValue;
int matrixBrightnessValue;
byte soundActive = false;

// Buzzer variables
int frequencies[] = { 100, 200, 300, 400, 500, 600, 700, 800 };  //possible values for buzzer
int frequencyJoystick = 500;                                     //frequency used for buzzer when moving the joystick
int buzzerStartTime;                                             // when the buzzer starts to ring
int buzzerDelay = 50;                                            // debounce delay for buzzer

//variables for brightness settings
int lcdBrightnessLen = 10;                                                    //length of lcdBrightnessLevel array
int matrixBrightnessLen = 9;                                                  //length of matrixBrightnessLevel array
int percentLcd;                                                               //brightness percent for lcd
int percentMatrix;                                                            //brightness percent for matrix
int progressBarCollumns;                                                      //no of blocks for progress bar
int lcdBrightnessLevel[] = { 0, 51, 77, 102, 128, 153, 179, 204, 230, 255 };  //possible values for lcd brightness
int matrixBrightnessLevel[] = { 0, 1, 3, 5, 7, 9, 11, 13, 15 };               //possible values for matrix brightness
int minBrightnessLcd = 0;
int maxBrightnessLcd = 255;
int minBrightnessMatrix = 0;
int maxBrightnessMatrix = 15;
int minPercent = 0;
int maxPercent = 100;

//characters for the lcd
byte speaker[] = {
  0b00001,
  0b00011,
  0b01111,
  0b01111,
  0b01111,
  0b00011,
  0b00001,
  0b00000
};

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte emptyHeart[8] = {
  0b00000,
  0b01010,
  0b10101,
  0b10001,
  0b10001,
  0b01010,
  0b00100,
  0b00000
};

byte brightnessBar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte check[] = {
  B00000,
  B00001,
  B00011,
  B10110,
  B11100,
  B01000,
  B00000,
  B00000
};

byte block[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};


const uint8_t levelsAnimation[][8] = {
  { 0b00011000,
    0b00111000,
    0b01111000,
    0b11011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b11111111 },
  { 0b00111000,
    0b01111100,
    0b01101100,
    0b01001100,
    0b00011000,
    0b00110000,
    0b01111110,
    0b11111111 },
  { 0b00111100,
    0b01111110,
    0b01000110,
    0b00011110,
    0b00011110,
    0b01000110,
    0b01111110,
    0b00111100 }
};

const uint8_t ledAnimation[][8] = {
  { 0b00111100,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00111100,
    0b00111100 }
};
byte upArrow[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
};  //Up-Arrow
byte downArrow[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
};  //Down-Arrow

byte rightArrow[8] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B01010,
  B00000,
  B00000,
  B00000
};

// arrow left
byte leftArrow[8] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000,
  B00000
};
// Predefined images array
const uint8_t images[][8] = {
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000 },
  { 0b00000000,
    0b00000000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00000000,
    0b00000000 },
  { 0b00000000,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000 },
  { 0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111 },
  { 0b00000000,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000 },
  { 0b00000000,
    0b00000000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00000000,
    0b00000000 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000 }
};
const int imagesLen = sizeof(images) / 8;

//function to clear the matrix - turn of all the leds
void clearMatrix() {  //turn off all leds
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
    }
  }
  updateMatrix();
}

//function for when the game is finished (either won or lost)
void handleGameOver() {  //display a certain animantion when the player looses or wins game and restart the game
  //reinitialize all the threshold and delay variables (sometimes the program changes their values without me modifing them)
  minThreshold = 200;
  maxThreshold = 600;
  delayLedPlayer = 300;
  pressInterval = 100;
  moveInterval = 200;
  bombExplodingTime = 2000;
  bombLightInterval = 200;
  delayAnimation = 100;
  //display an animation when the game ended
  if (currentMillis - lastAnimation > delayAnimation) {
    lastAnimation = currentMillis;
    displayImage(images[currentAnimation]);
    if (++currentAnimation >= imagesLen) {
      //gameStatus = true;

      //gameStarted = false;
      currentAnimation = 0;
      clearMatrix();
    }
  }

  //show message on lcd when the game ended for 2 seconds
  if (currentMillis - gameOverStartTime > gameOverMessageDelay) {
    //after 2 seconds we reset everything and show the main menu again
    gameStarted = false;  // to know that the game is not running anymore
    menuOn = true;        // go back to main menu
    index = 0;            // show the first option on the menu
    nrWalls = 0;          //reset the number of walls for the next game
    // reset the posiyion for player
    xPosPlayer = 0;
    yPosPlayer = 0;
    lcd.clear();
    clearMatrix();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  randomSeed(analogRead(2));
  lc.shutdown(0, false);  // Enable display
  lc.clearDisplay(0);     // Clear display
  // for each level will be a certain number of walls
  nrWallsMaxArray[0] = random(0, 10);   // Initialize maximum walls
  nrWallsMaxArray[1] = random(10, 20);  // Initialize maximum walls
  nrWallsMaxArray[2] = random(20, 30);  // Initialize maximum walls

  //initialize the lcd
  lcd.begin(16, 2);
  pinMode(LCD_Backlight, OUTPUT);

  //initialize the buzzer
  pinMode(buzzerPin, OUTPUT);

  //create characters for lcd
  lcd.createChar(1, heart);
  lcd.createChar(2, rightArrow);
  lcd.createChar(3, emptyHeart);
  lcd.createChar(4, speaker);
  lcd.createChar(5, check);
  lcd.createChar(6, block);
  lcd.createChar(7, upArrow);
  lcd.createChar(8, downArrow);

  //show welcome message
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(6, 1);
  lcd.write(1);
  lcd.setCursor(7, 1);
  lcd.write(1);
  lcd.setCursor(8, 1);
  lcd.write(1);

  // lcdBrightnessValue = EEPROM.read(lcdBrightnessAdress);
  // matrixBrightnessValue = EEPROM.read(matrixBrightnessAdress);
  // soundActive = EEPROM.read(soundAdress);

  readEepromValues();

  //into eeprom are saved the positions of the values in the brightness arrays
  //in order to find out the percent for every value of those arrays we need to map the values
  percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], minBrightnessLcd, maxBrightnessLcd, minPercent, maxPercent);
  percentMatrix = map(matrixBrightnessLevel[matrixBrightnessValue], minBrightnessMatrix, maxBrightnessMatrix, minPercent, maxPercent);

  //set the brightness for the matrix and the lcd
  analogWrite(LCD_Backlight, lcdBrightnessLevel[lcdBrightnessValue]);
  lc.setIntensity(0, matrixBrightnessLevel[matrixBrightnessValue]);  // Set brightness
}

void loop() {
  if (millis() > messageBeginDelay) {  //don't start till the welcome message disappers
    // reinitialize the threshold and delay values
    minThreshold = 200;
    maxThreshold = 600;
    delayLedPlayer = 300;
    pressInterval = 100;
    moveInterval = 200;
    bombExplodingTime = 2000;
    bombLightInterval = 200;
    delayAnimation = 100;
    gameOverMessageDelay = 2000;
    currentMillis = millis();                             //current time
    reading = digitalRead(buttonPin);                     //current state of the button
    if (currentMillis - buzzerStartTime > buzzerDelay) {  // stop the buzzer if it rang for more than 50 milliseconds
      noTone(buzzerPin);
    }
    if (!gameStarted) {  //when we are not playing, just show menu on lcd
      mainMenu();
    } else if (gameStarted) {  //when we start the game
      if (!gameStatus) {       //if the player finnished the game (either lost or won)
        handleGameOver();
      } else {  //the game can begin again
        if (level <= levelMax) {
          handleLives();
          handleGameRunning();  //while the game is on
        } else {
          gameStatus = false;  // the game is no longer running
          //show winning message
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Congratulations!");
          lcd.setCursor(0, 1);
          lcd.print("You won!");
          clearMatrix();
          gameOverStartTime = currentMillis;  //save the time the game ends so we know when to stop the winning message from showing
        }
        int var = checkMatrix();  //verify how many walls we still have to take down
        if (var == 0) {
          if (level < levelMax) {  // if the max level is not reached
            level++;               // increase the level number
            nrWalls = 0;           // reinitialize the no of walls for that level
          } else {                 // when the maximul level no is reached
            gameStatus = false;    // the game is no longer running
            //show winning message
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Congratulations!");
            lcd.setCursor(0, 1);
            lcd.print("You won!");
            clearMatrix();
            gameOverStartTime = currentMillis;  //save the time the game ends so we know when to stop the winning message from showing
          }
        }
      }
    }
  }
}

void updateEepromValues() {  //update all the data from the eeprom
  EEPROM.update(lcdBrightnessAdress, lcdBrightnessValue);
  EEPROM.update(matrixBrightnessAdress, matrixBrightnessValue);
  EEPROM.update(soundAdress, soundActive);

  // Set brightness for lcd and matrix
  analogWrite(LCD_Backlight, lcdBrightnessLevel[lcdBrightnessValue]);
  lc.setIntensity(0, matrixBrightnessLevel[matrixBrightnessValue]);

  percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], minBrightnessLcd, maxBrightnessLcd, minPercent, maxPercent);
  percentMatrix = map(matrixBrightnessLevel[matrixBrightnessValue], minBrightnessMatrix, maxBrightnessMatrix, minPercent, maxPercent);
}

void readEepromValues() {  //read all the data from the eeprom
  lcdBrightnessValue = EEPROM.read(lcdBrightnessAdress);
  matrixBrightnessValue = EEPROM.read(matrixBrightnessAdress);
  soundActive = EEPROM.read(soundAdress);
}

void drawProgressBar(int max) {  //bar for brightness
  for (int i = 0; i < max; i++) {
    lcd.setCursor(2 + i, 1);
    lcd.write(6);
  }
}
int checkMatrix() {  //check if the matrix still have leds on
  int verif = 0;
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      if (matrix[row][col]) {
        if (row == xPosPlayer && col == yPosPlayer) {  //verify to not count the player's position
          continue;
        } else {
          verif++;
        }
      }
    }
  }
  return verif;
}
void handleLives() {  //print the remaining lives, the score and the level
  //print the lives remaining
  lcd.setCursor(0, 0);
  lcd.print("Lives: ");
  for (int i = 0; i < nrLives; i++) {
    lcd.setCursor(7 + i, 0);
    lcd.write(1);
  }
  //print empty hearts for the lost lives
  for (int i = 0; i < nrTotalLives - nrLives; i++) {
    lcd.setCursor(7 + i + nrLives, 0);
    lcd.write(3);
  }
  //print the current score
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.setCursor(7, 1);
  lcd.print(score);

  //print the current level
  lcd.setCursor(11, 1);
  lcd.print("Lvl: ");
  lcd.setCursor(15, 1);
  lcd.print(level);
}

void mainMenu() {  //print the menu and its details
  if (currentMillis - lastMoved > moveInterval) {
    // game logic
    updatePositions();          // Update the options shown on the lcd based on joystick input
    lastMoved = currentMillis;  // Update the time of the last move
  }
  if (reading != lastReading) {  //check if the reading from the button is different than the last time
    lastPressed = currentMillis;
  }
  if ((currentMillis - lastPressed) > pressInterval) {  // when the button in pressed then we select an option from the menu
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        //Serial.print(1);
        if (soundActive) {  // if the sound is on then we start the buzzer
          tone(buzzerPin, frequencyJoystick);
          buzzerStartTime = currentMillis;
        }
        updateMenu();  // update what menu or what option is showed based on the selection made
      }
    }
  }
  lastReading = reading;

  //depaending on the menu, submenu or option we selected, there will be shown different things on the lcd
  if (menuOn) {
    settings = false;
    lcd.setCursor(0, 0);
    lcd.print("Main Menu ");
    lcd.setCursor(10, 0);
    lcd.write(7);
    lcd.setCursor(11, 0);
    lcd.write(8);
    lcd.setCursor(0, 1);
    lcd.print(mainMenuArray[index]);
  }
  if (settings) {
    menuOn = false;
    lcd.setCursor(0, 0);
    lcd.print("Settings");
    lcd.setCursor(10, 0);
    lcd.write(7);
    lcd.setCursor(11, 0);
    lcd.write(8);
    lcd.setCursor(0, 1);
    lcd.print(settingsArray[index]);
  }

  if (lcdBright) {
    lcd.setCursor(0, 0);
    lcd.print("LCD ");
    lcd.print(percentLcd);
    lcd.print(F("%  "));
    lcd.setCursor(11, 0);
    //lcd.write(9);
    lcd.setCursor(12, 0);
    lcd.write(2);
    lcd.setCursor(0, 1);
    lcd.print("0");
    lcd.setCursor(13, 1);
    lcd.print("100");
    drawProgressBar(lcdBrightnessValue);
  }

  if (matrixBright) {
    lcd.setCursor(0, 0);
    lcd.print("Matrix ");
    lcd.print(percentMatrix);
    lcd.print(F("%  "));
    lcd.setCursor(11, 0);
    //lcd.write(9);
    lcd.setCursor(11, 0);
    lcd.write(2);
    lcd.setCursor(0, 1);
    lcd.print("0");
    lcd.setCursor(13, 1);
    lcd.print("100");
    drawProgressBar(matrixBrightnessValue);
  }

  if (sound) {
    lcd.setCursor(5, 0);
    lcd.print("Sound");
    lcd.setCursor(0, 1);
    lcd.write(4);
    lcd.setCursor(3, 1);
    lcd.print("ON");
    lcd.setCursor(9, 1);
    lcd.print("OFF");
    lcd.setCursor(10, 0);
    lcd.write(2);
    if (soundActive) {
      lcd.setCursor(5, 1);
      lcd.write(5);
    } else {
      lcd.setCursor(12, 1);
      lcd.write(5);
    }
  }

  if (about) {
    lcd.setCursor(lenAboutMessage - character, 0);
    lcd.print("   Bomber Man");
    lcd.setCursor(2, 1);
    lcd.print("Made by Sorana; Github: Anca-Sorana");
    if (currentMillis - lastCharacterDisplay > characterDelay) {
      lcd.scrollDisplayLeft();
      character--;
      if (character < 0) {
        character = lenAboutMessage;
      }
      lastCharacterDisplay = currentMillis;
    }
  }

  if (info) {
    lcd.setCursor(0, 0);
    lcd.print("Explode walls");
    lcd.setCursor(0, 1);
    lcd.print("Use joystick");
    //    if (currentMillis - lastCharacterDisplay > characterDelay) {
    //      lcd.scrollDisplayLeft();
    //      character--;
    //      if (character < 0) {
    //        character = lenAboutMessage;
    //      }
    //      lastCharacterDisplay = currentMillis;
    //    }
  }
  if (reset) {
    settings = false;
    lcd.setCursor(0, 0);
    lcd.print("Are you sure?");
    lcd.setCursor(0, 1);
    lcd.print("   YES ");
    lcd.setCursor(9, 1);
    lcd.print("NO");
    if (resetActive) {
      lcd.setCursor(6, 1);
      lcd.write(5);
    } else {
      lcd.setCursor(11, 1);
      lcd.write(5);
    }
  }
}
void updateMenu() {  //change every flag depending on where we are in the menu and what we choose (this is only called when we press the button)
  if (menuOn) {
    if (index == gameStartedIndex) {  //start game
      // set the other flags to false
      lcd.clear();
      gameStarted = true;
      gameStatus = true;
      menuOn = false;
      settings = false;
      about = false;
      info = false;

      //initialize the variables for the game
      nrLives = nrTotalLives;
      level = 1;
      score = 0;

    } else if (index == settingsIndex) {  // settings
      lcd.clear();
      gameStarted = false;
      menuOn = false;
      about = false;
      index = 0;
      settings = true;
      info = false;
    } else if (index == aboutIndex) {  // about
      lcd.clear();
      gameStarted = false;
      menuOn = false;
      settings = false;
      about = true;
      info = false;
    } else if (index == infoIndex) {  // info
      lcd.clear();
      gameStarted = false;
      menuOn = false;
      settings = false;
      about = false;
      info = true;
    }
  }

  else if (settings) {              // if we are in the settings menu
    if (index == lcdBrightIndex) {  // if we press on lcd brightness we go to the lcd brightness submenu
      lcd.clear();
      lcdBright = true;
      matrixBright = false;
      menuOn = false;
      settings = false;
      sound = false;
      readEepromValues();
      reset = false;
    }
    if (index == matrixBrightIndex) {  // if we press on matrix brightness - we go to the matrix brightness submenu
      lcd.clear();
      lcdBright = false;
      matrixBright = true;
      menuOn = false;
      settings = false;
      sound = false;
      displayLed(ledAnimation[0]);
      readEepromValues();
      reset = false;
    }
    if (index == soundIndex) {  //if we press on sound - we go to the sound submenu
      lcd.clear();
      sound = true;
      lcdBright = false;
      matrixBright = false;
      settings = false;
      reset = false;
    }
    if (index == resetIndex) {  //if we press on reset - we go to the reset submenu
      lcd.clear();
      lcdBright = false;
      matrixBright = false;
      settings == false;
      sound = false;
      reset = true;
    }
    if (index == exitIndex) {  // if we press on exit - we go to the main menu
      lcd.clear();
      lcdBright = false;
      matrixBright = false;
      menuOn = true;
      settings == false;
      index = 0;
      sound = false;
      reset = false;
    }
  } else if (lcdBright) {  // after we chose the brightness value, press the button and save the lcd brightness value
    lcd.clear();
    settings = true;
    matrixBright = false;
    menuOn = false;
    lcdBright = false;
    index = 0;
    sound = false;
    reset = false;
  } else if (matrixBright) {  // after we chose the brightness value, press the button and save the matrix brightness value
    lcd.clear();
    settings = true;
    matrixBright = false;
    menuOn = false;
    lcdBright = false;
    index = 0;
    sound = false;
    reset = false;
    clearMatrix();
  } else if (sound) {  // after we chose the sound option, press the button and save the sound option
    lcd.clear();
    settings = true;
    matrixBright = false;
    menuOn = false;
    lcdBright = false;
    sound = false;
    reset = false;
    index = 0;
  } else if (about) {  //exit about and go to main menu
    lcd.clear();
    settings = false;
    matrixBright = false;
    lcdBright = false;
    menuOn = true;
    about = false;
    sound = false;
    index = 0;
    info = false;
    reset = false;
    character = 35;
  } else if (info) {  // exit info and go to main menu
    lcd.clear();
    settings = false;
    matrixBright = false;
    lcdBright = false;
    menuOn = true;
    about = false;
    sound = false;
    info = false;
    reset = false;
    index = 0;
  } else if (reset) {  // after decinding if you want to reset or not press the button and go back to settings menu
    lcd.clear();
    settings = true;
    matrixBright = false;
    menuOn = false;
    lcdBright = false;
    sound = false;
    reset = false;
    index = 0;
    if (resetActive) {
      resetActive = false;
      lcdBrightnessValue = lcdBrightnessLen - 1;
      matrixBrightnessValue = 1;
      soundActive = 0;
      updateEepromValues();
      readEepromValues();
    }
  }
}

void displayImage(const byte* image) {  //display the animation when the player loses
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, bitRead(image[i], matrixSize - 1 - j));
    }
  }
}

void displayLed(const byte* image) {  //display the animation when we set the intensity of matrix
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, bitRead(image[i], matrixSize - 1 - j));
    }
  }
}

void updateMatrix() {
  minThreshold = 200;
  maxThreshold = 600;
  delayLedPlayer = 300;
  pressInterval = 100;
  moveInterval = 200;
  bombExplodingTime = 2000;
  bombLightInterval = 200;
  delayAnimation = 100;

  //update the matrix everytime a led changes state
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void generateMap() {  // generate random position for the walls on the matrix but avoid the leds near the starting point
  // Map generation logic
  xPositionRandom = random(matrixSize + 1);
  yPositionRandom = random(matrixSize + 1);
  if (xPositionRandom > xPosPlayer + 1 || yPositionRandom > yPosPlayer + 1) {
    if (!matrix[xPositionRandom][yPositionRandom]) {  //in case this position was already set as a wall or is the same position as the player
      nrWalls++;
      matrix[xPositionRandom][yPositionRandom] = !matrix[xPositionRandom][yPositionRandom];
    }
  }

  if (nrWalls <= nrWallsMax) {
    generateMap();
  }
}

void updatePositions() {  // when we use the joystick
  minThreshold = 100;
  maxThreshold = 700;
  delayLedPlayer = 300;
  pressInterval = 100;
  moveInterval = 200;
  bombExplodingTime = 2000;
  bombLightInterval = 200;
  delayAnimation = 100;
  //for every movement on the x and y axis we increment or decrement the values for brightness, change from on to off (sound) and change the position of the player
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  // Store the last positions of the LED
  xLastPosPlayer = xPosPlayer;
  yLastPosPlayer = yPosPlayer;
  // Update xPos based on joystick movement (X-axis)
  if (xValue > maxThreshold) {
    // if the sound is on start the buzzer
    if (soundActive) {
      tone(buzzerPin, frequencyJoystick);
      buzzerStartTime = currentMillis;
    }
    if (gameStarted) {  // when the game is running we use the joystick to navigate trough the matrix
      if (xPosPlayer > 0) {
        xPosPlayer--;
        if (matrix[xPosPlayer][yPosPlayer]) {
          xPosPlayer++;
        }
      }
    } else if (sound) {  //when we are inside the sound submenu move left and right to change the option and save the current value
      soundActive = false;
      lcd.setCursor(5, 1);
      lcd.print(" ");
      updateEepromValues();
    } else if (lcdBright) {  //when we are inside the lcd brightness submenu move left and right to change the option and save the current value
      if (lcdBrightnessValue < lcdBrightnessLen - 1) {
        lcdBrightnessValue++;
        updateEepromValues();
        percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], minBrightnessLcd, maxBrightnessLcd, minPercent, maxPercent);
        analogWrite(LCD_Backlight, lcdBrightnessLevel[lcdBrightnessValue]);
      }
    } else if (matrixBright) {  //when we are inside the matrix brightness submenu move left and right to change the option and save the current value
      if (matrixBrightnessValue < matrixBrightnessLen - 1) {
        matrixBrightnessValue++;
        updateEepromValues();
        percentMatrix = map(matrixBrightnessLevel[matrixBrightnessValue], minBrightnessMatrix, maxBrightnessMatrix, minPercent, maxPercent);
        lc.setIntensity(0, matrixBrightnessLevel[matrixBrightnessValue]);  // Set brightness
      }
    } else if (reset) {  //when we are inside the reset submenu move left and right to change the option and save the current value
      resetActive = false;
      lcd.setCursor(5, 1);
      lcd.print(" ");
    }
  }
  if (xValue < minThreshold) {
    if (soundActive) {  // if the sound is on start the buzzer
      tone(buzzerPin, frequencyJoystick);
      buzzerStartTime = currentMillis;
    }
    if (gameStarted) {  // when the game is running we use the joystick to navigate trough the matrix
      if (xPosPlayer < matrixSize - 1) {
        xPosPlayer++;
        if (matrix[xPosPlayer][yPosPlayer]) {
          xPosPlayer--;
        }
      }
    } else if (sound) {  //when we are inside the sound submenu move left and right to change the option and save the current value
      soundActive = true;
      lcd.setCursor(12, 1);
      lcd.print(" ");
      updateEepromValues();
    } else if (lcdBright) {
      if (lcdBrightnessValue > 0) {  //when we are inside the lcd brightness submenu move left and right to change the option and save the current value
        lcdBrightnessValue--;
        updateEepromValues();
        lcd.setCursor(lcdBrightnessValue + 2, 1);
        lcd.print(" ");
        // percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], minBrightnessLcd, maxBrightnessLcd, minPercent, maxPercent);
        percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], 0, 255, 0, 100);
        analogWrite(LCD_Backlight, lcdBrightnessLevel[lcdBrightnessValue]);
      }
    } else if (matrixBright) {  //when we are inside the matrix brightness submenu move left and right to change the option and save the current value
      if (matrixBrightnessValue > 0) {
        matrixBrightnessValue--;
        updateEepromValues();
        lcd.setCursor(matrixBrightnessValue + 2, 1);
        lcd.print(" ");
        percentMatrix = map(matrixBrightnessLevel[matrixBrightnessValue], minBrightnessMatrix, maxBrightnessMatrix, minPercent, maxPercent);
        lc.setIntensity(0, matrixBrightnessLevel[matrixBrightnessValue]);  // Set brightness
      }
    } else if (reset) {  //when we are inside the reset submenu move left and right to change the option and save the current value
      resetActive = true;
      lcd.setCursor(11, 1);
      lcd.print(" ");
    }
  }
  if (yValue > maxThreshold) {
    if (soundActive) {  // if the sound is on start the buzzer
      tone(buzzerPin, frequencyJoystick);
      buzzerStartTime = currentMillis;
    }
    if (menuOn) {  //when we are inside the main menu move up and down to select one option
      if (index < lenMenu - 1) {
        index++;
        lcd.clear();
      }
    } else if (settings) {  //when we are inside the settings submenu move up and down to select one option
      if (index < lenMenuSettings - 1) {
        index++;
        lcd.clear();
      }
    } else if (yPosPlayer > 0 && gameStarted) {  // when the game is running we use the joystick to navigate trough the matrix
      yPosPlayer--;
      if (matrix[xPosPlayer][yPosPlayer]) {
        yPosPlayer++;
      }
    }
  }
  // Update xPos based on joystick movement (Y-axis)
  if (yValue < minThreshold) {
    if (soundActive) {  // if the sound is on start the buzzer
      tone(buzzerPin, frequencyJoystick);
      buzzerStartTime = currentMillis;
    }
    if (menuOn) {  //when we are inside the main menu move up and down to select one option
      if (index > 0) {
        index--;
        lcd.clear();
      }
    } else if (settings) {  //when we are inside the settings submenu move up and down to select one option
      if (index > 0) {
        index--;
        lcd.clear();
      }
    } else if (yPosPlayer < matrixSize - 1 && gameStarted) {  // when the game is running we use the joystick to navigate trough the matrix
      yPosPlayer++;
      if (matrix[xPosPlayer][yPosPlayer]) {
        yPosPlayer--;
      }
    }
  }
  // Check if the position has changed and update the matrix if necessary
  if (xPosPlayer != xLastPosPlayer || yPosPlayer != yLastPosPlayer) {
    matrixChanged = true;
    matrix[xLastPosPlayer][yLastPosPlayer] = 0;
    //    matrix[xPos][yPos] = 1;
  }
}

void placeBomb() {  //bomb placed and we save the coordinates for that bomb
  // Place bomb logic
  xPosBomb = xPosPlayer;
  yPosBomb = yPosPlayer;

  bombActive = true;
  startBomb = currentMillis;
  index = 0;
}


void explode() {  //when the time goes off for the bomb we need to verifi which walls we have to distroy
  // Directions in which the explosion will affect: [x, y]
  count = 0;
  for (int i = 0; i < directionsLen; i++) {
    int x = xPosBomb + directions[i][0];
    int y = yPosBomb + directions[i][1];
    // Check bounds and update matrix
    if (x >= 0 && x < matrixSize && y >= 0 && y < matrixSize) {  //check if the walls we want to take down are not outside the matrix
      if (matrix[x][y]) {
        if (i != 0) {
          count++;
        }
      }
      matrix[x][y] = 0;
      // Check for player's position
      if (x == xPosPlayer && y == yPosPlayer) {
        nrLives--;
        count--;
        if (!nrLives) {
          gameStatus = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("You lost!");
          gameOverStartTime = currentMillis;
          //gameStarted = false;
          //lcd.clear();
          menuOn = true;
          //clearMatrix();
        }
      }
    }
  }
  score = score + count * (count % 10);
}

void printAnimation(const byte* image) {
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, bitRead(image[i], matrixSize - 1 - j));
    }
  }
}

void handleGameRunning() {
  minThreshold = 200;
  maxThreshold = 600;
  delayLedPlayer = 300;
  pressInterval = 100;
  moveInterval = 200;
  bombExplodingTime = 2000;
  bombLightInterval = 300;
  delayAnimation = 100;
  nrWallsMax = nrWallsMaxArray[level - 1];
  // Game running logic here
  if (nrWalls <= nrWallsMax) {  //generate the walls of the game
    generateMap();
    matrixChanged = true;
    xPosPlayer = 0;  //keep the players position at 0
    yPosPlayer = 0;
  } else {
    if (currentMillis - lastDebounceDelayPlayer > delayLedPlayer) {  //Hilight the current position of the player by turning the led on and off
      lastDebounceDelayPlayer = currentMillis;
      matrix[xPosPlayer][yPosPlayer] = !matrix[xPosPlayer][yPosPlayer];
      matrixChanged = true;
    }
    if (currentMillis - lastMoved > moveInterval) {  // Check if it's time to move the LED
      // game logic
      updatePositions();          // Update the position of the LED based on joystick input
      lastMoved = currentMillis;  // Update the time of the last move
    }
    if (reading != lastReading) {  //check if the reading from the button is different than the last time
      lastPressed = currentMillis;
    }

    if ((currentMillis - lastPressed) > pressInterval) {  // when the button in pressed then we place a bomb at that location
      if (reading != buttonState) {
        buttonState = reading;
        if (buttonState == LOW) {
          //Serial.print(1);
          placeBomb();
        }
      }
    }
    lastReading = reading;
    if (bombActive && (currentMillis - startBomb) < bombExplodingTime) {  //if the a bomb was placed at a certain location, in order to see it, we will make the led at that position blick faster than the player
      if (currentMillis - lastChangedBomb > bombLightInterval) {
        matrix[xPosBomb][yPosBomb] = !matrix[xPosBomb][yPosBomb];
        lastChangedBomb = currentMillis;

        // if the sound is on, start the buzzer everytime the bomb lights up
        if (soundActive) {
          if (matrix[xPosBomb][yPosBomb]) {
            tone(buzzerPin, frequencies[index++]);
          } else {
            noTone(buzzerPin);
          }
        }
        matrixChanged = true;
      }
    } else if (bombActive) {  //when the bomb is ready to explode
      bombActive = false;
      explode();
      matrixChanged = true;
    }
    if (matrixChanged == true) {  // Check if the matrix display needs updating
      updateMatrix();             // Update the LED matrix display
      matrixChanged = false;      // Reset the update flag
    }
  }
}
