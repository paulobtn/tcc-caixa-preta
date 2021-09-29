// SRAM (SPI) - Rotinas para acesso
// CXP - Caixa Preta
// 22/01/2019
//
// 23LC1024 - SRAM SPI de 128 KB

// Copiar toda a SRAM para a FLASH
// Gasta 9,2 seg.
// #define FLASH_PAG   128
// #define SRAM_TAM_CHIP 0x20000L = Tamanho de um Chip SRAM
// Cada Flash tem 128 K e são 128 bytes por pagina
// São 1024 páginas x 3 mseg por pag  = 3,072  seg
// Com 2 flash temos 6,144 seg
// Então temos 3 seg gastos na comunicação SPI e TWI
void sram_flash(void){
  byte buf[FLASH_PAG];
  long adr=0;

  //Cada laço gasta 9,186 mseg
  // 1K x 9,186 ms = 9,4 seg no total
  while (adr<SRAM_TAM_CHIP){
    SCP1();
    sram_rd_blk(adr,buf,FLASH_PAG);
    flash_wr_blk(adr,buf,FLASH_PAG);
    scp1();
    sram_rd_blk(adr+0x20000L,buf,FLASH_PAG);
    flash_wr_blk(adr+0x20000L,buf,FLASH_PAG);
    adr+=FLASH_PAG;
  }
}

// Operação - Imprimir dados da Calibração ao Ligar o carro na SRAM
// Não imprime qualquer tipo de rótulo, só os dados
void sram_op_dados(void){
  int x,y;
  long xl;
  byte msg[20];

  ser_dec16unz(sram_rd_16b(OP_OK));         ser_crlf(1);  //Fez Calibração ao ligar?
  ser_dec16unz(sram_rd_16b(OP_BATEU));      ser_crlf(1);  //Bateu?
  ser_dec16unz(sram_rd_16b(OP_ST_OK));      ser_crlf(1);  //Passou MPU Self-Test?
  ser_dec16unz(sram_rd_16b(OP_STH_OK));     ser_crlf(1);  //Passou MAG Self-Test?
  ser_dec16unz(sram_rd_16b(OP_CF_OK));      ser_crlf(1);  //Fez Calibração de Fábrica (acel e giro)?
  ser_dec16unz(sram_rd_16b(OP_CFH_OK));     ser_crlf(1);  //Fez Calibração do Magnetômetro?
  ser_dec16unz(sram_rd_16b(OPC_FREQ_AG));   ser_crlf(1);  //Calibração ao ligar - Freq Hz
  ser_dec16unz(sram_rd_16b(OPC_BW_AG));     ser_crlf(1);  //Calibração ao ligar - BW
  ser_dec16unz(sram_rd_16b(OPC_ESC_AC));    ser_crlf(1);  //Calibração ao ligar - Escala Acelerômetro
  ser_dec16unz(sram_rd_16b(OPC_ESC_GI));    ser_crlf(1);  //Calibração ao ligar - Escala Giroscópio
  ser_dec16unz(sram_rd_16b(OPC_QTD_AG));    ser_crlf(1);  //Calibração ao ligar - Qtd de medidas Acel e Giro
  sram_rd_blk(OPC_AX,msg,14);                             //Médias = Erro intrínseco
  ser_lin_ac_tp_gi(msg); 
  ser_dec16unz(sram_rd_16b(OP_FREQ_AG));    ser_crlf(1);  //Operação - Freq Hz
  ser_dec16unz(sram_rd_16b(OP_BW_AG));      ser_crlf(1);  //Operação - BW
  ser_dec16unz(sram_rd_16b(OP_ESC_AC));     ser_crlf(1);  //Operação - Escala Acelerômetro
  ser_dec16unz(sram_rd_16b(OP_ESC_GI));     ser_crlf(1);  //Operação - Escala Giroscópio
  sram_rd_blk(OP_LIM_AX,msg,14);                          //Limiares para disparo
  ser_lin_ac_gi(msg); 
  ser_dec32unz(sram_rd_32b(OP_MPU_ADR));    ser_crlf(1);  //Endereço MPU no disparo
  ser_dec32unz(sram_rd_32b(OP_GPS_ADR));    ser_crlf(1);  //Endereço GPS no disparo
  ser_dec16unz(sram_rd_16b(OP_DISP_TP));    ser_crlf(1);  //Temperatura no momento do disparo
  sram_rd_blk(OP_DISP_AX,msg,14);                         //Quem disparou?
  ser_lin_ac_gi(msg); 
  ser_dec16unz(sram_rd_16b(OP_BRK));        ser_crlf(1);  //Aquisição interrompida?
  ser_dec32unz(sram_rd_32b(OP_ULT_ADR));    ser_crlf(1);  //Último endereço usado
  sram_rd_str(OP_AC_DATA, msg, 14);                       //Data do acidente
  ser_str(msg);   ser_crlf(1);
  sram_rd_str(OP_AC_HORA, msg, 14);                       //Hora do acidente
  ser_str(msg);   ser_crlf(1);
}


// Operação - Mostrar configuração ao ligar o carro na SRAM
void sram_op_mostra(void){
  int x,y;
  byte msg[20];
  //lcd_str(1,0,"SRAM - Configuracao");
  //lcd_str(2,0,"Use o Monitor Serial");
  ser_str("\n--- SRAM: Dados da Configuração ao Ligar o carro ---\n");

  // Bateu?
  x=sram_rd_16b(OP_BATEU);
  if (x==COD_SIM) ser_str("Caixa Preta Acidentada.\n");
  if (x==COD_NAO) ser_str("Caixa Preta Não Acidentada.\n");

  // Sef-Test?
  x=sram_rd_16b(OP_ST_OK);
  ser_str("Sef Test = ");
  if (x==COD_SIM) ser_str("OK.\n");
  if (x==COD_NAO) ser_str("NOK.\n");

  // Calibração de Fábrica?
  x=sram_rd_16b(OP_CF_OK);
  ser_dec16(x);
  ser_str("Calibracao de Fabrica = ");
  if (x==COD_SIM) ser_str("SIM.\n");
  if (x==COD_NAO) ser_str("NAO.\n");

  x=sram_rd_16b(OPC_FREQ_AG);
  ser_str("\n\nFreq Amost = ");   ser_dec16unz(x);      //Calibração - Freq de amostragem 
  ser_str(" Hz"); 
  
  x=sram_rd_16b(OPC_BW_AG);
  ser_str("\nBanda BW = ");   ser_dec16unz(x);          //Calibração - BW=Banda Passante
  ser_str(" Hz");
  
  x=sram_rd_16b(OPC_ESC_AC);                    
  ser_str("\nEscala Acelerometro = ");  ser_dec16unz(x);  //Calibração - Escala Acelerômetro
  ser_str(" g");
  
  x=sram_rd_16b(OPC_ESC_GI);                    
  ser_str("\nEscala Giroscopio = ");  ser_dec16unz(x);  //Calibração - Escala Giroscópio
  ser_str(" graus/seg\n");

  // Qtd de medidas Acel e Giro
  x=sram_rd_16b(OPC_QTD_AG);
  ser_dec16unz(x);
  ser_str(" medidas para Calibrar ao Ligar");

  ser_str("\nCalibracao (ax-ay-az-tp-gx-gy-gz): ");
  for (x=0; x<7; x++){
    ser_dec16(sram_rd_16b(OPC_AX+2*x));
    ser_spc(1);
  }

  //Parâmetros operação e limiares
  x=sram_rd_16b(OP_FREQ_AG);
  ser_str("\n\nOpera Freq Amost = ");   ser_dec16unz(x);      //Operação - Freq de amostragem 
  ser_str(" Hz"); 
  
  x=sram_rd_16b(OP_BW_AG);
  ser_str("\nOpera Banda BW = ");   ser_dec16unz(x);          //Operação - BW=Banda Passante
  ser_str(" Hz");
  
  x=sram_rd_16b(OP_ESC_AC);                    
  ser_str("\nOpera Escala Acelerometro = ");  ser_dec16unz(x);  //Operação - Escala Acelerômetro
  ser_str(" g");
  
  x=sram_rd_16b(OP_ESC_GI);                    
  ser_str("\nOpera Escala Giroscopio = ");  ser_dec16unz(x);  //Operação - Escala Giroscópio
  ser_str(" graus/seg");

  x=sram_rd_16b(OP_ESC_MG);                    
  ser_str("\nOpera Escala magnetometro = ");  ser_dec16unz(x);  //Operação - Escala Giroscópio
  ser_str(" ???");

  ser_str("\nLimiares de disparo (ax-ay-az-gx-gy-gz): ");
  for (x=0; x<6; x++){
    ser_dec16unz(sram_rd_16b(OP_LIM_AX+2*x));
    ser_spc(1);
  }

  //Quem disparou
  ser_str("\nMPU: Disparo no endereco ");
  ser_dec32unz(sram_rd_32b(OP_MPU_ADR));  
  ser_str("\nGPS: Disparo no endereco ");
  ser_dec32unz(sram_rd_32b(OP_GPS_ADR));  
  ser_str("\nQuem Disparou: AX AY AZ GX GY GZ\n");
  ser_spc(15);
  for (x=0; x<6; x++){
    y=sram_rd_16b(OP_DISP_AX+2*x);
    if (y==COD_SIM) ser_str(" S ");
    if (y==COD_NAO) ser_str(" N ");
  }

  ser_str("\nMPU: Ultimo endereco = ");
  ser_dec32unz(sram_rd_32b(OP_ULT_ADR));  

  ser_str("\nAquisicao interrompida: ");
  x=sram_rd_16b(OP_BRK);
  if (x==COD_SIM) ser_str("Sim");                    
  else            ser_str("Nao");                    
  
  //Data e Hora
  sram_rd_str(OP_AC_DATA, msg, 14);
  ser_str("\nData do acidente: ");
  ser_char(msg[0]); ser_char(msg[1]); ser_char('/'); 
  ser_char(msg[2]); ser_char(msg[3]); ser_char('/'); 
  ser_char(msg[4]); ser_char(msg[5]);
  
  sram_rd_str(OP_AC_HORA, msg, 14);
  ser_str("\nHora do acidente: ");
  ser_char(msg[0]); ser_char(msg[1]); ser_char(':'); 
  ser_char(msg[2]); ser_char(msg[3]); ser_char(':'); 
  ser_char(msg[4]); ser_char(msg[5]);

  ser_str("\n--- SRAM: Fim dos Dados da Configuração ao Ligar o carro ---\n");
}

// Dump da SRAM
// Sempre começa no múltiplo de 16, inferior a adr
void sram_dump(long adr, long qtd){
  long i=0;
  byte vt[16];
  adr = adr & 0xFFFFF0;
  for (i=0; i<qtd; i+=16){
    if ( (i%256) == 0)  ser_crlf(1);
    sram_rd_blk(adr+i,vt,16);
    ser_dump_memo(adr+i,vt);
  }
}


// Zerar área usada pelo MPU e GPS
// Salva Calib ao Ligar, zera tudo e depois a reescreve
void sram_zera_mpu_gps(void){
  long adr;
  byte vet[CXP_ADR_FIM-CXP_ADR_INI+20]; //Vetor para guardar calib ao ligar

  //Guardar área de config ao ligar
  sram_rd_blk(CXP_ADR_INI,vet,CXP_ADR_FIM-CXP_ADR_INI);
  
  // Zerar SRAM 0
  adr=0;
  spi_cs0();  //Selecionar SRAM0
  spi_transf(SRAM_WRITE);      //Indicar escrita
  spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
  spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((byte)adr);                //ender (7..0)
  for (adr=0; adr<SRAM_TAM_CHIP; adr++)   spi_transf(0);   //Escrever ZERO
  spi_CS0();  //Desabilitar SRAM0
  
  // Zerar SRAM 1
  adr=0;
  spi_cs1();  //Selecionar SRAM0
  spi_transf(SRAM_WRITE);      //Indicar escrita
  spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
  spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((byte)adr);                //ender (7..0)
  for (adr=0; adr<SRAM_TAM_CHIP; adr++)   spi_transf(0);   //Escrever ZERO
  spi_CS1();  //Desabilitar SRAM1

  //Gravar área de config ao ligar
  sram_wr_blk(CXP_ADR_INI,vet,CXP_ADR_FIM-CXP_ADR_INI);
}


// Ler um valor de 32 bits da SRAM
// Big Endian
long sram_rd_32b(long adr){
  long x;
  x=sram_rd(adr++);           //MSB
  x = (x<<8)|sram_rd(adr++);  
  x = (x<<8)|sram_rd(adr++);  
  x = (x<<8)|sram_rd(adr);    //LSB
  return x;
}

// Escrever um valor de 32 bits da SRAM
// Big Endian
void sram_wr_32b(long adr, long dado){
  sram_wr(adr++,(dado>>24)&0xFF); //MSB
  sram_wr(adr++,(dado>>16)&0xFF);
  sram_wr(adr++,(dado>>8)&0xFF);
  sram_wr(adr,dado&0xFF);         //LSB
}


// Ler um valor de 16 bits da SRAM
// Big Endian
int sram_rd_16b(long adr){
  int x;
  x=sram_rd(adr++);             //MSB
  x = (x<<8)|sram_rd(adr);    //LSB
  return x;
}

// Escrever um valor de 16 bits da SRAM
// Big Endian
void sram_wr_16b(long adr, int dado){
  int x;
  sram_wr(adr++,dado>>8);     //MSB
  sram_wr(adr,dado&0xFF);     //LSB
}

// Ler uma string da SRAM
// O ponteiro msg deve ter espaço adequado
// Qtd indica a quantidade máxima, o último byte é o zero final
void sram_rd_str(long adr, byte *msg, word qtd){
  word i=0,cont=0;
  byte x;
  if (qtd > 0){
    do{
      x=sram_rd(adr++);
      msg[i++]=x;
      cont++;
    }
    while( x!='\0' && cont<qtd);
    if (cont==qtd)  msg[qtd-1]='\0';
  }
  else  msg[0]='\0';
}

// Gravar uma string na SRAM
void sram_wr_str(long adr, byte *msg){
  word i=0;
  while( msg[i] != '\0')  sram_wr(adr++, msg[i++]);
  sram_wr(adr, '\0');   //Gravar o zero final
}

// Zerar toda a SRAM
// Nova rotina para ficar rápido
void sram_zera(void){
  long adr;

  // Zerar SRAM 0
  adr=0;
  spi_cs0();  //Selecionar SRAM0
  spi_transf(SRAM_WRITE);      //Indicar escrita
  spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
  spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((byte)adr);                //ender (7..0)
  for (adr=0; adr<SRAM_TAM_CHIP; adr++)   spi_transf(0);   //Escrever ZERO
  spi_CS0();  //Desabilitar SRAM0
  
  // Zerar SRAM 1
  adr=0;
  spi_cs1();  //Selecionar SRAM0
  spi_transf(SRAM_WRITE);      //Indicar escrita
  spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
  spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((byte)adr);                //ender (7..0)
  for (adr=0; adr<SRAM_TAM_CHIP; adr++)   spi_transf(0);   //Escrever ZERO
  spi_CS1();  //Desabilitar SRAM1
}


// Escrever uma sequência
// se adr = 0x0 0000 -> 0x1 FFFF, SRAM0 #CS0
// se adr = 0x2 0000 -> 0x3 FFFF, SRAM1 #CS1
// Faz a transição de 0x1FFFF para 0x20000
// **==>> NÃO faz a volta de 0x3FFFF para 0x00000 (fica na pag 1: volta para 0x20000)
void sram_wr_blk(long adr, byte *vet, word qtd){
  word cont,aux;
  long adrf;
  adrf=adr+qtd-1;
  if ( (adr&0xFE0000L) == (adrf&0xFE0000L) ){ //Verificar fronteira de 128K
    //ser_str("\nWR Pag1\n");
    if ((adr&0xF0000L)<0x20000L)  spi_cs0();  //Selecionar SRAM0
    else                          spi_cs1();  //Selecionar SRAM1
    spi_transf(SRAM_WRITE);      //Indicar escrita
    spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
    spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
    spi_transf((byte)adr);                //ender (7..0)
    // Escrever os bytes indicados
    for (cont=0; cont<qtd; cont++)  spi_transf(vet[cont]);
  }
  else{
    //ser_str("\nWR Pag2\n");
    aux=SRAM_TAM_CHIP-adr;      //Quantos para pag 1, SRAM_TAM_CHIP = 0x20000L
    spi_cs0();
    spi_transf(SRAM_WRITE);      //Indicar escrita
    spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
    spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
    spi_transf((byte)adr);                //ender (7..0)
    for (cont=0; cont<aux; cont++)  spi_transf(vet[cont]);
    spi_CS0();
    qtd=qtd-aux;              //Quantos na pag 1, 
    adr=SRAM_TAM_CHIP;        //SRAM_TAM_CHIP = 0x20000L
    spi_cs1();
    spi_transf(SRAM_WRITE);      //Indicar escrita
    spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
    spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
    spi_transf((byte)adr);                //ender (7..0)
    for (cont=0; cont<qtd; cont++)  spi_transf(vet[aux+cont]);
    spi_CS1();
  }
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
}
/*
void sram_wr_blk(long adr, byte *vet, word qtd){
  word cont;
  if ((adr&0xF0000L)<0x20000L)  spi_cs0();  //Selecionar SRAM0
  else                          spi_cs1();  //Selecionar SRAM1
  spi_transf(SRAM_WRITE);      //Indicar escrita
  spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
  spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((byte)adr);                //ender (7..0)
  // Escrever os bytes indicados
  for (cont=0; cont<qtd; cont++)  spi_transf(vet[cont]);
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
}
*/

// Ler uma sequência
// se adr = 0x0 0000 -> 0x1 FFFF, SRAM0 #CS0
// se adr = 0x2 0000 -> 0x3 FFFF, SRAM1 #CS1
// Faz a transição de 0x1FFFF para 0x20000
// **==>> NÃO faz a volta de 0x3FFFF para 0x00000 (fica na pag 1: volta para 0x20000)
void sram_rd_blk(long adr, byte *vet, word qtd){
  word cont,aux;
  long adrf;
  adrf=adr+qtd-1;
  if ( (adr&0xFE0000L) == (adrf&0xFE0000L) ){ //Verificar pag de 128K
    //ser_str("\nRD Pag1\n");
    if ((adr&0xF0000L)<0x20000L)  spi_cs0();  //Selecionar SRAM0
    else                          spi_cs1();  //Selecionar SRAM1
    spi_transf(SRAM_READ);       //Indicar leitura
    spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
    spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
    spi_transf((byte)adr);                //ender (7..0)
    // Ler os bytes indicados
    for (cont=0; cont<qtd; cont++)  vet[cont] = spi_transf(0x00);
  }
  else{
    //ser_str("\nRD Pag2\n");
    aux=SRAM_TAM_CHIP-adr;      //Quantos para pag 1, SRAM_TAM_CHIP = 0x20000L
    spi_cs0();
    spi_transf(SRAM_READ);       //Indicar leitura
    spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
    spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
    spi_transf((byte)adr);                //ender (7..0)
    for (cont=0; cont<aux; cont++)  vet[cont] = spi_transf(0x00);
    spi_CS0();
    qtd=qtd-aux;              //Quantos n pag 1, 
    adr=SRAM_TAM_CHIP;        //SRAM_TAM_CHIP = 0x20000L
    spi_cs1();
    spi_transf(SRAM_READ);       //Indicar leitura
    spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
    spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
    spi_transf((byte)adr);                //ender (7..0)
    for (cont=0; cont<qtd; cont++)  vet[aux+cont] = spi_transf(0x00);
    spi_CS1();
  }
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
}
/*
void sram_rd_blk(long adr, byte *vet, word qtd){
  word cont;
  if ((adr&0xF0000L)<0x20000L)  spi_cs0();  //Selecionar SRAM0
  else                          spi_cs1();  //Selecionar SRAM1
  spi_transf(SRAM_READ);       //Indicar leitura
  spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
  spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((byte)adr);                //ender (7..0)
  // Ler os bytes indicados
  for (cont=0; cont<qtd; cont++)  vet[cont] = spi_transf(0x00);
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
}
*/

// Escrever o dado no endereço adr da SRAM
// se adr = 0x0 0000 -> 0x1 FFFF, SRAM0 #CS0
// se adr = 0x2 0000 -> 0x3 FFFF, SRAM1 #CS1
void sram_wr(long adr, byte dado){
  if ((adr&0xF0000L)<0x20000L)  spi_cs0();  //Selecionar SRAM0
  else                          spi_cs1();  //Selecionar SRAM1
  spi_transf(SRAM_WRITE);      //Indicar escrita
  spi_transf((byte)(adr >> 16) & 0xff); //ender (23..16)
  spi_transf((byte)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((byte)adr);                //ender (7..0)
  spi_transf(dado);       //Escrever um Byte
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
}

// Ler o endereço adr da SRAM
// se adr = 0x0 0000 -> 0x1 FFFF, SRAM0 #CS0
// se adr = 0x2 0000 -> 0x3 FFFF, SRAM1 #CS1
byte sram_rd(long adr){
  byte dado;
  if ((adr&0xF0000L)<0x20000L)  spi_cs0();  //Selecionar SRAM0
  else                          spi_cs1();  //Selecionar SRAM1
  spi_transf(SRAM_READ);       //Indicar leitura
  spi_transf((uint8_t)(adr  >> 16) & 0xff); //ender (23..16)
  spi_transf((uint8_t)(adr >> 8) & 0xff);  //ender (15..8)
  spi_transf((uint8_t)adr);                //ender (7..0)
  dado = spi_transf(0);    //Ler um Byte
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
  return dado;
}

// Escrever no registrador de modo
// qual=0 --> SRAM0 #CS0 (0x0 0000 -> 0x1 FFFF)
// qual=1 --> SRAM1 #CS1 (0x2 0000 -> 0x3 FFFF)
void sram_modo_wr(byte qual, byte dado){
  if (qual==0)  spi_cs0();   //Selecionar SRAM0
  else          spi_CS1();   //Selecionar SRAM1
  spi_transf(SRAM_WRMR);  //Indicar escrita no Reg de Modo
  //spi_transf(modo);        //Escrever o Modo
  spi_transf(dado);        //Escrever o Modo
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
}

// Ler o registrador de modo
// qual=0 --> SRAM0 #CS0 (0x0 0000 -> 0x1 FFFF)
// qual=1 --> SRAM1 #CS1 (0x2 0000 -> 0x3 FFFF)
byte sram_modo_rd(byte qual){
  byte dado;
  if (qual==0)  spi_cs0();   //Selecionar SRAM0
  else          spi_CS1();   //Selecionar SRAM1
  spi_transf(SRAM_RDMR);    //Indicar escrita no Reg de Modo
  dado=spi_transf(0);       //Ler o Modo
  spi_CS0();  //Mais fácil desabilitar as duas
  spi_CS1();  //Mais fácil desabilitar as duas
  return dado;
}

// Enviar um Byte pela porta SPI
// Rotina envia e recebe um Byte ao mesmo tempo
byte spi_transf(byte dado){
  SPDR = dado;
  while ( !(SPSR & (1<<SPIF)));
  return SPDR;
}

// CS0 (PL0) = SRAM 0 (23LC1024)
void spi_cs0(void){ PORTL &= ~(1<<DDL0);  }  //CS0=LOW
void spi_CS0(void){ PORTL |=  (1<<DDL0);  }  //CS0=HIGH

// CS1 (PL1) = SRAM 1 (23LC1024)
void spi_cs1(void){ PORTL &= ~(1<<DDL1);  }  //CS1=LOW
void spi_CS1(void){ PORTL |=  (1<<DDL1);  }  //CS1=HIGH

// CS2 (PL2) = SRAM 2 (W25Q64)
void spi_cs2(void){ PORTL &= ~(1<<DDL2);  }  //CS2=LOW
void spi_CS2(void){ PORTL |=  (1<<DDL2);  }  //CS2=HIGH

// Inicializar porta SPI do Mega
void spi_config(byte clk){

  DDRB  |= (1<<DDB2)|(1<<DDB1)|(1<<DDB0);   //MOSI=SCK=SS=Saída
  PORTB |= (1<<DDB2)|(1<<DDB1)|(1<<DDB0);   //MOSI=SCK=SS=1
  DDRB  &= ~(1<<DDB3);                      //MISO=Entrada
  PORTB |= (1<<DDB3);                       //MISO=com pullup
  DDRL  |= (1<<DDL2)|(1<<DDL1)|(1<<DDL0);   //CS2=CS1=CS0=Saída
  PORTL |= (1<<DDL2)|(1<<DDL1)|(1<<DDL0);   //CS2=CS1=CS0=1

  // Incializar porta SPI
  SPCR = ((1<<SPE) | (1<<MSTR));  //Hab. SPI Mestre, (SPR1=SPR0=0)
  SPSR = 0;                       //Desab Int e SPI2X=0
  switch(clk){
    case SPI_125K:                      SPCR |= (1<<SPR1) | (1<<SPR0);  break;
    case SPI_250K:                      SPCR |= (1<<SPR1);              break;
    case SPI_500K: SPSR |= (1<<SPI2X);  SPCR |= (1<<SPR1);              break;
    case SPI_1M:                        SPCR |= (1<<SPR0);              break;
    case SPI_2M:   SPSR |= (1<<SPI2X);  SPCR |= (1<<SPR0);              break;
    case SPI_4M:                                                        break;
    case SPI_8M:   SPSR |= (1<<SPI2X);                                  break;
    defualt:                            SPCR |= (1<<SPR1) | (1<<SPR0);  break;
  }
}
