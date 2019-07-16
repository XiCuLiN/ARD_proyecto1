// by Franky (xiculin@gmail.com)

// Para el RTC DS3231 (0x57 0x68)
// SCL -> A5
// SDA -> A4
// Wire.h
// RTClib.h

// Para el LCD (0x27)
// SCL -> A5
// SDA -> A4
// Wire.h
// LCD.h
// LiquidCrystal_I2C.h

// Para el DHT11 
// I/O -> 3
// DHT11.h

// Para los DIGitos
// Din/MISO -> 12
// Sck/Clock -> 11
// CS -> 10
// Dout/MOSI -> No utilizamos


// <DHT> Libreria 
#include <DHT11.h>

// <RTC> Librería 
#include <Wire.h>
#include "RTClib.h"

// <LCD> Librerias 
// Wire.h (ya cargado antes)  
#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>

// <DIG> Librerias
#include "LedControl.h"

// <DHT> Configuraciones iniciales
// Indicamos el pin donde conectamos el dht
int pin = 3; 
// indicamos a la libreria donde está conectado
DHT11 dht11(pin);
// Generamos en modo binario el simbolo de grados (º)
byte grado[8] =
{
  0b00001100,     // Los definimos como binarios 0bxxxxxxx
  0b00010010,
  0b00010010,
  0b00001100,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};

// <LCD> Configuraciones iniciales

#define LCD_1    0x27 //definimos su dirección
LiquidCrystal_I2C             lcd(LCD_1, 2, 1, 0, 4, 5, 6, 7); // lcd(dirección i2c, el resto son pines del modulo i2c)
char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"}; //personalizamos los dias de la semana

//  <RTC> Configuraciones inciales
RTC_DS3231 rtc; //Declaramos un RTC DS3231

// <DIG> Configuraciones inciales
LedControl lc = LedControl(12, 11, 10, 1);  // LedControl(Din, Clock, CS, Nº displays)


void setup () {
  Serial.begin(9600); // Para ver info en consola serial
  // <DIG> Iniciamos, ajustamos brillo, limpiamos datos
  lc.shutdown(0, false);
  lc.setIntensity(0, 3);
  lc.clearDisplay(0);
  // <RTC> declaramos pines para estado del RTC
  pinMode( 6, OUTPUT) ; // Led ROJO RTC
  pinMode( 7, OUTPUT) ; // Led VERDE RTC
  rtc.begin(); // Iniciamos rtc

  // <RTC> Ponemos en hora, solo la primera vez, luego comentar y volver a cargar.
  // <RTC> Ponemos en hora con los valores de la fecha y la hora en que el sketch ha sido compilado.
  // <RTC> rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // <LCD>
  lcd.begin (16, 2);   // Inicializar el display con 16 caraceres 2 lineas
  lcd.setBacklightPin(3, POSITIVE); // La luz de fondo 
  lcd.setBacklight(HIGH); // La encendemos
  lcd.createChar(1, grado); // creamos el caracter "grado" con el numero 1
}

void loop () {
  // <DHT> 
  float temp, hum;  //cargamos datos temperatura y humedad
  // <RTC>
  DateTime now = rtc.now(); // <RTC> leemos la fecha/hora
  //  Serial.print(now.day());
  //  Serial.print('/');
  //  Serial.print(now.month());
  //  Serial.print('/');
  //  Serial.print(now.year());
  //  Serial.print(" ");
  //  Serial.print(now.hour());
  //  Serial.print(':');
  //  Serial.print(now.minute());
  //  Serial.print(':');
  //  Serial.print(now.second());
  //  Serial.println();
  if (now.day() > 32) { // <RTC> Si hay error en lectura, el "dia" es superior a 32
   // Serial.println("No hay un módulo RTC");
    digitalWrite( 7 , LOW) ; // <RTC> si falla el rtc, apagamos la verde
    digitalWrite( 6 , HIGH) ; // <RTC> y encendemos la roja
    lcd.home (); // <LCD> Posicion de incio
    lcd.print("ERROR de RTC    "); // <LCD> Mensaje de error, los espacios para tapar posibles caracteres
    //    lcd.setCursor ( 0, 1 );        // go to the 2nd line
    //  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    //    lcd.print("  <Sin datos>   ");
  }
  if ( now.day() < 32 ) { // <RTC> SI no hay error...
    //    Serial.println("Módulo RTC correcto");
    digitalWrite( 6 , LOW) ; // <RTC> Apagamos el rojo
    digitalWrite( 7 , HIGH) ; // <RTC> Encendemos el verde
    lcd.home ();  // <LCD> Posicion de incio
    plcd(now.day()); // <LCD> mostramos el dia, el "plcd" nos pone en formato de 2 numeros
    lcd.print("/"); // <LCD> separador
    plcd(now.month()); // <LCD> mostramos el mes... con dos digitos siempre
    lcd.print("/"); // <LCD> separador
    lcd.print(now.year()); // <LCD> mostramos el año... con dos digitos
    lcd.print(" "); // <LCD> separamos con 1 espacio
    //    lcd.setCursor ( 0, 1 );        // go to the 2nd line
    //  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    //    lcd.print("    ");
    plcd(now.hour()); // <LCD> mostramos la hora, también en dos digitos
    if (now.second() % 2 == 0) { // Condicion para saber si el segundo es par o impar. Para hacer un parpadeo de los dos puntos y saber que va bien ya que no vemos los segundos
      lcd.print(" "); // <LCD> si es par, ponemos un espacio
    }
    else {
      lcd.print(":"); // <LCD> si es impar, ponemos los dos puntos
    }
    plcd(now.minute()); // <LCD> mostramos los minutos, también con dos digitos
    //    lcd.print(":");
    //    plcd(now.second());
    //    lcd.print("    ");
  }
  // lc.setChar(0, 1, 'now.day', false);

  lcd.setCursor ( 0, 1 );  // <LCD> nos ponemos al principio de la segunda linea
  int err; // <RTC> comprobamos si hay error en lectura
  if ((err = dht11.read(hum, temp)) == 0) // <RTC> si no hay error....
  {
    lcd.print(temp, 0); // <LCD> mostramos la temperatura, el ,0 indica que no queremos decimales
    lcd.print(" "); // <LCD> un espacio...
    lcd.write(1); // <LCD> mostramos el caracter creado numero 1, el simbolo de grados (º)
    lcd.write("C "); // <LCD> mostramos C y un espacio
    lcd.print(hum, 0); // <LCD> mostramos la humedad
    lcd.print(" %HR "); // <LCD> mostramos que es el % de Humedad Relativa
  }
  else
  {
    lcd.print("temp/hum no data"); // <LCD> Si hay error en lectura, mostramos texto
  }

  delay(1000); // repetimos el loop cada 1 segundo
}

// para mostrar 2 digitos en vez de 1
void plcd(int digits)
{ if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

