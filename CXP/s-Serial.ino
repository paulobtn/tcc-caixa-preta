// Serial - Porta Serial 0
// CXP - Caixa Preta
// 10/01/2019

// COM0 --> Arduino
// COM1 --> 
// COM2 --> Bluetooth
// COM3 --> GPS

// Imprimir cabeçalho para Modo de Operação em caso de batida
// Também serve para o Teste 12
void ser_cab(char qual){
  byte vet_gps[GPS_PASSO];  //Ler mensagem GPS
  int aux;
  long adri;
    
  if (qual=='o'){     //Para o modo Opera 1
    sram_rd_str(OP_AC_DATA, vet_gps, 10);   //data do acidente
    ser_str(vet_gps);                       
    ser_crlf(1);
    sram_rd_str(OP_AC_HORA, vet_gps, 10);   //hora do acidente
    ser_str(vet_gps);                       
    ser_crlf(1);
    
    //aux=sram_rd_16b(OP_DISP_TP);            //Temperatura instante acidente
    ser_dec16unz(sram_rd_16b(OP_DISP_TP));                      
    ser_crlf(1);

      /*
    //aux=sram_rd_16b(OP_ST_OK);              //Self test de acel e giro
    ser_dec16unz(aux=sram_rd_16b(OP_ST_OK));
    ser_crlf(1);
    //aux=sram_rd_16b(OP_STH_OK);             //Self test de Magnetômetro
    ser_dec16unz(sram_rd_16b(OP_STH_OK));
    ser_crlf(1);
    */
    
    //adri=sram_rd_32b(OP_MPU_ADR);           //Endereço 32 bits do MPU qdo SEL foi acionada
    ser_dec32unz(sram_rd_32b(OP_MPU_ADR));                     
    ser_crlf(1);
    
    //adri=sram_rd_32b(OP_GPS_ADR);           //Endereço 32 bits GPS qdo SEL foi acionada
    ser_dec32unz(sram_rd_32b(OP_GPS_ADR));
    ser_crlf(1);

  }
  if (qual=='t'){   //Cabeçalho falso para o Teste 12
    ser_str("010420\n");      //ddmmyy    data do acidente (falso)
    ser_str("111111.11\n");   //hhmmss.ss hora do acidente(falso)
    ser_dec16unz(2222);     ser_crlf(1);  //temperatura(falso)
    ser_dec32unz(33333L);  ser_crlf(1);  //adr ponteiro MPU (falso)
    ser_dec32unz(44444L);  ser_crlf(1);  //adr ponteiro GPS (falso)
  }

  // Linha 1
  ser_dec16nz(eeprom_rd_16b(CF_OK));      ser_spc(1);   //1.cf_ok
  ser_dec16nz(eeprom_rd_16b(CF_ST_OK));   ser_spc(1);   //2.st_cf
  ser_dec16nz(eeprom_rd_16b(CF_ESC_AC));  ser_spc(1);   //3.aesc_cf
  ser_dec16nz(eeprom_rd_16b(CF_ESC_GI));  ser_spc(1);   //4.gesc_cf
  ser_dec16nz(eeprom_rd_16b(CF_TP));      ser_spc(1);   //5.tp_cf
  ser_dec16nz(eeprom_rd_16b(CF_AX));      ser_spc(1);   //6.ax offset
  ser_dec16nz(eeprom_rd_16b(CF_AY));      ser_spc(1);   //7.ay offset
  ser_dec16nz(eeprom_rd_16b(CF_AZ));      ser_spc(1);   //8.az offset
  ser_dec16nz(eeprom_rd_16b(CF_GX));      ser_spc(1);   //9.gx offset
  ser_dec16nz(eeprom_rd_16b(CF_GY));      ser_spc(1);   //10.gy offset
  ser_dec16nz(eeprom_rd_16b(CF_GZ));      ser_crlf(1);  //11.gz offset

  // Linha 2
  ser_dec16nz(eeprom_rd_16b(CF_MAG_OK));  ser_spc(1);   //1.cf_ok  Magnetômetro
  ser_dec16nz(eeprom_rd_16b(CF_STH_OK));  ser_spc(1);   //2.sth_cf Magnetômetro
  ser_dec16nz(eeprom_rd_16b(CF_HX_ASA));  ser_spc(1);   //3.hx ASA
  ser_dec16nz(eeprom_rd_16b(CF_HY_ASA));  ser_spc(1);   //4.hy ASA
  ser_dec16nz(eeprom_rd_16b(CF_HZ_ASA));  ser_spc(1);   //5.hz ASA
  ser_dec16nz(eeprom_rd_16b(CF_HX_OFF));  ser_spc(1);   //6.hx offset
  ser_dec16nz(eeprom_rd_16b(CF_HY_OFF));  ser_spc(1);   //7.hy offset
  ser_dec16nz(eeprom_rd_16b(CF_HZ_OFF));  ser_spc(1);   //8.hz offset
  ser_dec16nz(eeprom_rd_16b(CF_HX_ESC));  ser_spc(1);   //9.hx escala
  ser_dec16nz(eeprom_rd_16b(CF_HY_ESC));  ser_spc(1);   //10.hy escala
  ser_dec16nz(eeprom_rd_16b(CF_HZ_ESC));  ser_crlf(1);  //11.hz escala
  
  // Linha 3
  ser_dec16nz(sram_rd_16b(OP_ST_OK));   ser_spc(1);   //1.st_op
  ser_dec16nz(sram_rd_16b(OP_STH_OK));  ser_spc(1);   //2.sth_op
  ser_dec16nz(sram_rd_16b(OP_ESC_AC));  ser_spc(1);   //3.aesc_op
  ser_dec16nz(sram_rd_16b(OP_ESC_GI));  ser_spc(1);   //4.gesc_op
  ser_dec16nz(sram_rd_16b(OP_FREQ_AG)); ser_spc(1);   //5.fammost
  ser_dec16nz(sram_rd_16b(OP_BW_AG));                 //bw
  // Preencher espaço reservado 1, 2,3, 4,5
  for (aux=1; aux<6; aux++){
    ser_spc(1);   
    ser_dec16nz(aux);  ser_spc(1);                      //Reserva de espaço
  }
  ser_crlf(1);   
}

// Imprimir LATITUDE
// Recebe no vet = ddmm.mmmmm
//           ns = N ou S
// Imprime ==> +/-dd.ddddddd
void ser_lat(byte *vet, byte ns){
  byte msg[24];
  str_lat(vet,ns,msg);
  ser_str(msg);
}

// Imprimir LONGITUDE
// Recebe no vet = dddmm.mmmmm
//           ew = E ou W
// Imprime ==> +/-ddd.ddddddd
void ser_long(byte *vet, byte ew){
  byte msg[24];
  str_long(vet,ew,msg);
  ser_str(msg);
}

// Auxiliar no DUMP de memórias
void ser_dump_memo(long adr, byte *vet){
  word i;
  ser_hex16(adr>>16);
  ser_spc(1);
  ser_hex16(adr&0xFFFF);
  ser_str(": ");
  for (i=0; i<16; i++){
    ser_hex8(vet[i]);
    ser_spc(1);
  }
  ser_str("- ");
  
  for (i=0; i<16; i++){
    if (vet[i]>0x1F)  ser_char(vet[i]);
    else              ser_char('.');
  }
  ser_crlf(1);
}

// Imprimir GPS em uma só linha, dados no pacote gps_dados
// Apenas separa os campos por espaços
void ser_gps_dados_lin(char *gps_vt){
    ser_str(&gps_vt[GPS_STATUS]);   ser_spc(1);     //Status

    /*
    ser_char(gps_vt[GPS_DATA+0]);  ser_char(gps_vt[GPS_DATA+1]);    ser_char('/');  //Data
    ser_char(gps_vt[GPS_DATA+2]);  ser_char(gps_vt[GPS_DATA+3]);    ser_char('/');
    ser_char(gps_vt[GPS_DATA+4]);  ser_char(gps_vt[GPS_DATA+5]);    ser_spc(1);

    ser_char(gps_vt[GPS_HORA+0]);  ser_char(gps_vt[GPS_HORA+1]);    ser_char(':');  //Hora
    ser_char(gps_vt[GPS_HORA+2]);  ser_char(gps_vt[GPS_HORA+3]);    ser_char(':');
    ser_char(gps_vt[GPS_HORA+4]);  ser_char(gps_vt[GPS_HORA+5]);    ser_spc(1);
    */

    gps_vt[GPS_HORA+6]='\0';  //Remover fração de segundos
    ser_str(&gps_vt[GPS_HORA]);     ser_spc(1);     //Hora
    ser_str(&gps_vt[GPS_DATA]);     ser_spc(1);     //Data
    ser_str(&gps_vt[GPS_LAT]);      ser_spc(1);     //Lat
    ser_str(&gps_vt[GPS_NS]);       ser_spc(1);     //N-S
    ser_str(&gps_vt[GPS_LONG]);     ser_spc(1);     //Long
    ser_str(&gps_vt[GPS_EW]);       ser_spc(1);     //E-W
    ser_str(&gps_vt[GPS_VEL_KPH]);  ser_spc(1);     //Vel km/h
    ser_str(&gps_vt[GPS_VEL_UN]);   ser_spc(1);     //Unidade da velocidade
    ser_str(&gps_vt[GPS_VEL_NOS]);  ser_spc(1);     //Vel Nós
    ser_str(&gps_vt[GPS_CURSO]);    ser_spc(1);     //Curso
    ser_str(&gps_vt[GPS_ALT]);      ser_spc(1);     //Altitude
    ser_str(&gps_vt[GPS_ALT_UN]);   ser_spc(1);     //Unidade da Altitude
    ser_str(&gps_vt[GPS_QTD_SAT]);  ser_spc(1);     //Qtd de Satélites
    ser_str(&gps_vt[GPS_PDOP]);     ser_spc(1);     //PDOP
    ser_str(&gps_vt[GPS_HDOP]);     ser_spc(1);     //HDOP
    ser_str(&gps_vt[GPS_VDOP]);     ser_spc(1);     //VDOP
    ser_str(&gps_vt[GPS_ADR_SRAM]); ser_crlf(1);    //ADR SRAM-FLASH
}


// Imprimir GPS, dados no pacote gps_dados
void ser_gps_dados(char *gps_vt){
    sero_espera(11);
    ser_str("gps_dados: Status=");  ser_str(&gps_vt[GPS_STATUS]);
    ser_str(" Hora=");      ser_str(&gps_vt[GPS_HORA]);
    ser_str(" Data=");    ser_str(&gps_vt[GPS_DATA]);
    ser_str(" Lat=");     ser_str(&gps_vt[GPS_LAT]);
    ser_str(" N/S=");     ser_str(&gps_vt[GPS_NS]);
    ser_str(" Long=");    ser_str(&gps_vt[GPS_LONG]);
    ser_str(" E/W=");     ser_str(&gps_vt[GPS_EW]);
    ser_str(" Vel Nos="); ser_str(&gps_vt[GPS_VEL_NOS]);
    ser_str(" Curso=");   ser_str(&gps_vt[GPS_CURSO]);
    ser_str(" PDOP=");   ser_str(&gps_vt[GPS_PDOP]);
    ser_str(" HDOP=");   ser_str(&gps_vt[GPS_HDOP]);
    ser_str(" VDOP=");   ser_str(&gps_vt[GPS_VDOP]);
    ser_str(" Vel_kph=");   ser_str(&gps_vt[GPS_VEL_KPH]);
    ser_str(" Vel_uni=");   ser_str(&gps_vt[GPS_VEL_UN]);
    //ser_str(" Fix=");   ser_str(&gps_vt[GPS_FIX);        //Sem uso
    ser_str(" Qtd_Sat=");   ser_str(&gps_vt[GPS_QTD_SAT]);
    ser_str(" Alt=");   ser_str(&gps_vt[GPS_ALT]);
    ser_str(" Alt_uni=");   ser_str(&gps_vt[GPS_ALT_UN]);
    ser_str(" Adr_MPU=");   ser_str(&gps_vt[GPS_ADR_SRAM]);
}



// Compor e imprimir uma linha com acel, temp e giro
// Recebe vetor com bytes e monta words para imprimir
void ser_lin_ac_tp_gi(byte *vet){
  byte i;
  for (i=0; i<14; i+=2){
    ser_dec16( ( (word)vet[i]<<8) | vet[i+1]);
    ser_spc(1);
  }
  ser_crlf(1);
}

// Compor e imprimir uma linha com acel e giro
// Recebe vetor com bytes e monta words para imprimir
void ser_lin_ac_gi(byte *vet){
  byte i;
  for (i=0; i<12; i+=2){
    ser_dec16( ( (word)vet[i]<<8) | vet[i+1]);
    ser_spc(1);
  }
  ser_crlf(1);
}

// Compor e imprimir uma linha com cel, giro e mag
void ser_lin_ac_gi_mg(byte *vet){
  byte i,tout=0;
  for (i=0; i<18; i+=2){
    sero_espera(10);
    ser_dec16( ( (word)vet[i]<<8) | vet[i+1]);
    ser_spc(1);
  }
  ser_crlf(1);
}

// Compor acel, giro e mag e imprimir 1 por linha 
void ser_ac_gi_mg(byte *vet){
  byte i;
  for (i=0; i<18; i+=2){
    ser_dec16u( ( (word)vet[i]<<8) | vet[i+1]);
    ser_crlf(1);
  }
}

////////////////////////////////////////////////////
/////////// float (double) /////////////////////////
////////////////////////////////////////////////////

// Imprimir float = + xxx xxx xxx , ddd ddd ddd ddd (24 posições)
//  9 posições = limite da parte inteira
// 12 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
// No Arduino, double e float têm a mesma precisão
void ser_float(float fx, byte prec){
  char msg[24];
  str_float(fx,prec,msg);
  ser_str(msg);
}

////////////////////////////////////////////////////
//////////////// 32 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 32 bits em Decimal, com sinal e com zeros à esq
void ser_dec32(long dt){
  char msg[12];
  str_dec32(dt, msg);
  ser_str(msg);
}

// Escrever 32 bits em Decimal, sem sinal e com zeros à esq
void ser_dec32u(long dt){
  char msg[12];
  str_dec32u(dt, msg);
  ser_str(msg);
}

// Escrever 32 bits em Decimal, com sinal e sem zeros à esq
void ser_dec32nz(long dt){
  char msg[12];
  byte i;
  str_dec32(dt, msg);
  str_rmvz_s(msg);
  ser_str(msg);
}

// Escrever 32 bits em Decimal, sem sinal e sem zeros à esq
void ser_dec32unz(long dt){
  char msg[12];
  str_dec32u(dt, msg);
  str_rmvz_u(msg);
  ser_str(msg);
}

// Escrever word (32 bits) em Hexa na posição atual
void ser_hex32(long dt){
  char msg[9];
  str_hex32(dt, msg);
  ser_str(msg);
}
////////////////////////////////////////////////////
//////////////// 16 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 16 bits em Decimal, com sinal e com zeros à esq
void ser_dec16(int dt){
  char msg[7];
  str_dec16(dt, msg);
  ser_str(msg);
}

// Escrever 16 bits em Decimal, sem sinal e com zeros à esq
void ser_dec16u(word dt){
  char msg[7];
  str_dec16u(dt, msg);
  ser_str(msg);
}

// Escrever 16 bits em Decimal, com sinal e sem zeros à esq
void ser_dec16nz(int dt){
  char msg[7];
  byte i;
  str_dec16(dt, msg);
  str_rmvz_s(msg);
  ser_str(msg);
}

// Escrever 16 bits em Decimal, sem sinal e sem zeros à esq
void ser_dec16unz(word dt){
  char msg[7];
  str_dec16u(dt, msg);
  str_rmvz_u(msg);
  ser_str(msg);
}

// Escrever word (16 bits) em Hexa na posição atual
void ser_hex16(word dt){
  char msg[5];
  str_hex16(dt, msg);
  ser_str(msg);
}

////////////////////////////////////////////////////
///////////////// 8 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 8 bits em Decimal, com sinal e com zeros à esq
void ser_dec8(byte dt){
  char msg[5];
  str_dec8(dt, msg);
  ser_str(msg);
}

// Escrever 8 bits em Decimal, sem sinal e com zeros à esq
void ser_dec8u(byte dt){
  char msg[5];
  str_dec8u(dt, msg);
  ser_str(msg);
}

// Escrever 8 bits em Decimal, com sinal e sem zeros à esq
void ser_dec8nz(byte dt){
  char msg[5];
  byte i;
  str_dec8(dt, msg);
  str_rmvz_s(msg);
  ser_str(msg);
}

// Escrever 8 bits em Decimal, sem sinal e sem zeros à esq
void ser_dec8unz(byte dt){
  char msg[5];
  str_dec8u(dt, msg);
  str_rmvz_u(msg);
  ser_str(msg);
}

// Escrever byte em Hexa
void ser_hex8(byte dt){
  char msg[3];
  str_hex8(dt, msg);
  ser_str(msg);
}

// Imprimir qtd de pares CR e LF
void ser_crlf(byte qtd){
  int i;
  for (i=0; i<qtd; i++){
    ser_char('\r'); //0xD
    ser_char('\n'); //0xA
  }
}

// Imprimir qtd Carriage Return
void ser_cr(byte qtd){
  int i;
  for (i=0; i<qtd; i++)
    ser_char('\r'); //0xD
}

// Imprimir qtd Line Feed
void ser_lf(byte qtd){
  int i;
  for (i=0; i<qtd; i++)
    ser_char('\n'); //0xA
}

// Imprimir qtd brancos
void ser_spc(byte qtd){
  int i;
  for (i=0; i<qtd; i++)
    ser_char(' ');
}

// Imprimir string
void ser_str(byte *msg){
  int i=0;
  while( msg[i] != '\0')
    ser_char(msg[i++]);
}

// Imprimir um char
// Espera ser fila estiver quase cheia
// Alerta led VERMELHO
void ser_char(byte dt){
  if (sero_schnell==FALSE){
    while(sero_uso()>90) led_VM();  //Fila passou 90%
    led_vm();
    sero_poe(dt);
  }
  else{     //Escrever direto nos registradores
    UDR0=dt;   //UART0 transmite
    UDR2=dt;   //UART0 transmite
    while ( (UCSR0A&(1<<TXC0)) == 0); //Esperar transmitir (TXC0 = 1)
    UCSR0A = UCSR0A | (1<<TXC0);      //Apagar bit TXC0
    while ( (UCSR2A&(1<<TXC2)) == 0); //Esperar transmitir (TXC0 = 1)
    UCSR2A = UCSR2A | (1<<TXC2);      //Apagar bit TXC0
    //delay(1);
  }
  
}

////////////////////////////////////////////////////
////////////////// Fila SERI ///////////////////////
////// UART0 (Arduino) + UART2 (Bluetooth) /////////
/////// Interrupção chama seri_poe() ///////////////
/////// CXP chama seri_tira() //////////////////////
////////////////////////////////////////////////////

// Retirar um número de 16 bits da fila SERI
// Monta ao número na base 10 (-32.768, ..., 0, 1, ..., 32767)
// Consome CR e LF que estiverem antes do número
// Retorna TRUE  --> retirou um número
// Retorna FALSE --> Não retirou número, não alterou a fila
byte seri_num16(int *nr){
  byte x=0,col=0;
  int num=0,sinal;    //Montar o número

  // Consumir possíveis CR e LF que antecedem
  while (TRUE){
    while (seri_xereta(&x) == FALSE);
    if (x==CR || x==LF) seri_tira(&x);
    else                  break;
  }

  //Se é letra, retorna Falso
  //Senão, acerta o sinal
  sinal = 1;
  if      (x=='+') sinal=1;
  else if (x=='-') sinal=-1;
  else if (x<'0' || x>'9')   return FALSE;
  seri_tira(&x);
  
  // Se for número, montar os dígitos
  while(TRUE){
      while(seri_xereta(&x) == FALSE);
      if (x<'0' || x>'9') break;
      seri_tira(&x);
      num = 10*num + x-0x30;
  }
  *nr=sinal*num;
  return TRUE;
}

// Retirar um número de 8 bits absoluto da fila SERI
// Monta ao número na base 10 (0, 1, ..., 255)
// Consome CR e LF que estiverem antes do número
// Retorna T --> retirou um número
// Retorna F --> Não retirou número, não alterou a fila
byte seri_num8(byte *nr){
  byte x=0;
  byte num=0; //Montar o número

  // Consumir possíveis CR e LF que antecedem
  while (TRUE){
    if (seri_xereta(&x) == FALSE) return FALSE;
    if (x==CR || x==LF) seri_tira(&x);
    else                  break;
  }
  //É letra, retorna Falso
  if (x<'0' || x>'9')   return FALSE;
  
  // Se for número, montar os dígitos?
  while(TRUE){
      seri_tira(&x);
      num = 10*num + x-0x30;
      while(seri_xereta(&x) == FALSE);
      if (x<'0' || x>'9') break;
  }
  *nr=num;
  return TRUE;
}

// Retirar uma letra da fila SERI
// Só não aceita números 0, 1, ..., 9
// Consome CR e LF
// Retorna T --> retirou uma letra
// Retorna F --> Não retirou letra, não alterou a fila
byte seri_letra(char *cha){
  byte x=0;

  // Consumir possíveis CR e LF que antecedem
  while (TRUE){
    if (seri_xereta(&x) == FALSE) return FALSE;
    if ( x==CR || x==LF) seri_tira(&x);
    else                  break;
  }

  // É letra?
  if ( x<'0' || x>'9'){
    seri_tira(&x);
    *cha=x;
    return TRUE;    
  }
  return FALSE;
}

// Informa qual o próximo byte da fila SERI
// Não altera estado dos ponteiros
// Chamada fora das interrupções
char seri_xereta(char *cha){
  word pin,pout;
  noInterrupts();
  pout=seri_pout; //Copiar
  pin=seri_pin;   //ponteiros
  interrupts();
  pout++;
  if (pout == SERI_FILA_TAM)    pout=0;
  if (pout == pin){
    return FALSE;
  }
  else{
    *cha=seri_fila[pout];
    return TRUE;
  }
}

// Inicializar fila SERI
void seri_config(void){
  seri_pin=1;
  seri_pout=0;
}

// Colocar um byte na fila SERI
// Chamada dentro Interrupções de RX UART0 ou UART2
char seri_poe(char cha){
  if (seri_pin == seri_pout){
    seri_cheia();     //Msg de fila cheia
    return FALSE;     //SERI cheia
  }
  else{
    seri_fila[seri_pin++]=cha;
    if (seri_pin==SERI_FILA_TAM)
      seri_pin=0;
    return TRUE;
  }
}

// Mensagem de Fila SERI Cheia
void seri_cheia(void){
  char msg[]="\nERRO: SERI Cheia!\n";
  byte i=0;
  while(msg[i]!='\0'){
    UDR0=msg[i];
    while ( (UCSR0A&(1<<TXC0)) == 0); //Esperar transmitir (TXC0 = 1)
    UCSR0A = UCSR0A | (1<<TXC0);      //Apagar bit TXC0
    UDR2=msg[i];
    while ( (UCSR2A&(1<<TXC2)) == 0); //Esperar transmitir (TXC2 = 1)
    UCSR2A = UCSR2A | (1<<TXC2);      //Apagar bit TXC2
    i++;
  }
}

// Tirar um byte da fila SERI
// Chamada fora das interrupções
char seri_tira(char *cha){
  word pout_aux;
  noInterrupts();
  pout_aux=seri_pout+1;
  if (pout_aux == SERI_FILA_TAM)    pout_aux=0;
  if (pout_aux == seri_pin){
    interrupts();
    return FALSE;
  }
  else{
    *cha=seri_fila[pout_aux];
    seri_pout=pout_aux;
    interrupts();
    return TRUE;
  }
}

////////////////////////////////////////////////////
////////////////// Fila SERO ///////////////////////
////// UART0 (Arduino) + UART2 (Bluetooth) /////////
/////// CXP chama sero_poe() ///////////////////////
/////// Interrupção chama sero_tira() //////////////
////////////////////////////////////////////////////

// Inicializar fila SERO
void sero_config(void){
  sero_pin=1;
  sero_pout=0;
  sero_schnell=FALSE;      //Não habilitar modo rápido
}

// Colocar um byte na fila SERO
// Chamada fora das interrupções
char sero_poe(char cha){
  noInterrupts();
  if (sero_pin == sero_pout){
    sero_cheia();     //Msg de fila cheia
    interrupts();
    return FALSE;     //SERI cheia
  }
  else{
    sero_fila[sero_pin++]=cha;
    if (sero_pin==SERO_FILA_TAM)
      sero_pin=0;
    interrupts();
    return TRUE;
  }
}

// Mensagem de Fila SERO Cheia
void sero_cheia(void){
  char msg[]="\nERRO: SERO Cheia!\n";
  byte i=0;
  while(msg[i]!='\0'){
    UDR0=msg[i];
    while ( (UCSR0A&(1<<TXC0)) == 0); //Esperar transmitir (TXC0 = 1)
    UCSR0A = UCSR0A | (1<<TXC0);      //Apagar bit TXC0
    UDR2=msg[i];
    while ( (UCSR2A&(1<<TXC2)) == 0); //Esperar transmitir (TXC2 = 1)
    UCSR2A = UCSR2A | (1<<TXC2);      //Apagar bit TXC2
    i++;
  }
}

// Tirar um byte da fila SERO
// Chamada dentro Interrupções de TX UART0 ou UART2
char sero_tira(char *cha){
  word pout_aux;
  pout_aux=sero_pout+1;
  if (pout_aux == SERO_FILA_TAM)    pout_aux=0;
  if (pout_aux == sero_pin){
    return FALSE;
  }
  else{
    *cha=sero_fila[pout_aux];
    sero_pout=pout_aux;
    return TRUE;
  }
}

// Esperar SERO esvaziar
// Gera Time out
// Acho que pode ser removida
void sero_espera(byte nr){
  word tout=0;
  while(sero_vazia()==FALSE){
    delay(1);
    if (tout++ == 0xFFFF) sero_timeout(nr);
  }
}

// SERO vazia?
// TRUE  ==> Vazia
// FALSE ==> Não vazia
char sero_vazia(void){
  word pin,pout;
  noInterrupts();
  pout=sero_pout; //Copiar
  pin=sero_pin;   //ponteiros
  interrupts();
  pout++;
  if (pout == SERO_FILA_TAM)    pout=0;
  if (pout == pin)  return TRUE;
  else              return FALSE;
}

// SERO uso
// 0% até 100% Qto da fila foi usada
byte sero_uso(void){
  word pin,pout;
  noInterrupts();
  pout=sero_pout; //Copiar
  pin=sero_pin;   //ponteiros
  interrupts();
  if (pin==pout) return 100;   //Fila cheia
  if (pin>pout) return (100*(pin-pout))/SERO_FILA_TAM;
  else          return (100*(SERO_FILA_TAM-pout+pin))/SERO_FILA_TAM;
}

// TIME OUT
// Algumas rotinas serias podem encher o buffer SERO
// Elas esperam o SERO esvaziar, isso pode prender
void sero_timeout(char nr){
            // 01234567890123 
  char msg[]="\nTimeOut: ddd";
  byte i=0;
  str_dec8u(nr, &msg[10]);
  while(msg[i]!='\0'){
    UDR0=msg[i];
    while ( (UCSR0A&(1<<TXC0)) == 0); //Esperar transmitir (TXC0 = 1)
    UCSR0A = UCSR0A | (1<<TXC0);      //Apagar bit TXC0
    UDR2=msg[i];
    while ( (UCSR2A&(1<<TXC2)) == 0); //Esperar transmitir (TXC2 = 1)
    UCSR2A = UCSR2A | (1<<TXC2);      //Apagar bit TXC2
    i++;
  }
}


//////////////////////////////////////////////////
//////// Interrupções UART0 UART2 ////////////////
//////////////////////////////////////////////////

// UART0 TX: Interrupção por dado enviado
// Retirar dado da sero
ISR(USART0_TX_vect){
  byte x;
  sero_seg++;
  if (sero_seg == 2){  //UART2 já transmitiu?
    sero_seg=0;
    if (sero_tira(&x)==TRUE){
      UDR0=x;   //UART0 transmite
      UDR2=x;   //UART2 transmite
      sero_parou=FALSE;
    }
    else{
      sero_parou=TRUE;
    }
  }
}

// UART2 TX: Interrupção por dado transmitido
// Retirar dado da sero
ISR(USART2_TX_vect){
  byte x;
  sero_seg++;
  if (sero_seg == 2){  //UART0 já transmitiu?
    sero_seg=0;
    if (sero_tira(&x)==TRUE){
      UDR0=x;   //UART0 transmite
      UDR2=x;   //UART2 transmite
      sero_parou=FALSE;
    }
    else{
      sero_parou=TRUE;
    }
  }
}

// UART0: RX: Interrupção por dado recebido
ISR(USART0_RX_vect){
  seri_poe(UDR0);
}

// UART2: RX: Interrupção por dado recebido
ISR(USART2_RX_vect){
  seri_poe(UDR2);
}

////////////////////////////////////////////////////
/////////////// Configuração ///////////////////////
////////////////////////////////////////////////////

// UART0: Habilitar ou Desabilitar Interrupção TX e RX
void ser0_tx_int_EN(void){ UCSR0B |=  (1<<TXCIE0);}   //Hab Interrup TX
void ser0_tx_int_en(void){ UCSR0B &= ~(1<<TXCIE0);}   //Desab Interrup TX
void ser0_rx_int_EN(void){ UCSR0B |=  (1<<RXCIE0);}   //Hab Interrup RX
void ser0_rx_int_en(void){ UCSR0B &= ~(1<<RXCIE0);}   //Desab Interrup RX

// UART0: Habilitar ou Desabilitar TX e RX
void ser0_tx_EN(void){ UCSR0B |= (1<<TXEN0);}    //Hab TX
void ser0_tx_en(void){ UCSR0B &= ~(1<<TXEN0);}   //Desab TX
void ser0_rx_EN(void){ UCSR0B |= (1<<RXEN0);}    //Hab RX
void ser0_rx_en(void){ UCSR0B &= ~(1<<RXEN0);}   //Desab RX

// UART2: Habilitar ou Desabilitar Interrupção TX e RX
void ser2_tx_int_EN(void){ UCSR2B |=  (1<<TXCIE2);}   //Hab Interrup TX
void ser2_tx_int_en(void){ UCSR2B &= ~(1<<TXCIE2);}   //Desab Interrup TX
void ser2_rx_int_EN(void){ UCSR2B |=  (1<<RXCIE2);}   //Hab Interrup RX
void ser2_rx_int_en(void){ UCSR2B &= ~(1<<RXCIE2);}   //Desab Interrup RX

// UART2: Habilitar ou Desabilitar TX e RX
void ser2_tx_EN(void){ UCSR2B |= (1<<TXEN2);}    //Hab TX
void ser2_tx_en(void){ UCSR2B &= ~(1<<TXEN2);}   //Desab TX
void ser2_rx_EN(void){ UCSR2B |= (1<<RXEN2);}    //Hab RX
void ser2_rx_en(void){ UCSR2B &= ~(1<<RXEN2);}   //Desab RX

// UART0 e UART2 são configuradas aqui
// Habilitar recepção e transmissão
// Habilitar interrupção por transmissão e recepção
void ser_config(long br){
  word dv;
  dv=(2000000L/br)-1;

  //UART0
  UBRR0H = dv>>8;     //MSB
  UBRR0L = dv&0xFF;   //LSB
  // Configurar 8 bits, sem paridade, 1 stop
  UCSR0A = (1<<TXC0) | (1<<U2X0);
  UCSR0B = (1<<RXCIE0) | (1<<TXCIE0) | (1<<RXEN0) | (1<<TXEN0);  //RXIE=TXIE=1, hab TX e RX
  UCSR0C = (1<<UCSZ01)| (1<<UCSZ00);

  //UART2
  UBRR2H = dv>>8;     //MSB
  UBRR2L = dv&0xFF;   //LSB
  // Configurar 8 bits, sem paridade, 1 stop
  UCSR2A = (1<<TXC2) | (1<<U2X2);
  UCSR2B = (1<<RXCIE2) | (1<<TXCIE2) | (1<<RXEN2) | (1<<TXEN2);  //RXIE=TXIE=1, hab TX e RX
  UCSR2C = (1<<UCSZ21)| (1<<UCSZ20);
}
