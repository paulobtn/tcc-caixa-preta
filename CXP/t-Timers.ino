// Timers
// CXP - Caixa Preta
// 16/01/2019

// Configurar Timer 2 para FREQ_T2 (define)
// Modo CTC com limite indicado por OCR2A
void timer2_config(void){
  cli();                          //Int Desab geral
  TCCR2A = (1<<WGM21);           //WGM21=1 
  TCCR2B = (1<<CS22);             // Modo CTC e CLK/64
  TIMSK2 = 0;                 //Interrup desab CTC (ORC1A)
  OCR2A = (25000/(FREQ_T2/10))-1;
  TIFR2 = 0;
  ASSR = 0;
  GTCCR = 0;
  sei();                                    //Int: Hab geral
}

// ISR Timer 2: FREQ_T2 (1000 Hz)
ISR(TIMER2_COMPA_vect){
  byte aux;
  if (lcd_ix<21){
    switch(lcd_fase){
      case 0: aux=(lcd_lin[lcd_ix])&0xF0;
                PORTC &= ~(LCD_RW|LCD_RS|LCD_E);   //RW=RS=E=LOW
                PORTC |= LCD_E;                   //E=H
                PORTA &= ~(BIT7|BIT6|BIT5|BIT4);  //D7654=0
                PORTA |= aux;                     //D7654=MSNible
                //PORTC &= ~LCD_E;                  //E=L
                lcd_fase++;
                break;
      case 1: aux=(lcd_lin[lcd_ix]&0xF)<<4;
                PORTC &= ~(LCD_RW|LCD_RS|LCD_E);   //RW=RS=E=LOW
                PORTC |= LCD_E;                   //E=H
                PORTA &= ~(BIT7|BIT6|BIT5|BIT4);  //D7654=0
                PORTA |= aux;                     //D7654=MSNible
                //PORTC &= ~LCD_E;                  //E=L
                lcd_fase++;
                lcd_ix++;
                break;
      case 2: aux=lcd_lin[lcd_ix]&0xF0;
                PORTC &= ~(LCD_RW|LCD_RS|LCD_E);   //RW=RS=E=LOW
                PORTC |= LCD_RS|LCD_E;            //RS=E=H
                PORTA &= ~(BIT7|BIT6|BIT5|BIT4);  //D7654=0
                PORTA |= aux;                     //D7654=MSNible
                //PORTC &= ~LCD_E;                  //E=L
                lcd_fase++;
                break;
      case 3: aux=(lcd_lin[lcd_ix]&0xF)<<4;
                PORTC &= ~(LCD_RW|LCD_RS|LCD_E);   //RW=RS=E=LOW
                PORTC |= LCD_RS|LCD_E;            //RS=E=H
                PORTA &= ~(BIT7|BIT6|BIT5|BIT4);  //D7654=0
                PORTA |= aux;                     //D7654=MSNible
                //PORTC &= ~LCD_E;                  //E=L
                lcd_fase=2;
                lcd_ix++;
                break;
    }
  }
  else{
    PORTC &= ~LCD_E;          //Último E=L (última letra)
    TIMSK2 &= ~(1<<OCIE2A);   //Desabilitar interrup CTC (ORC1A)
    lcd_busy=FALSE;           //Liberar acesso ao LCD
  }
}

// Configurar Timer 1 para FREQ_T1 (define)
void timer1_config(void){
  cli();                                    //Int Desab geral
  TCCR1A = 0;                               //WGM11=WGM10=0 
  TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10);  // Modo CTC e CLK/64
  TCCR1C = 0;
  TIMSK1 = (1<<OCIE1A);                     //Int CTC (ORC1A)
  OCR1A = (25000/(FREQ_T1/10))-1;
  sei();                                    //Int: Hab geral
}

// ISR Timer 1: FREQ_T1 (100 Hz)
ISR(TIMER1_COMPA_vect){
  byte i;
  flag_10ms=TRUE;
  // Verificar se LCD precisa de atualização
  if (lcd_mudou){
    if (lcd_busy==FALSE){
      for (i=0; i<NRL; i++){
        if (lcd_linha_alterou(lcd_rr)==TRUE){
          lcd_fase=0;
          lcd_ix=0;
          lcd_busy=TRUE;
          TIFR2 |= (1<<OCF2A);      //Apagar interrup pendente
          TIMSK2 |= (1<<OCIE2A);    //Habilitar interrup CTC (ORC1A)
          break;
        }
        lcd_rr = (lcd_rr+1)&3;
      }
    }
  }
  lcd_rr = (lcd_rr+1)&3;

  //Verificar fila sero (saída UART0 e UART2)
  if (sero_parou == TRUE){
    if (sero_tira(&i) == TRUE){
      UDR0=i;
      UDR2=i;
      sero_parou=FALSE;
    }
  }

  // Demais tarefas do Timer 1
  switch(timer1_cont){
    
    case 0: case 13: case 16: case 29:
      ADCSRA |= 1<<ADSC;     //Disparar ADC;
      break;

    case 12:
      adc_val+=ADCH;
      ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(1<<MUX0);  //VCAR: Canal 1, Ref = 2,56
      sw_val=adc_val/2;
      sw_ler(sw_val);
      break;

    case 14: case 30:
      adc_val=ADCH;
      ADCSRA |= 1<<ADSC;     //Disparar ADC;
      break;

    case 15:
      vcar_val=(adc_val+ADCH)/2;      //Ler VCAR
      ADMUX = (1<<REFS0)|(1<<ADLAR);  //Progr TEC: Canal 0, Ref=AVCC
      break;

    case 28:
      adc_val+=ADCH;
      ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(1<<MUX1);  //Prog VCAP: Canal 2, Ref = 2,56
      sw_val=adc_val/2;
      sw_ler(sw_val);
      break;

    case 31:
      vcap_val=(adc_val+ADCH)/2;      //Ler VCAP
      ADMUX = (1<<REFS0)|(1<<ADLAR);  //TEC: Canal 0, Ref=AVCC
      break;

    default:
      if ( (timer1_cont&1) == 1){  //1,3,5,7,9,11, 17,19, 21, 23, 25, 27
        adc_val=ADCH;       //Ler ADC
        ADCSRA |= 1<<ADSC;  //Disparar ADC;
      }
      else{                     //2, 4, 6, 8, 10,   18, 20, 22, 24, 26
        //adc_val+=ADCH;
        sw_val=(adc_val+ADCH)/2;
        sw_ler(sw_val>>5);
      }
      break;
  }
  timer1_cont++;
  timer1_cont &= 0x1F;
}

// qual = 0, 1, 2, 3
byte lcd_linha_alterou(byte qual){
  switch(qual){
    case 0: if ( (lcd_mudou&LCD_LINHA0) == LCD_LINHA0){
              lcd_lin=lcd_buf[0];
              lcd_mudou &= ~LCD_LINHA0; 
              return TRUE;
            }
            else return FALSE;
    case 1: if ( (lcd_mudou&LCD_LINHA1) == LCD_LINHA1){
              lcd_lin=lcd_buf[1];
              lcd_mudou &= ~LCD_LINHA1; 
              return TRUE;
            }
            else return FALSE;
    case 2: if ( (lcd_mudou&LCD_LINHA2) == LCD_LINHA2){
              lcd_lin=lcd_buf[2];
              lcd_mudou &= ~LCD_LINHA2; 
              return TRUE;
              }
            else return FALSE;
    case 3: if ( (lcd_mudou&LCD_LINHA3) == LCD_LINHA3){
              lcd_lin=lcd_buf[3];
              lcd_mudou &= ~LCD_LINHA3; 
              return TRUE;
            }
            else return FALSE;
  }
}
