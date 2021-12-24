#define Rele 7
#define Botao1 8
//#define Botao2 9
//#define Vazao 6

bool estadoRele = 1; //  1 = desligado / 0 = ligado  (negocio sem nexo da p***)
bool estadoLed = 0;

void setup()
{
    pinMode(Rele, OUTPUT); //define o pino do Rele e do Led como OUTPUT (Saida de dados)
    pinMode(LED_BUILTIN, OUTPUT); 

    pinMode(Botao1, INPUT_PULLUP); //define o pino do Botao como INPUT (Entrada de dados)

    Serial.begin(9600); //inicializa comunicação serial

    digitalWrite(Rele, HIGH); //inicializa com o rele desligado
}

void loop()
{
    if (digitalRead(Botao1) != 0) {
        while (digitalRead(Botao1) != 0) { //while pra nao loopar infinitamente
            delay(100); }

        estadoRele = !estadoRele; //inverte os estados do Rele/Led
        estadoLed = !estadoLed;

        Serial.print("Estado do Rele: "); //printa no monitor serial o estado do Rele/Led
        Serial.println(estadoRele);
        Serial.print("Estado do Led: ");
        Serial.println(estadoLed);

        digitalWrite(Rele, estadoRele); //muda o estado do Rele/Led
        digitalWrite(LED_BUILTIN, estadoLed);
    }
}