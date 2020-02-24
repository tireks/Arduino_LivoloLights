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
unsigned long RusLivoloTimeConst = 15 * 1000; //time, when system is ignoring PIR after manual
											  //turn on/off the light --MAYBE,NOT SURE
unsigned long RusPIRTimeConst = 3 * 1000;//time, when system is ignoring PIR after PIR-controlled
										 //turn on/off the light --MAYBE,NOT SURE

bool Debug = true; //режим отладки

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
	dontCheckStateRus = millis(); // used for some kind of a delay, cos livolo is makin' some 
								  //voltage noise after "process of applying voltage to a specific pin"
}

void LivoloOn(int n) //while turning on and turning off switcher, calling "SwitchLivolo",
{                    // because modifying state of switcher is process of applying voltage to a specific pin
	if ((n == 1) && (!bRusL1On))
	{
		SwitchLivolo(1);
	}
	if ((n == 2) && (!bRusL2On))
	{
		SwitchLivolo(2);
	}
}

void LivoloOff(int n)//while turning on and turning off switcher, calling "SwitchLivolo",
{                    //because modifying state of switcher is process of applying voltage to a specific pin
	if ((n == 1) && (bRusL1On))
	{
		SwitchLivolo(1);
	}
	if ((n == 2) && (bRusL2On))
	{
		SwitchLivolo(2);
	}
}


void setup() 
{
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

		if (/*(hour()>17 || hour()<10) &&*/ (state == HIGH))//проверяем датчик движения только с 17:00 до 10:00(todo, but now disactivated)
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
	if ((millis() - dontCheckStateRus) > 700)
	{
		if ((state == HIGH && bRusL1On) || (state == LOW && !bRusL1On))
		{
			if (Debug)
			{
				Serial.println("Ruslan Livolo 1 manual Event");
			}
			//Состояние изменилось, инвертируем состояние и выводим изменение на экран
			//bL1On = !bL1On;
			if (state == HIGH)
			{
				bRusL1On = false;
			}
			else
			{
				bRusL1On = true;
			}
			lastRusLivoloEvent = millis();
		}
	}
	
	//  printLivoloState(1);

	state = digitalRead(RusState2Pin);
	if ((millis() - dontCheckStateRus) > 700)
	{
		if ((state == HIGH && bRusL2On) || (state == LOW && !bRusL2On))
		{
			//Состояние изменилось, инвертируем состояние и выводим изменение на экран
			if (Debug)
			{
				Serial.println("Ruslan Livolo 2 manual Event!");
			}
			//bRusL2On = !bRusL2On;
			if (state == HIGH)
			{
				bRusL2On = false;
			}
			else
			{
				bRusL2On = true;
			}
			lastRusLivoloEvent = millis();
		}
	}
	
}

