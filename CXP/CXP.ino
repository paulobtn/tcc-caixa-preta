// CXP - Caixa Preta

#include "b-Defs.h"
#include "b-Globs.h"

void setup() {
  timer1_config();
  sw_config();
  leds_config();
  scope_config();
  lcd_config();
  ser_config(115200L);    //UART0 e UART2
  sero_parou=TRUE;

  timer2_config();
  timer1_config();
  //twi_config_100k();     //Configurar TWI
  twi_config_400k();       //Configurar TWI
  //spi_config(SPI_500K);  //Configurar SPI
  spi_config(SPI_8M);      //Configurar SPI
  mpu_config();            //Configura MPU +/- 2g
  mpu_mag_config();      //Configurar o magnetômetro
  seri_config();
  sero_config();
  gps_config(9600);           //UART3 configurar

}

void loop() {

  //teste();  //Forçar a entrada  no modo teste
  if (sw_val < 10)  teste();
  else              opera();
  
  while(TRUE);  //Para em loop infinito
}


// Selecionar o modo
// Serve selecionar modo de Operação
// Serve selecionar modo de Teste
// A linha 0 é preparada por quem chama
// Usa as linhas 1, 2 e 3
byte sel_modo(char *msg[], byte total){
  char prov=total;    //provisório = 1, 2, ..., OPERA_TOT
  byte tecla,aux;
  byte ser;
  byte fase;    //Indicar se estava recebendo nr pela serial
  fase=0;
  prov=0;
  while(TRUE){
    lcd_apaga_lin(1);
    lcd_apaga_lin(2);
    lcd_apaga_lin(3);
    lcd_str(2,0,"-->");
    aux=prov;
    lcd_str(1,3,msg[aux++]);
    if (aux>total)  aux=0;    
    lcd_str(2,3,msg[aux]);
    ser_str(msg[aux++]);  ser_crlf(1);
    if (aux>total)  aux=0;    
    lcd_str(3,3,msg[aux]);
    //Esperar tecla ou serial
    while(TRUE){
      if ( sw_tira(&tecla) == TRUE) break;
      if (seri_letra(&ser) == TRUE) return ser;
      if (seri_num8(&ser) == TRUE)   return ser;
    }
    switch(tecla){
      case SW_SUP:  prov--;  break;
      case SW_INF:  prov++;  break;
      case SW_SEL:  if (++prov>total)  prov=0;
                    return prov;
    }
    ser_crlf(1);
    if (prov>total)  prov=0;
    if (prov==-1)    prov=total;
  }
}
