#include <Wire.h>

#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>
#include <Keypad.h>


/* Funcion de configuracion de pines del modulo LCD/I2C 
    (Direccion,en,rw,rs,d4,d5,d6,d7,backlight,polaridad)*/
 LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const byte Filas = 4;  //Cuatro filas
const byte Cols = 4;   //Cuatro columnas
byte Pins_Filas[] = {9,8,7,6};//Pines Arduino a las filasbyte 
byte Pins_Cols[] = {5,4,3,2}; // Pines Arduino a las columnas.
//no utilizar los pines 1 y 0 para no interferir en Rx y Tx

char Teclas [ Filas ][ Cols ] =
 {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
 };

char codigoSecreto[4] = {'1','2','3','4'}; // Aqui va el codigo secreto
// Para cambiar el tamaño de la clave, solo hay que cambiar el tamaño del array

int posicion=0;    // necesaria para la clave
int cursor=5;      // posicion inicial de la clave en el LCD
int clave=0;       // para el LCD
int luz=0;         // para el LCD
int tiempo=0;      // para el LCD
int ledVerde=11;   // pin para el LED verde
int ledRojo=12;    // pin para el LED rojo
int buzzer=10;     // pin altavoz

Keypad Teclado1 = Keypad(makeKeymap(Teclas), Pins_Filas, Pins_Cols, Filas, Cols);

void setup()
   {
      Serial.begin(9600) ;
      lcd.begin(16,2);       // inicializamos el LCD.
      lcd.noBacklight();     // apagamos LCD
      pinMode (ledVerde,OUTPUT);
      pinMode (ledRojo, OUTPUT);
      pinMode (buzzer, OUTPUT);
      digitalWrite(ledRojo,HIGH); // encendemos el LED rojo
      digitalWrite(ledVerde, LOW); // apagamos el verde

      lcd.setCursor(0,0);     // situamos el cursor el la posición 2 de la linea 0.
      lcd.print("Introduzca clave"); // escribimos en LCD
      lcd.setCursor(cursor,1); // cursor en la posicion de la variable, linea 1
    }

void loop()
   {
    int mode = 0;
      char pulsacion = Teclado1.getKey() ; // leemos pulsacion
      if (pulsacion != 0) //Si el valor es 0 es que no se ha pulsado ninguna tecla
        { // descartamos almohadilla y asterisco
          if (pulsacion != '#' && pulsacion != '*' && clave==0)
           { lcd.print(pulsacion); // imprimimos pulsacion
             cursor++;             // incrementamos el cursor
             tone(buzzer,350);     // tono de pulsacion
             delay(200);
             noTone(buzzer);

      //--- Condicionales para comprobar la clave introducida -----------
      // comparamos entrada con cada uno de los digitos, uno a uno
      if (pulsacion == codigoSecreto[posicion] && mode == 0)
          posicion ++; // aumentamos posicion si es correcto el digito

      if (posicion == 4 && mode == 0)
       { // comprobamos que se han introducido los 4 correctamente
         digitalWrite (13,HIGH);  // encendemos LED
         lcd.setCursor(0,0);      // situamos el cursor el la pos 0 de la linea 0.
         lcd.print("Clave correcta ");         // escribimos en LCD
         delay(200);                           // tono de clave correcta
         tone(buzzer,500);
         delay(100);
         noTone(buzzer);
         tone(buzzer,600);
         delay(100);
         noTone(buzzer);
         tone(buzzer,800);
         delay(100);
         noTone(buzzer);

         lcd.setCursor(5,1); // cursor en la posicion 5, linea 1
         clave=1; // indicamos que se ha introducido la clave
         digitalWrite(ledRojo,LOW); // apagamos el LED rojo
         digitalWrite(ledVerde, HIGH); // encendemos el verde
     }
     //--- En el caso de que este incompleta o no hayamos acertado ----------
     if(cursor>8)        // comprobamos que no pase de la cuarta posicion
       {   cursor=5;     // lo volvemos a colocar al inicio
           posicion=0;           // borramos clave introducida
           lcd.setCursor(5,1);
           lcd.print(" ");       // borramos la clave de la pantalla
           lcd.setCursor(5,1);
           if(clave==0)         // comprobamos que no hemos acertado
              { tone(buzzer,70,500); // para generar
                delay(250); // tono de error
                noTone(buzzer);
              }
        }
     }
   }

 //--- Condicionales para encender o apagar el LCD --------------
 if (pulsacion == '#' && luz==0)
     { // comprobamos tecla y encendemos si esta apagado
       lcd.backlight(); // encendemos
       luz=1; // indicamos que esta encendida
       pulsacion =0; // borramos el valor para poder leer el siguiente condicional
     }

 if (pulsacion == '#' && luz==1)
     { // comprobamos tecla y estado
       lcd.noBacklight(); // apagamos
       luz=0; // indicamos que esta apagada
     }

 //--- Condicionales para resetear clave introducida -------------
 if (pulsacion == '*')
     { // asterisco para resetear el contador
       posicion = 0;
       cursor = 5;
       clave=0;
       posicion=0;
       lcd.setCursor(0,0); // situamos el cursor el la posición 2 de la linea 0.
       lcd.print("Introduzca clave"); // escribimos en LCD
       lcd.setCursor(5,1);
       lcd.print(" "); // borramos de la pantalla los numeros
       lcd.setCursor(5,1);
       mode = 0;
    
       digitalWrite(ledRojo,HIGH); // encendemos el LED rojo
       digitalWrite(ledVerde, LOW); // apagamos el verde
    }

     if (pulsacion == 'D')
     { // asterisco para resetear el contador
       lcd.clear();
       lcd.setCursor(0,0); // situamos el cursor el la posición 2 de la linea 0.
       lcd.print("Cambiar contraseña"); // escribimos en LCD
       mode = 1;
       int counter = 0;
       int indx = 0;
      while (indx <= 3){
        char pulsacion2 = Teclado1.getKey();
        if (pulsacion2 != 0){
          codigoSecreto[indx] = pulsacion2;
          tone(buzzer,500);
          delay(250); // tono de error
          noTone(buzzer);
          indx = indx + 1;
        }
        
      }
       lcd.clear();
       lcd.setCursor(0,0); // situamos el cursor el la posición 2 de la linea 0.
       lcd.print("Cambiado"); // escribimos en LCD
        } 
      
    
 }
 
void loop()
{
  char customKey = customKeypad.getKey(); //se guarda en la variable customKey el caracter de la tecla presionada
  if (customKey != NO_KEY)         //se evalúa si se presionó una tecla
  {

    }
  }
}
