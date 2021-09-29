// Modos de operação
// CXP - Caixa Preta

// Modo Operação 
void opera(void){
  byte modo;    //Modo selecionado
  byte i;
  word w;
  long z;
  float f;
  while(TRUE){
    lcd_apaga();
    lcd_str(0,0,"OPERA");
    ser_str("\n==> Modo Opera <==\n");
    lcd_str(0,5," - Selecionar");
    ser_str("Selecionar com LCD\n");

    modo=sel_modo(opera_msg, OPERA_TOT);
    //modo=8;
    lcd_apaga();
    ser_crlf(1);
    switch(modo){
      case 'T':
      case 't':
      case OPERA_0:  teste();    break;
      case OPERA_1:  opera_1(modo);  break;
      case OPERA_2:  opera_2(modo);  break;
      case OPERA_3:  opera_3(modo);  break;
      case OPERA_4:  opera_4(modo);  break;
      case OPERA_5:  opera_5(modo);  break;
      case OPERA_6:  opera_6(modo);  break;
      case OPERA_7:  opera_7(modo);  break;
      case OPERA_8:  opera_8(modo);  break;
      case OPERA_9:  opera_9(modo);  break;
      case '?':   opera_mostra();    break;
      case 'O':
      case 'o': break;
      default:  break;
    }
  }
  while(1);
}

// Mostrar as opções do Modo Opera
void opera_mostra(void){
  byte i;
  ser_str("OPERA:\n");
  for (i=0; i<=OPERA_TOT; i++){
    ser_str(opera_msg[i]);
    ser_crlf(1);
  }
}


// 1 - Adquirir dados
// Após <bateu>, Preenche toda a memória.
char opera_1(char md){
  char *msg1="[1] Adquirir Dados";
  char *msg2="Zerando SRAM";
  char *msg3="SEL = Inic Aquisicao";
  char *msg3a="SW_INF = Interrompe";
  char *msg3b="S=sel, I=Interrompe, X=sai";
  char *msg4=" MPUxxxxx   GPS xxxxx";
  char *msg5="Faltam xxxxx segundos";
  char *msg6="==>> ADQUIRINDO <<==";
  char *msg7="==>> TX  SERIAL <<==";
  char *msg8="FIM Modo Aquisicao!\n";
  unsigned int mpu_cont, gps_cont;    //Contar dados para animar tela
  unsigned long mpu_adr;        //MPU - Contador de endereços para gravação
  unsigned long mpu_adr_bateu;  //MPU - Endereço para finalizar gravação após batida
  unsigned long gps_adr;        //GPS - Contador de endereços para gravação
  unsigned long cont_pb;        //Contador decrescente para indicar o que falta pós batida
  int z;
  byte tec,letra;
  byte bateu;                   //Indicar se bateu (FALSE / TRUE)
  byte flag_brk;                //Indicar que laço de aquisição foi interrompido
  word segs;                    //Segundos que faltam para finalizar
  byte vt[18];                  //Receber ax, ay, az, gx, gy, gz, hx, hy, hz ou gps_dados
  char msg_aux[20];
  byte data[10];                 //Última atualização da data
  byte hora[12];                //Última atualização da hora
  byte so_lat_long;       //Imprimir? 0=tudo ou 1=só lat e long

  //Variáveis para enviar LAT e LONG pelo Bluetooth
  int graus,mini,fracm;          //Graus, minito e fração de minuto
  long lati,longi;              //Valor de latitude e longitude
  float flat,flong;             //Valor de latitude e longitude
  unsigned long bt_seq=0;       //Contar sequência

  lcd_apaga();
  lcd_str(0,0,msg1);
  //ser_str(msg1);  ser_crlf(1);
  opera1_prepara();

  //Laço de principal
  so_lat_long=FALSE;    //<<=== Selecionar so LAT e LONG?
  gps_int();
  while (TRUE){
    lcd_apaga();
    lcd_str(0,0,msg1);
    ser_crlf(1);
    ser_str(msg1);  ser_crlf(1);
    lcd_str(1,0,msg2);
    ser_str(msg2);  ser_crlf(1);
    sram_zera_mpu_gps();   //Zerar SRAM, preserva Calib ao Ligar
    mpu_cont=0;
    gps_cont=0;
    lcd_apaga_lin(1);
    lcd_str(1,0,msg3);
    lcd_str(2,0,msg3a);
    ser_str(msg3);  ser_spc(2);
    ser_str(msg3a);  ser_crlf(1);
    ser_str(msg3b);  ser_crlf(1);
    lcd_str(3,0,msg4);
    bateu=FALSE;
    mpu_adr=MPU_ADR_INI;
    gps_adr=GPS_ADR_INI;
    gps_tipo=GPS_NADA;
    mpu_dado_ok=FALSE;
    flag_brk=FALSE;       //Flag para indicar interrupção da aquisição
    //gps_int();

    // Laço de Aquisição
    for (z=0; z<GPS_PASSO; z++) gps_dados[z]='\0'; //***Prencher com zeros
    while(TRUE){
      lcd_dec16u(3,4,mpu_cont);
      lcd_dec16u(3,15,gps_cont);
  
      // MPU trabalha por interrupção
      if (mpu_dado_ok == TRUE){   //MPU a 100 Hz (10 ms)
        mpu_dado_ok=FALSE;
        mpu_cont++;
        mpu_rd_ac_gi_mg(vt);
        sram_wr_blk(mpu_adr, vt, MPU_PASSO); //Gravar MPU na SRAM
        mpu_adr += MPU_PASSO;
        if (mpu_adr == MPU_ADR_FIM){
          mpu_adr=MPU_ADR_INI;
          lcd_char(3,0,'*');
        }
        if (bateu == TRUE){
          //ser_str("\nmpu_adr=");  ser_hex32(mpu_adr);
          //ser_str(" cont_pb=");   ser_dec32(cont_pb);
          cont_pb -= MPU_PASSO;       //Se bateu, acionar contador decrescente
        }
      }

      // GPS usa interrupção com dois buffers selecionados pela variável gps_msg_fase
      if (gps_msg_ok == TRUE){
        gps_msg_ok=FALSE;
        if (gps_msg_fase==0){
          //ser_str(gps_msg_1);     //Imprimir msg GPS
          //ser_crlf(1);            //CR LF
          gps_extrai(gps_msg_1);  //Extrair parâmetros do GPS
        }
        else{
          //ser_str(gps_msg_0);     //Imprimir msg gps
          //ser_crlf(1);            //CR LF
          gps_extrai(gps_msg_0);  //Extrair parâmetros do GPS
        }
      }

      // GPS - Só grava após receber GPS_GLL
      if (gps_tipo == GPS_GLL){
        gps_tipo=GPS_NADA;
        gps_cont++;
        str_dec32u(mpu_adr, msg_aux);                 //Gerar string do contador de endereço mpu_adr
        str_rmvz_u(msg_aux);                          //Remover zeros à esquerda
        str_copia(msg_aux,&gps_dados[GPS_ADR_SRAM]);  //Copiar para o vetor gps_dados
        sram_wr_blk(gps_adr, gps_dados, GPS_PASSO);   //Gravar gps_dados na SRAM
        //sram_dump(gps_adr,GPS_PASSO);

        // Impressão: Tudo ou só lat e long
        if (so_lat_long==FALSE){
          //ser_char('[');          //<<==
          //ser_dec32u(gps_adr);    //<<==
          //ser_str("  0x");
          //ser_hex32(gps_adr);     //<<==
          //ser_char(']');          //<<==
          
//          ser_gps_dados_lin(gps_dados);  //Imprimir tudo que veio do GPS
        }
        else{     //Enviar só LAT e LONG pelo serial
          //bt_lat(gps_dados);
          //bt_long(gps_dados);
          bt_seq++;
          ser_dec32unz(bt_seq);
          ser_char(',');
          if (gps_dados[GPS_STATUS] == 'A'){
            ser_lat(&gps_dados[GPS_LAT],gps_dados[GPS_NS]);
            ser_char(',');
            ser_long(&gps_dados[GPS_LONG],gps_dados[GPS_EW]);
          }
          else{
            ser_str("Nao valido.");   
          }
          ser_crlf(1);
        }
        
        for (z=0; z<6; z++){
          hora[z]=gps_dados[GPS_HORA+z];
          data[z]=gps_dados[GPS_DATA+z];
        }
        data[z]=hora[z]='\0';

        for (z=0; z<GPS_PASSO; z++)  gps_dados[z]='\0'; //***Prencher com zeros
        gps_adr += GPS_PASSO;                         //Avançar contador de endereço gps_adr
        if (gps_adr == GPS_ADR_FIM){                  //Verificar limite do contador
          gps_adr=GPS_ADR_INI;
          lcd_char(3,11,'*');
        }
      }

      // Se bateu, verificar se já preencheu toda a memória
      if (bateu == TRUE){
        segs=cont_pb/(MPU_PASSO*100);
        lcd_dec16u(2,7,segs);   //Imprimir contador segundos após batida (SEL)
        if (cont_pb == 0)   break;      //cont_pb=0 ==> FIM
      }

      //Tecla? Dá início à aquisição
      letra=tec=SW_NADA;
      if (sw_tira(&tec)==TRUE || seri_letra(&letra)==TRUE){
        letra &= ~0x20;   //transformar para Maiúscula
         
        // Interromper aquisição ?
        if (bateu==TRUE){
          if (tec==SW_INF || letra=='I'){
            flag_brk=TRUE;
            break;
          }
        }
        
        if (tec==SW_SEL || letra=='S'){
          bateu=TRUE;       //Indicar que bateu
          sram_wr_32b(OP_MPU_ADR, mpu_adr);                      //Guardar contador de endereços SRAM(MPU) no instante da batida
          sram_wr_32b(OP_GPS_ADR, gps_adr);                      //Guardar contador de endereços SRAM(GPS) no instante da batida
          sram_wr_str(OP_AC_DATA, data);                          //Guardar data da batida
          sram_wr_str(OP_AC_HORA, hora);                          //Guardar hora da batida
          sram_wr_16b(OP_BATEU,COD_SIM);                          //Marcar BATIDA
          sram_wr_16b(OP_DISP_AX, COD_SIM);                       //AX simula disparo da batida
          cont_pb=MPU_ADR_FIM-MPU_ADR_INI;                        //Inicializar contador pós batida
          lcd_str(1,0,msg6);
          lcd_str(2,0,msg5);
          ser_crlf(1);
          ser_str(msg6);
          ser_crlf(1);
        }
        if (tec==SW_SEQ1 || letra == 'X'){      //Sair do Opera 1
          gps_des_int();  //GPS: Desabilitar interrupção
          mpu_des_int();  //MPU: Desabilitar interrupção
          lcd_apaga();
          lcd_str(0,0,msg1);
          lcd_str(2,0,msg8);
          ser_crlf(2);
          ser_str(msg8);
          //sw_qq_tecla();
          return md;
        }
      }
    }

    // Marcar o final
    // Gravar linha com 22222 = 0x56CE
    for (z=0; z<18; z+=2){vt[z+0]=0x56; vt[z+1]=0xCE; }
    if (mpu_adr==0) mpu_adr=MPU_ADR_FIM-MPU_PASSO;  //Voltar ponteiro um passo
    else            mpu_adr = mpu_adr-MPU_PASSO;
    sram_wr_blk(mpu_adr, vt, MPU_PASSO);            //Gravar linha com 22222 na SRAM
    //ser_str("\nGravar 2222 em adr="); ser_hex32(mpu_adr);
    sram_wr_32b(OP_ULT_ADR,mpu_adr);    //Último endereço gravado pelo MPU

    //Gravar temperaturo do MPU no instante do acidente
    sram_wr_16b(OP_DISP_TP, mpu_rd_tp()); //Gravar Temp na SRAM (área de calibra ao ligar)

    // Marcar se aquisição foi interrompida
    if (flag_brk==TRUE){
      sram_wr_16b(OP_BRK,COD_SIM);
      ser_str("*** Interrompido ***");
    }
    else  sram_wr_16b(OP_BRK,COD_NAO);                          
    
    ser_crlf(1);

    // TX serial;
    lcd_str(1,0,msg7);
    ser_str(msg7);
    opera1_serial();    //TX todos os dados
  }
  gps_des_int();  //GPS: Desabilitar interrupção
  mpu_des_int();  //MPU: Desabilitar interrupção
  lcd_str(3,0,"--Erro--");    //Se chegou aqui, deu erro no laço
  ser_str("\n\n--Erro--");    //Se chegou aqui, deu erro no laço
  while(TRUE);
}

// TX serial para opera 1
void opera1_serial(void){
  char *msg1="MPU faltam xxxxx";
  byte vet_mpu[MPU_PASSO];      //Ler bytes: ax-ay-az-gx-gy-gz
  byte vet_gps[GPS_PASSO];  //Ler mensagem GPS
  long adr;       //Ponteiro para ler os dados
  long dif,adri,adrf; //Endereço de disparo e final
  long hora,hora_min;
  word qtd;     //Quantidade de linhas MPU ou GPS
  word cont;    //Contador do envio serial
  word aux,aux1;

  ser_str("\nVai entrar modo schnell\n");
  //Ativar modo schnell
  while(sero_vazia()==FALSE);   //Esperar esvaziar fila
  sero_parou=FALSE;             //Timer pensa que fila está andando
  ser0_tx_int_en();   //Desabilitar interrupção Serial 0
  ser2_tx_int_en();   //Desabilitar interrupção Serial 2
  led_vd();
  sero_schnell=TRUE;

  // MPU TX
  lcd_apaga_lin(2);
  lcd_str(2,0,msg1);
  ser_str("\n\n#[m\n");                       //Marcar início TX MPU
  ser_cab('o');                             //Imprimir cabecalho MPU modo operação

  /*
  ser_str("imprimir\n");
  ser_dec32unz(sram_rd_32b(OP_MPU_ADR));    //Ponteiro MPU na hora do disparo
  ser_crlf(1);
  ser_dec32unz(sram_rd_32b(OP_GPS_ADR));    //Ponteiro GPS na hora do disparo
  ser_crlf(1);
  */

  adri=MPU_ADR_INI;
  adrf=sram_rd_32b(OP_MPU_ADR);
  adrf+=MPU_PASSO;                             //Avançar para o próximo (mais velho)
  if (adrf==MPU_ADR_FIM) adrf=MPU_ADR_INI;
  qtd=(MPU_ADR_FIM - MPU_ADR_INI)/MPU_PASSO;  //Qtd de linhas do MPU
  
  //Verificar se aquisição foi interrompida
  aux=sram_rd_16b(OP_BRK);
  if (aux==COD_SIM){
    dif=adrf-adri;
    if (dif<0)  dif+=(MPU_ADR_FIM - MPU_ADR_INI);
    qtd=dif/MPU_PASSO;  //Qtd de linhas do MPU até interrupção
    adr=adri;
  }

  /*
  ser_str("debug\n");
  ser_str("adri="); ser_dec32unz(adri);  ser_spc(1);
  ser_str("adrf="); ser_dec32unz(adrf);  ser_spc(1);
  ser_str("dif="); ser_dec32unz(dif);  ser_spc(1);
  ser_str("qtd="); ser_dec16unz(qtd);  ser_crlf(1);
  */
  


  ser_dec16unz(qtd);  ser_crlf(1);    //Qtd de linhas do MPU

  for (cont=0; cont<qtd; cont++){
    sram_rd_blk(adr,vet_mpu,18);
    ser_dec16u(cont);
    ser_spc(1);
    //ser_dec32u(adr);
    ser_hex32(adr);
    ser_spc(1);
    ser_lin_ac_gi_mg(vet_mpu);
    adr+=MPU_PASSO;
    if (adr==MPU_ADR_FIM) adr=MPU_ADR_INI;
    lcd_dec16u(2,11,qtd-cont);
  }
  ser_str("m]#\n");    //Marcar fim TX MPU

  // GPS TX
  lcd_str(1,0,"GPS");
  ser_str("#[g\n");    //Marcar início TX GPS
  sram_rd_str(OP_AC_DATA, vet_gps, 10);       //Ler data do acidente
  ser_str(vet_gps);                           //Imprimir data do acidente
  ser_crlf(1);
  sram_rd_str(OP_AC_HORA, vet_gps, 10);       //Ler hora do acidente
  ser_str(vet_gps);                           //Imprimir hora do acidente
  ser_crlf(1);
  adrf=sram_rd_32b(OP_GPS_ADR);                //Endereço qdo SEL foi acionada
  ser_dec32unz(adrf);                          //Imprimir endereço
  ser_crlf(1);

  //sram_dump(GPS_ADR_INI,32768L);

  adri=GPS_ADR_FIM-GPS_PASSO;
  aux=sram_rd_16b(adri);
  //ser_str("adr=");  ser_dec32(adri);   ser_str(" 0x");   ser_hex32(adri);
  //ser_str("  leu=");  ser_dec16(aux);
  //ser_crlf(1);



  if ( aux == 0){  //Não encheu toda a área do GPS
    //ser_str("\nNao encheu\n");
    adri=GPS_ADR_INI;                            //
    aux=1;                                       //
    cont=0;                                      //
    while (aux != 0){                            // Calcular qtd linhas do GPS
      adri+=GPS_PASSO;                           //
      aux=sram_rd_16b(adri);                     //
      cont++;                                    //
    }                                            //
    ser_dec16unz(cont-1); ser_crlf(1);           //Imprimir quantidade de linhas GPS
    adri=GPS_ADR_INI+GPS_PASSO;                  //
    cont=0;                                      //
    aux=1;                                       //
    while (aux != 0){                            //
      sram_rd_blk(adri,vet_gps,GPS_PASSO);       //
      ser_dec32u(cont++);                        //
      ser_spc(1);                                //
      ser_dec32(adri);                           //
      ser_spc(1);                                //
      ser_gps_dados_lin(vet_gps);                //
      adri+=GPS_PASSO;                           //
      aux=sram_rd_16b(adri);                     //
    }                                            //
  }                                              //
  
  else{                                       //Encheu toda a área de SRAM do GPS
    //ser_str("\nEncheu\n");                    //
    qtd=(GPS_ADR_FIM-GPS_ADR_INI)/GPS_PASSO;  //
    ser_dec16unz(qtd); ser_crlf(1);           //Imprimir quantidade de linhas GPS
                                              //Buscar pelo registro mais antigo
    //ser_str("\nBuscar pela linha mais antiga:\n");
    adrf=adri=GPS_ADR_INI;
    
    hora_min=246060;    //24H 60M 60Seg - fictícia
    for (cont=0; cont<qtd; cont++){
      sram_rd_str(adri+GPS_HORA,vet_gps,10);    //Ler hora do acidente
      vet_gps[6]=0;                            //Remover parte fracionária
      hora=str_to_dec32(vet_gps);              //
      //ser_str("Min=["); ser_dec32(adrf);  ser_str("]="); ser_dec32(hora_min);
      //ser_str("   atual=["); ser_dec32(adri);  ser_str("]="); ser_dec32(hora);
      //ser_crlf(1);
      if (hora<hora_min){
        hora_min=hora;
        adrf=adri;
      }
      adri=adri+GPS_PASSO;                    //
      if (adri==GPS_ADR_FIM) adri=GPS_ADR_INI;//
    }




    /*
    adri=sram_rd_32b(OP_GPS_ADR);             //Endereço qdo SEL foi acionada
    sram_rd_str(adri+GPS_HORA,vet_gps,10);    //Ler hora do acidente
    hora1=str_to_dec32(vet_gps);              //
    adrf=adri+GPS_PASSO;                      //
    if (adrf == GPS_ADR_FIM) adrf=GPS_ADR_INI;//
    sram_rd_str(adrf+GPS_HORA,vet_gps,10);    //Ler hora do acidente
    hora2=str_to_dec32(vet_gps);              //
    while(hora2>hora1){                       //Busca pela hora mais cedo
      hora1=hora2;                            //
      adrf=adrf+GPS_PASSO;                    //
      if (adrf==GPS_ADR_FIM) adrf=GPS_ADR_INI;//
      sram_rd_str(adrf+GPS_HORA,vet_gps,10);  //Ler hora do acidente
      hora2=str_to_dec32(vet_gps);            //
    }                                         //
    */
    
    for (cont=0; cont<qtd; cont++){           //Imprimir linhas GPS
      sram_rd_blk(adrf,vet_gps,GPS_PASSO);    //
      ser_dec32u(cont);                       //
      ser_spc(1);                             //
      ser_dec32(adrf);                        //
      ser_spc(1);                             //
      ser_gps_dados_lin(vet_gps);             //
      adrf+=GPS_PASSO;                        //
      if (adrf==GPS_ADR_FIM) adrf=GPS_ADR_INI;//
    }                                         //
  } 
  ser_str("g]#\n");    //Marcar fim TX GPS

  ser_str("#[l\n");    //Marcar Início TX Calibração do Ligar
  sram_op_dados();
  ser_str("l]#\n");    //Marcar Fim TX Calibração do Ligar

  ser_str("#[f\n");    //Marcar Início TX Calibração do Ligar
  eeprom_cf_dados();
  ser_str("f]#\n");    //Marcar Fim TX Calibração do Ligar - Fim de tudo

  //Sair do modo Schnell
  ser0_tx_int_EN();   //Habilitar interrupção Serial 0
  ser2_tx_int_EN();   //Habilitar interrupção Serial 2
  sero_schnell=FALSE;
  sero_parou=TRUE;    //Timer pensa que fila está andando


}

// Fazer toda a preparação para a operação 1
void opera1_prepara(void){
  char *msg1="Erro Who am I = ";
  char *msg2="MPU Self Test Falhou";
  char *msg3="MAG Self Test Falhou";
  byte x;
  byte st;        //Resultado do Self Test
  int st_vet[24]; //Receber resultados do Self Test
  byte esc_ac,esc_gi,esc_mg;   //Guardar Escalas de acel, giro e mag
  int aux[7];

  sram_wr_16b(OP_OK,COD_SIM);         //Marcar que fez Calibração ao ligar
  mpu_acorda();
  mpu_config();
  x=mpu_whoami();                     //Who am I
  if (x != MPU9250_WHO){
    lcd_str(1,0,msg1); lcd_dec16unz(1,16,x);
    ser_str(msg1);     ser_dec16unz(x);  ser_crlf(1);
    delay(1000);
  }
  sram_wr_16b(OP_BATEU,COD_NAO);        //Nao Batida

  st=mpu_self_test(st_vet,FALSE);       //Self Test MPU
  if (st==TRUE)
    sram_wr_16b(OP_ST_OK,COD_SIM);
  else{
    sram_wr_16b(OP_ST_OK,COD_NAO);
    lcd_str(1,0,msg2);
    ser_str(msg2);  ser_crlf(1);
    delay(200);
  }

  st=mpu_mag_self_test(st_vet,FALSE);       //Self Test MAG
  if (st==TRUE)
    sram_wr_16b(OP_STH_OK,COD_SIM);
  else{
    sram_wr_16b(OP_STH_OK,COD_NAO);
    lcd_str(1,0,msg2);
    ser_str(msg3);  ser_crlf(1);
    delay(200);
  }

  
  x=eeprom_rd_16b(CF_OK);   //Calibração de Fábrica
  sram_wr_16b(OP_CF_OK,x);  //Gravar código de Calibr de Fábrica

  //Calibração Acel e Giro ao ligar o carro
  esc_ac=ACEL_FS_2G;    //Escala acel para calibrar ao ligar
  esc_gi=GIRO_FS_250;   //Escala giro para calibrar ao ligar
  mpu_calibra(aux, OP_QTD_MED_AG, esc_ac, esc_gi);  //Função de calibração
  sram_wr_16b(OP_OK,COD_SIM);                       //Fez calibração ao ligar
  x=mpu_rd_freq();      sram_wr_16b(OPC_FREQ_AG,x); //Freq de amostragem
  x=mpu_rd_bw();        sram_wr_16b(OPC_BW_AG,x);   //Banda Passante
  x=mpu_rd_esc_acel();  sram_wr_16b(OPC_ESC_AC,x);  //Escala do Acelerômetro
  x=mpu_rd_esc_giro();  sram_wr_16b(OPC_ESC_GI,x);  //Escala do Giroscópio
  sram_wr_16b(OPC_QTD_AG,OP_QTD_MED_AG);            //Qtd de medidas para calibração ao ligar
  for (x=0; x<7; x++)   sram_wr_16b(OPC_AX+(2*x),aux[x]);

 
  //Configuração Acel e Giro para a Operação
  mpu_config();         //Novamente inicializa MPU
  esc_ac=ACEL_FS_2G;    //Escala acel para calibrar ao ligar
  esc_gi=GIRO_FS_250;   //Escala giro para calibrar ao ligar
  mpu_escalas(esc_gi,esc_ac);       //Programar escalas
  mpu_sample_rt(SAMPLE_RT_100Hz);   //Programar taxa de amostragem
  mpu_mag_config();                 //Configurar Magnetômetro

  //Definir freq e limiares de acidente
  x=mpu_rd_freq();      sram_wr_16b(OP_FREQ_AG,x); //Freq de amostragem
  x=mpu_rd_bw();        sram_wr_16b(OP_BW_AG,x);   //Banda Passante
  x=mpu_rd_esc_acel();  sram_wr_16b(OP_ESC_AC,x);  //Escala do Acelerômetro
  x=mpu_rd_esc_giro();  sram_wr_16b(OP_ESC_GI,x);  //Escala do Giroscópio
  sram_wr_16b(OP_LIM_AX,LIMIAR_AX);     //AX - Limiar para disparo de acidente
  sram_wr_16b(OP_LIM_AY,LIMIAR_AY);     //AY - Limiar para disparo de acidente
  sram_wr_16b(OP_LIM_AZ,LIMIAR_AZ);     //AZ - Limiar para disparo de acidente
  sram_wr_16b(OP_LIM_GX,LIMIAR_GX);     //GX - Limiar para disparo de acidente
  sram_wr_16b(OP_LIM_GY,LIMIAR_GY);     //GY - Limiar para disparo de acidente
  sram_wr_16b(OP_LIM_GZ,LIMIAR_GZ);     //GZ - Limiar para disparo de acidente

  sram_wr_32b(OP_MPU_ADR,0L);           //Ponteiro MPU quando ocorreu disparo
  sram_wr_32b(OP_GPS_ADR,0L);           //Ponteiro GPS quando ocorreu disparo

  //Quem disparou
  for (x=0; x<6; x++)   sram_wr_16b(OP_DISP_AX+2*x,COD_NAO);

  //Data e Hora
  sram_wr_str(OP_AC_DATA,"ddmmyy");
  sram_wr_str(OP_AC_HORA,"hhmmss.sss");

  //sram_op_mostra();
  //ser_crlf(2);
  //sram_op_dados();
}

// 2 - Vazio
char opera_2(char md){
  lcd_str(0,0,opera_msg[md]);
  ser_str(opera_msg[md]);
  sw_qq_tecla();
  return md;
}

// 3 - Vazio
char opera_3(char md){
  lcd_str(0,0,opera_msg[md]);
  ser_str(opera_msg[md]);
  sw_qq_tecla();
  return md;
}

// 4 - Vazio
char opera_4(char md){
  lcd_str(0,0,opera_msg[md]);
  ser_str(opera_msg[md]);
  sw_qq_tecla();
  return md;
}

// 5 - Calibração de Fábrica
char opera_5(char md){
  char *msg1="[5]Calibracao Fabrica";
  byte x;
  lcd_str(0,0,msg1);
  lcd_str(2,0,"Use Monitor Serial");
  calibra_fab();
  while(fim_qqtec_x() == FALSE);  //qq Tecla o letra x para finalizar
  return md;
}

// [5] Rotina de Calibração de Fábrica
void calibra_fab(void){
  float gp,gl;      //Ac.da gravidade
  word mpu_gp=0;    //Receber Ac.da gravidade padrão em 16 bits (+/- 2g)
  word mpu_gl=0;    //Receber Ac.da gravidade local em 16 bits (+/- 2g)
  long cont=0;
  long atg_soma[7];   //Somatório para calc media [ax-ay-az-tp-gx-gy-gz]
  int atg[7];         //Receber leituras MPU
  int atg_pri[7];     //Receber a primeira medida
  int atg_ult[7];     //Receber a última medida
  word freq;
  word x,aux=0;
  byte who;
  byte vet[40];     //Receber strings para imprimir float
  int vet_st[24];   //Receber resultados do Self test
  int vet_st_mag[3];
  byte st;
  byte st_mag;
  int ac_cal[6];
  byte magASA[3];
  int magExtr[6];
  lcd_str(2,0,"Use Monitor Serial");
  ser_str("\n--- Calibracao de Fabrica ---\n");
  //eeprom_wr_ff();
  //eeprom_dump(0,256);

  mpu_config();
  who=mpu_whoami();
  mpu_escalas(GIRO_FS_250,ACEL_FS_2G);   //+/- 2g e +/-250gr/seg
  freq=CF_FREQ;
  switch(freq){
    case 100:   mpu_sample_rt(SAMPLE_RT_100Hz); break;
    case 200:   mpu_sample_rt(SAMPLE_RT_200Hz); break;
    case 250:   mpu_sample_rt(SAMPLE_RT_250Hz); break;
    case 500:   mpu_sample_rt(SAMPLE_RT_500Hz); break;
  }
  mpu_int();
  mpu_dado_ok=FALSE;
  cont=0;
  aux=0;

  ser_str("Calculando erros intrínsecos.\n");
  ser_str("Mantenha o dispositivo parado por 10 segundos.\n");
  while (cont<CF_ESPERA){
    while (mpu_dado_ok == FALSE);   //Aguardar MPU fazer medida
      mpu_dado_ok=FALSE;
      aux++;
      if (aux==freq){
        aux=0;
        cont++;         //Passou mais um segundo
        lcd_dec16unz(3,8,CF_ESPERA-cont);
        ser_dec32unz (CF_ESPERA-cont);
        ser_crlf(1);
      }
  }

  //Gravar dados de local e data
  eeprom_wr_str(CF_DATA,CF_HOJE);         //Data - Indicado em Defines
  eeprom_wr_str(CF_LOCAL,CF_BSB);         //Local - Indicado em Defines
  gp=G_PADRAO;
  gl=G_BSB;
  str_float(gp,7,vet);
  eeprom_wr_str(CFG_PADRAO,&vet[1]);      //g padrão, pula o sinal de  +9,8...
  str_float(gl,7,vet);
  eeprom_wr_str(CFG_LOCAL,&vet[1]);       //g local, pula o sinal de +9,8...
  eeprom_wr_16b(CFG_PADRAO_BIN,0x3FF);    //1g na escala de +/- 2g
  aux=0x3FF*(gl/gp);                       
  eeprom_wr_16b(CFG_LOCAL_BIN,aux);       //g Local na escala de +/- 2g
  eeprom_wr_16b(CF_WHO,who);              //Resposta ao Who am I
  //
  aux=mpu_rd_freq();
  eeprom_wr_16b(CF_FA,aux);               //Freq Amostragem usada
  aux=mpu_rd_bw();
  eeprom_wr_16b(CF_BW,aux);               //Banda Passante
  aux=mpu_rd_esc_acel();
  eeprom_wr_16b(CF_ESC_AC,aux);           //Escala Acelerômetro
  aux=mpu_rd_esc_giro();
  eeprom_wr_16b(CF_ESC_GI,aux);           //Escala Giroscópio
  
  // Fazer a primeira medida
  mpu_dado_ok=FALSE;
  while (mpu_dado_ok == FALSE);   //Aguardar MPU fazer medida
  mpu_rd_ac_tp_gi(atg);           //Ler MPU
  for (x=0; x<7; x++){
    atg_pri[x] =atg[x];           //Guardar a primeira medida
    atg_soma[x]=atg[x];           //Inicializar Acumulador de medidas
  }
  
  //Fazer as demais medidas
  cont=0;
  while(cont<CF_QTD_MED-2){       //Primeira e última medidas feitas à parte
    while (mpu_dado_ok == FALSE);  //Aguardar MPU fazer medida
    mpu_dado_ok=FALSE;
    mpu_rd_ac_tp_gi(atg);          //Ler MPU
    for (x=0; x<7; x++)   atg_soma[x]+=atg[x];  //Acumular
    cont++;
  }

  // Fazer a última medida
  while (mpu_dado_ok == FALSE);  //Aguardar MPU fazer medida
  mpu_rd_ac_tp_gi(atg);          //Ler MPU
  for (x=0; x<7; x++){
    atg_ult[x]  =atg[x];        //Guardar a última medida
    atg_soma[x]+=atg[x];        //Acumular
  }

  // Gravar as médias, as primeiras e as últimas medidas
  eeprom_wr_16b(CF_QTD,CF_QTD_MED);
  for (x=0; x<7; x++){
    eeprom_wr_16b(CF_AX+(2*x),atg_soma[x]/CF_QTD_MED);  //Médias
    eeprom_wr_16b(CF_AX_PRI+(2*x),atg_pri[x]);          //Primeiras
    eeprom_wr_16b(CF_AX_ULT+(2*x),atg_ult[x]);          //Últimas
    eeprom_wr_32b(CF_AX_SOMA+(4*x),atg_soma[x]);        //Somatórios
  }

  ser_str("Realizando self-test\n");

//  função mpu_self_test está errada 
//  st=mpu_self_test(vet_st,FALSE);

  st = mpu_self_test_2(FALSE); 
  st_mag = mpu_mag_self_test(vet_st_mag,TRUE);
  
  if (st==TRUE && st_mag==TRUE)   eeprom_wr_16b(CF_ST_OK,COD_SIM);
  else            eeprom_wr_16b(CF_ST_OK,COD_NAO);
  
  
  for (x=0; x<24; x++)
    eeprom_wr_16b(CF_ST_OFF_AX+(2*x),vet_st[x]);

  eeprom_wr_16b(CF_OK,COD_SIM);


  //calibra o acelerômetro
  acel_calibra(ac_cal,TRUE);

  //grava dados dados coletados do acelerômetro na eeprom
  for (x=0; x<6; x++)
    eeprom_wr_16b(CF_AX_X_CIMA+(2*x),ac_cal[x]);

  //calibra o magnetômetro
  mpu_mag_rd_rom(magASA);  
  mag_calibra(magASA, magExtr, TRUE );

  //grava dados dados coletados do magnetômetro na eeprom
  eeprom_wr_16b(CF_HX_ASA,(int)magASA[0]); //(ASAx) Ajuste de sensibilidade de hx
  eeprom_wr_16b(CF_HY_ASA,(int)magASA[1]); //(ASAy) Ajuste de sensibilidade de hy
  eeprom_wr_16b(CF_HZ_ASA,(int)magASA[2]); //(ASAz) Ajuste de sensibilidade de hz
  for (x=0; x<6; x++)
    eeprom_wr_16b(CF_HX_MIN+(2*x),magExtr[x]);
  
 
  eeprom_cf_mostra();
  //ser_crlf(2);
  //eeprom_cf_dados();
  
  //ser_str("\nFim. Qq tecla para finalizar.");
  //sw_qq_tecla();
  
}

// 6 - Vazio
char opera_6(char md){
  lcd_str(0,0,opera_msg[md]);
  ser_str(opera_msg[md]);
  sw_qq_tecla();
  return md;
}

// 7 - Calibração do Magnetometro com matlab
char opera_7(char md){
   char *msg="[7] Calib Mag Matlab";
  byte mag_st,who,asa[3];
  int vetor[3],hx,hy,hz;
  float vtf[7];
  word ac_esc, giro_esc;
  lcd_apaga();
  lcd_str(0,0,msg);
  ser_str(msg);
  ser_crlf(1);

  mpu_config();         //MPU configurar
  mpu_mag_config();     //MAG configurar

  who = mag_whoami();
  lcd_str(1,0,"Who am I = ");
  lcd_hex8(1,11,who);
  ser_str("MPU MAG retornou Who am I = ");
  ser_hex8(who);
  if (who == MAG_WHO){
    lcd_str(1,13,"h OK");  //MPU respondendo
    ser_str("h ==> OK!\n");
  }
  else{
    lcd_str(1,13,"h NOK");  //MPU Não respondendo
    ser_str("h ==> NOK! ERRO\n");
    return FALSE;
  }
  
  // Iniciar transmissão
  ser_str("#[");      //Avisar Matlab
  mpu_mag_rd_rom(asa);
  ser_dec8(asa[0]);   ser_crlf(1);
  ser_dec8(asa[1]);   ser_crlf(1);
  ser_dec8(asa[2]);   ser_crlf(1);
  
  // Habilitar interrupção MPU (Dado Pronto)
  mpu_sample_rt(SAMPLE_RT_100Hz);
  mpu_int();

  lcd_str(2,0,"DRDY=X    HOFL=X");
  lcd_str(3,0,"X:hhhh Y:hhhh Z:hhhh");
  while(TRUE){
    while (mpu_dado_ok == FALSE);   //Agaurdar MPU a 100 Hz (10 ms)
    mpu_dado_ok=FALSE;
    mag_st=mpu_rd_mg_out(vetor);

    if (mag_st==1){        //Tudo certo
     lcd_char(2, 5,'1');  
     lcd_char(2,15,'0');  
   }
   else if(mag_st==0){    //Dado não pronto
     lcd_char(2, 5,'0');  
     lcd_char(2,15,'0');  
     //vetor[0]=vetor[1]=vetor[2]=0;
     //vetor[0]=hx;
     //vetor[1]=hy;
     //vetor[2]=hz;
   }
   else if(mag_st==2){    //Sensor Overflow
     lcd_char(2, 5,'1');  
     lcd_char(2,15,'1');  
   }
    ser_dec16(vetor[0]);    ser_crlf(1);    //hx
    ser_dec16(vetor[1]);    ser_crlf(1);    //hy
    ser_dec16(vetor[2]);    ser_crlf(1);    //hz

    //escreve no LCD   
    lcd_hex16(3,2, vetor[0]);
    lcd_hex16(3,9, vetor[1]);
    lcd_hex16(3,16,vetor[2]);
    hx=vetor[0];
    hy=vetor[1];
    hz=vetor[2];

    //if (sw_tira(&who))     break;
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar

  }
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  ser_dec16(22222);      ser_crlf(1);           //Finalizar com Matlab
  lcd_apaga();
  hx=hy=hz=1111;
  lcd_str(0,0,"Recebido do Matlab");
  seri_num16(&hx);        lcd_dec16(1, 0,hx);
  seri_num16(&hy);        lcd_dec16(1, 7,hy);
  seri_num16(&hz);        lcd_dec16(1,14,hz);
  
  // (Offset) Gravar Calibração na EEPROM
  eeprom_wr_16b(CF_MAG_OK,COD_SIM);      //Marcar que fez calibração do Magnetômetro
  eeprom_wr_16b(CF_HX_OFF,hx);           //Offset de hx (dividir por 10)
  eeprom_wr_16b(CF_HY_OFF,hy);           //Offset de hy (dividir por 10)
  eeprom_wr_16b(CF_HZ_OFF,hz);           //Offset de hz (dividir por 10)

  seri_num16(&hx);  lcd_dec16(2, 0,hx);
  seri_num16(&hy);  lcd_dec16(2, 7,hy);
  seri_num16(&hz);  lcd_dec16(2,14,hz);

  // (Escala) Gravar Calibração na EEPROM
  eeprom_wr_16b(CF_HX_ESC,hx);          //Escala de hx (dividir por 10)
  eeprom_wr_16b(CF_HY_ESC,hy);          //Escala de hy (dividir por 10)
  eeprom_wr_16b(CF_HZ_ESC,hz);          //Escala de hz (dividir por 10)

  // (ASA) Gravar Ajustes de Sensibilidade
  mpu_mag_rd_rom(asa);
  eeprom_wr_16b(CF_HX_ASA,(int)asa[0]); //(ASAx) Ajuste de sensibilidade de hx
  eeprom_wr_16b(CF_HY_ASA,(int)asa[1]); //(ASAy) Ajuste de sensibilidade de hy
  eeprom_wr_16b(CF_HZ_ASA,(int)asa[2]); //(ASAz) Ajuste de sensibilidade de hz
  
  while(fim_qqtec_x() == FALSE);  //qq Tecla ou letra x para finalizar
  ser_str("\n--- Fim ---\n");
  while(sw_tira(&who) == FALSE);
  
  return md;
}

// 8 - Calibração do acelerometro com matlab
char opera_8(char md){
  // Faz 50 medidas cada vez que aperta o botão. É usado nos scripts de calibração do acelerômetro
  
  char *msg="[8] MPU --> Calibra Acel";
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
}

// 9 - Vazio
char opera_9(char md){
  lcd_str(0,0,opera_msg[md]);
  ser_str(opera_msg[md]);
  sw_qq_tecla();
  return md;
}
