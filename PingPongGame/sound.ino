int buzzerPin = 3; // Пин для динамика

// Частоты нот для краткой победной мелодии
int melody[] = {
  523, 659, 784, 1047 // C5, E5, G5, C6
};

// Длительности нот (в миллисекундах)
int noteDurations[] = {
  200, 200, 200, 300
};

// Пауза между нотами (в миллисекундах)
const int pauseBetweenNotes = 20;

int melodyLength = 4; // Количество нот
int currentNote = 0; // Текущая нота
unsigned long previousMillis = 0; // Время последней смены ноты
bool isPlaying = false; // Флаг воспроизведения

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Управление мелодией
  if (isPlaying) {
    playMelody();
  } else {
    // Запускаем мелодию заново через 2 секунды
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 2000) {
      isPlaying = true;
      currentNote = 0;
      previousMillis = currentMillis; // Сбрасываем время
    }
  }

  // Здесь можно добавить другие процессы
  // Например:
  // digitalWrite(ledPin, HIGH);
  // readSensors();
}

void playMelody() {
  unsigned long currentMillis = millis();

  if (currentNote < melodyLength) {
    // Проверяем, пора ли начать следующую ноту
    unsigned long duration = (currentNote == 0 ? 0 : noteDurations[currentNote - 1] + pauseBetweenNotes);
    if (currentMillis - previousMillis >= duration) {
      // Останавливаем предыдущую ноту
      noTone(buzzerPin);

      // Проигрываем текущую ноту
      tone(buzzerPin, melody[currentNote]);
      previousMillis = currentMillis;
      currentNote++;
    }
  } else if (currentNote == melodyLength) {
    // Ждем окончания последней ноты
    if (currentMillis - previousMillis >= noteDurations[melodyLength - 1]) {
      noTone(buzzerPin);
      isPlaying = false;
      previousMillis = currentMillis; // Начинаем отсчет паузы перед повтором
      currentNote++; // Переходим в состояние ожидания
    }
  }
}
