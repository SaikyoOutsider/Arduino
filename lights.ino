#define btn A5
#define led_red 2
#define led_yellow 3
#define led_green 4
#define led_blue 6

bool butt_flag = 0;
bool butt;
unsigned long last_press = 0;
unsigned short mode = 0;
int leds[] = {led_red, led_yellow, led_green, led_blue};
int count = 4;

void setup() {
  pinMode(btn, INPUT_PULLUP); // Наша кнопка
  pinMode(led_red, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  bool butt = !digitalRead(btn);
  if (butt == 1 && butt_flag == 0 && millis() - last_press > 50){
    butt_flag = 1;
    mode = (mode + 1) % 6;
    Serial.print("Current mode: ");
    Serial.println(mode);
    last_press = millis();
  }
  if (butt == 0 && butt_flag == 1){
    butt_flag = 0;
  }
  if (mode == 1){
    digitalWrite(led_red, 1);
    digitalWrite(led_yellow, 1);
    digitalWrite(led_green, 1);
    digitalWrite(led_blue, 1);
  }
  if (mode == 2){
    digitalWrite(led_red, HIGH);
    digitalWrite(led_yellow, HIGH);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_blue, HIGH);

    delay(100);

    digitalWrite(led_red, LOW);
    digitalWrite(led_yellow, LOW);
    digitalWrite(led_green, LOW);
    digitalWrite(led_blue, LOW);

    delay(100);
  }
  if (mode == 3){
    for (int i = 0; i < count; i++) {
      digitalWrite(leds[i], HIGH);
      delay(200);
      digitalWrite(leds[i], LOW);
    }
  }
  if (mode == 4){
    for (int i = count - 1; i >= 0; i--) {
      digitalWrite(leds[i], HIGH);
      delay(200);
      digitalWrite(leds[i], LOW);
    }
  }
  if (mode == 5) {
  for (int brightness = 0; brightness <= 255; brightness++) {
    analogWrite(led_blue, brightness);
    delay(10); // скорость нарастания яркости
  }
  for (int brightness = 255; brightness >= 0; brightness--) {
    analogWrite(led_blue, brightness);
    delay(10); // скорость убывания
  }
}

  if (mode == 0){
    digitalWrite(led_red, 0);
    digitalWrite(led_yellow, 0);
    digitalWrite(led_green, 0);
    digitalWrite(led_blue, 0);
  }
}
