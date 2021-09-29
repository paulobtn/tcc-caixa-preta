// Teclas- Monitorar teclas - sw_xxxxx
// Leds - Controlar LEDs led_xxxxx
// // CXP - Caixa Preta
// 16/01/2019

// Leds da Caixa Preta
// PB7 = AM-Original
// PD7 = Verde
// PG2 = Amarelo
// PG1 = Vermelho

//Pinos para Osciloscópio
// Pino 7 = PH4 = SCP1
// Pino 8 = PH5 = SCP2

//////////////////////////////////////////////////
///////////////// TECLAS /////////////////////////
//////////////////////////////////////////////////

// Esperar por uma tecla qualquer
// Retorna quando conseguir ler algo da fila
// O que é lido é ignorado
void sw_qq_tecla(void){
  byte lixo;
  while( sw_tira(&lixo) == FALSE );
}

// Limpar fila do teclado. Joga tudo fora
void sw_fila_limpa(void){
  sw_pin=1;
  sw_pout=0; 
}

// Analisa com o passado e se for o caso coloca o código no buffer
void sw_ler(byte val){
  sw_2=val;
  if (sw_2 != SW_NAOSEI){
    if (sw_1 == sw_2){
      sw_n=sw_2;
      if (sw_v != sw_n){
        if (sw_v == SW_NADA){
          sw_v=sw_n;
          sw_poe(sw_n);
          sw_busca_seq1(sw_n);
          sw_busca_seq2(sw_n);
        }
        else  sw_v=sw_n;
      } 
    }
    else{  sw_1=sw_2;  }
  }
}

// Buscar pela Sequência 1
void sw_busca_seq1(byte val){
  switch(sw_st_seq1){
    case 0: if (val == SW_ESQ)   sw_st_seq1=1;    break;
    case 1: if (val == SW_SUP)  sw_st_seq1 =2;   break;
    case 2: if (val == SW_DIR)   sw_poe(SW_SEQ1); sw_st_seq1=0;  break;
    default:  sw_st_seq1=0;
  }
}

// Buscar pela Sequência 2
void sw_busca_seq2(byte val){
  switch(sw_st_seq2){
    case 0: if (val == SW_ESQ)    sw_st_seq2=1;    break;
    case 1: if (val == SW_INF)  sw_st_seq2 =2;   break;
    case 2: if (val == SW_DIR)    sw_poe(SW_SEQ2); sw_st_seq2=0;  break;
    default:  sw_st_seq2=0;
  }
}

// Configura o ADC e a fila para chaves
void sw_config(void){
  cli();                //Desabilitar interrupções
  // Configurar ADC
  ADMUX = (1<<REFS0)  |  //Ref = AVCC
          (1<<ADLAR);    //Alinhar pela esquerda, ler apenas ADCH
  ADCSRA = (1<<ADEN)  |  //Habilita ADC
           //(1<<ADIE)  |  //Habilitar interrupção ADC
           (1<<ADPS1) |  //Divisor = 8
           (1<<ADPS0);   //CLK = 16/8 = 2 MHz
  ADCSRB = 0;            //
  ADCSRA |= 1<<ADSC;     //Fazer primeiro disparo
  adc_ok=FALSE;
  sei();                 //Int: Habilitação geral
  // Inicializar fila
  sw_pin=1;
  sw_pout=0;
  sw_1=sw_2=sw_n=sw_v=sw_st_seq1=sw_st_seq2=0;
}

// Colocar uma chave no buffer
byte sw_poe(byte cha){
  if (sw_pin == sw_pout)  return FALSE;    // fila cheia
  else{
    sw_fila[sw_pin++]=cha;
    if (sw_pin==SW_FILA_TAM)  sw_pin=0;
    return TRUE;
  }
}

// Tirar uma chave do buffer
byte sw_tira(byte *cha){
  byte pout_aux;
  pout_aux=sw_pout+1;
  if (pout_aux==SW_FILA_TAM)  pout_aux=0;
  if (pout_aux == sw_pin)    return(FALSE);
  else{
    *cha=sw_fila[pout_aux];
    sw_pout=pout_aux;
    return TRUE;
  }
}

//////////////////////////////////////////////////
///////////////// LEDS ///////////////////////////
//////////////////////////////////////////////////

// Contador binário com 4 leds
// Laranja - Verde - Amarelo - Vermelho
void leds_cont(byte ct){
  ct &= 0xF;
  if ( (ct&BIT0) == BIT0) led_VM();
  else                    led_vm();
  if ( (ct&BIT1) == BIT1) led_AM();
  else                    led_am();
  if ( (ct&BIT2) == BIT2) led_VD();
  else                    led_vd();
  if ( (ct&BIT3) == BIT3) led_AMO();
  else                    led_amo();
}

// Configurar pinos para Osciloscópio
void scope_config(void){
  DDRH |= (1<<DDH5)|(1<<DDH4);      //SCP1 e SCP2
  PORTH &= ~((1<<DDH5)|(1<<DDH4));  //SCP1=SCP2=Low
}

// Pino 7 = SCP1 (PH4) = Scope 1: Apagar / Acender / Inverter
void scp1(void){ PORTH &= ~(1<<DDH4);  }  //SCP1 = Apagado
void SCP1(void){ PORTH |=  (1<<DDH4);  }  //SCP1 = Aceso
void Scp1(void){ PORTH ^=  (1<<DDH4);  }  //SCP1 = Invertido

// Pino 8 = SCP2 (PH5) = Scope 2: Apagar / Acender / Inverter
void scp2(void){ PORTH &= ~(1<<DDH5);  }  //SCP2 = Apagado
void SCP2(void){ PORTH |=  (1<<DDH5);  }  //SCP2 = Aceso
void Scp2(void){ PORTH ^=  (1<<DDH5);  }  //SCP2 = Invertido

// Configurar pinos dos leds
void leds_config(void){
  DDRB  |=  (1<<DDB7);  //AMO = Amarelo Original
  PORTB &= ~(1<<DDB7);  //AMO = Apagado
  DDRD  |=  (1<<DDD7);  //VD
  PORTD &= ~(1<<DDD7);  //VD = Apagado
  DDRG  |=  (1<<DDG2)|(1<<DDG1);    //AM e VM
  PORTG &= ~((1<<DDG2)|(1<<DDG1));  //AM e VM Apagados
}

// AMO (PB7) = Amarelo Original: Apagar / Acender / Inverter
void led_amo(void){ PORTB &= ~(1<<DDB7);  }  //AMO = Apagado
void led_AMO(void){ PORTB |=  (1<<DDB7);  }  //AMO = Aceso
void led_Amo(void){ PORTB ^=  (1<<DDB7);  }  //AMO = Invertido

// VD (PD7) = Verde: Apagar / Acender / Inverter
void led_vd(void){ PORTD &= ~(1<<DDD7);  }  //VD = Apagado
void led_VD(void){ PORTD |=  (1<<DDD7);  }  //VD = Aceso
void led_Vd(void){ PORTD ^=  (1<<DDD7);  }  //VD = Invertido

// AM (PG2) = Amarelo: Apagar / Acender / Inverter
void led_am(void){ PORTG &= ~(1<<DDG2);  }  //AM = Apagado
void led_AM(void){ PORTG |=  (1<<DDG2);  }  //AM = Aceso
void led_Am(void){ PORTG ^=  (1<<DDG2);  }  //AM = Invertido

// VM (PG1) = Vermelho: Apagar / Acender / Inverter
void led_vm(void){ PORTG &= ~(1<<DDG1);  }  //VM = Apagado
void led_VM(void){ PORTG |=  (1<<DDG1);  }  //VM = Aceso
void led_Vm(void){ PORTG ^=  (1<<DDG1);  }  //VM = Invertido
