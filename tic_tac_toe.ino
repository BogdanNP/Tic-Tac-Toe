#include "item.h"
#include "MCUFRIEND_kbv.h"
MCUFRIEND_kbv tft;

bool gameOver = false;

// TFT -screen

//uint16_t - color type
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define WHITE   0xFFFF

uint16_t g_identifier;


// width and height for game
int wG, hG;
// position for the center of X or O
int posX, posY;

Item lockedSelections[9];
int n = 0;

bool isO = false;


void _setUpTft() {
  tft.reset();
  g_identifier = tft.readID();
  tft.begin(g_identifier);

  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);  tft.setTextSize(5);
  tft.setCursor(0, 0);

  wG = tft.width();
  hG = tft.height();

  posX = wG / 6;
  posY = wG / 6;
  writeTurn(isO, WHITE);
}

// draw functions:

void drawGame() {
  tft.drawLine(0, 0, 0, wG, WHITE);
  tft.drawLine(wG / 3, 0, wG / 3, wG, WHITE);
  tft.drawLine(2 * wG / 3, 0, 2 * wG / 3, wG, WHITE);
  tft.drawLine(wG - 1, 0, wG - 1, wG, WHITE);
  tft.drawLine(0, 0, wG, 0, WHITE);
  tft.drawLine(0, wG / 3, wG, wG / 3, WHITE);
  tft.drawLine(0, 2 * wG / 3, wG,  2 * wG / 3, WHITE);
  tft.drawLine(wG - 1, wG - 1, 0, wG - 1, WHITE);
}


void drawItem(int x, int y, int w, bool isO, uint16_t color) {
  if (isO) {
    drawO(x, y, w, color);
  } else {
    drawX(x, y, w, color);
  }
}

void deleteItem(int x, int y, int w, bool isO) {
  if (isO) {
    drawO(x, y, w, BLACK);
  } else {
    drawX(x, y, w, BLACK);
  }
}

void drawO(int x, int y, int w, uint16_t color) {
  tft.drawCircle(x, y, w / 6 - 5, color);
}

void drawX(int x, int y, int w, uint16_t color) {
  tft.drawLine(x, y, x + w / 6 , y + w / 6, color);
  tft.drawLine(x, y, x + w / 6 , y - w / 6, color);
  tft.drawLine(x, y, x - w / 6 , y + w / 6, color);
  tft.drawLine(x, y, x - w / 6 , y - w / 6, color);
}

// END draw functions


// transform functions:

int toDrawingFromLogic(int x) {
  int wX = wG / 6;
  if (x == 1)
    wX = wG / 2;
  if (x == 2)
    wX = 5 * wG / 6;
  return wX;
}

int toLogicFromDrawing(int wX) {
  int x = 0;
  if (wX == wG / 2) {
    x = 1;
  }
  if (wX == 5 * wG / 6) {
    x = 2;
  }
  return x;
}

void drawLockedSelections() {
  for (int i = 0; i < n; ++i) {
    drawItem(
      toDrawingFromLogic(lockedSelections[i].x),
      toDrawingFromLogic(lockedSelections[i].y),
      wG,
      lockedSelections[i].isO,
      lockedSelections[i].isO ? RED : BLUE
    );
  }
}


void _loopTft() {
  drawGame();
  drawLockedSelections();
  drawItem(posX, posY, wG, isO, GREEN);
}



void writeTurn(bool isO, uint16_t color) {
  tft.setTextSize(3);
  tft.setTextColor(color);
  tft.setCursor(0, hG - 70);
  if (isO) {
    tft.print("Is O's turn.");
  }
  else {
    tft.print("Is X's turn.");
  }
}


void writeGameOver(char winner, uint16_t color) {
  tft.setTextSize(3);
  tft.setTextColor(color);
  tft.setCursor(0, hG - 70);
  tft.print("GAME OVER!");
  tft.setCursor(0, hG - 40);
  if (winner == 'O')
    tft.print("O WON!");
  else if (winner == 'X')
    tft.print("X WON!");
  else if (winner == 'T')
    tft.print("TIE!!!");
  else
    tft.print("ERROR!");
}

// END TFT -screen


// Keypad Matrix

void _selectCallback() {
  Item item = Item();
  item.x = toLogicFromDrawing(posX);
  item.y = toLogicFromDrawing(posY);
  item.isO = isO;
  _makeSelection(item, lockedSelections, n);
}

void _beforeMove() {
  deleteItem(posX, posY, wG, isO);
}

void _downCallback() {
  if (posY < 2 * wG / 3) {
    posY += wG / 3;
  }
}

void _upCallback() {
  if (posY > wG / 3) {
    posY -= wG / 3;
  }
}

void _leftCallback() {
  if (posX > wG / 3) {
    posX -= wG / 3;
  }
}

void _rightCallback() {
  if (posX < 2 * wG / 3) {
    posX += wG / 3;
  }
}


// Keypad Matrix

byte h = 0, v = 0;         // variables used in for loops
const int period = 50;     // A little delay to avoid errors.
int kdelay = 0;            // Non Blocking Delay
const byte rows = 4;       // Rows in Keypad
const byte columns = 4;    // Columns in Keypad
const byte Output[rows] = {53, 51, 49, 47}; //Row connceted to Arduino Pins
const byte Input[columns] = {45, 43, 41, 39}; //Columns connected to Arduino Pins

void _setUpMatrix() {
  for (byte i = 0; i < rows; i++) //Loop to declare output pins.
  {
    pinMode(Output[i], OUTPUT);
  }
  for (byte s = 0; s < columns; s++) // Loop to decalre Input pins, Initial Pulledup.
  {
    pinMode(Input[s], INPUT_PULLUP);
  }
  Serial.begin(9600);
}

byte keypad()
{
  static bool no_press_flag = 0;
  for (byte x = 0; x < columns; x++)
  {
    if (digitalRead(Input[x]) == HIGH);
    else
      break;
    if (x == (columns - 1))
    {
      no_press_flag = 1;
      h = 0;
      v = 0;
    }
  }
  if (no_press_flag == 1)
  {
    for (byte r = 0; r < rows; r++)
      digitalWrite(Output[r], LOW);
    for (h = 0; h < columns; h++)
    {
      if (digitalRead(Input[h]) == HIGH)
        continue;
      else
      {
        for (v = 0; v < rows; v++)
        {
          digitalWrite(Output[v], HIGH);
          if (digitalRead(Input[h]) == HIGH)
          {
            no_press_flag = 0;
            for (byte w = 0; w < rows; w++)
              digitalWrite(Output[w], LOW);
            return v * 4 + h;
          }
        }
      }
    }
  }
  return 50;
}

void _loopMatrix() {
  if (millis() - kdelay > period) //used to make non-blocking delay
  {
    kdelay = millis(); //capture time from millis function
    switch (keypad())     //Switch to get which button is pressed.
    {
      case 5:
        //        Serial.println("DOWN");
        _beforeMove();
        _downCallback();
        break;
      case 8:
        //        Serial.println("LEFT");
        _beforeMove();
        _leftCallback();
        break;
      case 9:
        //        Serial.println("SELECT");
        _beforeMove();
        _selectCallback();

        break;
      case 10:
        //        Serial.println("RIGHT");
        _beforeMove();
        _rightCallback();

        break;
      case 13:
        //        Serial.println("UP");
        _beforeMove();
        _upCallback();

        break;
      default:
        break;
    }
  }
}

// End Keypad Matrix

// GAME LOGIC

bool hasWinWithLine(char matrix[3][3], char s) {
  int cntH = 0;
  int cntV = 0;
  for (int i = 0; i < 3; ++i) {
    cntH = 0;
    cntV = 0;
    for (int j = 0; j < 3; ++j) {
      if (matrix[i][j] == s) {
        cntH++;
      }
      if (matrix[j][i] == s) {
        cntV++;
      }
    }
    if (cntH == 3 || cntV == 3) {
      return true;
    }
  }
  return false;
}

bool hasWinWithDiag(char matrix[3][3], char s) {
  int cntP = 0;
  int cntS = 0;
  for (int i = 0; i < 3; ++i) {
    if (matrix[i][i] == s) {
      cntP++;
    }
    if (matrix[i][2 - i] == s) {
      cntS++;
    }
  }
  return cntP == 3 || cntS == 3;
}

char checkForWin(Item items[], int n) {
  char matrix[3][3];

  for (int i = 0; i < n; ++i) {
    Item item = items[i];
    matrix[item.x][item.y] = item.isO ? 'O' : 'X';
  }

  bool isXWinner = hasWinWithLine(matrix, 'X') || hasWinWithDiag(matrix, 'X');
  bool isOWinner = hasWinWithLine(matrix, 'O') || hasWinWithDiag(matrix, 'O');

  if (isXWinner) {
    // X is the winner!
    Serial.println("X is the winner!");
    gameOver = true;
    return 'X';
  }
  else if (isOWinner) {
    // O is the winner!
    Serial.println("O is the winner!");
    gameOver = true;
    return 'O';
  }
  else if (n >= 9) {
    // We have a tie!
    Serial.println("We have a tie!");
    gameOver = true;
    return 'T';
  }
  return ' ';
}

void _logData(Item item) {
  if (item.isO) {
    Serial.print("O: ");
  } else {
    Serial.print("X: ");
  }
  Serial.print(item.x);
  Serial.print(" ");
  Serial.print(item.y);
  Serial.println();
}

void _makeSelection(Item _currentItem, Item items[], int& n) {
  for (int i = 0; i < n; ++i) {
    Item item = items[i];
    if (item.x == _currentItem.x && item.y == _currentItem.y) {
      return;
    }
  }
  items[n] = _currentItem;
  _logData(_currentItem);
  n++;
  writeTurn(isO, BLACK);
  isO = !isO;
  char winner = checkForWin(items, n);
  if (gameOver) {
    writeGameOver(winner, WHITE);
  } else {
    writeTurn(isO, WHITE);
  }
}

// END GAME LOGIC


void setup() {
  _setUpMatrix();
  _setUpTft();
}

void loop() {
  if (!gameOver) {
    _loopMatrix();
    _loopTft();
  }

}
