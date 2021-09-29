// Modos de Teste
// CXP - Caixa Preta

byte linha[21];

byte teste(void){
  byte modo;    //Modo selecionado
  byte i;
  word w;
  long z;
  float f;

  while(TRUE){
    lcd_apaga();
    lcd_str(0,0,"TESTE");
    ser_str("\n==> Modo Teste <==\n");
    //eeprom_cf_mostra();
    //eeprom_cf_dados();

    while(sw_val<0x10);  //Esperar soltar tecla SEL
    lcd_str(0,5," - Selecionar");
    ser_str("Selecionar com LCD\n");
    //modo=13;
    modo=sel_modo(teste_msg, TESTE_TOT);

    lcd_apaga();
    ser_crlf(1);
    switch(modo){
      case 'O':
      case 'o':
      case TESTE_0:  return modo;
      case TESTE_1:  teste_1(modo);  break;
      case TESTE_2:  teste_2(modo);  break;
      case TESTE_3:  teste_3(modo);  break;
      case TESTE_4:  teste_4(modo);  break;
      case TESTE_5:  teste_5(modo);  break;
      case TESTE_6:  teste_6(modo);  break;
      case TESTE_7:  teste_7(modo);  break;
      case TESTE_8:  teste_8(modo);  break;
      case TESTE_9:  teste_9(modo);  break;
      case TESTE_10: teste_10(modo); break;
      case TESTE_11: teste_11(modo); break;
      case TESTE_12: teste_12(modo); break;
      case TESTE_13: teste_13(modo); break;
      case TESTE_14: teste_14(modo); break;
      case TESTE_15: teste_15(modo); break;
      case TESTE_16: teste_16(modo); break;
      case TESTE_17: teste_17(modo); break;
      case '?':   teste_mostra();    break;
      case 't':
      case 'T': break;
      default:  break;
    }
  }
  while(1);
}

// Mostrar as opções do Modo Teste
void teste_mostra(void){
  byte i;
  ser_str("TESTES:\n");
  for (i=0; i<=TESTE_TOT; i++){
    ser_str(teste_msg[i]);
    ser_crlf(1);
  }
}



// 1 - LEDs
char teste_1(char md){
  char *msg="[1] TESTE dos LEDs";
  byte cont=0,x;
  lcd_str(0,0,msg);
  ser_str(msg); ser_crlf(1);
  while(TRUE){
    leds_cont(cont++);
    if (fim_qqtec_x() == TRUE)  break;
    //if (sw_tira(&x))     break;
    delay(500);
  }
  led_amo();
  led_vd();
  led_am();
  led_vm();
  ser_str(msg);
  ser_str("\n--- Fim ---\n");
  return md;
}

// 2 - LCD
char teste_2(char md){
  byte j,lin,ct=0,base;
  char *msg="[2] TESTE do LCD";
  lcd_str(0,0,msg);
  ser_str(msg);
  delay(1000);
  base=0;
  while(TRUE){
    ct=base;
    for (lin=0; lin<NRL; lin++){
      lcd_hex8(lin,0,ct);
      lcd_str(lin,2,": ");
      for (j=0; j<16; j++)  lcd_char(lin,j+4,ct++);
    }
    base+=16;
    delay(1000);
    if (fim_qqtec_x() == TRUE)  break;
  }
  ser_str("\n--- Fim ---\n");
  return md;
}

// 3 - Teclado
// Só sai com Reset
char teste_3(char md){
  byte i;   //Para repetições com for
  byte x;   //Último valor lido pela ADC
  byte cod; //Código da tecla (x>>5)
  byte novo=99,velho=99;  //Evitar múltipla impressões porta serial
  char *msg="[3] TESTE das Teclas";
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);
  while (TRUE){
    x=sw_val;
    cod=x>>5;
    novo=cod;
    lcd_float(1,0,5.0*x/256,2);
    lcd_str(1,5,"V=");
    lcd_hex8(1,7,x);
    lcd_str(1,9,"=> ");
    lcd_dec8unz(1,12,cod);
    lcd_str(1,13," = ");
    lcd_str(1,16,sw_nome[cod]);
    lcd_str(2,0,"Fila: pin=");
    lcd_dec8unz(2,10,sw_pin);
    lcd_str(2,12," pout=");
    lcd_dec8unz(2,18,sw_pout);
    i=1;
    while(i<sw_pin && i<6){
      lcd_str(3,4*(i-1),sw_nome[sw_fila[i]]);
      i++;
    }
    if (novo != velho){
      ser_str("A0 = ");
      ser_float(5.0*x/256,2);
      ser_str("V ==> ");
      ser_hex8(x);
      ser_str(" ==> ");
      ser_dec8unz(cod);
      ser_str(" = ");
      ser_str(sw_nome[cod]);
      ser_crlf(1);
      ser_str("Fila: pin=");  ser_dec8unz(sw_pin);
      ser_str(" pout=");  ser_dec8unz(sw_pout);
      ser_str(" [ ");
      i=1;
      while(i<sw_pin && i<SW_FILA_TAM){
        ser_str(sw_nome[sw_fila[i]]);
        ser_spc(1);
        i++;
      }
      if (sw_pin == sw_pout){
        for (i=0; i<SW_FILA_TAM; i++){
          ser_str(sw_nome[sw_fila[i]]);
          ser_spc(1);
          
        }
      }
      ser_char(']');
      if (sw_pin == sw_pout)  ser_str(" Fila Cheia!");
      ser_str("\n\n");
      velho=novo;
    }
  }
  sw_qq_tecla();
  ser_str("\n--- Fim ---\n");
  return md;
}

// 4 - TWI
// Verificar se os escravos TWI geram ACK
// Resultados no LCD e na serial
byte teste_4(char md){
  char *msg="[4] TWI (I2C)";
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);

  twi_scan();
  lcd_str(1,0,"MPU9250 = ");
  if (twi_er_ok(MPU_ADR)) lcd_str(1,10,"OK "); 
  else                    lcd_str(1,10,"NOK");
  lcd_str(2,0,"FLASH 1 = ");
  if (twi_er_ok(FLASH1_ADR)) lcd_str(2,10,"OK "); 
  else                       lcd_str(2,10,"NOK");
  lcd_str(3,0,"FLASH 2 =  ");
  if (twi_er_ok(FLASH2_ADR)) lcd_str(3,10,"OK "); 
  else                       lcd_str(3,10,"NOK");
  while (fim_qqtec_x() == FALSE);
  ser_str("\n--- Fim ---\n");
  return;
}

// 5 - MPU - Acelerômetro e giroscópio
char teste_5(char md){
  byte who,i;
  int vt[7];
  float vtf[7];
  char *msg="[5] MPU6050 Acel e Giro";
  word ac_esc, giro_esc;
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);
  
  mpu_acorda();     //Acordar MPU
  who=mpu_whoami();
  lcd_str(1,0,"Who am I = ");
  lcd_hex8(1,10,who);
  ser_str("MPU retornou Who am I = ");
  ser_hex8(who);
  if (who == 0x73){
    lcd_str(1,15," OK");  //MPU respondendo
    ser_str("h ==> OK!");
  }
  else{
    lcd_str(1,15," NOK");  //MPU respondendo
    ser_str("h ==> NOK! ERRO");
  }

  mpu_config();         //MPU Configurar
  mpu_escalas(0,0);     //+/- 2g e +/-250gr/seg
  ac_esc=2;
  giro_esc=250;
  delay(2000);
  lcd_apaga();
  lcd_str(0,0,"Acel");
  lcd_str(1,3,"g");
  lcd_str(2,0,"Giro");
  lcd_str(3,0,"gr/s");
  while(TRUE){
    mpu_rd_ac_tp_gi(vt);
    vtf[0]=ac_esc*((float)vt[0]/32767);
    vtf[1]=ac_esc*((float)vt[1]/32767);
    vtf[2]=ac_esc*((float)vt[2]/32767);
    vtf[3]=36.53+(vt[3]/340);
    vtf[4]=giro_esc*((float)vt[4]/32767);
    vtf[5]=giro_esc*((float)vt[5]/32767);
    vtf[6]=giro_esc*((float)vt[6]/32767);
    
    lcd_hex16(0,5,vt[0]);  lcd_hex16(0,10,vt[1]);  lcd_hex16(0,15,vt[2]);
    lcd_float(1,5,vtf[0],1);
    lcd_float(1,10,vtf[1],1);
    lcd_float(1,15,vtf[2],1);
    
    lcd_hex16(2,5,vt[4]);  lcd_hex16(2,10,vt[5]);  lcd_hex16(2,15,vt[6]);
    lcd_apaga_lin(3);
    lcd_str(3,0,"gr/s");
    lcd_dec16nz(3,5,vtf[4]);
    lcd_dec16nz(3,10,vtf[5]);
    lcd_dec16nz(3,15,vtf[6]);
    
    ser_str("\nax=");   ser_hex16(vt[0]);   //ax
    ser_char('(');      ser_float(vtf[0],2);

    ser_str("g)  ay="); ser_hex16(vt[1]);   //ay
    ser_char('(');      ser_float(vtf[1],2);

    ser_str("g)  az="); ser_hex16(vt[2]); //az
    ser_char('(');      ser_float(vtf[2],2);
    
    ser_str("g)    tp=");   ser_hex16(vt[3]);
    ser_char('(');      ser_float(vtf[3],1);
    
    ser_str("C)    gx=");   ser_hex16(vt[4]);    //gx
    ser_char('(');      ser_float(vtf[4],2);
    
    ser_str("gr/s)  gy="); ser_hex16(vt[5]);  //gy
    ser_char('(');      ser_float(vtf[5],2);

    ser_str("gr/s)  gz="); ser_hex16(vt[6]);  //gz
    ser_char('(');      ser_float(vtf[6],2);
    ser_str("gr/s)");

    if (fim_qqtec_x() == TRUE){
      ser_str("\n--- Fim ---\n");
      return md;
    }
    delay(500);
  }
  return md;
}

// 6 - MPU - Magnetômetro
char teste_6(char md){
  byte mag_st,who,asa[3];
  int vetor[3];
  char *msg="[6] MPU6050 Magneto";
  unsigned int cont=0;
  // Devo colocar aqui a configuração
  mpu_mag_config();       //Configurar o magnetômetro

  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);

  who = mag_whoami();
  lcd_str(1,0,"Who am I = ");
  lcd_hex8(1,11,who);
  ser_str("MPU MAG retornou Who am I = ");
  ser_hex8(who);
  if (who == MAG_WHO){
    lcd_str(1,13,"h OK");  //MPU respondendo
    ser_str("h ==> OK!");
  }
  else{
    lcd_str(1,13,"h NOK");  //MPU respondendo
    ser_str("h ==> NOK! ERRO");
  }
  delay(800);

  //Ler e imprimir asax, asay e asaz
  mpu_mag_rd_rom(asa);
  ser_str("\nASA: ");
  ser_hex8(asa[0]);   ser_spc(2);
  ser_hex8(asa[1]);   ser_spc(2);
  ser_hex8(asa[2]);   ser_crlf(1);

  mpu_sample_rt(SAMPLE_RT_100Hz);
  mpu_int();

  lcd_str(2,0,"DRDY=X    HOFL=X");
  lcd_str(3,0,"X:hhhh Y:hhhh Z:hhhh");
  while(TRUE){
    if (mpu_dado_ok == TRUE){   //MPU a 100 Hz (10 ms)
     mpu_dado_ok=FALSE;
     mag_st=mpu_rd_mg_out(vetor);
 
     //escreve no LCD   
     lcd_hex16(3,2, vetor[0]);
     lcd_hex16(3,9, vetor[1]);
     lcd_hex16(3,16,vetor[2]);

     //escreve no serial
     ser_str("X:");     ser_hex16(vetor[0]);
     ser_str("   Y:");  ser_hex16(vetor[1]);
     ser_str("   Z:");  ser_hex16(vetor[2]);

     if (mag_st==1){        //Tudo certo
       lcd_char(2, 5,'1');  
       lcd_char(2,15,'0');
       cont=0;  
       ser_str("  OK");
     }
     else if(mag_st==0){    //Dado não pronto
       lcd_char(2, 5,'0');  
       lcd_char(2,15,'0');
       cont++;
       ser_str("  Dado Nao Pronto ");
       ser_dec16unz(cont);
     }
     else if(mag_st==2){    //Sensor Overflow
       lcd_char(2, 5,'1');  
       lcd_char(2,15,'1');  
       ser_str("  Sensor Overflow");      
     }
     ser_crlf(1);
   }
   if (fim_qqtec_x() == TRUE)  break;
  }
  ser_str("\n--- Fim ---\n");
  return;
}

// 7 - SRAM  (23LC1024)
char teste_7(char md){
  word x,cont=0;
  byte y;
  char *msg="[7]U7=SRAM0 U5=SRAM1";

  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);
  ser_str("(23LC1024) U7 = SRAM0, PAG0 = 0x0 0000 --> 0x0 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  ser_str("(23LC1024) U7 = SRAM0, PAG1 = 0x1 0000 --> 0x1 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  ser_str("(23LC1024) U5 = SRAM1, PAG0 = 0x2 0000 --> 0x2 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  ser_str("(23LC1024) U5 = SRAM1, PAG1 = 0x3 0000 --> 0x3 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  x=TCNT0;  x=x^TCNT1;  x=x^TCNT2;
  x=x&0xF;
  if (x==0) x=13; //Semente
  rrand_inic_tudo(x,17,11);   //Inicializar gerador aleat
  lcd_str(1,5,"PG0 PG1 PG2 PG3");
  lcd_str(2,0,"Byte");
  lcd_str(3,0," Blk");
  
  while(TRUE){
    ser_crlf(1);
    cont++;
    ser_str("Ensaio = ");   ser_dec16unz(cont); ser_crlf(1);
    if (teste_7_8_byte(0,0))  lcd_str(2, 5," OK");
    else                       lcd_str(2, 5,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_byte(0,1))  lcd_str(2, 9," OK");
    else                       lcd_str(2, 9,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_byte(0,2))  lcd_str(2,13," OK");
    else                       lcd_str(2,13,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_byte(0,3))  lcd_str(2,17," OK");
    else                       lcd_str(2,17,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(0,0)) lcd_str(3, 5," OK");
    else                       lcd_str(3, 5,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(0,1)) lcd_str(3, 9," OK");
    else                       lcd_str(3, 9,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(0,2)) lcd_str(3,13," OK");
    else                       lcd_str(3,13,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(0,3)) lcd_str(3,17," OK");
    else                       lcd_str(3,17,"NOK");
    //delay(20);   //Evitar fila SERO encher
    lcd_hex16(1,0,cont);
    if (fim_qqtec_x() == TRUE)  break;
    //if (sw_tira(&y))     break;        
    delay(1000);
  }
  ser_str("\n--- Fim ---\n");
  return;
}

// 8 - FLASH (24LC1025)
char teste_8(char md){
  word x,cont=0;
  byte y;
  char *msg="[8] U4=FSH0 U6=FSH1";
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);
  ser_str("(24LC1025) U4 = FLASH0, PAG0 = 0x0 0000 --> 0x0 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  ser_str("(24LC1025) U4 = FLASH0, PAG1 = 0x1 0000 --> 0x1 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  ser_str("(24LC1025) U6 = FLASH1, PAG0 = 0x2 0000 --> 0x2 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  ser_str("(24LC1025) U6 = FLASH1, PAG1 = 0x3 0000 --> 0x3 FFFF\n");
  //delay(20);   //Evitar fila SERO encher
  x=TCNT0;  x=x^TCNT1;  x=x^TCNT2;
  x=x&0xF;
  if (x==0) x=13; //Semente
  rrand_inic_tudo(x,17,11);   //Inicializar gerador aleat
  lcd_str(1,5,"PG0 PG1 PG2 PG3");
  lcd_str(2,0,"Byte");
  lcd_str(3,0," Blk");
  
  while(TRUE){
    ser_crlf(1);
    cont++;
    ser_str("Ensaio = ");   ser_dec16unz(cont); ser_crlf(1);
    if (teste_7_8_byte(1,0))  lcd_str(2, 5," OK");
    else                       lcd_str(2, 5,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_byte(1,1))  lcd_str(2, 9," OK");
    else                       lcd_str(2, 9,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_byte(1,2))  lcd_str(2,13," OK");
    else                       lcd_str(2,13,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_byte(1,3))  lcd_str(2,17," OK");
    else                       lcd_str(2,17,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(1,0)) lcd_str(3, 5," OK");
    else                       lcd_str(3, 5,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(1,1)) lcd_str(3, 9," OK");
    else                       lcd_str(3, 9,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(1,2)) lcd_str(3,13," OK");
    else                       lcd_str(3,13,"NOK");
    //delay(20);   //Evitar fila SERO encher
    if (teste_7_8_bloco(1,3)) lcd_str(3,17," OK");
    else                       lcd_str(3,17,"NOK");
    //delay(20);   //Evitar fila SERO encher
    lcd_hex16(1,0,cont);
    //if (sw_tira(&y))     break;        
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar
    delay(1000);
  }
  //sw_qq_tecla(); //Remover

  ser_str("\n--- Fim ---\n");
  return;
}


// Testar acesso byte
// Tipo=0==>SRAM  Tipo=1==>FLASH
// qual = 0 -> (U7) SRAM0, PAG0 (0 0000 -> 0 FFFF) 
// qual = 1 -> (U7) SRAM0, PAG1 (1 0000 -> 1 FFFF) 
// qual = 2 -> (U5) SRAM1, PAG2 (2 0000 -> 2 FFFF) 
// qual = 3 -> (U5) SRAM1, PAG3 (3 0000 -> 3 FFFF) 
char teste_7_8_byte(char tipo, char qual){
  long adr;
  byte d1,d2;
  adr=rrand_fx(15);
  adr=(adr<<4) | rrand_fx(15);
  adr=(adr<<4) | rrand_fx(15);
  adr=(adr<<4) | rrand_fx(15);
  adr=adr | ((long)qual<<16);

  // Gravar e ler
  if (tipo==0){        //SRAM
    d1=rrand_fx(15);
    sram_wr(adr,d1);
    d2=sram_rd(adr);
    ser_str("SRAM");
  }
  else{               //FLASH
    flash_wr(adr,d1); 
    flash_espera(adr);  //Esperar gravação
    d2=flash_rd(adr);
    ser_str("FLASH");
  }
  ser_dec8unz(qual>>1);

  ser_str(", PAG");  ser_dec8unz(qual&1);
  ser_str(", ADR=");  ser_hex32(adr);
  ser_str(" ==> WR("); ser_hex8(d1);
  ser_str(") RD("); ser_hex8(d2);
  if (d1==d2) { ser_str(") ==> Byte OK\n");  return TRUE; }
  else        { ser_str(") ==> Byte NOK\n"); return FALSE;}
}

// Testear SRAM acesso em bloco
// Tipo=0==>SRAM  Tipo=1==>FLASH
// 0 = (U7) SRAM0, PAG0 (0 0000 -> 0 FFFF) 
// 1 = (U7) SRAM0, PAG1 (1 0000 -> 1 FFFF) 
// 2 = (U5) SRAM1, PAG2 (2 0000 -> 2 FFFF) 
// 3 = (U5) SRAM1, PAG3 (3 0000 -> 3 FFFF) 
char teste_7_8_bloco(char tipo, char qual){
  long adr;
  byte i,erro,d1[16],d2[16];
  adr=rrand_fx(15);
  adr=(adr<<4) | rrand_fx(15);
  adr=(adr<<4) | rrand_fx(15);
  adr=(adr<<4) | rrand_fx(15);
  adr=adr | ((long)qual<<16);
  adr &= 0xFFFF0; //Abrir espaço para 16 endereços
  for (i=0; i<16; i++)  d1[i]=rrand_fx(15);

  // Gravar e ler
  if (tipo==0){              //SRAM
    sram_wr_blk(adr,d1,16);
    sram_rd_blk(adr,d2,16);
    ser_str("SRAM");
  }
  else{                    //FLASH
    flash_wr_blk(adr,d1,16);
    flash_espera(adr);    //Esperar gravação
    flash_rd_blk(adr,d2,16);
    ser_str("FLASH");  
  }

  erro=0;
  for (i=0; i<16; i++){
    if (d1[i]!=d2[i]) erro++;
  }
  ser_dec8unz(qual>>1);
  
  ser_str(", PAG");  ser_dec8unz(qual&1);
  ser_str(", ADR=");  ser_hex32(adr);
  ser_str(" ==> WR(");
  for (i=0; i<15; i++){
    ser_hex8(d1[i]);
    ser_char(',');
  }
  ser_hex8(d1[15]);
  ser_str(") RD(");
  for (i=0; i<15; i++){
    ser_hex8(d2[i]);
    ser_char(',');
  }
  ser_hex8(d2[15]);
  ser_str(") Erros=");
  ser_dec8unz(erro);
  if (erro==0){ ser_str(" ==> Bloco OK\n");  return TRUE; }
  else        { ser_str(" ==> Bloco NOK\n"); return FALSE;}
}

// 9 - Mostar tudo que chega do GPS
char teste_9(char md){
  char *msg="[9] GPS Tudo";
  word tipos[7];    //Contar tipos de mensagens
  byte dado;
  byte tp;
  byte cont=0,cmax=0;
  gps_st=0;
  for (dado=0; dado<7; dado++)  tipos[dado]=0;
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);
  delay(2000);
  lcd_str(0,0,"GPRMC  GPVTG  GPGGA");
  lcd_str(2,0,"GPGSA  GPGSV  GPGLL");
  //gps_rx_fila_config();   //GPS RX fila serial
  UCSR3B |= (1<<RXEN3) ;      //habilitar RX
  while(TRUE){
    while( (UCSR3A&(1<<RXC3)) == 0);   //Esperar chegar
    dado=UDR3;
    if (dado == '$'){
      if (cont>cmax)  cmax=cont;
      ser_crlf(1);
      ser_dec8u(cont);
      ser_spc(1);
      ser_dec8u(cmax);
      cont=0;
      ser_crlf(1);
    }
    cont++;
    ser_hex8(dado);
    ser_spc(1);
    
    //ser_char(dado);
    tp=teste_9_gps_msg(dado);
    tipos[tp]++;
    lcd_dec16u(1,0,tipos[1]);
    lcd_dec16u(1,7,tipos[2]);
    lcd_dec16u(1,14,tipos[3]);
    lcd_dec16u(3,0,tipos[4]);
    lcd_dec16u(3,7,tipos[5]);
    lcd_dec16u(3,14,tipos[6]);
    //if (sw_tira(&dado))   break;
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar
 }
  ser_str("\n--- Fim ---\n");
  return;
}

//Identificar tipos de mensagens do GPS
byte teste_9_gps_msg(char letra){
  switch(gps_st){
    case  0: if (letra=='$') gps_st=1;
             return GPS_NADA;

    case  1: if (letra=='G') gps_st=2;
             else            gps_st=0;
             return GPS_NADA;
    
    case  2: if (letra=='P') gps_st=3;
             else             gps_st=0;
             return GPS_NADA;
    
    case  3: if (letra=='R')     gps_st=4;
             else if(letra=='V') gps_st=6;
             else if(letra=='G') gps_st=8;
             else                gps_st=0;
             return GPS_NADA;
             
    case  4: if (letra=='M') gps_st=5;
             else            gps_st=0;
             return GPS_NADA;
    
    case  5: gps_st=0;
             if (letra=='C') return GPS_RMC;
             return GPS_NADA;

    case  6: if (letra=='T') gps_st=7;
             else            gps_st=0;
             return GPS_NADA;

    case  7: gps_st=0;
             if (letra=='G') return GPS_VTG;
             return GPS_NADA;

    case  8: if (letra=='L')     gps_st=9;
             else if(letra=='G') gps_st=10;
             else if(letra=='S') gps_st=11;
             else                gps_st=0;
             return GPS_NADA;
    
    case  9: gps_st=0;
             if (letra=='L') return GPS_GLL;
             return GPS_NADA;

    case 10: gps_st=0;
             if (letra=='A') return GPS_GGA;
             return GPS_NADA;
             
    case 11: gps_st=0;
             if (letra=='A')     return GPS_GSA;
             else if(letra=='V') return GPS_GSV;
             return GPS_NADA;
  }
}

// 10 - Extrair e interpretar as mensagens do GPS
char teste_10(char md){
  char *msg="[10] GPS Interpreta";
  byte i,dado;
  byte msg_tipo;
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);
  delay(2000);
  lcd_apaga_lin(0);

  //Zerar todo o vetor com os dados do extraídos do GPS
  for (i=0; i<GPS_DADOS_TAM; i++)  gps_dados[i]=0;
  
  // RX3: Habilitar recepção e sua interrupção
  UCSR3B = (1<<RXCIE3) | (1<<RXEN3);  //RXIE=1, RXEN=1

  gps_msg_ok=FALSE;
  while(TRUE){
    while(gps_msg_ok == FALSE);
    gps_msg_ok=FALSE;
    if (gps_msg_fase==0){
      ser_str(gps_msg_1);
      ser_crlf(1);
      gps_extrai(gps_msg_1);
    }
    else{
      ser_str(gps_msg_0);
      ser_crlf(1);
      gps_extrai(gps_msg_0);
    }
    ser_gps_dados(gps_dados);
    ser_crlf(2);
    lcd_gps_data(0,0);
    lcd_gps_hora(0,9);
    lcd_gps_lat(1,1);
    lcd_gps_long(2,0);
    lcd_gps_vel_kph(3,0);
    lcd_gps_dado(0,18,GPS_QTD_SAT);
    lcd_gps_dado(1,14,GPS_PDOP);
    lcd_gps_dado(2,14,GPS_ALT);
    lcd_gps_dado(2,19,GPS_ALT_UN);
    lcd_gps_dado(3,11,GPS_CURSO);
    lcd_gps_dado(3,19,GPS_STATUS);

    //if (sw_tira(&dado))     break;    
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar

  }
  // RX3: Desabilitar recepção e sua interrupção
  UCSR3B &= ~((1<<RXCIE3) | (1<<RXEN3));  //RXIE=0, RXEN=0
  return md;
}


// 11 - GPS ==> u-Center
char teste_11(char md){
  char *msg="[11] GPS-->u-Center";
  byte dado;
  byte flag_pc=0, flag_gps=0; //Marcar TX ou RX
  word cpc=0,cgps=0;  //Contadores de tráfego
  gps_st=0;
  lcd_apaga();
  lcd_str(0,0,msg);
  lcd_str(2,0,"GPS <== PC");
  lcd_str(3,0,"GPS ==> PC");
  lcd_dec16u(2,12,cpc);
  lcd_dec16u(3,12,cgps);
  ser_str(msg);
  ser_crlf(1);
  UCSR0B = (1<<RXEN0) | (1<<TXEN0);  //Só habilita TX e RX (Desab Interrup)
  UCSR3B = (1<<RXEN3) | (1<<TXEN3);  //Só habilita TX e RX

  while(TRUE){
    
    // PC --> GPS
    if ( (UCSR0A&(1<<RXC0)) == (1<<RXC0)){
      dado=UDR0;
      UDR3=dado;
      UCSR0A |= 1<<RXC0;  //Apagar flag RXC0
      cpc++;
      flag_pc=TRUE;
    }

    // GPS --> PC
    if ( (UCSR3A&(1<<RXC3)) == (1<<RXC3)){
      UDR0=UDR3;
      UCSR3A |= 1<<RXC3;  //Apagar flag RXC3
      cgps++;
      flag_gps=TRUE;
    }

    if (flag_pc == TRUE){
      flag_pc = FALSE;
      lcd_dec16u(2,12,cpc);
    }

    if (flag_gps == TRUE){
      flag_gps = FALSE;
      lcd_dec16u(3,12,cgps);
    }
  
    if (sw_tira(&dado))     break;

    // Naõ funciona, as interrup estão desabilitadas
    //if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar
  }
  // Voltar habilitar interrupções
  UCSR0B = (1<<RXCIE0) | (1<<TXCIE0) | (1<<RXEN0) | (1<<TXEN0);  //RXIE=TXIE=1, hab TX e RX
  ser_str("\n--- Fim ---\n");
  return md;
}

// 12 - MPU ==> MatLab
//char teste_12(char md){
//  char *msg="[12] MPU --> Matlab";
//  byte who;
//  word vt[7];
//  word cont;    //Contador de linhas
//  float vtf[7];
//  lcd_apaga();
//  lcd_str(0,0,msg);
//  ser_str(msg);
//  ser_crlf(1);
//
///*
//  mpu_acorda();     //Acordar MPU
//  who=mpu_whoami();
//  if (who != 0x73){
//    lcd_str(1,0,"MPU nao responde!");  //MPU Não responde
//    ser_str("MPU nao responde!");  //MPU Não responde
//    sw_qq_tecla();
//    ser_str("\n--- Fim ---\n");
//    return  FALSE;
//  }
//
//  lcd_str(3,0,"Inicia em 1 seg.");
//  ser_str("Inicia em 1 segundo.\n");
//  delay(1000);
//  
//  mpu_config();         //MPU configurar
//  mpu_escalas(0,0);     //+/- 2g e +/-250gr/seg
//  mpu_sample_rt(SAMPLE_RT_100Hz);
//  //mpu_sample_rt(SAMPLE_RT_200Hz);
//  */
//  teste12_prepara();
//  mpu_mag_config();
//
//  lcd_str(3,0,"Inicia em 1 seg.");
//  ser_str("Inicia em 1 segundo.\n");
//  delay(1000);
//
//
//  lcd_apaga_lin(1);
//  lcd_apaga_lin(2);
//  lcd_apaga_lin(3);
//  lcd_str(1,0,"Acel: ");
//  lcd_str(2,0,"Giro:");
//  lcd_str(3,0,"Mag:");
//  ser_str("#[m\n");      //Avisar Matlab
//  ser_cab('t');       //Cabeçalho modo Teste
//
//  ser_dec16unz(55555);  ser_crlf(1);   //Simular qtd de linhas (dados)
//      
//  // Habilitar interrupção MPU (Dado Pronto)
//  mpu_int();
//  cont=0;
//  while(TRUE){
//    while (mpu_dado_ok == FALSE);   //Agaurdar MPU a 100 Hz (10 ms)
//    mpu_dado_ok=FALSE;
//
//    mpu_rd_ac_tp_gi(vt);  //Ler MPU
//
//    // LCD
//    lcd_hex16(1,5,vt[0]);
//    lcd_hex16(1,10,vt[1]);
//    lcd_hex16(1,15,vt[2]);
//    
//    lcd_hex16(2,5,vt[4]);
//    lcd_hex16(2,10,vt[5]);
//    lcd_hex16(2,15,vt[6]);
//    
//    //Enviar pela porta serial
//    ser_dec16(cont);   ser_spc(1);           //contador
//    ser_dec16(cont++); ser_spc(1);           //simular endereço
//    
//    ser_dec16(vt[0]);   ser_spc(1);           //ax
//    ser_dec16(vt[1]);   ser_spc(1);           //ay
//    ser_dec16(vt[2]);   ser_spc(1);           //az
//    //ser_dec16(vt[3]);   ser_spc(1);           //temperatura
//    ser_dec16(vt[4]);   ser_spc(1);           //gx
//    ser_dec16(vt[5]);   ser_spc(1);           //gy
//    ser_dec16(vt[6]);   ser_spc(1);           //gz
//
//    mpu_rd_mg_out(vt);
//    lcd_hex16(3,5,vt[0]);
//    lcd_hex16(3,10,vt[1]);
//    lcd_hex16(3,15,vt[2]);
//    ser_dec16(vt[0]);   ser_spc(1);           //hx
//    ser_dec16(vt[1]);   ser_spc(1);           //hy
//    ser_dec16(vt[2]);   ser_crlf(1);          //hz
//    //if (sw_tira(&who))     break;    
//    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar
//  }
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_spc(1);           //Finalizar com Matlab
//  ser_dec16(22222);      ser_crlf(1);          //Finalizar com Matlab
//  ser_str("m]#\n--- Fim ---\n");
//  return md;
//}


char teste_12(char md){
  char *msg="[12] MPU --> Matlab";
  byte who;
  word vt[7];
  word cont;    //Contador de linhas
  float vtf[7];
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);

/*
  mpu_acorda();     //Acordar MPU
  who=mpu_whoami();
  if (who != 0x73){
    lcd_str(1,0,"MPU nao responde!");  //MPU Não responde
    ser_str("MPU nao responde!");  //MPU Não responde
    sw_qq_tecla();
    ser_str("\n--- Fim ---\n");
    return  FALSE;
  }

  lcd_str(3,0,"Inicia em 1 seg.");
  ser_str("Inicia em 1 segundo.\n");
  delay(1000);
  
  mpu_config();         //MPU configurar
  mpu_escalas(0,0);     //+/- 2g e +/-250gr/seg
  mpu_sample_rt(SAMPLE_RT_100Hz);
  //mpu_sample_rt(SAMPLE_RT_200Hz);
  */
  teste12_prepara();
  mpu_mag_config();

  lcd_str(3,0,"Inicia em 1 seg.");
  ser_str("Inicia em 1 segundo.\n");
  delay(1000);


  lcd_apaga_lin(1);
  lcd_apaga_lin(2);
  lcd_apaga_lin(3);
  lcd_str(1,0,"Acel: ");
  lcd_str(2,0,"Giro:");
  lcd_str(3,0,"Mag:");
  ser_str("#[m\n");      //Avisar Matlab
  ser_cab('t');       //Cabeçalho modo Teste

  ser_dec16unz(55555);  ser_crlf(1);   //Simular qtd de linhas (dados)
      
  // Habilitar interrupção MPU (Dado Pronto)
  mpu_int();
  cont=0;
  while(TRUE){
    while (mpu_dado_ok == FALSE);   //Agaurdar MPU a 100 Hz (10 ms)
    mpu_dado_ok=FALSE;

    mpu_rd_ac_tp_gi(vt);  //Ler MPU

    // LCD
    lcd_hex16(1,5,vt[0]);
    lcd_hex16(1,10,vt[1]);
    lcd_hex16(1,15,vt[2]);
    
    lcd_hex16(2,5,vt[4]);
    lcd_hex16(2,10,vt[5]);
    lcd_hex16(2,15,vt[6]);
    
    //Enviar pela porta serial
    ser_dec16(cont);   ser_crlf(1);           //contador
    ser_dec16(cont++); ser_crlf(1);           //simular endereço
    
    ser_dec16(vt[0]);   ser_crlf(1);           //ax
    ser_dec16(vt[1]);   ser_crlf(1);           //ay
    ser_dec16(vt[2]);   ser_crlf(1);           //az
    //ser_dec16(vt[3]);   ser_spc(1);           //temperatura
    ser_dec16(vt[4]);   ser_crlf(1);           //gx
    ser_dec16(vt[5]);   ser_crlf(1);           //gy
    ser_dec16(vt[6]);   ser_crlf(1);           //gz

    mpu_rd_mg_out(vt);
    lcd_hex16(3,5,vt[0]);
    lcd_hex16(3,10,vt[1]);
    lcd_hex16(3,15,vt[2]);
    ser_dec16(vt[0]);   ser_crlf(1);           //hx
    ser_dec16(vt[1]);   ser_crlf(1);           //hy
    ser_dec16(vt[2]);   ser_crlf(1);          //hz
    //if (sw_tira(&who))     break;    
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar
  }
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);          //Finalizar com Matlab
  ser_str("m]#\n--- Fim ---\n");
  return md;
}




// Preparação para o Teste 12
void teste12_prepara(void){
  char *msg1="Erro Who am I = ";
  char *msg2="MPU Self Test Falhou";
  char *msg3="MAG Self Test Falhou";
  byte x;
  byte st;        //Resultado do Self Test
  int st_vet[24]; //Receber resultados do Self Test
  byte esc_ac,esc_gi,esc_mg;   //Guardar Escalas de acel, giro e mag
  int aux[7];

  mpu_acorda();
  mpu_config();
  x=mpu_whoami();                     //Who am I
  if (x != MPU9250_WHO){
    lcd_str(1,0,msg1); lcd_dec16unz(1,16,x);
    ser_str(msg1);     ser_dec16unz(x);  ser_crlf(1);
    delay(1000);
  }
  sram_wr_16b(OP_OK,COD_SIM);         //Marcar que fez Calibração ao ligar
  
  // Self test acel e giro
  //st=mpu_self_test(st_vet,FALSE);     //Self Test
  st = mpu_self_test_2(FALSE); //versão correta do self test do mpu
  if (st==TRUE)
    sram_wr_16b(OP_ST_OK,COD_SIM);
  else{
    sram_wr_16b(OP_ST_OK,COD_NAO);
    lcd_str(1,0,msg2);
    ser_str(msg2);  ser_crlf(1);
    delay(200);
  }

  // Self test mag
  st=mpu_mag_self_test(st_vet,FALSE);     //Self Test
  if (st==TRUE)
    sram_wr_16b(OP_STH_OK,COD_SIM);
  else{
    sram_wr_16b(OP_STH_OK,COD_NAO);
    lcd_str(1,0,msg3);
    ser_str(msg3);  ser_crlf(1);
    delay(200);
  }
  
  x=eeprom_rd_16b(CF_OK);   //Calibração de Fábrica
  sram_wr_16b(OP_CF_OK,x);  //Gravar código de Calibr de Fábrica

  //Calibração ao ligar o carro Acel e Giro 
  esc_ac=ACEL_FS_2G;    //Escala acel para calibrar ao ligar
  esc_gi=GIRO_FS_250;   //Escala giro para calibrar ao ligar
  mpu_calibra(aux, OP_QTD_MED_AG, esc_ac, esc_gi);  //Função de calibração
  sram_wr_16b(OP_OK,COD_SIM);                       //Fez Calibração ao ligar
  x=mpu_rd_freq();      sram_wr_16b(OPC_FREQ_AG,x); //Freq de amostragem
  x=mpu_rd_bw();        sram_wr_16b(OPC_BW_AG,x);   //Banda Passante
  x=mpu_rd_esc_acel();  sram_wr_16b(OPC_ESC_AC,x);  //Escala do Acelerômetro
  x=mpu_rd_esc_giro();  sram_wr_16b(OPC_ESC_GI,x);  //Escala do Giroscópio
  sram_wr_16b(OPC_QTD_AG,OP_QTD_MED_AG);            //Qtd de medidas para calibração ao ligar
  for (x=0; x<7; x++)   sram_wr_16b(OPC_AX+(2*x),aux[x]);

  //Configuração de Acel e Giro para a Operação
  mpu_config();         //Novamente inicializa MPU
  esc_ac=ACEL_FS_2G;    //Escala acel para calibrar ao ligar
  esc_gi=GIRO_FS_250;   //Escala giro para calibrar ao ligar
  mpu_escalas(esc_gi,esc_ac);       //Programar escalas
  mpu_sample_rt(SAMPLE_RT_100Hz);   //Programar taxa de amostragem
  x=mpu_rd_freq();      sram_wr_16b(OP_FREQ_AG,x); //Freq de amostragem
  x=mpu_rd_bw();        sram_wr_16b(OP_BW_AG,x);   //Banda Passante
  x=mpu_rd_esc_acel();  sram_wr_16b(OP_ESC_AC,x);  //Escala do Acelerômetro
  x=mpu_rd_esc_giro();  sram_wr_16b(OP_ESC_GI,x);  //Escala do Giroscópio
  mpu_mag_config();                 //Configurar Magnetômetro
}

// 13 - BlueTooth
char teste_13(char md){
  byte x,i,j;
  lcd_str(0,0,teste_msg[md]);
  lcd_str(1,0,"Qq tecla termina");
  ser_str(teste_msg[md]);
  ser_crlf(1);
  x='A';
  i=0;
  while(TRUE){
    //Finalizar
    //if (sw_tira(&j)==TRUE)  break;
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar

    lcd_char(3,i++,x);
    ser_char(x);
    UDR2=x++;
    delay(500);
    //while( (UCSR2A&(1<<TXC2)) == 0);
    if (x=='Z'+1) x=CR;
    if (x==CR+1)  x=LF;
    if (x==LF+1)  x='A';
    if (i==20)  i=0;
  }
  return md;
}

char teste_14(char md){;}

/*
// 14 - Bluetooth - AT Command 
// PC <==> Arduino (COM0) opera em 115.200 bps
// Arduino (COM2) <==> HC-05 opera em 38.400 bps
// Habilitar <Ambos, NL e CR> no Monitor Serial
// 1) Remova o jumper J1
// 2) Acione o pequeno bptão do HC-05
// 3) Recoloque o (J1) jumper de alimentação
// 4) Led pisca lentamente
//
// AT+UART=115200,0,0
// ARRUMAR depois que unificou UART0 e UART2
char teste_14(char md){
  byte cmdo[50];  //Comando para HC-05
  byte b,i,j,cmdo_ok;
  word cont_tx,cont_rx;     //Contadores do que vai e vem HC-05
  lcd_str(0,0,teste_msg[md]);
  ser_str(teste_msg[md]);
  ser_str("\nSelecionar 115.200 bps e <Ambos, NL e CR>\n");
  lcd_str(1,0,"PC==>BT: ");
  lcd_str(2,0,"BT==>PC: ");
  lcd_str(3,0,"Qq tecla termina");  
  bt_config(38400);
  bt_tx_EN();     //BT Habilitar Transmissão
  bt_rx_EN();     //BT Habilitar a Recepção
  bt_rx_int_EN(); //BT Habilitar Interrup RX
  bt_tx_int_EN(); //BT Habilitar Interrup TX

  //bt_rx_fila_config();
  //bt_tx_fila_config();
  cont_tx=cont_rx=0;  //Contador de colunas para linhas 1 e 2
  bt_tx_ok=TRUE;
  cmdo_ok=FALSE;
  i=0;
  while(TRUE){

    //Finalizar
    if (sw_tira(&j)==TRUE)  break;

    //Ver se chegou algo do PC
    if(ser_rx_tira(&b) == TRUE){
      bt_tx_poe(b);
      cmdo[i++]=b;
      if (b==LF){
        cmdo_ok=TRUE;
        cmdo[i]=0;
      }
      cont_tx++;
      lcd_dec16unz(1,9,cont_tx);
    }
    
    if (cmdo_ok == TRUE){
    cmdo_ok=FALSE;
      ser_str("CMDO: [ ");
      for (j=0; j<i; j++){
        ser_hex8(cmdo[j]);
        ser_spc(1);
      }
      ser_str("] ==> ");
      ser_str(cmdo);
      b=0;
      i=0;
   }

    //Se puder, transmite para BT
    if (bt_tx_ok==TRUE){
      if (bt_tx_tira(&b)==TRUE){
        bt_tx_ok=FALSE;
        UDR2=b;        
      }
    }

    //Se puder, transmite para o PC
    if (ser_tx_ok==TRUE){
      if (bt_rx_tira(&b)==TRUE){
        ser_tx_ok=FALSE;
        UDR0=b;        
        cont_rx++;
        lcd_dec16unz(2,9,cont_rx);
        b=0;
      }
    }
  }
  return md;
}
*/


// 15 - SELF TEST MPU
char teste_15(char md){
  lcd_str(0,0,teste_msg[md]);
  ser_str(teste_msg[md]);

  char *msg1="Erro Who am I = ";
  char *msg2="MPU Self Test Passou";
  char *msg3="MPU Self Test Falhou";
  byte x;
  byte st;        //Resultado do Self Test
  int st_vet[24]; //Receber resultados do Self Test
  byte esc_ac,esc_gi,esc_mg;   //Guardar Escalas de acel, giro e mag
  int aux[7];

  mpu_acorda();
  mpu_config();
  
  x=mpu_whoami();                     //Who am I
  if (x != MPU9250_WHO){
    lcd_str(1,0,msg1); lcd_dec16unz(1,16,x);
    ser_str(msg1);     ser_dec16unz(x);  ser_crlf(1);
    delay(1000);
  }
  sram_wr_16b(OP_OK,COD_SIM);         //Marcar que fez Calibração ao ligar
  
  // Self test acel e giro
  //st=mpu_self_test_2(st_vet,TRUE);     //Self Test
  st=mpu_self_test_2(TRUE);

  ser_crlf(1);
  if (st==TRUE){
    lcd_str(1,0,msg2);
    ser_str(msg2);  ser_crlf(1);
  }
  else{
    lcd_str(1,0,msg3);
    ser_str(msg3);  ser_crlf(1);
    
  }
  
  sw_qq_tecla();
  return md;
}

// 16 - Self test magnetometro
char teste_16(char md){
  lcd_str(0,0,teste_msg[md]);
  ser_str(teste_msg[md]);

  char *msg1="MAG Self Test Passou";
  char *msg2="MAG Self Test Falhou";
  int vet[3];
  byte st = FALSE;
  int st_vet;

//  mpu_mag_config(); 
  st=mpu_mag_self_test(st_vet,TRUE);

  ser_crlf(1);
  if (st==TRUE){
    lcd_str(1,0,msg1);
    ser_str(msg1);  ser_crlf(1);
  }
  else{
    lcd_str(1,0,msg2);
    ser_str(msg2);  ser_crlf(1);
    
  }

  
  sw_qq_tecla();
  return md;
}

// 17 - Calibração do Acelerometro
char teste_17(char md){

  // Faz 50 medidas cada vez que aperta o botão. É usado nos scripts de calibração do acelerômetro
  
  char *msg="[17] MPU --> Calibra Acel";
  byte who;
  word vt[7];
  int cont=0;    //Contador de linhas
  float vtf[7];
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);

  char *msg1="Erro Who am I = ";
  
  byte whoami;
  byte esc_ac,esc_gi,esc_mg;   //Guardar Escalas de acel, giro e mag
  int aux[7];
  int qtd_medidas=50;

  mpu_acorda();
  mpu_config();
  whoami=mpu_whoami();                    
  if (whoami != MPU9250_WHO){
    lcd_str(1,0,msg1); lcd_dec16unz(1,16,whoami);
    ser_str(msg1);     ser_dec16unz(whoami);  ser_crlf(1);
    delay(1000);
  }

  delay(1000);

  lcd_apaga_lin(1);
  lcd_apaga_lin(2);
  lcd_apaga_lin(3);
  lcd_str(1,0,"Acel: ");
  lcd_str(2,0,"Giro:");
  lcd_str(3,0,"Mag:");

  int quit=FALSE;
  byte x=0;
//  for(int i = 0 ; i < 6 ; i++){
  for(;;){  

    qtd_medidas=50;
    
    //Inicia coleta de dados

    //espera usuario apertar botao para medir, ou terminar mandando X pelo serial
    while(TRUE){
      while (TRUE){
        if (seri_tira(&x)==FALSE)   break;
        if (x=='x' || x=='X')       quit=TRUE;
      }     
      if ( sw_tira(&x) == TRUE) break;
      if(quit == TRUE) break;
    }

    if(quit == TRUE) break;

    //inicia as leituras
    mpu_int();
    
    //faz algumas leituras para estabilizar e evitar o movimento de apertar o botão
    int l=150;  
    while(l--){
      while (mpu_dado_ok == FALSE);
      mpu_rd_ac_tp_gi(vt);  //Ler MPU
    }
    
    ser_str("#[m\n");      //Avisar Matlab
   
    while(qtd_medidas--){
        while (mpu_dado_ok == FALSE);   //Agaurdar MPU a 100 Hz (10 ms)
        mpu_dado_ok=FALSE;
        
        mpu_rd_ac_tp_gi(vt);  //Ler MPU
        
        // LCD
        lcd_hex16(1,5,vt[0]);
        lcd_hex16(1,10,vt[1]);
        lcd_hex16(1,15,vt[2]);
        
        lcd_hex16(2,5,vt[4]);
        lcd_hex16(2,10,vt[5]);
        lcd_hex16(2,15,vt[6]);
        
        //Enviar pela porta serial
        ser_dec16(cont);   ser_crlf(1);           //contador
        ser_dec16(cont);   ser_crlf(1);           //simular endereço
        cont++;
        
        ser_dec16(vt[0]);   ser_crlf(1);           //ax
        ser_dec16(vt[1]);   ser_crlf(1);           //ay
        ser_dec16(vt[2]);   ser_crlf(1);           //az
        
      }
      ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
      ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
      ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
      ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
      ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab

      mpu_des_int();
  }
 

  return md;
      
  // Habilitar interrupção MPU (Dado Pronto)
  /*
  mpu_int();
  mpu_des_int();
  mpu_int();
  cont=0;
  while(TRUE){
    while (mpu_dado_ok == FALSE);   //Agaurdar MPU a 100 Hz (10 ms)
    mpu_dado_ok=FALSE;

    mpu_rd_ac_tp_gi(vt);  //Ler MPU

    // LCD
    lcd_hex16(1,5,vt[0]);
    lcd_hex16(1,10,vt[1]);
    lcd_hex16(1,15,vt[2]);
    
    lcd_hex16(2,5,vt[4]);
    lcd_hex16(2,10,vt[5]);
    lcd_hex16(2,15,vt[6]);
    
    //Enviar pela porta serial
    ser_dec16(cont);   ser_crlf(1);           //contador
    ser_dec16(cont++); ser_crlf(1);           //simular endereço
    
    ser_dec16(vt[0]);   ser_crlf(1);           //ax
    ser_dec16(vt[1]);   ser_crlf(1);           //ay
    ser_dec16(vt[2]);   ser_crlf(1);           //az
    //ser_dec16(vt[3]);   ser_spc(1);           //temperatura
    ser_dec16(vt[4]);   ser_crlf(1);           //gx
    ser_dec16(vt[5]);   ser_crlf(1);           //gy
    ser_dec16(vt[6]);   ser_crlf(1);           //gz

    //if (sw_tira(&who))     break;    
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar
  }
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);          //Finalizar com Matlab
  ser_str("m]#\n--- Fim ---\n");
  return md;
  */
}

// Selecionar o modo de operação
byte teste_sel(void){
  byte prov=TESTE_TOT;    //provisório = 1, 2, ..., TESTE_TOT
  byte tecla,aux;
  while(TRUE){
    lcd_apaga_lin(1);
    lcd_apaga_lin(2);
    lcd_apaga_lin(3);
    lcd_str(2,0,"-->");
    aux=prov;
    lcd_str(1,3,teste_msg[aux++]);
    if (aux>TESTE_TOT)  aux=1;    
    lcd_str(2,3,teste_msg[aux]);
    ser_str(teste_msg[aux++]);  ser_crlf(1);
    if (aux>TESTE_TOT)  aux=1;    
    lcd_str(3,3,teste_msg[aux]);
    //Esperar tecla
    while ( sw_tira(&tecla) == FALSE);
    switch(tecla){
      case SW_SUP:  prov--;  break;
      case SW_INF:  prov++;  break;
      case SW_SEL:  if (++prov>TESTE_TOT)  prov=1;
                    return prov;
    }
    if (prov>TESTE_TOT)  prov=1;
    if (prov==0)         prov=TESTE_TOT;
  }
}

// Opção para terminar um dos modos de teste
// Vai retirando letras da fila até achar x
// Opções para terminar:
// 1) qq tecla
// 2)letra x  ou X
byte fim_qqtec_x(void){
  byte x;

  // Esvaziar fila procurando x
  while (TRUE){
    if (seri_tira(&x)==FALSE)   break;
    if (x=='x' || x=='X')       return TRUE; 
  }
  return sw_tira(&x); 
}
