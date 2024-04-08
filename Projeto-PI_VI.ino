//projeto Irrigação PI_V

// Bibliotecas

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // biblioteca para o cristal líquido

//Pinos de controle Mux 
#define PinS0  D5
#define PinS1  D6
#define PinS2  D7
#define PinS3  D8
#define PinSIG A0
#define endereco 0x27 //Endereços comuns : 0x27, 0x3F
#define colunas 16
#define linhas 2

LiquidCrystal_I2C lcd(endereco, colunas, linhas); //Instanciando o Lcd
int mix[4] = {PinS0, PinS1, PinS2, PinS3};

// variáveis globais
int umidade;

int valores_analogicos[16];

// valores em binário

  byte muxChannel [ 16 ] [ 4 ] = { 
    //S0, S1, S2, S3    Canais
    { 0 , 0 , 0 , 0 } ,  //canal 0 
    { 1 , 0 , 0 , 0 } ,  //canal 1 
    { 0 , 1 , 0 , 0 } ,  // canal 2 
    { 1 , 1 , 0 , 0 } ,  //canal 3 
    { 0 , 0 , 1 , 0 } ,  //canal 4 
    { 1 , 0 , 1 , 0 } ,  //canal 5 
    { 0 , 1 , 1 , 0 } ,  //canal 6 
    { 1 , 1 , 1 , 0 } ,  //canal 7 
    { 0 , 0 , 0 , 1 } ,  //canal 8 
    { 1 , 0 , 0 , 1 } ,  //canal 9 
    { 0 , 1 , 0 , 1 } ,  //canal 10 
    { 1 , 1 , 0 , 1 } ,  //canal 11 
    { 0 , 0 , 1 , 1 } ,  //canal 12 
    { 1 , 0 , 1 , 1 } ,  //canal 13 
    { 0 , 1 , 1 , 1 } ,  //canal 14 
    { 1 , 1 , 1 , 1 } ,  //canal 15 
  } ;

// Função de leitura

void multiplex(){

  for (int x=0; x<=2; x++ ){

    for(int y=0; y<=3; y++){
      digitalWrite (mix[y], muxChannel[x][y]);
    }
  valores_analogicos[x] = analogRead(PinSIG);
  valores_analogicos[x] =map(valores_analogicos[x], 0, 1023, 0, 100);
  valores_analogicos[x]=(100-valores_analogicos[x]);
  }
}
void setup ( ) { 
  pinMode ( PinS0,  OUTPUT );  
  pinMode ( PinS1,  OUTPUT );  
  pinMode ( PinS2,  OUTPUT );  
  pinMode ( PinS3,  OUTPUT );
  pinMode ( PinSIG, INPUT );

  digitalWrite ( PinS0 ,  LOW ); 
  digitalWrite ( PinS1 ,  LOW ); 
  digitalWrite ( PinS2 ,  LOW ); 
  digitalWrite ( PinS3 ,  LOW );

  
  Wire.begin(); 
  
  Serial.begin (9600);   // inicia comunicação Serial
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY

  
}


void loop ( ) {

multiplex();

  for (int x = 0; x <= 5; x++) {

Serial.print("canal");
Serial.print(" ");
Serial.print(x);
Serial.print ("= ");
Serial.print(valores_analogicos[x]);
Serial.print("%");
Serial.println("");
delay(1000);


lcd.setCursor(0, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 1
lcd.print("Canal");
lcd.setCursor(5, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 5
lcd.print("-");
lcd.setCursor(6, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 6
lcd.print(x);
lcd.setCursor(7, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 7
lcd.print("=");
lcd.setCursor(8, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 8
lcd.print(valores_analogicos[x]);
lcd.setCursor(10, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 10
lcd.print("%");
delay(1000);
  
  }


}