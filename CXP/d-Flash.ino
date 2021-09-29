// Flash (I2C) - Rotinas para acesso Usa TWI
// CXP - Caixa Preta
// 22/01/2019
//
// 24LC1025 - Flash TWI de 128 KB

// Flash Batida
// Apresentar todos os dados do acidente
void flash_dados_acid(void){
  word bateu;   //Indicar se a Flash tem dados da batida
  byte vet_mpu[MPU_PASSO];      //Ler bytes: ax-ay-az-gx-gy-gz
  byte vet_gps[GPS_PASSO];  //Ler mensagem GPS
  long adr;       //Ponteiro para ler os dados
  long cont;      //Contador de acessos ao MPU
  long qtd;       //Quantidade de ac-gi-mg gravados
  lcd_str(3,0,"Monitor Serial");
  ser_str("==> FLASH: Dados do Acidente <==\n");
  eeprom_cf_mostra(); //Calibração de Fábrica
  flash_op_mostra();  //Calibraçao ao Ligar o carro
  bateu=flash_rd_16b(OP_BATEU);
  if (bateu==COD_SIM){

    // MPU - dados gravados
    qtd = (MPU_ADR_FIM - MPU_ADR_INI) / MPU_PASSO;
    adr=flash_rd_32b(OP_MPU_ADR);                 //Endereço no instante da batida
    adr+=(MPU_ADR_FIM - MPU_ADR_INI)/2;           //Somar metade do espaço para pegar o início
    if (adr>MPU_ADR_FIM)
      adr = MPU_ADR_INI+(adr-MPU_ADR_FIM);        //Se ultrapassar o fim, somar diferença ao início
    ser_str("\n--- (Ordenados) Dados do MPU = ");
    ser_dec32unz(qtd);
    ser_str("\nContador    Endereço    ax     ay     az     gx     gy     gz\n");
    for (cont=0; cont<qtd; cont++){
      flash_rd_blk(adr,vet_mpu,12);
      ser_dec32u(cont);
      ser_str("  ");
      ser_hex32(adr);
      ser_str("  ");
      ser_lin_ac_gi(vet_mpu);
      adr+=MPU_PASSO;
      if (adr==MPU_ADR_FIM) adr=MPU_ADR_INI;
    }
    
    // GPS - dados gravados
    ser_str("\n--- Dados do GPS");
    qtd = (GPS_ADR_FIM - GPS_ADR_INI) / GPS_PASSO;
    adr=flash_rd_32b(OP_GPS_ADR);                 //Endereço no instante da batida
    adr+=(GPS_ADR_FIM - GPS_ADR_INI)/2;           //Somar metade do espaço para pegar o início
    if (adr>GPS_ADR_FIM)
      adr = GPS_ADR_INI+(adr-GPS_ADR_FIM);        //Se ultrapassar o fim, somar diferença ao início
    ser_str("\n--- (Ordenados) Dados do GPS = ");
    ser_dec32unz(qtd);
    ser_crlf(1);
    for (cont=0; cont<qtd; cont++){
      flash_rd_blk(adr,vet_gps,GPS_PASSO);
      ser_dec32u(cont);
      ser_spc(2);
      ser_hex32(adr);
      ser_spc(2);
      ser_gps_dados_lin(vet_gps);
      ser_crlf(1);
      adr+=GPS_PASSO;
      if (adr==GPS_ADR_FIM) adr=GPS_ADR_INI;
    }

  }
  else  ser_str("Sem registro de acidente!\n");

  ser_str("\n==> FLASH: Fim dos Dados do Acidente <==\n");
}

// FLASH: Mostrar configuração ao ligar o carro
void flash_op_mostra(void){
  int x,y;
  byte msg[20];
  //lcd_str(1,0,"SRAM - Configuracao");
  //lcd_str(2,0,"Use o Monitor Serial");
  ser_str("\n--- FLASH: Dados da Configuração ao Ligar o carro ---\n");

  // Bateu?
  x=flash_rd_16b(OP_BATEU);
  if (x==COD_SIM) ser_str("Caixa Preta Acidentada.\n");
  if (x==COD_NAO) ser_str("Caixa Preta Não Acidentada.\n");

  // Sef-Test?
  x=flash_rd_16b(OP_ST_OK);
  ser_str("Sef Test = ");
  if (x==COD_SIM) ser_str("OK.\n");
  if (x==COD_NAO) ser_str("NOK.\n");

  // Calibração de Fábrica?
  x=flash_rd_16b(OP_CF_OK);
  ser_str("Calibracao de Fabrica = ");
  if (x==COD_SIM) ser_str("SIM.\n");
  if (x==COD_NAO) ser_str("NAO.\n");

  // Qtd de medidas Acel e Giro
  x=flash_rd_16b(OPC_QTD_AG);
  ser_dec16unz(x);
  ser_str(" medidas para Calibrar ao Ligar");
  x=flash_rd_16b(OPC_ESC_AC);
  str_fs_acel(x,msg);
  ser_str("\nCalibra Escala Acel = ");  ser_str(msg);
  x=flash_rd_16b(OPC_ESC_GI);
  str_fs_giro(x,msg);
  ser_str("\nCalibra Escala Giro = ");  ser_str(msg);
  ser_str("\nCalibracao (ax-ay-az-tp-gx-gy-gz): ");
  for (x=0; x<7; x++){
    ser_dec16(flash_rd_16b(OPC_AX+2*x));
    ser_spc(1);
  }

  //Parâmetros operação e limiares
  x=flash_rd_16b(OP_FREQ_AG);
  x=1000/(x+1);
  ser_str("\nFreq de Amostragem = ");
  ser_dec16unz(x);
  ser_str(" Hz");
  x=flash_rd_16b(OP_ESC_AC);
  str_fs_acel(x,msg);
  ser_str("\nOpera Escala Acel = ");  ser_str(msg);
  x=flash_rd_16b(OP_ESC_GI);
  str_fs_giro(x,msg);
  ser_str("\nOpera Escala Giro = ");  ser_str(msg);
  ser_str("\nLimiares de disparo (ax-ay-az-gx-gy-gz): ");
  for (x=0; x<6; x++){
    ser_dec16unz(flash_rd_16b(OP_LIM_AX+2*x));
    ser_spc(1);
  }

  //Quem disparou
  ser_str("\nMPU: Disparo no endereco: 0x");
  ser_hex32(flash_rd_32b(OP_MPU_ADR));  
  ser_str("\nGPS: Disparo no endereco: 0x");
  ser_hex32(flash_rd_32b(OP_GPS_ADR));  
  ser_str("\nQuem Disparou: AX AY AZ GX GY GZ\n");
  ser_spc(15);
  for (x=0; x<6; x++){
    y=flash_rd_16b(OP_DISP_AX+2*x);
    if (y==COD_SIM) ser_str(" S ");
    if (y==COD_NAO) ser_str(" N ");
  }
  
  //Data e Hora
  flash_rd_str(OP_AC_DATA, msg, 14);
  ser_str("\nData do acidente: ");
  ser_char(msg[0]); ser_char(msg[1]); ser_char('/'); 
  ser_char(msg[2]); ser_char(msg[3]); ser_char('/'); 
  ser_char(msg[4]); ser_char(msg[5]);
  
  flash_rd_str(OP_AC_HORA, msg, 14);
  ser_str("\nHora do acidente: ");
  ser_char(msg[0]); ser_char(msg[1]); ser_char(':'); 
  ser_char(msg[2]); ser_char(msg[3]); ser_char(':'); 
  ser_char(msg[4]); ser_char(msg[5]);

  ser_str("\n--- FLASH: Fim dos Dados da Configuração ao Ligar o carro ---\n");
}

// Dump da FLASH
void flash_dump(long adr, long qtd){
  long i=0;
  byte vt[16];
  for (i=0; i<qtd; i+=16){
    if ( (i%256) == 0)  ser_crlf(1);
    flash_rd_blk(adr+i,vt,16);
    ser_dump_memo(adr+i,vt);
  }
}


// Ler um valor de 32 bits da FLASH
// Big Endian
long flash_rd_32b(long adr){
  long x;
  x=flash_rd(adr++);           //MSB
  x = (x<<8)|flash_rd(adr++);  
  x = (x<<8)|flash_rd(adr++);  
  x = (x<<8)|flash_rd(adr);    //LSB
  return x;
}

// Escrever um valor de 32 bits na FLASH
// Big Endian
void flash_wr_32b(long adr, long dado){
  flash_wr(adr++,(dado>>24)&0xFF); //MSB
  flash_wr(adr++,(dado>>16)&0xFF);
  flash_wr(adr++,(dado>>8)&0xFF);
  flash_wr(adr,dado&0xFF);         //LSB
}


// Ler um valor de 16 bits da FLASH
// Big Endian
int flash_rd_16b(long adr){
  int x;
  x=flash_rd(adr++);             //MSB
  x = (x<<8)|flash_rd(adr);    //LSB
  return x;
}

// Escrever um valor de 16 bits na FLASH
// Big Endian
void flash_wr_16b(long adr, int dado){
  int x;
  flash_wr(adr++,dado>>8);     //MSB
  flash_wr(adr,dado&0xFF);     //LSB
}

// Ler uma string da FLASH
// O ponteiro msg deve ter espaço adequado
// Qtd indica a quantidade máxima, o último byte é o zero final
void flash_rd_str(long adr, byte *msg, word qtd){
  word i=0,cont=0;
  byte x;
  if (qtd > 0){
    do{
      x=flash_rd(adr++);
      msg[i++]=x;
      cont++;
    }
    while( x!='\0' && cont<qtd);
    if (cont==qtd)  msg[qtd-1]='\0';
  }
  else  msg[0]='\0';
}

// Gravar uma string na FLASH
void flash_wr_str(long adr, byte *msg){
  word i=0;
  while( msg[i] != '\0')  flash_wr(adr++, msg[i++]);
  flash_wr(adr, '\0');   //Gravar o zero final
}

// (90) Escrever um bloco com FF
// qtd --> precisa estar dentro de página de 128 bytes
void flash_apg_blk(long adr, byte qtd){
  byte ct;
  byte er,et;

  // Selecionar endereço do escravo
  er=flash_qual(adr);
  er=er<<1;   //Escravo receptor
  et=er+1;    //Escravo transmissor

  twi_start(90);                //START
  twi_er(er, 91);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 92);      //MSB do endereço
  twi_dado_er(adr&0xFF, 93);    //LSB do endereço
  for (ct=0; ct<qtd; ct++)
    twi_dado_er(0xFF, 94);      //0xFF a ser escrito
  twi_stop();                   //Gerar STOP para finalizar
}

// (80) Ler um bloco a partir de um endereço
// qtd --> não há limitação dentro dos 16 bits (64 K)
void flash_rd_blk(long adr,  byte *vet, word qtd){
  word ct;
  byte er,et;

  //ser_str("\nEntrou flash rd adr = ");
  //ser_hex32(adr);

  // Selecionar endereço do escravo
  er=flash_qual(adr);
  er=er<<1;   //Escravo receptor
  et=er+1;    //Escravo transmissor

  twi_start(80);                //START
  twi_er(er, 81);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 82);      //MSB do endereço
  twi_dado_er(adr&0xFF, 83);    //LSB do endereço
  twi_start_rep(84);            //START Repetido
  twi_et(et, 85);        //Endereçar 24LCxxx para leitura
  for (ct=0; ct<qtd-1; ct++)
    vet[ct]=twi_dado_et_ack(86);//Ler um dado com ACK
  vet[ct]=twi_dado_et_nack(87); //Ler último dado com NACK
  twi_stop();                   //Gerar STOP para finalizar
  //ser_str(" Saiu flash rd\n");
}

// (70) Escrever um bloco a partir de um endereço
// qtd --> precisa estar dentro de página de 128 bytes
void flash_wr_blk(long adr, byte *vet, byte qtd){
  byte ct;
  byte er,et;

//  ser_str("\nEntrou flash wr adr = ");
//  ser_hex32(adr);
  // Selecionar endereço do escravo
  er=flash_qual(adr);
  er=er<<1;   //Escravo receptor
  et=er+1;    //Escravo transmissor

  twi_start(70);                //START
  twi_er(er, 71);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 72);      //MSB do endereço
  twi_dado_er(adr&0xFF, 73);    //LSB do endereço
  for (ct=0; ct<qtd; ct++)
    twi_dado_er(vet[ct], 74);   //Dado  a ser escrito
  twi_stop();                   //Gerar STOP para finalizar
//  ser_str(" Saiu flash wr\n");
}

// (50) Ler um endereço da memória
byte flash_rd(long adr) {
  byte dado,er,et;

  // Selecionar endereço do escravo
  er=flash_qual(adr);
  //ser_str("er=");
  //ser_hex8(er);
  //ser_crlf(1);
  er=er<<1;   //Escravo receptor
  et=er+1;    //Escravo transmissor
  
  twi_start(50);                //START
  twi_er(er, 51);               //Endereçar Escravo receptor
  twi_dado_er(adr>>8, 52);      //MSB do endereço
  twi_dado_er(adr&0xFF, 53);    //LSB do endereço
  twi_start_rep(54);            //START Repetido
  twi_et(et, 55);               //Endereçar Escravo transmissor
  dado = twi_dado_et_nack(56);  //Receber dado da 24LCxxx com NACK
  twi_stop();                   //Gerar STOP para finalizar
  return dado;
}

// (60) Escrever num endereço da memória
void flash_wr(long adr, byte dado) {
  byte er,et;

  // Selecionar endereço do escravo
  er=flash_qual(adr);
  //ser_str("er=");
  //ser_hex8(er);
  //ser_crlf(1);
  er=er<<1;   //Escravo receptor
  et=er+1;    //Escravo transmissor
  
  twi_start(60);                //START
  twi_er(er, 61);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 62);      //MSB do endereço
  twi_dado_er(adr&0xFF, 63);    //LSB do endereço
  twi_dado_er(dado, 64);        //Dado  a ser escrito
  twi_stop();                   //Gerar STOP para finalizar
}

// Retornar endereço TWI da flash em função do endereço a ser acessado.
byte flash_qual(long adr){
  byte qual;
  qual=(adr>>16);
  switch(qual){
    case 0: return  FLASH1_ADR+0;  //0 0000 --> 0 FFFF
    case 1: return  FLASH1_ADR+4;  //1 0000 --> 1 FFFF
    case 2: return  FLASH2_ADR+0;  //2 0000 --> 2 FFFF
    case 3: return  FLASH2_ADR+4;  //3 0000 --> 3 FFFF
  }
}

// (100) Esperar Flash terminar gravação
// Ficar acessando com WR até ela responder
void flash_espera(long adr){
  byte er;
  er=flash_qual(adr);
  er=er<<1;
  while(TRUE){
    twi_start(100);                //START
    if (twi_er_check(er, 101) == TRUE){
      twi_stop();
      break;
    }
    twi_stop();
  }
}
