#include <LiquidCrystal.h>
#include <RTClib.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4); //pinos RS, E, D4, D5, D6, D7
RTC_DS3231 rtc;

#define Rele 12
#define Botao1 11

#define Vazao 2
const float  calibracao = 6.41; //calibrado com regra de 3
float fluxo = 0;
float volume = 0;
float volumeTotal = 0;
volatile int contador;

unsigned long tempoIni = 0;
//unsigned long tempoAtu;
const unsigned long periodo = 500;

//char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
bool estadoRele = 1; //  1 = desligado / 0 = ligado  (negocio sem nexo da p***)
bool estadoLed = 0;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT); 
    pinMode(Rele, OUTPUT); //define o pino do Rele e do Led como OUTPUT (Saida de dados)
    digitalWrite(Rele, HIGH); //inicializa com o rele desligado
    pinMode(Botao1, INPUT_PULLUP); //define o pino do Botao como INPUT (Entrada de dados)

    pinMode(Vazao, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(2), contadorPulso, RISING); //funcao q define o pino de interrupcao

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
}

void loop()
{
    DateTime now = rtc.now(); //cria uma variavel do tipo DateTime (data) que vai receber a data/horario do momento
    
    Serial.println("xesque");
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
        lcd.setCursor(0,0); //muda o apenas estado do rele no display
        lcd.print("Estado Rele: ");
        if (estadoRele == 0)
        {
            lcd.print("ON"); //0 == LOW == Rele Ligado (pq sim, rele maluco)
        } else {
            lcd.print("OFF");
        }
        digitalWrite(Rele, estadoRele); //muda o estado do Rele/Led
        digitalWrite(LED_BUILTIN, estadoLed);
    } 

    while (estadoRele == 0) //enquanto o rele estiver ligado
    {
        if (millis() - tempoIni >= periodo) //vai repetir esse if a cada periodo(ms) de tempo
        {
            noInterrupts(); //desabilita a Interrupcao para fazer os calculos

            fluxo = ((periodo / (millis() - tempoIni)) * contador) / calibracao;
            volume = fluxo / 60;
            volumeTotal += volume;
            
            lcd.setCursor(0,1);
            lcd.print("Volume: ");
            lcd.print(volumeTotal);
            lcd.print(" L");
            
            contador = 0;
            tempoIni = millis();
            interrupts(); //habilita a Interrupcao para medir os pulsos
        }

        if (digitalRead(Botao1) != 1) {
            delay(400);
            estadoRele = !estadoRele;
            digitalWrite(Rele, estadoRele);
            lcd.clear();
        }
    }
}

void contadorPulso() { //funcao que conta os pulsos do sensor de vazao
    contador++;
}