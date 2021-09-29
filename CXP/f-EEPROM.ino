// EEPROM - Rotinas básicas
// CXP - Caixa Preta
// ATMEGA2560 tem 4KB (4.906) bytes de EEPROM 0x0000 --> 0x3FFF

// Imprimir só os dados da Calibração de Fábrica gravada na EEPROM
byte eeprom_cf_dados(void){
  int x;
  long z;
  byte msg[30];

  ser_dec16((x=eeprom_rd_16b(CF_OK))); ser_crlf(1); //Calibração de Fábrica ?

  if (x == 0x5353){     // Se houver Calibração de Fábrica
    eeprom_rd_str(CF_DATA,msg,26);    ser_str(msg); ser_crlf(1);  //Data
    eeprom_rd_str(CF_LOCAL,msg,26);   ser_str(msg); ser_crlf(1);  //Local
    eeprom_rd_str(CFG_PADRAO,msg,26); ser_str(msg); ser_crlf(1);  //g Padrao
    eeprom_rd_str(CFG_LOCAL,msg,26);  ser_str(msg); ser_crlf(1);  //g Local
    ser_dec16u(eeprom_rd_16b(CFG_PADRAO_BIN));      ser_crlf(1);  //g Padrao binário
    ser_dec16u(eeprom_rd_16b(CFG_LOCAL_BIN));       ser_crlf(1);  //g Local binário
    ser_dec16unz(eeprom_rd_16b(CF_WHO));            ser_crlf(1);  //Resposta ao Who am I em decimal
    ser_dec16unz(eeprom_rd_16b(CF_FA));             ser_crlf(1);  //Freq de amostragem em Hz
    ser_dec16unz(eeprom_rd_16b(CF_BW));             ser_crlf(1);  //Banda passante do filtro
    ser_dec16unz(eeprom_rd_16b(CF_ESC_AC));         ser_crlf(1);  //Escala do acelerômetro
    ser_dec16unz(eeprom_rd_16b(CF_ESC_GI));         ser_crlf(1);  //Escala do giroscópio
    ser_dec16unz(eeprom_rd_16b(CF_QTD));            ser_crlf(1);  //Qtd de medidas para a média
    eeprom_rd_blk(CF_AX,msg,14);  ser_lin_ac_tp_gi(msg);          //Médias = Erro intrínseco
    eeprom_rd_blk(CF_AX_SOMA,msg,28);                             //Somatorio por eixo (32 bits): 
    for (x=0; x<28; x+=4){
      z=msg[x];
      z = (z<<8)+msg[x+1];
      z = (z<<8)+msg[x+2];
      z = (z<<8)+msg[x+3];
      ser_dec32unz(z);
      ser_spc(1);
    }
    ser_crlf(1);
    eeprom_rd_blk(CF_AX_PRI,msg,14);  ser_lin_ac_tp_gi(msg);      //Primeira medida
    eeprom_rd_blk(CF_AX_ULT,msg,14);  ser_lin_ac_tp_gi(msg);      //Ultima medida
    ser_dec16(eeprom_rd_16b(CF_ST_OK));             ser_crlf(1);  //Self Test 
    eeprom_rd_blk(CF_ST_OFF_AX,msg,12); ser_lin_ac_gi(msg);       //Leituras Self Test OFF
    eeprom_rd_blk(CF_ST_ON_AX,msg,12);  ser_lin_ac_gi(msg);       //Leituras Self Test ON
    eeprom_rd_blk(CF_ST_REG_AX,msg,12); ser_lin_ac_gi(msg);       //Leituras Regs de Self Test
    eeprom_rd_blk(CF_ST_TOL_AX,msg,12); ser_lin_ac_gi(msg);       //Resultado Self test
  }   //Fim Calibração de Fábrica Aceleração e Giro

  // Calibração do Magnetômetro?
  ser_dec16((x=eeprom_rd_16b(CF_MAG_OK)));  ser_crlf(1);          //Fez Calibração do Magnetômetro?
  if (x==COD_SIM){
    ser_dec16nz(eeprom_rd_16b(CF_STH_OK));  ser_crlf(1);          //Self Teste do Magnetômetro
    ser_dec16nz(eeprom_rd_16b(CF_STH_HX));  ser_spc(1);           //hx Self test
    ser_dec16nz(eeprom_rd_16b(CF_STH_HY));  ser_spc(1);           //hy Self test
    ser_dec16nz(eeprom_rd_16b(CF_STH_HZ));  ser_crlf(1);          //hz Self test
    ser_dec16nz(eeprom_rd_16b(CF_HX_ASA));  ser_spc(1);           //ASA hx
    ser_dec16nz(eeprom_rd_16b(CF_HY_ASA));  ser_spc(1);           //ASA hy
    ser_dec16nz(eeprom_rd_16b(CF_HZ_ASA));  ser_crlf(1);          //ASA hz
    ser_dec16nz(eeprom_rd_16b(CF_HX_OFF));  ser_spc(1);           //Offset hx (dividir por 10)
    ser_dec16nz(eeprom_rd_16b(CF_HY_OFF));  ser_spc(1);           //Offset hy (dividir por 10)
    ser_dec16nz(eeprom_rd_16b(CF_HZ_OFF));  ser_crlf(1);          //Offset hz (dividir por 10)
    ser_dec16nz(eeprom_rd_16b(CF_HX_ESC));  ser_spc(1);           //Escala hx (dividir por 10)
    ser_dec16nz(eeprom_rd_16b(CF_HY_ESC));  ser_spc(1);           //Escala hy (dividir por 10)
    ser_dec16nz(eeprom_rd_16b(CF_HZ_ESC));  ser_crlf(1);          //Escala hz (dividir por 10)
  }
  return TRUE;
}

// Mostrar Calibração de Fábrica gravada na EEPROM
byte eeprom_cf_mostra(void){
  int x;
  long z;
  byte msg[30];

  ser_str("\n--- EEPROM: Dados da Calibração de Fábrica ---");
  // Verificar se já fez calibração ASCII(S)=0x53
  x=eeprom_rd_16b(CF_OK);
  if (x==COD_SIM){
    eeprom_rd_str(CF_DATA,msg,26);       
    ser_str("\nData = ");   ser_str(msg);           //Data
    eeprom_rd_str(CF_LOCAL,msg,26);       
    ser_str("\nLocal = ");   ser_str(msg);          //Local
    eeprom_rd_str(CFG_PADRAO,msg,26);        
    ser_str("\ng Padrao = ");   ser_str(msg);       //g Padrao
    x=eeprom_rd_16b(CFG_PADRAO_BIN);                
    ser_str(" (");  ser_dec16u(x);  ser_str(")");
    eeprom_rd_str(CFG_LOCAL,msg,26);        
    ser_str("\ng Local = ");   ser_str(msg);        //g Local
    x=eeprom_rd_16b(CFG_LOCAL_BIN);                
    ser_str(" (");  ser_dec16u(x);  ser_str(")");
    x=eeprom_rd_16b(CF_WHO);                        
    ser_str("\nWho am I = 0x");   ser_hex16(x);                       //Resposta ao Who am I
    x=eeprom_rd_16b(CF_FA);                    
    ser_str("\n\nFreq Amost = ");   ser_dec16unz(x);               //Freq de amostragem
    ser_str(" Hz");
    x=eeprom_rd_16b(CF_BW);
    ser_str("\nBanda BW = ");   ser_dec16unz(x);                       //BW=Banda Passante
    ser_str(" Hz");
    x=eeprom_rd_16b(CF_ESC_AC);                    
    ser_str("\nEscala Acelerometro = ");  ser_dec16unz(x);        //Escala Acelerômetro
    ser_str(" g");
    x=eeprom_rd_16b(CF_ESC_GI);                    
    ser_str("\nEscala Giroscopio = ");  ser_dec16unz(x);  //Escala Acelerômetro
    ser_str(" graus/seg");
    
    ser_crlf(1);
    ser_spc(20);
    ser_str("\nax     ay     az     tp     gx     gy     gz");
    ser_str("\nErro intrínseco:   ");
    eeprom_rd_blk(CF_AX,msg,14);
    ser_lin_ac_tp_gi(msg);
  
    ser_str("Primeira medida:  ");
    eeprom_rd_blk(CF_AX_PRI,msg,14);
    ser_lin_ac_tp_gi(msg);
  
    ser_str("Ultima medida:    ");
    eeprom_rd_blk(CF_AX_ULT,msg,14);
    ser_lin_ac_tp_gi(msg);
  
    ser_str("Somatorio por eixo (32 bits): ");
    eeprom_rd_blk(CF_AX_SOMA,msg,28);
    for (x=0; x<28; x+=4){
      z=msg[x];
      z = (z<<8)+msg[x+1];
      z = (z<<8)+msg[x+2];
      z = (z<<8)+msg[x+3];
      ser_dec32nz(z);
      ser_spc(1);
    }
    ser_crlf(1);
    ser_dec16unz(eeprom_rd_16b(CF_QTD));
    ser_str(" = Medidas para a Media\n");
  
    
    ser_str("\nSelf Test = ");
    x=eeprom_rd_16b(CF_ST_OK);
    if (x==COD_SIM) ser_str("OK!\n");
    else            ser_str("FALHOU!\n");
  
    ser_spc(17);
    ser_str("ax     ay     az     gx     gy     gz\n");
    ser_str("Self Test OFF: ");
    eeprom_rd_blk(CF_ST_OFF_AX,msg,12);
    ser_lin_ac_gi(msg);
    
    ser_str("Self Test ON:  ");
    eeprom_rd_blk(CF_ST_ON_AX,msg,12);
    ser_lin_ac_gi(msg);
  
    ser_str("Registradores: ");
    eeprom_rd_blk(CF_ST_REG_AX,msg,12);
    ser_lin_ac_gi(msg);
    
    ser_str("Result (<14%): ");
    eeprom_rd_blk(CF_ST_TOL_AX,msg,12);
    ser_lin_ac_gi(msg);
  }
  else ser_str("\nSem calibragem acel e giro!\n");

  x=eeprom_rd_16b(CF_MAG_OK);
  if (x==COD_SIM){
    ser_str("Magnetometro:\nASA: ");
    ser_dec16nz(eeprom_rd_16b(CF_HX_ASA));  ser_spc(1);
    ser_dec16nz(eeprom_rd_16b(CF_HY_ASA));  ser_spc(1);
    ser_dec16nz(eeprom_rd_16b(CF_HZ_ASA));  ser_spc(1);
    ser_str("\nOffset: ");
    ser_dec16nz(eeprom_rd_16b(CF_HX_OFF));  ser_spc(1);
    ser_dec16nz(eeprom_rd_16b(CF_HY_OFF));  ser_spc(1);
    ser_dec16nz(eeprom_rd_16b(CF_HZ_OFF));  ser_spc(1);
    ser_str("\nEscala: ");
    ser_dec16nz(eeprom_rd_16b(CF_HX_ESC));  ser_spc(1);
    ser_dec16nz(eeprom_rd_16b(CF_HY_ESC));  ser_spc(1);
    ser_dec16nz(eeprom_rd_16b(CF_HZ_ESC));  ser_spc(1);
  }
  else ser_str("\nSem calibragem Mag!\n");

  //mostra dados de calibração do acelerômetro
  ser_crlf(1);
  ser_crlf(1);
  ser_str("Dados utlizados na calibração do acelerômetro:\n");
  ser_str("ax_x_cima ax_x_baixo ay_y_cima ay_y_baixo az_z_cima az_z_baixo\n");
  ser_dec16nz(eeprom_rd_16b(CF_AX_X_CIMA));  ser_spc(1); 
  ser_dec16nz(eeprom_rd_16b(CF_AX_X_BAIXO));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_AX_Y_CIMA));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_AX_Y_BAIXO));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_AX_Z_CIMA));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_AX_Z_BAIXO));  ser_crlf(1);

  
  //mostra dados de calibração do magnetômetro
  //hxmin, hxmax, hymin, hymax, hzmin, hazmax
  ser_crlf(1);
  ser_str("Dados utlizados na calibração do magnetômetro:\n");
  ser_str("hxmin hxmax hymin hymax hzmin hazmax\n");
  ser_dec16nz(eeprom_rd_16b(CF_HX_MIN));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_HX_MAX));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_HY_MIN));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_HY_MAX));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_HZ_MIN));  ser_spc(1);
  ser_dec16nz(eeprom_rd_16b(CF_HZ_MAX));  ser_crlf(1);

  
  ser_str("\n--- EEPROM: Fim Calibração de Fábrica ---\n\n");
  //sram_op_mostra();
  //sram_dump(CXP_ADR_INI,256);

  return TRUE;
}

// Dump da EEPROM
void eeprom_dump(word adr, word qtd){
  word i=0;
  byte vt[16];
  for (i=0; i<qtd; i+=16){
    if ( (i%256) == 0)  ser_crlf(1);
    eeprom_rd_blk(adr+i,vt,16);
    ser_dump_memo(adr+i,vt);
  }
}

// Ler um valor de 32 bits da EEPROM
long eeprom_rd_32b(word adr){
  long x;
  x=eeprom_rd(adr++);           //MSB
  x = (x<<8)|eeprom_rd(adr++);  
  x = (x<<8)|eeprom_rd(adr++);  
  x = (x<<8)|eeprom_rd(adr);    //LSB
  return x;
}

// Escrever um valor de 32 bits da EEPROM
void eeprom_wr_32b(word adr, long dado){
  eeprom_wr(adr++,(dado>>24)&0xFF); //MSB
  eeprom_wr(adr++,(dado>>16)&0xFF);
  eeprom_wr(adr++,(dado>>8)&0xFF);
  eeprom_wr(adr,dado&0xFF);         //LSB
}

// Ler um valor de 16 bits da EEPROM
int eeprom_rd_16b(word adr){
  int x;
  x=eeprom_rd(adr++);             //MSB
  x = (x<<8)|eeprom_rd(adr);      //LSB
  return x;
}

// Escrever um valor de 16 bits da EEPROM
void eeprom_wr_16b(word adr, word dado){
  int x;
  eeprom_wr(adr++,dado>>8);     //MSB
  eeprom_wr(adr,dado&0xFF);     //LSB
}

// Ler uma string da EEPROM
// O ponteiro msg deve ter espaço adequado
// Qtd indica a quantidade máxima, o último byte é o zero final
void eeprom_rd_str(word adr, byte *msg, word qtd){
  word i=0,cont=0;
  byte x;
  if (qtd > 0){
    do{
      x=eeprom_rd(adr++);
      msg[i++]=x;
      cont++;
    }
    while( x!='\0' && cont<qtd);
    if (cont==qtd)  msg[qtd-1]='\0';
  }
  else  msg[0]='\0';
}

// Gravar uma string na EEPROM
void eeprom_wr_str(word adr, byte *msg){
  word i=0;
  while( msg[i] != '\0')  eeprom_wr(adr++, msg[i++]);
  eeprom_wr(adr, '\0');   //Gravar o zero final
}

// Gravar 0xFF em toda a EEPROM
void eeprom_wr_ff(void){
  word adr;
  for (adr=0; adr<EEPROM_TAM; adr++)
    eeprom_wr(adr, 0xFF);
}

// Ler sequencia de bytes da EEPROM
void eeprom_rd_blk(word adr, byte *vet, word qtd){
  word cont;
  for (cont=0; cont<qtd; cont++)
    vet[cont]=eeprom_rd(adr+cont);
}

// Gravar sequencia de bytes na EEPROM
void eeprom_wr_blk(word adr, byte *vet, word qtd){
  word cont;
  for (cont=0; cont<qtd; cont++)
    eeprom_wr(adr+cont, vet[cont]);
}

// Ler um byte da EEPROM
byte eeprom_rd(word adr){
  while( EECR & (1<<EEPE));   //Esperar terminar gravação anterior
  EEAR = adr&0x3FFF;           //Endereço truncado em 12 bits
  EECR |= (1<<EERE);          //Programar leitura
  return EEDR;                //Retornar valor lico
}

// Gravar um byte na EEPROM
void eeprom_wr(word adr, byte dado){
  while( EECR & (1<<EEPE));   //Esperar gravação anterior
  EEAR = adr&0x3FFF;           //Endereço truncado em 12 bits
  EEDR = dado;                //Dado
  cli();                      //Desabilita interrupts
  EECR |= (1<<EEMPE);         //Master Prog = 1
  EECR |= (1<<EEPE);          //Gravar
  sei();                      //Reabilita interrupts
}
