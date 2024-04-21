
// Projeto PI VI- Sistema de irrigação automático

// Inclusão de bibliotecasa necessárias

#define BLYNK_TEMPLATE_ID "TMPL2Kf-JDuOP"
#define BLYNK_TEMPLATE_NAME "Projeto Irrigação PI VI"
#define BLYNK_AUTH_TOKEN "5nDyapuPWlrwe_nwfYg7kkqX9_tGIHSS"
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>
#include <Wire.h>
#include <Adafruit_PCF8574.h>
#include <Blynk.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;  //Insira seu Blynk Auth token
char ssid[] = "Yasmin";  //Insira seu WIFI SSID
char pass[] = "248905822";  //insira seu WIFI Password

BlynkTimer timer;

#define PCF8574_ADDRESS 0x20 // endereço do PCF857 que controla os relés
#define RELAY_ON 1
#define RELAY_OFF 0

Adafruit_PCF8574 pcf;
DHT11 dht11(2);

// Definições dos pinos de controle do Mux
#define PinS0  D5
#define PinS1  D6
#define PinS2  D7
#define PinS3  D8
#define PinSIG A0

/*#define endereco 0x27 // Endereço I2C do LCD
#define colunas 16
#define linhas 2*/

LiquidCrystal_I2C lcd(0x27, 16, 2); // Instanciando o LCD
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
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  Blynk.begin(auth, ssid, pass);
  
  if (!pcf.begin()) {
    Serial.println("Falha ao encontrar o PCF8574");
    while (1);
  }

  

  // Configura todos os pinos do PCF8574 como saída
  for (int i = 0; i < 8; i++) {
    pcf.pinMode(i, OUTPUT);
  }
}

void loop() {

Blynk.run();
timer.run();
  
   multiplex();

   for (int x = 0; x < 8; x++) {
    Serial.print("Sensor_");
    Serial.print(x);
    Serial.print(" = ");
    Serial.print(valores_analogicos[x]);
    Serial.println("%");
    Blynk.virtualWrite(V0,valores_analogicos[0]);
    Blynk.virtualWrite(V1,valores_analogicos[1]);
    Blynk.virtualWrite(V2,valores_analogicos[2]);
    Blynk.virtualWrite(V3,valores_analogicos[3]);
    Blynk.virtualWrite(V4,valores_analogicos[4]);
    Blynk.virtualWrite(V5,valores_analogicos[5]);
    Blynk.virtualWrite(V6,valores_analogicos[6]);
    Blynk.virtualWrite(V7,valores_analogicos[7]);
    // Envia dados para o Lcd
    lcd.setCursor(0, 0);
    lcd.print("Sensor_");
    lcd.setCursor(7, 0);
    lcd.print(x);
    lcd.setCursor(8, 0);
    lcd.print("=");
    lcd.setCursor(9, 0);
    lcd.print(" ");
    lcd.setCursor(10,0);
    lcd.print(valores_analogicos[x]);
    lcd.setCursor(12, 0);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Rele_");
    lcd.setCursor(5, 1);
    lcd.print(x);
    lcd.setCursor(6, 1);
    lcd.print(":");
        
    // Controla o relé baseado na leitura do sensor
    if (valores_analogicos[x] < 40) {
      pcf.digitalWrite(x, RELAY_ON);// liga o relé com low
      lcd.setCursor(7, 1);
      lcd.print("  Ligado "); // LIGADO para Ligado
    } else if (valores_analogicos[x] >= 60) {
      pcf.digitalWrite(x, RELAY_OFF);//Desliga o relé com High
      lcd.setCursor(7, 1);
      lcd.print("Desligado"); //DESLIGADO para Desligado
    }
     int temperature, humidity;

    // Leitura dos valores de temperatura e umidade do sensorDHT1
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Checa dados da leitura do DHT11.
    // Se a leitura for bem sucedida,omprim temperatura e umidade, valores no formato cvs
    if (result == 0) {
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print(",Humidity:");
        Serial.println(humidity);
        Blynk.virtualWrite(V8,temperature);
        Blynk.virtualWrite(V9,humidity);

    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
    
    
    delay(1000);
  }
}