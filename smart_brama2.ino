/*
LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
//#include <LiquidCrystal.h>
#include <Arduino.h>
#include <Bounce2.h>
#include "Timers.h"
#include <OneWire.h>
//#include <DallasTemperature.h>
#include <Ultrasonic.h>
#include <SoftwareSerial.h>


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();

#define prz_brama1 2
#define prz_brama2 3
#define pk_brama1l 13
#define pk_brama1p 12
#define pwm_brama1l 11 // pwm_brama1l 11
#define pwm_brama1p 10 // pwm_brama1p 10
#define pk_dzwonek 9
#define trig 6
#define echo 5

digitalWrite(pk_brama1l, LOW);
#define pk_brama1l_otwieranie_Amps A0

float vpp=0.0048828;
float sensitivity=0.066;
bool stan_brama1=0; // 0 zamknięta 1-otwarta
bool stan_brama2=0;
bool brama1_otwieranie=0;
bool brama1_zamykanie=0;
int brama1_czas_pracy=10000; // czas w milisekundach
int brama1_zwloka=2000; // czas zwłoki do rozpoczęcia otwierania drugiego skrzydła
byte prz_rygiel_seconds;
byte prz_dzwonek_seconds=0;
byte sekundy=0;
byte O_seconds ;
int minutes=0;
int a=0;
byte odlicz=0;
// konfiguracja timerów
Timer spr_przyciskow;
Timer zwloka_otw_brama1;
Timer praca_otw_brama1;
Timer zwloka_zam_brama1;
Timer praca_zam_brama1;
Timer furtka_otwarta_jest;
Timer zamykanie;
Timer akcja;
Timer puls;
// inicjalizacja ultrasonic
 Ultrasonic ultrasonic(trig,echo);
void setup() {
// konfiguracja wejść
  pinMode ( prz_brama1, INPUT_PULLUP);
  pinMode ( prz_brama2, INPUT_PULLUP);
  pinMode ( pk_brama1l_otwieranie_Amps, INPUT);


// konfiguracja wyjść
  pinMode ( pk_brama1l, OUTPUT);
  pinMode ( pk_brama1p, OUTPUT);
  pinMode ( pwm_brama1l, OUTPUT);
  pinMode ( pwm_brama1p, OUTPUT);
  pinMode ( pk_dzwonek, OUTPUT);

// konfiguracja debouncerów
  debouncer1.attach(prz_brama1);
  debouncer1.interval(20);
  debouncer2.attach(prz_brama2);
  debouncer2.interval(20);

// inicjalizacja timerów
  spr_przyciskow.begin(200);
  puls.begin(1000);




// inicjalizacja portu szeregowego
 // SoftwareSerial mySerial(0, 1);
 // mySerial.begin(9600);


  Serial.begin(57600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }


    Serial.println("Goodnight moon!");

}

void loop() {


	if (spr_przyciskow.available()) {//timer przycisku dzwonka
	stan_przyciskow();
	spr_przyciskow.restart();
}

	if (zwloka_otw_brama1.available()) { // timer resetu stanu dzwonka
		digitalWrite(pk_brama1p, HIGH);
		zwloka_otw_brama1.begin(0);
		stan_brama1 = TRUE;
		praca_otw_brama1.begin(brama1_czas_pracy);
		}
	if (praca_otw_brama1.available()) { // timer resetu stanu dzwonka
			digitalWrite(pk_brama1l, LOW);
			digitalWrite(pk_brama1p, LOW);
		    praca_otw_brama1.begin(0);
		    brama1_otwieranie=FALSE;
			}
	if (zwloka_zam_brama1.available()) { // timer resetu stanu dzwonka
			digitalWrite(pwm_brama1l, HIGH);
			zwloka_zam_brama1.begin(0);
			stan_brama1 = FALSE;
			praca_zam_brama1.begin(brama1_czas_pracy);
			}
	if (praca_zam_brama1.available()) { // timer resetu stanu dzwonka
		digitalWrite(pwm_brama1l, LOW);
		digitalWrite(pwm_brama1p, LOW);
	    praca_zam_brama1.begin(0);
	    brama1_zamykanie=FALSE;
		}

	/*
if (rygiel.available()) { //timer przycisku rygla
	stan_rygiel();
	rygiel.restart();
	}
if (akcja.available()) {
	action();
	akcja.restart();
	}
	*/
if (puls.available()) { // timer co 1 sekunde
	heartbeat();
	puls.restart();

}
}

void stan_przyciskow(){

	if( 0 == (sekundy%3) ) {
			if (digitalRead(prz_brama1) == LOW && (brama1_otwieranie) == FALSE && (brama1_zamykanie) == FALSE && (stan_brama1) == FALSE )
		{ //Jeśli przycisk wciśnięty 1
				otwieranie_bramy1();
				}

	if (digitalRead(prz_brama1) == LOW && (brama1_otwieranie == TRUE) && (brama1_zamykanie) == FALSE  && (stan_brama1 == TRUE)  )
			{ //Jeśli przycisk wciśnięty a brama pracuje
					stop_bramy1();
			}

	}
	if (digitalRead(prz_brama1) == LOW && (brama1_otwieranie == FALSE) && (brama1_zamykanie) == TRUE  && (stan_brama1 == FALSE)  )
			{ //Jeśli przycisk wciśnięty a brama pracuje
					stop_bramy1();
			}

	if( 2 == (sekundy%3) ) {
			if (digitalRead(prz_brama1) == LOW && (brama1_otwieranie) == FALSE && (brama1_zamykanie) == FALSE && (stan_brama1) == TRUE  )
				{ //Jeśli przycisk wciśnięty a brama pracuje
						zamykanie_bramy1();
				}
								}

}
void otwieranie_bramy1() {
		brama1_otwieranie = TRUE;
		digitalWrite(pk_brama1l, HIGH);
		analogWrite(pwm_brama1l,255);
		zwloka_otw_brama1.begin(brama1_zwloka);
}

void stop_bramy1() {
		analogWrite(pwm_brama1l,0)
		digitalWrite(pk_brama1l, LOW);
		digitalWrite(pk_brama1p, LOW);
		analogWrite(pwm_brama1p, 0);

		praca_otw_brama1.begin(0);
	    brama1_otwieranie=FALSE;
	    brama1_zamykanie=FALSE;
}
void zamykanie_bramy1() {
		brama1_zamykanie = TRUE;
		digitalWrite(pk_brama1p, LOW);
		analogWrite(pwm_brama1p, 255);
		zwloka_zam_brama1.begin(brama1_zwloka);


}

void stan_rygiel(){
/*
	if (digitalRead(prz_rygiel) == LOW && (prz_rygiel_on == FALSE) )   { //Jeśli przycisk wciśnięty
				prz_rygiel_on = TRUE;}
	else { prz_rygiel_on = FALSE;
	}
*/
}

void action() {
/*
	if (prz_dzwonek_on) {
		dzwonek_reset.begin(5000);
		graj_dzwonek();
	}

	if (prz_rygiel_on) {
		//rygiel_reset.begin(6000);
		otwieranie_furtki();
	}
	//if (furtka_otwarta) {

		//furtka_otwarta_jest.begin(10000);

//	}

	if (rygiel_reset.available()) { // timer resetu stanu rygla
		prz_rygiel_on=0;
		rygiel_reset.begin(0);
		digitalWrite(pk_rygiel, LOW);
		digitalWrite(pk_otwieranie, LOW);
		furtka_otwarta_jest.begin(10000);

		}
	if (furtka_otwarta_jest.available()) {
		furtka_otwarta_jest.begin(0);
		zamykanie_furtki();
	}
	if (zamykanie.available()) {
		zamykanie.begin(0);
		digitalWrite(pk_zamykanie,LOW);
	}
	if (odlicz) {

		if (seconds>59) {
		seconds=0;      //whenever second is greater than 59 than second is made equal to
		minutes++;     // zero and a minute is counted for it
		}

}
*/
}

void graj_dzwonek() {
	/*

	// tutaj obsługa płytki mp3
	digitalWrite(pk_dzwonek, HIGH);
*/
}

void otwieranie_furtki() {
	// tutaj odgranie komunikatu
/*	prz_rygiel_on=FALSE;
	digitalWrite (pk_rygiel, HIGH);
	digitalWrite (pk_otwieranie, HIGH);
	rygiel_reset.begin(6000);
	furtka_otwarta=TRUE;
*/
	//
}

void zamykanie_furtki() {
/*	// tutaj odgranie komunikatu
		digitalWrite (pk_zamykanie, HIGH);
		zamykanie.begin(6000);
		furtka_otwarta=FALSE;
*/
}

void heartbeat(){
sekundy++;       //counts seconds from 0
if(sekundy>59) sekundy=0;
//s1_flag=1;

		int counts = analogRead(pk_brama1l_otwieranie_Amps )+1;
		float voltage = counts*vpp;
		voltage -=2.5;
		float amperage = voltage / sensitivity;
		Serial.println("AMPS:"+String(amperage));
		 Serial.print("Distance in CM: ");
		  // Pass INC as a parameter to get the distance in inches
		  Serial.println(ultrasonic.distanceRead());
}



