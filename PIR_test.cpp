#include <Arduino.h>
int RusPIRPin = A0;

//bool Debug = true; //temporary turned off

int RusSwitch1Pin = 33;
int RusState1Pin = A1;
int RusSwitch2Pin = 29;
int RusState2Pin = A3;
bool bRusL1On = false;
bool bRusL2On = false;
bool bRusAlreadyOn = false;
bool bRusPIROn = false;
int statesave = 1;

unsigned long lastRusPIRTime = 0;
bool bLastRusPIROn = false;
unsigned long lastRusLivoloEvent = 0;
unsigned long dontCheckStateRus = 0;
unsigned long RusLivoloTimeConst = 15 * 1000;
unsigned long RusPIRTimeConst = 3 * 1000;

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
	if (n == 1)
	{
		SwitchLivolo(1);
	}
	if (n == 2)
	{
		SwitchLivolo(2);
	}
}

void setup() {
	//if (Debug)  //temporary turned off
	//{
	Serial.begin(9600);
	//}

	pinMode(RusPIRPin, INPUT);
	pinMode(RusSwitch1Pin, OUTPUT);
	pinMode(RusState1Pin, INPUT_PULLUP);
	pinMode(RusSwitch2Pin, OUTPUT);
	pinMode(RusState2Pin, INPUT_PULLUP);

	lastRusPIRTime = millis();//means at what time pir was last activated
	lastRusLivoloEvent = millis();
	dontCheckStateRus = millis();
}

void loop()
{
	int state = 0;
	state = digitalRead(RusPIRPin);
	if (statesave != state) {
		Serial.println(state);
		statesave = state;
	}
}
