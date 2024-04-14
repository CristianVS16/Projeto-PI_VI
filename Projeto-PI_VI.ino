
// Projeto PI VI- Sistema de irrigação automático

// Inclusão de bibliotecasa necessárias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PCF8574.h>



#define PCF8574_ADDRESS 0x20 // endereço do PCF857 que controla os relés
#define RELAY_ON 1
#define RELAY_OFF 0

Adafruit_PCF8574 pcf;

// Definições dos pinos de controle do Mux
#define PinS0  D5
#define PinS1  D6
#define PinS2  D7
#define PinS3  D8
#define PinSIG A0

#define endereco 0x27 // Endereço I2C do LCD
#define colunas 16
#define linhas 2

LiquidCrystal_I2C lcd(endereco, colunas, linhas); // Instanciando o LCD
int mix[4] = {PinS0, PinS1, PinS2, PinS3};
// Array para armazenar os valores lidos dos sensores
int valores_analogicos[8];

// Valores dos pinos de controle do Mux para cada canal
const byte muxChannel[8][4] = {
  // S0, S1, S2, S3
  {0, 0, 0, 0}, // Canal 0
  {1, 0, 0, 0}, // Canal 1
  {0, 1, 0, 0}, // Canal 2
  {1, 1, 0, 0}, // Canal 3
  {0, 0, 1, 0}, // Canal 4
  {1, 0, 1, 0}, // Canal 5
  {0, 1, 1, 0}, // Canal 6
  {1, 1, 1, 0}  // Canal 7
   /* caso tenha mais sensores pode lê mais 8
  { 0 , 0 , 0 , 1 } ,  //canal 8 
  { 1 , 0 , 0 , 1 } ,  //canal 9 
  { 0 , 1 , 0 , 1 } ,  //canal 10 
  { 1 , 1 , 0 , 1 } ,  //canal 11 
  { 0 , 0 , 1 , 1 } ,  //canal 12 
  { 1 , 0 , 1 , 1 } ,  //canal 13 
  { 0 , 1 , 1 , 1 } ,  //canal 14 
  { 1 , 1 , 1 , 1 } ,  //canal 15 */
};

// Função para ler os valores dos sensores através do multiplexador
void multiplex() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 4; y++) {
      digitalWrite(mix[y], muxChannel[x][y]);
    }
    valores_analogicos[x] = analogRead(PinSIG);
    valores_analogicos[x] = map(valores_analogicos[x], 0, 1023, 0, 100);
    valores_analogicos[x] =(100-valores_analogicos[x]);
  }
}

void setup() {
  pinMode(PinS0, OUTPUT);
  pinMode(PinS1, OUTPUT);
  pinMode(PinS2, OUTPUT);
  pinMode(PinS3, OUTPUT);
  pinMode(PinSIG, INPUT);

  Wire.begin();

  Serial.begin(9600);

  if (!pcf.begin()) {
    Serial.println("Falha ao encontrar o PCF8574");
    while (1);
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Configura todos os pinos do PCF8574 como saída
  for (int i = 0; i < 8; i++) {
    pcf.pinMode(i, OUTPUT);
  }
}

void loop() {
 multiplex();

 

  for (int x = 0; x < 8; x++) {
    Serial.print("Sensor_");
    Serial.print(x);
    Serial.print(" = ");
    Serial.print(valores_analogicos[x]);
    Serial.println("%");
    
    // Envia dados para o Lcd
    lcd.setCursor(0, 0);
    lcd.print("Sensor_");
    lcd.print(x);
    lcd.print("=");
    lcd.print(" ");
    lcd.setCursor(0,10);
    lcd.print(valores_analogicos[x]);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Rele_");
    lcd.print(x);
    lcd.print(":");
    lcd.print(" ");
    // Controla o relé baseado na leitura do sensor
    if (valores_analogicos[x] < 45) {
      pcf.digitalWrite(x, RELAY_ON);// liga o relé com low
      lcd.print("L"); // LIGADO para Ligado
    } else if (valores_analogicos[x] >= 60) {
      pcf.digitalWrite(x, RELAY_OFF);//Desliga o relé com High
      lcd.print("D"); //DESLIGADO para Desligado
    }

    delay(1000);
  }
}