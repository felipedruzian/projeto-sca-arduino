# Protótipo do Simulador de Chuva Automatizado

## Sobre o Protótipo

### Hardware
+ Placa: Arduino Uno
+ Componentes: 
    + Relé
    + Valvula solenoide 12v 1/2"
    + 3 push buttons
    + 1 potenciometro
    + Display LCD 16x2
    + RTC DS3231
    + Sensor de vazão YF-S201 1/2"
 
Mais informções sobre a montagem em <a href="https://raw.githubusercontent.com/felipedruzian/projeto-sca-arduino/main/prototype/prototype_v0.5.png"> prototype_v0.5.png </a>

### Bibliotecas
+ <a href="https://github.com/adafruit/RTClib">RTCLib</a>
+ <a href="https://www.arduino.cc/en/Reference/LiquidCrystal">LiquidCrystal</a>

### Versões

+ Protótipo 0.5 (Finalizado)
    + Adicionado a funcionalidade de "alarme" do RTC para ele ligar a valvula sozinho no horario definido+
    + <a href="https://youtu.be/oR9g_SLRXh8">Vídeo</a>
+ Protótipo 0.45
    + Adicionado a função de fechar a valvula solenoide depois de X quantidade de agua
+ Protótipo 0.4
    + Adicionado a integração com o o sensor de vazão.
+ Protótipo 0.3-
    + Configurações básicas do Relé para valvula solenoide, botões, display lcd, rtc, etc...
