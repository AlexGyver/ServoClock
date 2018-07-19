/*
  Скетч к проекту "Сервоприводные часы"
  Страница проекта (схемы, описания):
  Исходники на GitHub:
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  http://AlexGyver.ru/
*/
// ********* НАСТРОЙКИ *********
#define SERVOMIN  150     // минимальный сигнал серво
#define SERVOMAX  550     // максимальный сигнал серво
#define IR_PIN 3          // пин ИК датчика
#define IR_SENSOR 1       // использовать ИК датчик движения
#define IR_TIMEOUT 300000 // таймаут датчика движения (умолч. 300000мс это 5 мин)
#define STOCK_UP 270      // значение максимального угла (сегмент поднят) по умолчанию
#define STRING_SPEED 500  // скорость бегущей строки
// текст бегущей строки настраивается примерно в строке №100

// ********* БИБЛИОТЕКИ *********
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "RTClib.h"
#include "EEPROMex.h"

// ********* ПЕРЕМЕННЫЕ *********
Adafruit_PWMServoDriver hrs_drv = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver mins_drv = Adafruit_PWMServoDriver(0x41);

RTC_DS3231 rtc;

uint16_t servoInterval = 15; // временной интервал приращения угла для плавного поворота.
// Взят 15мс, так как при повороте ВСЕХ серво этот блок выполняется 18 мс
byte servoStep = 15;      // приращение угла поворота серво. Хранится в EEPROM (servo speed)
uint32_t servoTimer, timeTimer, stringTimer, IRTimer;

boolean turnFlag;         // флаг на опрос блока с серво (разгружает процессор)
uint16_t angle[28];       // личный угол каждой серво
uint16_t new_angle[28];   // новый угол. Меняем значение массива - серво поворачивается
uint16_t up_angle[28];    // угол, при котором индикатор поднят. Хранится в EEPROM
boolean clock_state;
boolean adj_flag, adj_flag2;
byte adj_servo;
byte stringCounter;
boolean stringFlag;
boolean IRFlag;

/************** БУКВЫ И СИМВОЛЫ *****************/
// http://www.uize.com/examples/seven-segment-display.html
#define _A 0x77
#define _B 0x7f
#define _C 0x39
#define _D 0x3f
#define _E 0x79
#define _F 0x71
#define _G 0x3d
#define _H 0x76
#define _J 0x1e
#define _L 0x38
#define _N 0x37
#define _O 0x3f
#define _P 0x73
#define _S 0x6d
#define _U 0x3e
#define _Y 0x6e
#define _a 0x5f
#define _b 0x7c
#define _c 0x58
#define _d 0x5e
#define _e 0x7b
#define _f 0x71
#define _h 0x74
#define _i 0x10
#define _j 0x0e
#define _l 0x06
#define _n 0x54
#define _o 0x5c
#define _q 0x67
#define _r 0x50
#define _t 0x78
#define _u 0x1c
#define _y 0x6e
#define _dash 0x40
#define _under 0x08
#define _equal 0x48
#define _empty 0x00

#define _0 0x3f
#define _1 0x06
#define _2 0x5b
#define _3 0x4f
#define _4 0x66
#define _5 0x6d
#define _6 0x7d
#define _7 0x07
#define _8 0x7f
#define _9 0x6f

byte HEXnumbers[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
// текст бегущей строки
/*
  byte runningString[] = {_empty, _empty, _empty, _empty,
                        _H, _E, _L, _L, _O, _empty, _empty,
                        _G, _Y, _U, _E, _r, _empty, _empty,
                        _S, _e, _r, _u, _o, _empty, _empty,
                        _C, _L, _O, _C, _H,
                        _empty, _empty, _empty, _empty,
                       };
*/
byte runningString[] = {_empty, _empty, _empty, _empty,
                        _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
                        _empty, _empty, _empty, _empty,
                       };
/************** БУКВЫ И СИМВОЛЫ *****************/

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(300);

  if (!IR_SENSOR) IRFlag = true;    // если не используем ИК, разрешить работу
  
  // проверка RTC
  if (rtc.begin()) {
    Serial.println("RTC OK");
    clock_state = true;
  } else {
    Serial.println("RTC ERROR");
    clock_state = false;
  }
  help();   // вывод текстового меню

  // старт и настройка серво шилда
  hrs_drv.begin();
  hrs_drv.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  mins_drv.begin();
  mins_drv.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  // первый запуск
  if (EEPROM.read(100) != 10) {   // храним какое то число как индикатор запуска
    Serial.println("first start");
    EEPROM.writeByte(100, 10);
    for (byte i = 0; i < 28; i++) {
      EEPROM.writeInt(i * 2, STOCK_UP);
      EEPROM.writeByte(90, 15);
    }
  }

  servoStep = EEPROM.read(90);    // вспоминаем скорость поворота

  // при запуске все серво опустить
  for (int i = 0; i < 28; i++) {
    new_angle[i] = SERVOMIN;
    angle[i] = SERVOMIN;
    up_angle[i] = EEPROM.readInt(i * 2);   // восстановить из памяти верхние значения индикаторов
    if (i < 14)
      hrs_drv.setPWM(i, 0, angle[i]);   // первый шилд
    else {
      mins_drv.setPWM(i - 14, 0, angle[i - 14]);  // второй шилд
    }
    delay(70);   // задержка между поворотами, чтобы не брать большой ток
  }
  delay(700);
  for (int i = 0; i < 28; i++) {
    // отключить серво
    if (i < 14)
      hrs_drv.setPWM(i, 0, 4096);
    else {
      mins_drv.setPWM(i - 14, 0, 4096);
    }
  }
  delay(700);
  Serial.println(F("Servo ready"));
  timeTimer = 60000;    // искусственно переполнить таймер времени,
  // чтобы часы сразу показали время (см. timeTick() )
}

void help() {
  Serial.println(F("***********************************************************************"));
  Serial.println(F("Welcome to ServoClock by AlexGyver! You can use serial commands:"));
  Serial.println(F("<set-time HH:MM>   - set time (example: set-time 01:20)"));
  Serial.println(F("<get-time>         - return RTC time"));
  Serial.println(F("<start-clock>      - start counting time"));
  Serial.println(F("<stop-clock>       - stop counting time"));
  Serial.println(F("<start-string>     - start running string"));
  Serial.println(F("<stop-string>      - stop running string"));
  Serial.println(F("<all-down>         - turn all segments down"));
  Serial.println(F("<all-up>           - turn all segments up"));
  Serial.println(F("<down NUMBER>      - turn segment NUMBER down (example: down 10)"));
  Serial.println(F("<up NUMBER>        - turn segment NUMBER up (example: up 10)"));
  Serial.println(F("<speed VALUE>      - set servo speed to VALUE, default 15 (example: speed 20)"));
  Serial.println(F("<adj NUMBER VALUE> - set max VALUE for servo with NUMBER and store it in EEPROM (example: adj 05 300)"));
  Serial.println(F("<help>             - print this instruction again"));
  Serial.println(F("***********************************************************************"));
}

void loop() {
  serialTick();     // обработка команд из порта
  servoTick();      // плавное управление серво
  timeTick();       // таймер времени (по флагу clock_state)
  displayString();  // отображать бегущую строку (по флагу stringFlag)
  IRTick();         // проверка ИК датчика
}
void IRTick() {
  if (IR_SENSOR) {
    if (digitalWrite(IR_PIN)) {                       // если сработал датчик движения
      IRTimer = millis();                             // сбросить таймер
      if (!IRFlag) IRFlag = true;                     // включить часы
    } else {                                          // если датчик показывает 0
      if (millis() - IRTimer > IR_TIMEOUT) {          // проверка на таймаут
        if (IRFlag) IRFlag = false;                   // отключить часы
      }
    }
  }
}
void displayString() {
  if (stringFlag) {
    if (millis() - stringTimer > STRING_SPEED) {    // таймер скорости строки
      stringTimer = millis();

      byte servoNum = 0;                  // с 0 сервы
      for (byte i = 0; i < 4; i++) {      // для 4 индикаторов
        for (byte j = 0; j < 7; j++) {    // каждый сегмент
          boolean thisBit;
          thisBit = runningString[stringCounter + i] & (1 << j);  // получить 0 или 1
          if (thisBit) new_angle[servoNum] = up_angle[servoNum];  // поднять
          else new_angle[servoNum] = SERVOMIN;                    // опустить
          servoNum++;                                             // перейти к следующей серво
        }
      }
      stringCounter++;
      if (stringCounter > (sizeof(runningString) - 4)) stringCounter = 0;
      turnFlag = true;
    }
  }
}

void timeTick() {
  if (clock_state) {
    if (millis() - timeTimer > 60000) {   // отсчитываем минуту
      timeTimer = millis();
      // каждую минуту берём время с RTC тип

      DateTime now = rtc.now();
      Serial.print("New minute. Time is ");
      Serial.print(now.hour()); Serial.print(":"); Serial.println(now.minute());

      // забиваем массив для отображения
      uint8_t indicators[4];
      indicators[0] = now.hour() / 10;    // десятки часов
      indicators[1] = now.hour() % 10;    // единицы часов
      indicators[2] = now.minute() / 10;  // десятки минут
      indicators[3] = now.minute() % 10;  // единицы минут

      turnFlag = true;            // разрешить поворот серв
      byte servoNum = 0;
      for (byte i = 0; i < 4; i++) {      // для 4 индикаторов
        for (byte j = 0; j < 7; j++) {    // каждый сегмент
          boolean thisBit;
          // thisBit принимает 0 или 1 - поднять или опустить сегмент.
          // HEXnumbers разбивается на биты, например 0000110 это цифра 1
          // и побитно принимается решение, поднять или опустить сегмент
          thisBit = HEXnumbers[indicators[i]] & (1 << j);
          if (i == 0 && indicators[0] == 0) thisBit = 0;          // не показывать ноль в десятках часов
          if (thisBit) new_angle[servoNum] = up_angle[servoNum];  // поднять
          else new_angle[servoNum] = SERVOMIN;                    // опустить
          servoNum++;                                             // перейти к следующей серво
        }
      }
    }
  }
}

// функция плавного поворота серво
void servoTick() {
  if (turnFlag && IRFlag) {
    if (millis() - servoTimer > servoInterval) {
      servoTimer = millis();
      boolean waitFlag = false;

      for (byte i = 0; i < 28; i++) {
        if (new_angle[i] > angle[i]) {        // если угол больше нужного
          waitFlag = true;
          angle[i] += servoStep;
          angle[i] = constrain(angle[i], SERVOMIN, up_angle[i]);
          if (i < 14) {
            hrs_drv.setPWM(i, 0, angle[i]);
          } else {
            mins_drv.setPWM(i - 14, 0, angle[i]);
          }
          //Serial.print("up servo #"); Serial.print(i); Serial.print(" "); Serial.println(angle[i]);
        }
        else if (new_angle[i] < angle[i]) {   // если угол меньше нужного
          waitFlag = true;
          angle[i] -= servoStep;
          angle[i] = constrain(angle[i], SERVOMIN, up_angle[i]);
          if (i < 14) {
            hrs_drv.setPWM(i, 0, angle[i]);
          } else {
            mins_drv.setPWM(i - 14, 0, angle[i]);
          }
          //Serial.print("down servo #"); Serial.print(i); Serial.print(" "); Serial.println(angle[i]);
        } else if (new_angle[i] == angle[i]) {  // если угол равен нужному
          // отключить серво
          if (i < 14) {
            hrs_drv.setPWM(i, 0, 4096);
          } else {
            mins_drv.setPWM(i - 14, 0, 4096);
          }
          if (adj_flag && i == adj_servo) {
            // тут короч "опустить и снова поднять индикатор при команде adj"
            adj_flag = false;
            adj_flag2 = true;
            new_angle[adj_servo] = up_angle[adj_servo];
          }
          //Serial.print("servo #"); Serial.print(i); Serial.println(" disabled");
        }
      }
      turnFlag = waitFlag;
    }
  }
  // тут короч "опустить и снова поднять индикатор при команде adj" - продолжение
  if (adj_flag2) {
    adj_flag2 = false;
    turnFlag = true;
  }
}

// опросчик и парсер сериал
void serialTick() {
  if (Serial.available() > 0) {
    String buf = Serial.readString();
    if (buf.startsWith("all-down")) {
      turnFlag = true;
      for (int i = 0; i < 28; i++) {
        new_angle[i] = SERVOMIN;
      }
      Serial.println(F("Servos down"));
    }
    else if (buf.startsWith("all-up")) {
      turnFlag = true;
      for (int i = 0; i < 28; i++) {
        new_angle[i] = up_angle[i];
      }
      Serial.println(F("Servos up"));
    }
    else if (buf.startsWith("set-time")) {    // set-time 01:20
      int newH = buf.substring(9, 11).toInt();
      int newM = buf.substring(12, 14).toInt();
      if (newH >= 0 && newH <= 23 && newM >= 0 && newM <= 59) {
        rtc.adjust(DateTime(2012, 21, 12, newH, newM, 0));
        timeTimer = 60000 + millis();
        Serial.print(F("RTC time set to "));
        Serial.print(newH); Serial.print(":"); Serial.println(newM);
      } else {
        Serial.println(F("Wrong value!"));
      }
    }
    else if (buf.startsWith("get-time")) {
      Serial.print(F("RTC time is "));
      DateTime now = rtc.now();
      Serial.print(now.hour()); Serial.print(":"); Serial.println(now.minute());
    }
    else if (buf.startsWith("down")) {
      int value = buf.substring(5).toInt();
      if (value >= 0 && value <= 28) {
        Serial.print(F("Turning down servo #"));
        Serial.println(value);
        new_angle[value] = SERVOMIN;
        turnFlag = true;
      } else {
        Serial.println(F("Wrong value!"));
      }
    }
    else if (buf.startsWith("up")) {
      int value = buf.substring(3).toInt();
      if (value >= 0 && value <= 28) {
        Serial.print(F("Turning up servo #"));
        Serial.println(value);
        new_angle[value] = up_angle[value];
        turnFlag = true;
      } else {
        Serial.println(F("Wrong value!"));
      }
    }
    else if (buf.startsWith("speed")) {
      int value = buf.substring(6).toInt();
      if (value >= 0 && value <= 255) {
        servoStep = value;
        EEPROM.writeByte(90, value);
        Serial.print(F("Speed set to "));
        Serial.println(servoStep);
      } else {
        Serial.println(F("Wrong value!"));
      }
    }
    else if (buf.startsWith("adj")) {   // adj 12 600
      byte num = buf.substring(4, 6).toInt();
      int val = buf.substring(7).toInt();
      up_angle[num] = val;          // установить новое значение
      new_angle[num] = SERVOMIN;
      EEPROM.writeByte(num * 2, val);   // записать в память
      Serial.print(F("Adjust servo #")); Serial.print(num);
      Serial.print(F(" value ")); Serial.println(val);
      turnFlag = true;
      adj_flag = true;
      adj_servo = num;
    }
    if (buf.startsWith("start-clock")) {
      Serial.println(F("Clock start"));
      clock_state = true;
    }
    if (buf.startsWith("stop-clock")) {
      Serial.println(F("Clock stop"));
      clock_state = false;
    }
    if (buf.startsWith("start-string")) {
      Serial.println(F("Start string"));
      clock_state = false;
      stringFlag = true;
    }
    if (buf.startsWith("stop-string")) {
      Serial.println(F("Stop string"));
      clock_state = true;
      stringFlag = false;
    }
    if (buf.startsWith("help")) {
      help();
    }
  }
}
