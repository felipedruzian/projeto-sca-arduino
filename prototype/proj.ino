#define Rele 7
#define Botao1 8
//#define Botao2 9
//#define Vazao 6

bool estadoRele = 1; //  1 = desligado / 0 = ligado  (negocio sem nexo da p***)
bool estadoLed = 0;

void setup()
{
    pinMode(Rele, OUTPUT);
    pinMode(Botao1, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    digitalWrite(Rele, HIGH); //inicializa com o rele desligado
}

void loop()
{
    if (digitalRead(Botao1) != 0) {
        while (digitalRead(Botao1) != 0) {
            delay(100); }

        estadoRele = !estadoRele;
        estadoLed = !estadoLed;

        Serial.print("Estado do Rele: ");
        Serial.println(estadoRele);
        Serial.print("Estado do Led: ");
        Serial.println(estadoLed);

        digitalWrite(Rele, estadoRele);
        digitalWrite(LED_BUILTIN, estadoLed);
    }
}