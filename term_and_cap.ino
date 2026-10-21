#include <Servo.h>

// Пины сенсоров
const byte CAP_PINS[] = {2, 3, 4, 5, 6}; // D2-D6

// Пины светодиодов
const byte LED_PINS[] = {7, 8, 9, 10};   // D7-D10
const byte NUM_LEDS = 4;

// Пин серво
const byte SERVO_PIN = 11;

// Последовательность кодов (4 шага по 5 бит)
const bool SEQUENCE[4][5] = {
  {1, 0, 1, 0, 1},
  {0, 1, 0, 1, 0},
  {0, 0, 1, 0, 0},
  {0, 1, 1, 1, 0}
};

// Порог: ниже — касание (1), выше — нет (0)
unsigned long baseline[5];        // базовые значения в покое
unsigned long TOUCH_THRESHOLD[5]; // индивидуальные пороги

// Время удержания для подтверждения кода (в миллисекундах)
const unsigned long HOLD_TIME = 800;

// Время игнорирования дребезга (в миллисекундах)
const unsigned long DEBOUNCE_TIME = 100;

Servo myservo;
byte currentStep = 0;          // Какой шаг последовательности ожидаем
unsigned long lastValidTime = 0;
bool lastCodeMatched = false;
bool sequenceCompleted = false;

void setup() {
  Serial.begin(9600);

  // Инициализация светодиодов
  for (byte i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  // Инициализация серво
  myservo.attach(SERVO_PIN);
  myservo.write(0); // Начальное положение — 0 градусов

  // Инициализация сенсорных пинов — в readCap они переключаются по мере надобности
  // Калибровка!
  calibrateSensors();
  for (byte i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH);
  }
  delay(100);
  for (byte i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
  delay(1000); // пауза, чтобы Serial успел вывести
}


void calibrateSensors() {
  const byte numSamples = 20;
  unsigned long sum[5] = {0, 0, 0, 0, 0};

  Serial.println("Калибровка... Не трогайте сенсоры!");

  for (byte s = 0; s < numSamples; s++) {
    for (byte i = 0; i < 5; i++) {
      sum[i] += readCapFiltered(CAP_PINS[i]);
    }
    delay(20);
  }

  for (byte i = 0; i < 5; i++) {
    baseline[i] = sum[i] / numSamples;
    TOUCH_THRESHOLD[i] = baseline[i] / 12; // <-- коэффициент 8

    // Защита от слишком низких/высоких порогов
    if (TOUCH_THRESHOLD[i] < 50)   TOUCH_THRESHOLD[i] = 50;
    if (TOUCH_THRESHOLD[i] > 1000) TOUCH_THRESHOLD[i] = 1000;

    Serial.print("Сенсор "); Serial.print(i);
    Serial.print(": baseline="); Serial.print(baseline[i]);
    Serial.print(", порог="); Serial.println(TOUCH_THRESHOLD[i]);
  }
}


// Измерение ёмкости (как в оригинале)
unsigned long readCap(byte pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(10);

  unsigned long start = micros();
  pinMode(pin, INPUT);
  while (digitalRead(pin) == LOW) {
    if (micros() - start > 100000) break; // 100 мс максимум
  }
  return micros() - start;
}

// Фильтрация сортировкой (медиана)
unsigned long readCapFiltered(byte pin) {
  const int N = 5;
  unsigned long buf[N];
  for (int i = 0; i < N; i++) {
    buf[i] = readCap(pin);
    delay(2);
  }
  // Сортировка
  for (int i = 0; i < N - 1; i++) {
    for (int j = i + 1; j < N; j++) {
      if (buf[i] > buf[j]) {
        unsigned long t = buf[i];
        buf[i] = buf[j];
        buf[j] = t;
      }
    }
  }
  return buf[N / 2];
}

// Чтение 5-битного кода
void readSensors(bool code[5]) {
  for (byte i = 0; i < 5; i++) {
    unsigned long val = readCapFiltered(CAP_PINS[i]);
    code[i] = (val < TOUCH_THRESHOLD[i]); // независимо для каждого
    // Для отладки:
    Serial.print(val); Serial.print(" ("); Serial.print(TOUCH_THRESHOLD[i]); Serial.print(") ");
  }
  Serial.println();
}

// Сравнение двух 5-битных кодов
bool codesEqual(const bool a[5], const bool b[5]) {
  for (byte i = 0; i < 5; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

void loop() {
  if (sequenceCompleted) {
    // Ничего не делаем после завершения
    return;
  }

  bool currentCode[5];
  readSensors(currentCode);

  // Проверяем, совпадает ли текущий код с ожидаемым шагом
  Serial.print("currentCode = ");
  for(int i=0;i<5;i++){
    Serial.print(currentCode[i] ? "1" : "0");
  }
  Serial.print(" SEQUENCE[");
  Serial.print(currentStep);
  Serial.print("] = ");
  for(int i=0;i<5;i++){
    Serial.print(SEQUENCE[currentStep][i] ? "1" : "0");
  }
  Serial.println();
  if (codesEqual(currentCode, SEQUENCE[currentStep])) {
    unsigned long now = millis();

    if (!lastCodeMatched) {
      // Только что началось совпадение — запоминаем время
      lastValidTime = now;
      lastCodeMatched = true;
    } else {
      // Уже в совпадении — проверяем, прошло ли HOLD_TIME
      if (now - lastValidTime >= HOLD_TIME) {
        // Подтверждённый ввод шага
        digitalWrite(LED_PINS[currentStep], HIGH);
        currentStep++;

        if (currentStep >= 4) {
          // Последовательность завершена
          myservo.write(90);
          sequenceCompleted = true;
          // Можно добавить сигнал или задержку, если нужно
        }

        // Сброс состояния, чтобы не засчитать повторно
        lastCodeMatched = false;
        lastValidTime = 0;
        delay(200); // небольшая пауза, чтобы избежать повторного срабатывания
      }
    }
  } else {
    // Код не совпадает — сбрасываем таймер совпадения
    lastCodeMatched = false;
    lastValidTime = 0;
  }

  delay(20); // небольшая задержка для стабильности
}