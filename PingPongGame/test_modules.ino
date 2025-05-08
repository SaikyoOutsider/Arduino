#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define MAIN_BTN A3
#define PLAYER1_UP_BTN 4
#define PLAYER2_UP_BTN 5
#define PLAYER2_DOWN_BTN 7
#define PLAYER1_DOWN_BTN 6
#define POTENCIOMETR

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int buzzerPin = 3;

int melody[] = {523, 659, 784, 1047};
int noteDurations[] = {200, 200, 200, 300};
const int pauseBetweenNotes = 20;

int melodyLength = 4;
int currentNote = 0;
unsigned long previousNoteMillis = 0;
bool isPlaying = false;

unsigned long previousDisplayMillis = 0;
const int displayInterval = 1000;

void playMelody() {
  unsigned long currentMillis = millis();

  if (currentNote < melodyLength) {
    unsigned long duration = (currentNote == 0 ? 0 : noteDurations[currentNote - 1] + pauseBetweenNotes);
    if (currentMillis - previousNoteMillis >= duration) {
      noTone(buzzerPin);
      tone(buzzerPin, melody[currentNote]);
      previousNoteMillis = currentMillis;
      currentNote++;
    }
  } else if (currentNote == melodyLength) {
    if (currentMillis - previousNoteMillis >= noteDurations[melodyLength - 1]) {
      noTone(buzzerPin);
      isPlaying = false;
      previousNoteMillis = currentMillis;
      currentNote++;
    }
  }
}

void setup() {
  Wire.begin();
  pinMode(buzzerPin, OUTPUT);
  pinMode(MAIN_BTN, INPUT_PULLUP);
  pinMode(PLAYER1_UP_BTN, INPUT_PULLUP);
  pinMode(PLAYER2_UP_BTN, INPUT_PULLUP);
  pinMode(PLAYER1_DOWN_BTN, INPUT_PULLUP);
  pinMode(PLAYER2_DOWN_BTN, INPUT_PULLUP);
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Не удалось найти дисплей SSD1306"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello, World!");
  display.display();
}

void loop() {
  unsigned long currentMillis = millis();

  int potValue = analogRead(A2);
  int contrast = map(potValue, 0, 1023, 0, 255);

  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(contrast);

  // Инвертировать экран при высоком или низком контрасте
  if (contrast > 200) {
    display.invertDisplay(true);
  } else if (contrast < 50) {
    display.invertDisplay(false);
  }

  // Обновляем дисплей каждые 1000 мс
  if (currentMillis - previousDisplayMillis >= displayInterval) {
    previousDisplayMillis = currentMillis;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("CYCLE");
    display.setCursor(0, 10);
    display.print("Time: ");
    display.print(currentMillis / 1000);
    display.print(" c");
    display.display();
  }

  bool main_btn = !digitalRead(MAIN_BTN);
  if (main_btn){
    Serial.print("MAIN_BTN: ");
    Serial.println(main_btn);
  }

  bool p1u = !digitalRead(PLAYER1_UP_BTN);
    if (p1u){
    Serial.print("PLAYER1_UP_BTN: ");
    Serial.println(p1u);
  }
  
  bool p1d = !digitalRead(PLAYER1_DOWN_BTN);
    if (p1d){
    Serial.print("PLAYER1_DOWN_BTN: ");
    Serial.println(p1d);
  }

  bool p2u = !digitalRead(PLAYER2_UP_BTN);
    if (p2u){
    Serial.print("PLAYER2_UP_BTN: ");
    Serial.println(p2u);
  }

  bool p2d = !digitalRead(PLAYER2_DOWN_BTN);
    if (p2d){
    Serial.print("PLAYER2_DOWN_BTN: ");
    Serial.println(p2d);
  }

  // Обработка кнопки (нажатие запускает мелодию)
  if (!digitalRead(MAIN_BTN) && !isPlaying) {
    isPlaying = true;
    currentNote = 0;
    previousNoteMillis = currentMillis;
  }

  // Если идёт воспроизведение — играем
  if (isPlaying) {
    playMelody();
  }
}
