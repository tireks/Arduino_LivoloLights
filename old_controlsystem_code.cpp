/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

////1111
#include <TimeLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//Beginning of Auto generated function prototypes by Atmel Studio
void SwitchLivolo(int n);
void LivoloOn(int n);
void LivoloOff(int n);
//End of Auto generated function prototypes by Atmel Studio



// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 12

 // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);//Сеть для датчиков температуры

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);//Датчики температуры

unsigned long lastTempTime = 0;           // время последнего запроса температуры
bool bTemperNeedUpdate = false;

DeviceAddress RusThermo = { 0x28, 0xFF, 0x74, 0xC1, 0x3E, 0x04, 0x00, 0x9D };//28FF74C13E04009D                          
DeviceAddress ArtThermo = { 0x28, 0xFF, 0x1B, 0x87, 0x3D, 0x04, 0x00, 0xEE };//28FF1B873D0400EE                          
DeviceAddress AlexThermo = { 0x28, 0xFF, 0xAF, 0xCD, 0x3C, 0x04, 0x00, 0xEE };//28FFAFCD3C0400EE

// массивы температур целой части и дробной части
float RusTemper = DEVICE_DISCONNECTED_C;
float ArtTemper = DEVICE_DISCONNECTED_C;
float AlexTemper = DEVICE_DISCONNECTED_C;

bool Debug = true; //режим отладки

///////////////////////////////////////////
//// Livolo control 
int RusPIRPin = 5;

int RusSwitch1Pin = A3;
int RusState1Pin = A2;
int RusSwitch2Pin = 9;
int RusState2Pin = 8;
bool bRusL1On = false;
bool bRusL2On = false;
bool bRusAlreadyOn = false;
bool bRusPIROn = false;

unsigned long lastRusPIRTime = 0;
bool bLastRusPIROn = false;
unsigned long lastRusLivoloEvent = 0;
unsigned long dontCheckStateRus = 0;
unsigned long RusLivoloTimeConst = 15 * 1000;
unsigned long RusPIRTimeConst = 3 * 1000;

///////////////////////////////////////////
//// Livolo control end

void SwitchLivolo(int n)//process of applying voltage to a specific pin
{
	if (n == 1)
	{
		digitalWrite(RusSwitch1Pin, HIGH);
		delay(100);
		digitalWrite(RusSwitch1Pin, LOW);
		delay(50);
	}
	if (n == 2)
	{
		digitalWrite(RusSwitch2Pin, HIGH);
		delay(100);
		digitalWrite(RusSwitch2Pin, LOW);
		delay(50);
	}
	dontCheckStateRus = millis();
}

void LivoloOn(int n) //while turning on and turning off switcher, calling "SwitchLivolo", because modifying state of switcher is process of applying voltage to a specific pin
{
	if ((n == 1) && (!bRusL1On))
	{
		SwitchLivolo(1);
	}
	if ((n == 2) && (!bRusL2On))
	{
		SwitchLivolo(2);
	}
}

void LivoloOff(int n)//while turning on and turning off switcher, calling "SwitchLivolo", because modifying state of switcher is process of applying voltage to a specific pin
{
	if ((n == 1) && (bRusL1On))
	{
		SwitchLivolo(1);
	}
	if ((n == 2) && (bRusL2On))
	{
		SwitchLivolo(2);
	}
}


void setup() {
	if (Debug)
	{
		Serial.begin(9600);
	}

	pinMode(RusPIRPin, INPUT);
	pinMode(RusSwitch1Pin, OUTPUT);
	pinMode(RusState1Pin, INPUT_PULLUP);
	pinMode(RusSwitch2Pin, OUTPUT);
	pinMode(RusState2Pin, INPUT_PULLUP);

	lastRusPIRTime = millis();//means at what time pir was last activated
	lastRusLivoloEvent = millis();
	dontCheckStateRus = millis();

	// Start up the library DallasTemperature
	sensors.begin();

	sensors.setWaitForConversion(false); //Переводим получение температуры в асинхронный режим

	sensors.setResolution(RusThermo, TEMPERATURE_PRECISION);
	sensors.setResolution(ArtThermo, TEMPERATURE_PRECISION);
	sensors.setResolution(AlexThermo, TEMPERATURE_PRECISION);
}

void loop() {
	/////////////////////////////////////////////////
	//////////////////////////////  Livolo start

	int state = 0;
	// lcd.setCursor(10, 0); 
	//  lcd.print("      "); 

	if ((millis() - lastRusLivoloEvent) > RusLivoloTimeConst)
	{
		state = digitalRead(RusPIRPin);

		if (/*(hour()>17 || hour()<10) &&*/ (state == HIGH))//проверяем датчик движения только с 17:00 до 10:00
		{
			if (!bRusAlreadyOn)
			{
				LivoloOn(1);
				LivoloOn(2);
				if (Debug)
				{
					Serial.println("Ruslan Livolo On by PIR!");
				}
				bRusAlreadyOn = true;
			}
			lastRusPIRTime = millis();
			bRusPIROn = true;
			//      lcd.setCursor(10, 0); 
			//      lcd.print("PIR ON"); 


		}
		else
		{
			if ((millis() - lastRusPIRTime) > RusPIRTimeConst)
			{
				LivoloOff(1);
				LivoloOff(2);
				if (Debug)
				{
					Serial.println("Ruslan Livolo Off by PIR!");
				}
				//bL1On = false;
				//bL2On = false;
				bRusAlreadyOn = false;
			}
			bRusPIROn = false;
		}
	}
	else
	{
		//      lcd.setCursor(10, 0); 
		//      lcd.print("EVENT"); 
	}

	state = digitalRead(RusState1Pin);
	if ((millis() - dontCheckStateRus) > 1000)
	{
		if ((state == HIGH && bRusL1On) || (state == LOW && !bRusL1On))
		{
			if (Debug)
			{
				Serial.println("Ruslan Livolo 1 Event!");
			}
			//Состояние изменилось, инвертируем состояние и выводим изменение на экран
			//bL1On = !bL1On;
			lastRusLivoloEvent = millis();
		}
	}
	if (state == HIGH)
	{
		bRusL1On = false;
	}
	else
	{
		bRusL1On = true;
	}
	//  printLivoloState(1);

	state = digitalRead(RusState2Pin);
	if ((millis() - dontCheckStateRus) > 1000)
	{
		if ((state == HIGH && bRusL2On) || (state == LOW && !bRusL2On))
		{
			//Состояние изменилось, инвертируем состояние и выводим изменение на экран
			if (Debug)
			{
				Serial.println("Ruslan Livolo 2 Event!");
			}
			bRusL2On = !bRusL2On;
			lastRusLivoloEvent = millis();
		}
	}
	if (state == HIGH)
	{
		bRusL2On = false;
	}
	else
	{
		bRusL2On = true;
	}


	////////////////////////////////////////////////
	//////////////////////////////  Livolo end

	  // call sensors.requestTemperatures() to issue a global temperature 
	  // request to all devices on the bus

	int i = 0;

	if (millis() - lastTempTime > 60000)
	{
		if (Debug)
		{
			Serial.print("Requesting temperatures...");
		}
		sensors.requestTemperatures();
		if (Debug)
		{
			Serial.println("DONE");
		}
		lastTempTime = millis();
		bTemperNeedUpdate = true;
	}

	if (bTemperNeedUpdate && (millis() - lastTempTime > 800))
	{
		// Get current temperatures
		float temp = DEVICE_DISCONNECTED_C;
		temp = sensors.getTempC(RusThermo);
		if (temp != DEVICE_DISCONNECTED_C)
		{
			RusTemper = temp;
			if (Debug)
			{
				Serial.print("Ruslan temperature: ");
				Serial.print(RusTemper);
				Serial.println();
			}
		}

		temp = sensors.getTempC(ArtThermo);
		if (temp != DEVICE_DISCONNECTED_C)
		{
			ArtTemper = temp;
			if (Debug)
			{
				Serial.print("Artem temperature: ");
				Serial.print(ArtTemper);
				Serial.println();
			}
		}

		temp = sensors.getTempC(RusThermo);
		if (temp != DEVICE_DISCONNECTED_C)
		{
			AlexTemper = temp;
			if (Debug)
			{
				Serial.print("Alexander temperature: ");
				Serial.print(AlexTemper);
				Serial.println();
			}
		}
		bTemperNeedUpdate = false;
	}


}
