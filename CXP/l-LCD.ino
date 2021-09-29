// LCD
// CXP - Caixa Preta
// 10/01/2019

// Pinos do LCD
// PC1 = RS
// PC2 = R/#W
// PC0 = E
// PA4..7 = D4..7
//PC3 = Back Light

// Imprimir data do GPS no LCD
void lcd_gps_data(byte lin, byte col){
  lcd_char(lin,col+0,gps_dados[GPS_DATA+0]);
  lcd_char(lin,col+1,gps_dados[GPS_DATA+1]);
  lcd_char(lin,col+2,'/');
  lcd_char(lin,col+3,gps_dados[GPS_DATA+2]);
  lcd_char(lin,col+4,gps_dados[GPS_DATA+3]);
  lcd_char(lin,col+5,'/');
  lcd_char(lin,col+6,gps_dados[GPS_DATA+4]);
  lcd_char(lin,col+7,gps_dados[GPS_DATA+5]);
}

// Imprimir HH:MM:SS no LCD
void lcd_gps_hora(byte lin, byte col){
  lcd_char(lin,col+0,gps_dados[GPS_HORA+0]);
  lcd_char(lin,col+1,gps_dados[GPS_HORA+1]);
  lcd_char(lin,col+2,':');
  lcd_char(lin,col+3,gps_dados[GPS_HORA+2]);
  lcd_char(lin,col+4,gps_dados[GPS_HORA+3]);
  lcd_char(lin,col+5,':');
  lcd_char(lin,col+6,gps_dados[GPS_HORA+4]);
  lcd_char(lin,col+7,gps_dados[GPS_HORA+5]);
}

// Imprimir Latitude DD MM SS.SS N/S no LCD
void lcd_gps_lat(byte lin, byte col){
  lcd_char(lin,col+0,gps_dados[GPS_LAT+0]);
  lcd_char(lin,col+1,gps_dados[GPS_LAT+1]);
  lcd_char(lin,col+2,' ');
  lcd_str(lin,col+3,&gps_dados[GPS_LAT+2]);
  lcd_char(lin,col+11,gps_dados[GPS_NS]);
}

// Imprimir Longitude DDD MM SS.SS E/W no LCD
void lcd_gps_long(byte lin, byte col){
  lcd_char(lin,col+0,gps_dados[GPS_LONG+0]);
  lcd_char(lin,col+1,gps_dados[GPS_LONG+1]);
  lcd_char(lin,col+2,gps_dados[GPS_LONG+2]);
  lcd_char(lin,col+3,' ');
  lcd_str(lin,col+4,&gps_dados[GPS_LONG+3]);
  lcd_char(lin,col+12,gps_dados[GPS_EW]);
}

// Imprimir velocidade kph
void lcd_gps_vel_kph(byte lin, byte col){
  lcd_str(lin,col,"         ");
  lcd_str(lin,col,&gps_dados[GPS_VEL_KPH]);
  lcd_str(lin,col+str_tam(&gps_dados[GPS_VEL_KPH]),"kph");
}

// Imprimir determinado dado do GPS
void lcd_gps_dado(byte lin, byte col, byte qual){
  lcd_str(lin,col,&gps_dados[qual]);
}

////////////////////////////////////////////////////
/////////// float (double) com buffer //////////////
////////////////////////////////////////////////////

// Imprimir float = + xxx xxx xxx , ddd ddd ddd ddd (24 posições)
//  9 posições = limite da parte inteira
// 12 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
// No Arduino, double e float têm a mesma precisão
void lcd_float(byte lin, byte col, float fx, byte prec){
  char msg[24];
  str_float(fx,prec,msg);
  lcd_str(lin,col,msg);
}

////////////////////////////////////////////////////
//////////// 32 bits com buffer ////////////////////
////////////////////////////////////////////////////

// Escrever 32 bits em Decimal, com sinal e com zeros à esq
void lcd_dec32(byte lin, byte col, long dt){
  char msg[12];
  str_dec32(dt, msg);
  lcd_str(lin,col,msg);
}

// Escrever 32 bits em Decimal, sem sinal e com zeros à esq
void lcd_dec32u(byte lin, byte col, long dt){
  char msg[12];
  str_dec32u(dt, msg);
  lcd_str(lin,col,msg);
}

// Escrever 32 bits em Decimal, com sinal e sem zeros à esq
void lcd_dec32nz(byte lin, byte col, long dt){
  char msg[12];
  byte i;
  str_dec32(dt, msg);
  str_rmvz_s(msg);
  lcd_str(lin,col,msg);
}

// Escrever 32 bits em Decimal, sem sinal e sem zeros à esq
void lcd_dec32unz(byte lin, byte col, long dt){
  char msg[12];
  str_dec32u(dt, msg);
  str_rmvz_u(msg);
  lcd_str(lin,col,msg);
}

// Escrever word (32 bits) em Hexa na posição atual
void lcd_hex32(byte lin, byte col, long dt){
  char msg[9];
  str_hex32(dt, msg);
  lcd_str(lin,col,msg);
}


////////////////////////////////////////////////////
//////////// 16 bits com buffer ////////////////////
////////////////////////////////////////////////////

// Escrever 16 bits em Decimal, com sinal e com zeros à esq
void lcd_dec16(byte lin, byte col, int dt){
  char msg[7];
  str_dec16(dt, msg);
  lcd_str(lin,col,msg);
}

// Escrever 16 bits em Decimal, sem sinal e com zeros à esq
void lcd_dec16u(byte lin, byte col, word dt){
  char msg[7];
  str_dec16u(dt, msg);
  lcd_str(lin,col,msg);
}

// Escrever 16 bits em Decimal, com sinal e sem zeros à esq
void lcd_dec16nz(byte lin, byte col, int dt){
  char msg[7];
  byte i;
  str_dec16(dt, msg);
  str_rmvz_s(msg);
  lcd_str(lin,col,msg);
}

// Escrever 16 bits em Decimal, sem sinal e sem zeros à esq
void lcd_dec16unz(byte lin, byte col, word dt){
  char msg[7];
  str_dec16u(dt, msg);
  str_rmvz_u(msg);
  lcd_str(lin,col,msg);
}

// Escrever word (16 bits) em Hexa na posição atual
void lcd_hex16(byte lin, byte col, word dt){
  char msg[5];
  str_hex16(dt, msg);
  lcd_str(lin,col,msg);
}

////////////////////////////////////////////////////
//////////// 8 bits com buffer /////////////////////
////////////////////////////////////////////////////

// Escrever 8 bits em Decimal, com sinal e com zeros à esq
void lcd_dec8(byte lin, byte col, byte dt){
  char msg[5];
  str_dec8(dt, msg);
  lcd_str(lin,col,msg);
}

// Escrever 8 bits em Decimal, sem sinal e com zeros à esq
void lcd_dec8u(byte lin, byte col, byte dt){
  char msg[5];
  str_dec8u(dt, msg);
  lcd_str(lin,col,msg);
}

// Escrever 8 bits em Decimal, com sinal e sem zeros à esq
void lcd_dec8nz(byte lin, byte col, byte dt){
  char msg[5];
  byte i;
  str_dec8(dt, msg);
  str_rmvz_s(msg);
  lcd_str(lin,col,msg);
}

// Escrever 8 bits em Decimal, sem sinal e sem zeros à esq
void lcd_dec8unz(byte lin, byte col, byte dt){
  char msg[5];
  str_dec8u(dt, msg);
  str_rmvz_u(msg);
  lcd_str(lin,col,msg);
}

// Escrever byte em Hexa a partir da posição (lin,col)
void lcd_hex8(byte lin, byte col, byte dt){
  char msg[3];
  str_hex8(dt, msg);
  lcd_str(lin,col,msg);
}

// Escrever uma qtd de espaços a partir da posição (lin,col)
void lcd_spc(byte lin, byte col, byte qtd){
  byte i=0;
  if (lin<NRL){   //lin é válida? (0,1,2 ou 3)
    for (i=0; (i<qtd)&&(col<NRC); i++){
      lcd_buf[lin][col+1]=' ';
      col++;
    }
    lcd_mudou |= (1<<lin);
  }
}

// Escrever uma string no buffer a partir da posição (lin,col)
// lin = 0,1,2,3 e col=0,1,2,...,NRC
void lcd_str(byte lin, byte col, char *msg){
  byte i=0;
  if (lin<NRL){   //lin é válida? (0,1,2 ou 3)
    while ( (msg[i]!='\0') && (col<NRC) ){
      lcd_buf[lin][col+1]=msg[i];
      i++;
      col++;
    }
    lcd_mudou |= (1<<lin);
  }
}

// Escrever um char, na posição (lin,col)
void lcd_char(byte lin, byte col, byte dt){
  if ( (lin<NRL) && (col<NRC) ){ //lin e col válidos?
    lcd_buf[lin][col+1]=dt;  
    lcd_mudou |= (1<<lin);
  }
}

// Apagar uma linha do LCD - Escreve brancos
void lcd_apaga_lin(byte lin){
  byte col=0;
  for (col=1; col<NRC+1; col++)
    lcd_buf[lin][col]=' ';
  lcd_mudou |= (1<<lin);
}

// Apagar todo o LCD - Escreve brancos
// Acerta os caractes para posicionar cursor
void lcd_apaga(void){
  byte lin=0,col=0;
  for (lin=0; lin<NRL; lin++){
    for (col=1; col<NRC+1; col++){
      lcd_buf[lin][col]=' ';
    }
  }
  lcd_buf[0][0]=0x80 | 0x00;  //Cmdo para posicionar cursor
  lcd_buf[1][0]=0x80 | 0x40;  //Cmdo para posicionar cursor
  lcd_buf[2][0]=0x80 | 0x14;  //Cmdo para posicionar cursor
  lcd_buf[3][0]=0x80 | 0x54;  //Cmdo para posicionar cursor
  lcd_mudou = 0xF;    //Atualizar todas as linhas
}

// Configurar o buffer para o LCD
void lcd_config(void){
  byte lin=0,col=0;
  lcd_pinos();  //Configurar pinos para usar com LCD
  lcd_inic();   //Inicializar LCD
  lcd_BL();

  for (lin=0; lin<NRL; lin++){
    for (col=1; col<NRC+1; col++){
      lcd_buf[lin][col]=' ';
    }
  }
  lcd_buf[0][0]=0x80 | 0x00;  //Cmdo para posicionar cursor
  lcd_buf[1][0]=0x80 | 0x40;  //Cmdo para posicionar cursor
  lcd_buf[2][0]=0x80 | 0x14;  //Cmdo para posicionar cursor
  lcd_buf[3][0]=0x80 | 0x54;  //Cmdo para posicionar cursor
  lcd_busy=FALSE; //LCD livre
  lcd_mudou=0;    //Nada mudou
}

//********************************************************
/////////////////////////////////////////////////////////
//////// Rotinas para LCD modo terminal /////////////////
////////            INÍCIO              /////////////////
/////////////////////////////////////////////////////////
//********************************************************

// Escrever um char, espera Busy = 0
void lcdt_char(byte dt){
  while(lcd_busy==TRUE);    //Esperar Interrup Timer 2 atualizar LCD
  lcd_busy_hold();
  PORTC &= ~((1<<DDC2)|(1<<DDC1)|(1<<DDC0));  //R/W=RS=E = LOW
  lcd_RS();
  lcd_E();  lcd_E();  //2X para gerar um delay
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
  PORTA |= dt&0xF0;
  lcd_e();  lcd_e();  //2X para gerar um delay
  lcd_E();  lcd_E();  //2X para gerar um delay
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
  PORTA |= (dt&0xF)<<4;
  lcd_e();  lcd_e();  //2X para gerar um delay
}

// Escrever um char
// Não testa o Busy, por isso usa lcdt_nib_wr() com delays
void lcdt_char_nbusy(byte dt){
  PORTC &= ~((1<<DDC2)|(1<<DDC1)|(1<<DDC0));  //R/W=RS=E = LOW
  lcd_RS();
  lcd_nib_wr(dt>>4);
  lcd_nib_wr(dt&0xF);
}

//********************************************************
/////////////////////////////////////////////////////////
//////// Rotinas para LCD modo terminal /////////////////
////////              FIM               /////////////////
/////////////////////////////////////////////////////////
//********************************************************

/////////////////////////////////////////////////////////
////////// Rotinas básicas para LCD  ////////////////////
/////////////////////////////////////////////////////////

// Escrever um comando
// Espera Busy = 0
void lcd_cmdo(byte dt){
  while(lcd_busy==TRUE);    //Esperar Interrup Timer 2 atualizar LCD
  lcd_busy_hold();
  PORTC &= ~((1<<DDC2)|(1<<DDC1)|(1<<DDC0));  //R/W=RS=E = LOW
  lcd_rs();
  lcd_E();    lcd_E();    lcd_E();    //2X para gerar um delay
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
  PORTA |= dt&0xF0;
  lcd_e();  lcd_e();  lcd_e();    //2X para gerar um delay
  lcd_E();    lcd_E();  lcd_E();    //2X para gerar um delay
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
  PORTA |= (dt&0xF)<<4;
  lcd_e();  lcd_e();  lcd_e();    //2X para gerar um delay
}

// Esperar BUSY = 0
void lcd_busy_hold(void){
  int x;
  DDRA  &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = Entradas
  PORTA |=  ((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = PULL UP
  lcd_RW(); //Leitura
  lcd_rs(); //Status
  while(TRUE){
    led_VM();
    lcd_E();  lcd_E();  //2X para gerar um delay
    x=PINA&0xF0;
    lcd_e();  lcd_e();
    lcd_E();  lcd_E();  //2X para gerar um delay
    x|=(PINA&0xF0)>>4;
    lcd_e();  lcd_e();
    //Serial.println(x,HEX);
    if ( (x&0x80) == 0)  break;
  }
  led_vm();
  DDRA  |=  ((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = Saídas
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
  lcd_rs();
  lcd_rw();
}

// Retorna status (Busy + AC)
byte lcd_status(void){
  byte x;
  DDRA  &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = Entradas
  PORTA |=  ((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = PULL UP
  lcd_RW(); //Leitura
  lcd_rs(); //Status
  lcd_E();  lcd_E();  //2X para gerar um delay
  x=PINA&0xF0;
  lcd_e();
  lcd_E();  lcd_E();  //2X para gerar um delay
  x|=(PINA&0xF0)>>4;
  lcd_e();
  DDRA  |=  ((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = Saídas
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
  lcd_rs();
  lcd_rw();
  return x;
}

// LCD - Configurar pinos
void lcd_pinos(void){
  DDRC  |=   (1<<DDC3)|(1<<DDC2)|(1<<DDC1)|(1<<DDC0);   //BL=R/W=RS=E = Saídas
  PORTC &= ~((1<<DDC3)|(1<<DDC2)|(1<<DDC1)|(1<<DDC0));  //BL=R/W=RS=E = LOW
  DDRA  |=   (1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4);   //D7=D6=D5=D4 = Saídas
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
}

// LCD - Inicializar
void lcd_inic(void){
  delay(400);
  lcd_nib_wr(3);  //3
  delay(20);
  lcd_nib_wr(3);  //3
  delay(10);
  lcd_nib_wr(3);  //3
  delay(1);
  lcd_nib_wr(2);  //2
  delay(1);

  /*
  lcd_nib_wr(2);  lcd_nib_wr(8);    delay(1); //0x28
  lcd_nib_wr(0);  lcd_nib_wr(8);    delay(1);   //0x08
  lcd_nib_wr(0);  lcd_nib_wr(1);    delay(1);   //0x01
  lcd_nib_wr(0);  lcd_nib_wr(6);    delay(1);   //0x06
  lcd_nib_wr(0);  lcd_nib_wr(0xF);  delay(1); //0x0F 
  */
  
  lcd_cmdo(0x28); delay(1);   //Bus de 4 bits
  lcd_cmdo(0x08); delay(1);   //Display ativado
  lcd_cmdo(0x01); delay(1);   //Limpar display
  lcd_cmdo(0x06); delay(1);   //Incrementa cursor
  //lcd_cmdo(0x0F); delay(1);   //Cursor hab e piscante
  lcd_cmdo(0x0C); delay(1);   //Cursor desabilitado
}

// Escrever um nibble no LCD
// Recebe nibble = 0000 xxxx B
// Não altera RS e RW
void lcd_nib_wr(byte nib){
  volatile int i;
  lcd_E();  lcd_E();  //2X para gerar um delay
  //delayMicroseconds(400);
  PORTA &= ~((1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4));  //D7=D6=D5=D4 = LOW
  PORTA |= (nib&0xF)<<4;
  //delayMicroseconds(500);
  lcd_e();  lcd_e();  //2X para gerar um delay
  //delayMicroseconds(500);
}


// RW/RS/E - Sinais de controle do LCD
void lcd_e(void) { PORTC &= ~(1<<DDC0);  }  //E=0
void lcd_E(void) { PORTC |=  (1<<DDC0);  }  //E=1
void lcd_rs(void){ PORTC &= ~(1<<DDC1);  }  //RS=0 - Cmdo
void lcd_RS(void){ PORTC |=  (1<<DDC1);  }  //RS=1 - Dado
void lcd_rw(void){ PORTC &= ~(1<<DDC2);  }  //RW=0 - Escrita
void lcd_RW(void){ PORTC |=  (1<<DDC2);  }  //RW=1 - Leitura

// BL (PC3) = Back Light do LCD: Apagar / Acender / Inverter
void lcd_bl(void){ PORTC &= ~(1<<DDC3);  }  //BL = Apagado
void lcd_BL(void){ PORTC |=  (1<<DDC3);  }  //BL = Aceso
void lcd_Bl(void){ PORTC ^=  (1<<DDC3);  }  //BL = Invertido
