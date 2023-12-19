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
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int xPin = A0;      // Joystick X-axis
const int yPin = A1;      // Joystick Y-axis
const int buttonPin = 2;  // Button pin

// LED matrix configuration
const byte matrixSize = 8;                                 // Size of the LED matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // Initialize LedControl object
byte matrixBrightness = 2;                                 // Brightness level of the matrix

// Player variables
byte xPosPlayer = 0;
byte yPosPlayer = 0;
byte xLastPosPlayer = 0;
byte yLastPosPlayer = 0;
int delayLedPlayer = 300;  // Delay for LED player update
byte xPositionRandom;
byte yPositionRandom;
int lastDebounceDelayPlayer;

// Joystick thresholds
int minThreshold = 200;
int maxThreshold = 600;
byte moveInterval = 200;  // Interval for LED movement
unsigned long long lastMoved = 0;

// Button variables
byte pressInterval = 50;
int lastPressed = 0;
;
byte buttonState = LOW;
int lastReading;
int currentMillis;
int reading;

// Game status variables
bool matrixChanged = true;  // Flag for matrix update
bool gameStatus = true;     // Game running status
int nrWalls = 0;            // Current number of walls
int level = 1;
const int levelMax = 3;
byte nrWallsMax;
byte nrWallsMaxArray[levelMax] = { 0 };

// Bomb variables
byte bombActive = false;
int startBomb;
int bombExplodingTime = 2000;
int lastChangedBomb;
int bombLightInterval = 200;
int xPosBomb;
int yPosBomb;

// Animation variables
int lastAnimation = 0;
int delayAnimation = 100;
int currentAnimation = 0;

// Matrix state array
byte matrix[matrixSize][matrixSize] = { 0 };

const char mainMenuArray[4][15] = { "1. Start Game", "2. Settings", "3. About", "4. Info"};
const char settingsArray[4][20] = { "1.LCD Intensity", "2.Mtx Intensity", "3.Sound", "4.Exit" };
int lenMenu = 4;
int lenMenuSettings = 4;

byte gameStarted = false;
byte menuOn = true;
byte settings = false;
byte about = false;
byte lcdBright = false;
byte matrixBright = false;
byte sound = false;
byte soundActive = false;
byte info = false;

int index = 0;
int percent;
int messageBeginDelay = 3000;
int character = 50;
int lenAboutMessage = 50;
int characterDelay = 500;
int lastCharacterDisplay;
byte nrTotalLives = 3;
byte nrLives = 3;
byte levelAnimationDelay = 1500;
byte levelStartTime;

int lcdBrightnessAdress = 0;
int matrixBrightnessAdress = 1;
int soundAdress = 2;

int lcdBrightnessValue;
int matrixBrightnessValue;

int lcdBrightnessLevel[] = { 0, 51, 77, 102, 128, 153, 179, 204, 230, 255 };
int matrixBrightnessLevel[] = { 0, 1, 3, 5, 7, 9, 11, 13, 15 };

int directions[][2] = { { 0, 0 }, { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
int count = 0;
int directionsLen = 5;

int lcdBrightnessLen = 10;
int matrixBrightnessLen = 9;
int percentLcd;
int percentMatrix;
int progressBarCollumns;
int score = 0;

int gameOverMessageDelay = 2000;
int gameOverStartTime;
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
    0b11111111
  },
  { 0b00111000,
    0b01111100,
    0b01101100,
    0b01001100,
    0b00011000,
    0b00110000,
    0b01111110,
    0b11111111
  },
  { 0b00111100,
    0b01111110,
    0b01000110,
    0b00011110,
    0b00011110,
    0b01000110,
    0b01111110,
    0b00111100
  }
};

const uint8_t ledAnimation[][8] = {
  { 0b00111100,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00111100,
    0b00111100
  }
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
    0b00000000
  },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000
  },
  { 0b00000000,
    0b00000000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00000000,
    0b00000000
  },
  { 0b00000000,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000
  },
  { 0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
  },
  { 0b00000000,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000
  },
  { 0b00000000,
    0b00000000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00000000,
    0b00000000
  },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000
  },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }
};
const int imagesLen = sizeof(images) / 8;

void clearMatrix() {            //turn off all leds
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
    }
  }
  updateMatrix();
}

void handleGameOver() {  //display a certain animantion when the player loses teh game and restart the game
  minThreshold = 200;
  maxThreshold = 600;
  delayLedPlayer = 300;
  pressInterval = 100;
  moveInterval = 200;
  bombExplodingTime = 2000;
  bombLightInterval = 200;
  delayAnimation = 100;
  if (currentMillis - lastAnimation > delayAnimation) {
    lastAnimation = currentMillis;
    displayImage(images[currentAnimation]);
    if (++currentAnimation >= imagesLen) {
      //gameStatus = true;

      //gameStarted = false;
      currentAnimation = 0;
      clearMatrix();
      nrWalls = 0;
      xPosPlayer = 0;
      yPosPlayer = 0;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  randomSeed(analogRead(2));
  lc.shutdown(0, false);  // Enable display
  lc.clearDisplay(0);     // Clear display
  //nrWallsMax = random(32, 48); // Initialize maximum walls
  nrWallsMaxArray[0] = random(0, 10);   // Initialize maximum walls
  nrWallsMaxArray[1] = random(10, 20);  // Initialize maximum walls
  nrWallsMaxArray[2] = random(20, 30);  // Initialize maximum walls
  lcd.begin(16, 2);
  pinMode(LCD_Backlight, OUTPUT);

  //create characters for lcd
  lcd.createChar(1, heart);
  lcd.createChar(2, rightArrow);
  lcd.createChar(3, emptyHeart);
  lcd.createChar(4, speaker);
  lcd.createChar(5, check);
  lcd.createChar(6, block);
  lcd.createChar(7, upArrow);
  lcd.createChar(8, downArrow);
  //lcd.createChar(9, leftArrow);

  //show welcome message
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(6, 1);
  lcd.write(1);
  lcd.setCursor(7, 1);
  lcd.write(1);
  lcd.setCursor(8, 1);
  lcd.write(1);

  // EEPROM.update(lcdBrightnessAdress, lcdBrightnessLen - 1);
  // EEPROM.update(matrixBrightnessAdress, 1);
  // EEPROM.update(soundAdress, 0);

  lcdBrightnessValue = EEPROM.read(lcdBrightnessAdress);
  matrixBrightnessValue = EEPROM.read(matrixBrightnessAdress);
  soundActive = EEPROM.read(soundAdress);
  percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], 0, 255, 0, 100);
  percentMatrix = map(matrixBrightnessLevel[matrixBrightnessValue], 0, 15, 0, 100);
  analogWrite(LCD_Backlight, lcdBrightnessLevel[lcdBrightnessValue]);
  lc.setIntensity(0, matrixBrightnessLevel[matrixBrightnessValue]);  // Set brightness
}

void loop() {
  if (millis() > messageBeginDelay) {  //don't start till the welcome message disappers
    minThreshold = 200;
    maxThreshold = 600;
    delayLedPlayer = 300;
    pressInterval = 100;
    moveInterval = 200;
    bombExplodingTime = 2000;
    bombLightInterval = 200;
    delayAnimation = 100;
    currentMillis = millis();
    reading = digitalRead(buttonPin);

    if (!gameStarted) {   //when we are not playing, just show menu on lcd
      mainMenu();
    } else if (gameStarted) {   //wehn we start the game
      if (!gameStatus) {  //if the player lost
        handleGameOver();
        if (currentMillis - gameOverStartTime > gameOverMessageDelay) {  //show message on lcd when you lose
          gameStarted = false;
          lcd.clear();
          clearMatrix();
        }
        //gameStarted = false;
      } else {  //the game can begin again
        handleLives();
        handleGameRunning();      //while the game is on
        int var = checkMatrix();    //verify how many walls we still have to take down
        if (var == 0) {
          if (level <= levelMax) {
            level++;
            nrWalls = 0;
            levelStartTime = currentMillis;
          } else {
            gameStatus = false;
            // gameStarted = false;
            lcd.clear();
            menuOn = true;
            clearMatrix();
          }
        }
      }
    }
  }
}
void updateEepromValues() {     //update all the data from the eeprom
  EEPROM.update(lcdBrightnessAdress, lcdBrightnessValue);
  EEPROM.update(matrixBrightnessAdress, matrixBrightnessValue);
  EEPROM.update(soundAdress, soundActive);
}

void readEepromValues() {       //read all the data from the eeprom
  lcdBrightnessValue = EEPROM.read(lcdBrightnessAdress);
  matrixBrightnessValue = EEPROM.read(matrixBrightnessAdress);
  soundActive = EEPROM.read(soundAdress);
}

void drawProgressBar(int max) {     //bar for brightness
  for (int i = 0; i < max; i++) {
    lcd.setCursor(2 + i, 1);
    lcd.write(6);
  }
}
int checkMatrix() {                 //check if the matrix still have leds on
  int verif = 0;
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      if (matrix[row][col]) {
        verif++;
      }
    }
  }
  return verif;
}
void handleLives() {              //print the remaining lives, the score and the level
  lcd.setCursor(0, 0);
  lcd.print("Lives: ");
  for (int i = 0; i < nrLives; i++) {
    lcd.setCursor(7 + i, 0);
    lcd.write(1);
  }

  for (int i = 0; i < nrTotalLives - nrLives; i++) {
    lcd.setCursor(7 + i + nrLives, 0);
    lcd.write(3);
  }
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.setCursor(7, 1);
  lcd.print(score);
  lcd.setCursor(11, 1);
  lcd.print("Lvl: ");
  lcd.setCursor(15, 1);
  lcd.print(level);
}
void mainMenu() {           //print the menu and its details
  if (currentMillis - lastMoved > moveInterval) {
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
        updateMenu();
      }
    }
  }
  lastReading = reading;
  if (menuOn) {
    settings = false;
    lcd.setCursor(0, 0);
    lcd.print("Main Menu    ");
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
    lcd.print("Settings    ");
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
    lcd.write(9);
    lcd.setCursor(11, 0);
    lcd.write(10);
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
    lcd.print("Explode walls!");
    lcd.setCursor(0, 1);
    lcd.print("Use joystick!");
    //    if (currentMillis - lastCharacterDisplay > characterDelay) {
    //      lcd.scrollDisplayLeft();
    //      character--;
    //      if (character < 0) {
    //        character = lenAboutMessage;
    //      }
    //      lastCharacterDisplay = currentMillis;
    //    }
  }
}
void updateMenu() {     //change every flag depending on where we are in the menu and what we choose (this is only called when we press the button)
  if (menuOn) {
    if (index == 0) {
      lcd.clear();
      gameStarted = true;
      menuOn = false;
      settings = false;
      about = false;
      info = false;
      nrLives = nrTotalLives;
      level = 1;
      levelStartTime = currentMillis;
      score = 0;
    } else if (index == 1) {
      gameStarted = false;
      menuOn = false;
      about = false;
      index = 0;
      settings = true;
      info = false;
      //Serial.print(settings);
    } else if (index == 2) {
      gameStarted = false;
      menuOn = false;
      settings = false;
      about = true;
      info = false;
    }
    else if (index == 3) {
      gameStarted = false;
      menuOn = false;
      settings = false;
      about = false;
      info = true;
    }
  }

  else if (settings) {
    if (index == 0) {
      lcd.clear();
      lcdBright = true;
      matrixBright = false;
      menuOn = false;
      settings = false;
      sound = false;
      readEepromValues();
      //updateLcdBright();
    }
    if (index == 1) {
      lcd.clear();
      lcdBright = false;
      matrixBright = true;
      menuOn = false;
      settings = false;
      sound = false;
      displayLed(ledAnimation[0]);
      readEepromValues();
    }
    if (index == 2) {
      lcd.clear();
      sound = true;
      lcdBright = false;
      matrixBright = false;
      settings = false;
    }
    if (index == 3) {
      lcd.clear();
      lcdBright = false;
      matrixBright = false;
      menuOn = true;
      settings == false;
      index = 0;
      sound = false;
    }
  } else if (lcdBright) {
    lcd.clear();
    settings = true;
    matrixBright = false;
    menuOn = false;
    lcdBright = false;
    index = 0;
    sound = false;
  } else if (matrixBright) {
    lcd.clear();
    settings = true;
    matrixBright = false;
    menuOn = false;
    lcdBright = false;
    index = 0;
    sound = false;
    clearMatrix();
  } else if (sound) {
    lcd.clear();
    settings = true;
    matrixBright = false;
    menuOn = false;
    lcdBright = false;
    sound = false;
    index = 0;
  } else if (about) {
    lcd.clear();
    settings = false;
    matrixBright = false;
    lcdBright = false;
    menuOn = true;
    about = false;
    sound = false;
    index = 0;
    info = false;
    character = 35;
  }
  else if (info) {
    lcd.clear();
    settings = false;
    matrixBright = false;
    lcdBright = false;
    menuOn = true;
    about = false;
    sound = false;
    info = false;
    index = 0;
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
  //for every movement on the x and y axis we increment or decrement the values for brightness, change from on to off (sound) and chnage the position of the player
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  // Store the last positions of the LED
  xLastPosPlayer = xPosPlayer;
  yLastPosPlayer = yPosPlayer;
  // Update xPos based on joystick movement (X-axis)
  if (xValue > maxThreshold) {
    if (gameStarted) {
      if (xPosPlayer > 0) {
        xPosPlayer--;
        if (matrix[xPosPlayer][yPosPlayer]) {
          xPosPlayer++;
        }
      }
    } else if (sound) {
      soundActive = false;
      lcd.setCursor(5, 1);
      lcd.print(" ");
      updateEepromValues();
    } else if (lcdBright) {
      if (lcdBrightnessValue < lcdBrightnessLen - 1) {
        lcdBrightnessValue++;
        updateEepromValues();
        percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], 0, 255, 0, 100);
        analogWrite(LCD_Backlight, lcdBrightnessLevel[lcdBrightnessValue]);
      }
    } else if (matrixBright) {
      if (matrixBrightnessValue < matrixBrightnessLen - 1) {
        matrixBrightnessValue++;
        updateEepromValues();
        percentMatrix = map(matrixBrightnessLevel[matrixBrightnessValue], 0, 15, 0, 100);
        lc.setIntensity(0, matrixBrightnessLevel[matrixBrightnessValue]);  // Set brightness
      }
    }
  }
  if (xValue < minThreshold) {
    if (gameStarted) {
      if (xPosPlayer < matrixSize - 1) {
        xPosPlayer++;
        if (matrix[xPosPlayer][yPosPlayer]) {
          xPosPlayer--;
        }
      }
    } else if (sound) {
      soundActive = true;
      lcd.setCursor(12, 1);
      lcd.print(" ");
      updateEepromValues();
    } else if (lcdBright) {
      if (lcdBrightnessValue > 0) {
        lcdBrightnessValue--;
        updateEepromValues();
        lcd.setCursor(lcdBrightnessValue + 2, 1);
        lcd.print(" ");
        percentLcd = map(lcdBrightnessLevel[lcdBrightnessValue], 0, 255, 0, 100);
        analogWrite(LCD_Backlight, lcdBrightnessLevel[lcdBrightnessValue]);
      }
    } else if (matrixBright) {
      if (matrixBrightnessValue > 0) {
        matrixBrightnessValue--;
        updateEepromValues();
        lcd.setCursor(matrixBrightnessValue + 2, 1);
        lcd.print(" ");
        percentMatrix = map(matrixBrightnessLevel[matrixBrightnessValue], 0, 15, 0, 100);
        lc.setIntensity(0, matrixBrightnessLevel[matrixBrightnessValue]);  // Set brightness
      }
    }
  }
  if (yValue > maxThreshold) {
    if (menuOn) {
      if (index < lenMenu - 1) {
        index++;
        lcd.clear();
      }
    } else if (settings) {
      if (index < lenMenuSettings - 1) {
        index++;
        lcd.clear();
      }
    } else if (yPosPlayer > 0 && gameStarted) {
      yPosPlayer--;
      if (matrix[xPosPlayer][yPosPlayer]) {
        yPosPlayer++;
      }
    }
  }
  // Update xPos based on joystick movement (Y-axis)
  if (yValue < minThreshold) {
    if (menuOn) {
      if (index > 0) {
        index--;
        lcd.clear();
      }
    } else if (settings) {
      if (index > 0) {
        index--;
        lcd.clear();
      }
    } else if (yPosPlayer < matrixSize - 1 && gameStarted) {
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
          clearMatrix();
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
  levelAnimationDelay = 1500;
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
        matrixChanged = true;
        //Serial.print(1);
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
