#include <LiquidCrystal.h>
#include <RTClib.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //pinos RS, E, D4, D5, D6, D7
RTC_DS3231 rtc;

#define Rele 9
#define Botao1 8
//#define Vazao 10

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
bool estadoRele = 1; //  1 = desligado / 0 = ligado  (negocio sem nexo da p***)
bool estadoLed = 0;

void setup()
{
    pinMode(Rele, OUTPUT); //define o pino do Rele e do Led como OUTPUT (Saida de dados)
    pinMode(LED_BUILTIN, OUTPUT); 

    pinMode(Botao1, INPUT_PULLUP); //define o pino do Botao como INPUT (Entrada de dados)

    Serial.begin(9600); //inicializa comunicação serial
    lcd.begin(16, 2);

    if (!rtc.begin()) //se o rtc n inicializar ele printa no display q teve um erro
    {
        lcd.print("RTC Error");
        while (1) delay(10); //loop para o programa se manter aqui caso de o erro
    }
    

    if (rtc.lostPower()) //usado apenas pra configurar o horario
    {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Configura o relogio pra hora que o sketch foi compilado
    }
    

    digitalWrite(Rele, HIGH); //inicializa com o rele desligado
}

void loop()
{
    DateTime now = rtc.now(); //cria uma variavel do tipo DateTime (data) que vai receber a data/horario do momento
    
    lcd.setCursor(0,0);         //prints no display
    lcd.print("Estado Rele: ");
    if (estadoRele == 0)
        {
            lcd.print("ON");
        } else {
            lcd.print("OFF");
        }
    lcd.setCursor(0,1);
    lcd.print(now.day(), DEC);
    lcd.print("/");
    lcd.print(now.month(), DEC);
    lcd.print(" ");
    lcd.print(now.hour(), DEC);
    lcd.print(":");
    lcd.print(now.minute(), DEC);
    lcd.print(":");
    lcd.print(now.second(), DEC);
    
    if (digitalRead(Botao1) != 1) {
        delay(400);

        estadoRele = !estadoRele; //inverte os estados do Rele/Led
        estadoLed = !estadoLed;

        lcd.clear();
        lcd.setCursor(0,14); //muda o apenas estado do rele no display
        if (estadoRele == 0)
        {
            lcd.print("ON"); //0 == LOW == Rele Ligado (pq sim, rele maluco)
        } else {
            lcd.print("OFF");
        }
    } 
    digitalWrite(Rele, estadoRele); //muda o estado do Rele/Led
    digitalWrite(LED_BUILTIN, estadoLed);
}