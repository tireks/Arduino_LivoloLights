//when typing date and time in serial input, type in format "dd/mm/yyyy hh:mm:ss"
//but serial output will be in format "hh:mm:ss dd/mm/yyyy"




#include <DS1307RTC.h>

#include <Time.h>

#include <Wire.h>    


String inSer = "";

boolean strFull = false;

tmElements_t datetime;




void setup() {

	Serial.begin(9600);

}




void loop() {

	// serial data incoming


	if (strFull) {

		datetime.Hour = (int(inSer[11]) - 48) * 10 + (int(inSer[12]) - 48);
		Serial.println(inSer[11]);
		Serial.println(inSer[12]);
		datetime.Minute = (int(inSer[14]) - 48) * 10 + (int(inSer[15]) - 48);
		datetime.Second = (int(inSer[17]) - 48) * 10 + (int(inSer[18]) - 48);
		datetime.Day = (int(inSer[0]) - 48) * 10 + (int(inSer[1]) - 48);
		datetime.Month = (int(inSer[3]) - 48) * 10 + (int(inSer[4]) - 48);
		datetime.Year = CalendarYrToTm((int(inSer[6]) - 48) * 1000 + (int(inSer[7]) - 48) * 100 + (int(inSer[8]) - 48) * 10 + (int(inSer[9]) - 48));


		RTC.write(datetime); // wrote into DS3231
		Serial.println("time wrote\r\n");

		// clear the string

		inSer = "";

		strFull = false;

	}



	// recieving data from ds3231

	if (RTC.read(datetime)) {

		print2(datetime.Hour, ":");

		print2(datetime.Minute, ":");

		print2(datetime.Second, " ");

		print2(datetime.Day, "/");

		print2(datetime.Month, "/");

		print2(tmYearToCalendar(datetime.Year), "");

		Serial.println();

	}



	else {

		Serial.print("error");

		delay(5000);

	}

	delay(1000);

}



void print2(int nn, String str) {

	if (nn >= 0 && nn < 10)

	{
		Serial.print("0");
	}

	Serial.print(nn);

	Serial.print(str);

}



void serialEvent() {
	Serial.print("serial event started\r\n");

	while (Serial.available()) {

		// recieve another bite:

		char c = (char)Serial.read();
		Serial.print("readin symbols\r\n");

		// add to string

		inSer += c;


		// /n - end of translation


		if (c == '\n')

		{
			strFull = true;
		}
		Serial.print("upd strfull\r\n");

	}

}