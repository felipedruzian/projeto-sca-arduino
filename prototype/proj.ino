/**
 * @file proj.ino
 * @author Felipe Barreto Druzian
 * @brief 
 * @version 0.5
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022 Felipe
 * 
 */
#include <LiquidCrystal.h>
#include <RTClib.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4); //pinos RS, E, D4, D5, D6, D7 do display
RTC_DS3231 rtc;

#define Botao1 10 //pinos dos botoes
#define Botao2 11
#define Botao3 12
#define Rele A3 //pino do rele que liga a valvula solenoide
//o rele no A3 pq no 13 ele estava bugando (provavel conflito com o led acoplado no arduino) 
//(A3 pq o 3 era o outro pino de interrupcao)
bool estadoRele = 1; //  1 = desligado / 0 = ligado  (negocio sem nexo da p***)

#define rtcInterrupt 3 //pino com funcao de interrupcao (pino SQW do RTC)

#define vazaoInterrupt 2 //pino com funcao de interrupcao
const float  calibracao = 6.41; //calibrado com regra de 3
float volume = 0;
float volumeTotal = 0;
float volumeLimite = 0; //em Litros
float limite = 1.0; //limite em Litros acima do volumeTotal ja registrado
volatile int contador; //variavel que incrementa durante a funcao de interrupcao

unsigned long tempoIni = 0; //variavel auxiliar para calculo com millis()
const unsigned long periodo = 500; //periodo de tempo entre cada interrupcao

int opc = 0; //variavel para o switch

void setup()
{
    pinMode(Rele, OUTPUT); //define o pino do Rele como OUTPUT (Saida de dados)
    digitalWrite(Rele, HIGH); //inicializa com o rele desligado
    pinMode(Botao1, INPUT_PULLUP); //define os pinos dos Botoes como INPUT (Entrada de dados) PULLUP (com resistor interno)
    pinMode(Botao2, INPUT_PULLUP); 
    pinMode(Botao3, INPUT_PULLUP);

    pinMode(vazaoInterrupt, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(vazaoInterrupt), contadorPulso, RISING); 
    //funcao q define o pino de interrupcao, funcao e o modo

    pinMode(rtcInterrupt, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(rtcInterrupt), Alarme, FALLING);

    //Serial.begin(9600); //inicializa comunicação serial
    lcd.begin(16, 2); //inicializa o Display LCD 16x2

    if (!rtc.begin()) //se o RTC n inicializar ele printa no display q teve um erro
    {
        lcd.print("RTC Error");
        while (1) delay(10); //loop para o programa se manter aqui caso de o erro
    }
    
    if (rtc.lostPower()) //se o RTC perder energia ou for a primeira vez ligado
    {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Configura o relogio pra hora que o sketch foi compilado
    }
    rtc.disable32K(); //desabilita o pino 32K do RTC 
    rtc.clearAlarm(1); //limpa o status do Alarme 1
    //rtc.disableAlarm(1);
    rtc.disableAlarm(2); //desabilita o Alarme 2
    rtc.writeSqwPinMode(DS3231_OFF); //interrompe a oscilacao no sinal do pino SQW

}

void loop()
{
    DateTime now = rtc.now(); //cria uma variavel do tipo DateTime (data) que vai receber a data/horario do momento
    
    //prints no display
    lcd.setCursor(0,0);
    lcd.print("Rele: ");
    if (estadoRele == 0)
        {
            lcd.print("ON");
        } else {
            lcd.print("OFF");
        }
    lcd.setCursor(0,1);
    char data[10] = "DD/MM";
    rtc.now().toString(data);
    lcd.print(data);
    char hora[10] = "hh:mm:ss";
    rtc.now().toString(hora);
    lcd.print(" ");
    lcd.print(hora);

    if (digitalRead(Botao1) != 1) {
        delay(400); //um delay para ser mais natural o aperto do botao
        opc = 1; //vai para o case 1
    } else if (digitalRead(Botao2) != 1) {
        delay(400);
        opc = 2;
    } else if (digitalRead(Botao3) != 1) {
        delay(400);
        rtc.clearAlarm(1); //limpa e desabilita o alarme 1 anterior para definir um novo
        rtc.disableAlarm(1);
        rtc.setAlarm1(rtc.now() + TimeSpan(10), DS3231_A1_Second);
        // define o horario do alarme para 10 segundos no futuro
        // DS3231_A1_Second = liga o alarme toda vez que os segundos coincidirem
        lcd.clear();//limpa o display
        lcd.setCursor(0,1);
        lcd.print("Alarme em 10s");
        delay(1000);
        opc = 0; //segue sem ir pro switch e continua no void loop
    }
    if (digitalRead(rtcInterrupt) == 0) { //quando ocorre a interrupcao o pino passa para LOW
        opc = 3;
    }

    switch (opc)
    {
        case 1:
            estadoRele = !estadoRele; //inverte os estados do Rele

            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print("Rele: ");
            if (estadoRele == 0)
            {
                lcd.print("ON"); //0 == LOW == Rele Ligado (pq sim, rele maluco)
            } else {
                lcd.print("OFF");
            }
            digitalWrite(Rele, estadoRele); //muda o estado do Rele/Led
            

            while (estadoRele == 0) //enquanto o rele estiver ligado
            {
                if (millis() - tempoIni >= periodo) //vai repetir esse if a cada periodo(ms) de tempo
                {
                    noInterrupts(); //desabilita a Interrupcao para fazer os calculos

                    volume = (((periodo / (millis() - tempoIni)) * contador) / calibracao)/60; // (fluxo)/60 
                    volumeTotal += volume;
                    
                    lcd.setCursor(0,1);
                    lcd.print("Volume: ");
                    lcd.print(volumeTotal);
                    lcd.print(" L");
                    
                    contador = 0;
                    tempoIni = millis();
                    interrupts(); //habilita a Interrupcao para medir os pulsos
                }

                if (digitalRead(Botao1) != 1) { //se o botao for pressionado novamente
                    delay(400);
                    estadoRele = !estadoRele; //desliga o rele e sai do loop do while e do case 1
                    digitalWrite(Rele, estadoRele);
                }
            }
            lcd.clear(); //limpa o display para n ficar nenhum residuo ou letra perdida/bugada nele
            opc = 0; //reseta a opcao do case
        break;

        case 2:
            volumeLimite = volumeTotal + limite;

            estadoRele = !estadoRele; //inverte os estados do Rele

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Rele: ");
            lcd.print(volumeLimite); //mostra o volume limite no display
            lcd.print(" L MAX");

            digitalWrite(Rele, estadoRele); //muda o estado do Rele
            
            while (volumeTotal < volumeLimite && estadoRele == 0)
            { //enquanto volume limite nao for atingido OU o rele estiver ligado
                if (millis() - tempoIni >= periodo) 
                {
                    noInterrupts(); //desabilita a Interrupcao para fazer os calculos

                    volume = (((periodo / (millis() - tempoIni)) * contador) / calibracao)/60; // (fluxo)/60 
                    volumeTotal += volume;
                
                    lcd.setCursor(0,1);
                    lcd.print("Volume: ");
                    lcd.print(volumeTotal);
                    lcd.print(" L");
                
                    contador = 0;
                    tempoIni = millis();
                    interrupts(); //habilita a Interrupcao para medir os pulsos
                }
                if (digitalRead(Botao2) != 1) { //se o botao for pressionado dnv.. sai do case 2
                    delay(400);
                    estadoRele = !estadoRele;
                    digitalWrite(Rele, estadoRele);
                }
            }
            //se ele saiu do while por ter atingido o vol limite
            if (estadoRele == 0) {
                estadoRele = !estadoRele; //desliga o rele
                digitalWrite(Rele, estadoRele);
            }
            lcd.clear();
            opc = 0;
        break;

        case 3:
            volumeLimite = volumeTotal + limite;

            estadoRele = !estadoRele; //inverte os estados do Rele

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Rele: ");
            lcd.print(volumeLimite);
            lcd.print(" L MAX");

            digitalWrite(Rele, estadoRele);

            while (volumeTotal < volumeLimite && estadoRele == 0) 
            {
                if (millis() - tempoIni >= periodo) 
                {
                    noInterrupts(); //desabilita a Interrupcao para fazer os calculos

                    volume = (((periodo / (millis() - tempoIni)) * contador) / calibracao)/60; // (fluxo)/60 
                    volumeTotal += volume;
                
                    lcd.setCursor(0,1);
                    lcd.print("Volume: ");
                    lcd.print(volumeTotal);
                    lcd.print(" L");
                
                    contador = 0;
                    tempoIni = millis();
                    interrupts(); //habilita a Interrupcao para medir os pulsos
                }
                if (digitalRead(Botao3) != 1) {
                    delay(400);
                    estadoRele = !estadoRele;
                    digitalWrite(Rele, estadoRele);
                }
            }
            
            if (estadoRele == 0) {
                estadoRele = !estadoRele;
                digitalWrite(Rele, estadoRele);
            }
            lcd.clear();
            rtc.clearAlarm(1); //limpa o status do alarme
            rtc.disableAlarm(1); //desabilita o alarme, se nao sempre que os segundos coincidirem
            //por causa do modo do alarme, ele toca o alarme denovo
            opc = 0;
        break;

        default: // qualquer opc != de 1, 2 ou 3, nao vai acontecer nada e volta para o void loop
        break;
    }
}

void contadorPulso() { //funcao que conta os pulsos do sensor de vazao
    contador++;
}

void Alarme() {
    //nao precisa ter nada aqui
}