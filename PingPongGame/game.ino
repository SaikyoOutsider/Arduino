#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define MAIN_BTN A3
#define PLAYER1_UP_BTN 4
#define PLAYER1_DOWN_BTN 6
#define PLAYER2_UP_BTN 5
#define PLAYER2_DOWN_BTN 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int buzzerPin = 3;
int potPin = A2;

int melody[] = {523, 659, 784, 1047};
int noteDurations[] = {200, 200, 200, 300};
const int pauseBetweenNotes = 20;

int winMelody[] = {659, 784, 880, 988, 1047, 880};
int winNoteDurations[] = {150, 150, 150, 150, 200, 400};
int winMelodyLength = 6;

enum GameState { MENU, SETTINGS, COUNTDOWN, PLAYING, GAME_OVER };
GameState gameState = MENU;

int targetScore = 3;
int player1Score = 0;
int player2Score = 0;

int ballX = SCREEN_WIDTH / 2;
int ballY = SCREEN_HEIGHT / 2;
int ballSpeedX = 2;
int ballSpeedY = 1;

int paddleWidth = 2;
int paddleHeight = 12;
int paddle1Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
int paddle2Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
int paddleSpeed = 2;

unsigned long previousMillis = 0;
const long countdownDuration = 3000;
unsigned long countdownStart = 0;
bool isPlayingMelody = false;
int currentNote = 0;
unsigned long noteStartMillis = 0;

void setup() {
  pinMode(MAIN_BTN, INPUT_PULLUP);
  pinMode(PLAYER1_UP_BTN, INPUT_PULLUP);
  pinMode(PLAYER1_DOWN_BTN, INPUT_PULLUP);
  pinMode(PLAYER2_UP_BTN, INPUT_PULLUP);
  pinMode(PLAYER2_DOWN_BTN, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("PONG GAME");
  display.display();
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();
  adjustContrast();

  if (digitalRead(MAIN_BTN) == LOW) {
    delay(200);
    if (gameState == MENU) {
      gameState = SETTINGS;
    } else if (gameState == SETTINGS) {
      gameState = COUNTDOWN;
      countdownStart = millis();
    } else if (gameState == PLAYING) {
      gameState = MENU;
    } else if (gameState == GAME_OVER) {
      gameState = MENU;
      player1Score = 0;
      player2Score = 0;
    }
  }

  switch (gameState) {
    case MENU:
      drawMenu();
      break;
    case SETTINGS:
      settingsMenu();
      break;
    case COUNTDOWN:
      if (currentMillis - countdownStart >= countdownDuration) {
        gameState = PLAYING;
        resetBall();
        paddle1Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
        paddle2Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
      } else {
        drawCountdown(3 - (currentMillis - countdownStart) / 1000);
      }
      break;
    case PLAYING:
      updateGame();
      break;
    case GAME_OVER:
      if (!isPlayingMelody) {
        playWinMelody();
      } else {
        continueMelody();
      }
      drawWinner();
      break;
  }
}

void adjustContrast() {
  int potValue = analogRead(potPin);
  int contrast = map(potValue, 0, 1023, 0, 255);
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(contrast);

  if (contrast > 200) {
    display.invertDisplay(true);
  } else if (contrast < 50) {
    display.invertDisplay(false);
  }
}

void drawMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PONG GAME");
  display.println("START - MAIN_BTN");
  display.display();
}

void settingsMenu() {
  if (digitalRead(PLAYER1_UP_BTN) == LOW || digitalRead(PLAYER2_UP_BTN) == LOW) {
    if (targetScore < 9) targetScore++;
    delay(200);
  }
  if (digitalRead(PLAYER1_DOWN_BTN) == LOW || digitalRead(PLAYER2_DOWN_BTN) == LOW) {
    if (targetScore > 1) targetScore--;
    delay(200);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("SET TARGET SCORE:");
  display.print("Score: ");
  display.println(targetScore);
  display.println("MAIN_BTN - confirm");
  display.display();
}

void drawCountdown(int secondsLeft) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(40, 20);
  display.print(secondsLeft);
  display.display();
  display.setTextSize(1);
}

void updateGame() {
  if (digitalRead(PLAYER1_UP_BTN) == LOW && paddle1Y > 0) paddle1Y -= paddleSpeed;
  if (digitalRead(PLAYER1_DOWN_BTN) == LOW && paddle1Y + paddleHeight < SCREEN_HEIGHT) paddle1Y += paddleSpeed;
  if (digitalRead(PLAYER2_UP_BTN) == LOW && paddle2Y > 0) paddle2Y -= paddleSpeed;
  if (digitalRead(PLAYER2_DOWN_BTN) == LOW && paddle2Y + paddleHeight < SCREEN_HEIGHT) paddle2Y += paddleSpeed;

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  if (ballY <= 0 || ballY >= SCREEN_HEIGHT) {
    ballSpeedY *= -1;
    tone(buzzerPin, 1000, 30);
  }
  if (ballX <= paddleWidth && ballY >= paddle1Y && ballY <= paddle1Y + paddleHeight) {
    ballSpeedX *= -1;
    tone(buzzerPin, 1200, 30);
  }
  if (ballX >= SCREEN_WIDTH - paddleWidth && ballY >= paddle2Y && ballY <= paddle2Y + paddleHeight) {
    ballSpeedX *= -1;
    tone(buzzerPin, 1200, 30);
  }

  if (ballX < 0) {
    player2Score++;
    playMelody();
    resetBall();
  }
  if (ballX > SCREEN_WIDTH) {
    player1Score++;
    playMelody();
    resetBall();
  }

  if (player1Score >= targetScore || player2Score >= targetScore) {
    gameState = GAME_OVER;
    currentNote = 0;
    isPlayingMelody = false;
  }

  display.clearDisplay();
  display.fillRect(0, paddle1Y, paddleWidth, paddleHeight, SSD1306_WHITE);
  display.fillRect(SCREEN_WIDTH - paddleWidth, paddle2Y, paddleWidth, paddleHeight, SSD1306_WHITE);
  display.fillRect(ballX, ballY, 2, 2, SSD1306_WHITE);
  display.setCursor(30, 0);
  display.print("P1: "); display.print(player1Score);
  display.setCursor(80, 0);
  display.print("P2: "); display.print(player2Score);
  display.display();
}

void drawWinner() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Player ");
  display.print(player1Score >= targetScore ? "1" : "2");
  display.println(" WINS!");
  display.setCursor(0, 30);
  display.print("Score: ");
  display.print(player1Score);
  display.print(" - ");
  display.println(player2Score);
  display.display();
}

void resetBall() {
  ballX = SCREEN_WIDTH / 2;
  ballY = SCREEN_HEIGHT / 2;
  ballSpeedX = (ballSpeedX < 0) ? -2 : 2;
  ballSpeedY = (ballSpeedY > 0) ? 1 : -1;
  paddle1Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
  paddle2Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
}

void playMelody() {
  for (int i = 0; i < 4; i++) {
    tone(buzzerPin, melody[i], noteDurations[i]);
    delay(noteDurations[i] + pauseBetweenNotes);
  }
  noTone(buzzerPin);
}

void playWinMelody() {
  isPlayingMelody = true;
  currentNote = 0;
  noteStartMillis = millis();
  tone(buzzerPin, winMelody[currentNote], winNoteDurations[currentNote]);
}

void continueMelody() {
  if (currentNote >= winMelodyLength) {
    noTone(buzzerPin);
    isPlayingMelody = false;
    return;
  }
  if (millis() - noteStartMillis >= winNoteDurations[currentNote]) {
    currentNote++;
    if (currentNote < winMelodyLength) {
      noteStartMillis = millis();
      tone(buzzerPin, winMelody[currentNote], winNoteDurations[currentNote]);
    }
  }
}
