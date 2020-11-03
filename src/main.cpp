#include <Arduino.h>
/*
 Регистратор данных с использованием SD карт

 Пример сохранения данных с аналоговых портов на SD карте.
 Данные будут сохраняться в файле в виде набора строк с разделителем полей в виде символа ","

 Схема подключения:
 * Аналоговые сенсоры подключаются к аналоговым пинам
 * Модуль SD карты подключен в SPI по стандартной схеме:
 ** MOSI - пин 11
 ** MISO - пин12
 ** CLK - пин 13
 ** CS - pin 4
 */

#include <SPI.h>
#include <SD.h>
#include "DS1307.h" 
#include "ACS712.h"


ACS712 sensor(ACS712_20A, A0);
const int PIN_CHIP_SELECT = 10;
DS1307 rtc;

void setup() {
	Serial.begin(9600);

	// Этот пин обязательно должен быть определен как OUTPUT
	pinMode(PIN_CHIP_SELECT, OUTPUT);
  pinMode(A0,0);
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  Serial.print("Done! c="+ String(sensor.calibrate())+"\n\r");

  Serial.print("Initializing SD card...");
	// Пытаемся проинициализировать модуль
	if (!SD.begin(PIN_CHIP_SELECT)) {
		Serial.println("Card failed, or not present");
		// Если что-то пошло не так, завершаем работу:
		return;
	  }
  Serial.println("card initialized.");
  rtc.start();

}

void loop() {
  // Строка с данными, которые мы поместим в файл:
  uint8_t sec, min, hour, day, month;
  uint16_t year;
  static  unsigned long count =0;
  String logStringData = "";


  logStringData += String(count++);
  logStringData +="\t";

    //get time from RTC
  rtc.get(&sec, &min, &hour, &day, &month, &year);
  logStringData+=String(day);
  logStringData+="/";
  logStringData+=String(month);
  logStringData+="/";
  logStringData+=String(year);
  logStringData+="\t";
  logStringData+=String(hour);
  logStringData+=":";
  logStringData+=String(min);
  logStringData+=":";
  logStringData+=String(sec);
  logStringData+="\t";


  String str1=String((float)analogRead(A0));  
  String Current = String(sensor.getCurrentDC());
  str1.replace(String("."),String(","));
  Current.replace(String("."),String(","));
  logStringData += str1;
  logStringData+="\t";
  logStringData += Current;
  // Открываем файл, но помним, что одновременно можно работать только с одним файлом.
  // Если файла с таким именем не будет, ардуино создаст его.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // Если все хорошо, то записываем строку:
  if (dataFile) {
    dataFile.println(logStringData);
    dataFile.close();
    // Публикуем в мониторе порта для отладки
    Serial.println(logStringData);
  }
  else {
  // Сообщаем об ошибке, если все плохо
    Serial.print("error opening datalog.csv");
    Serial.println("\tCurrent ="+Current);
  }
  delay(500);
}