// TWI (I2C) - Rotinas para acesso
// CXP - Caixa Preta

// Verificar escravo receptor num endereço
// TRUE  se escravo respondeu com ACK
// FALSE se escravo respondeu com NACK
byte twi_er_ok(byte adr){
  byte resp;
  twi_start(5);
  resp = twi_er_check (adr<<1,6);
  twi_stop();
  return(resp);
}

// Enviar endereço de Escrita do Escravo Receptor (ER) e esperar ACK
// Retorna TRUE se escravo gerou ACK
// Retorna FALSE se gerou NACK (não gera msg de erro)
// ee = endereço do escravo
byte twi_er_check (byte ee, byte ix){
  TWDR = ee;                      //Endereço do  escravo
  TWCR = (1<<TWINT) | (1<<TWEN);  //Enviar endereço
  while ( !(TWCR & (1<<TWINT)));  //Esperar TWINT = 1
  if ( (TWSR & 0xF8) != TWI_SLA_WR_ACK)  return FALSE;
  return TRUE;
}

// Buscar por todos os dispositivos TWI para escrita
// Como busca por escrita, faz adr<<1 = bbbb bbb0
void twi_scan(void){
  byte adr;
  ser_str("Buscar por dispontivos TWI:\n");
  for (adr=0; adr<128; adr++){
    twi_start(5);
    if (twi_er_check (adr<<1,6) == TRUE){
      ser_str("Achou = ");  ser_hex8(adr);
      ser_str("h ou ");     ser_dec8u(adr);
      ser_str("d ==> ");
      switch(adr){
        case MPU_ADR:       ser_str("MPU9250");         break;
        case FLASH1_ADR_B0:  ser_str("FLASH1, Bloco 0");  break;
        case FLASH1_ADR_B1:  ser_str("FLASH1, Bloco 1");  break;
        case FLASH2_ADR_B0:  ser_str("FLASH2, Bloco 0");  break;
        case FLASH2_ADR_B1:  ser_str("FLASH2, Bloco 1");  break;
        default:            ser_str("? Desconhecido");  break;
      }
      ser_crlf(1);
   }
   twi_stop();
 }
}

// Configurar TWI para 400k (freq SCL)
void twi_config_400k(void){
  TWBR = 12; //SCL = 400 kHz
  TWSR = 0;  //TWPS1 = TWPS0 = 0 Pré-escalonador
}

// Configurar TWI para 100k (freq SCL)
void twi_config_100k(void){
  TWBR = 72; //SCL = 100 kHz
  TWSR = 0;  //TWPS1 = TWPS0 = 0 Pré-escalonador  
}

//Gerar um START no TWI
void twi_start (byte ix){
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //Enviar START
  while ( !(TWCR & (1<<TWINT))) ;              //Esperar TWINT = 1
  if ( (TWSR & 0xF8) != TWI_START_OK)  twi_erro(TWI_ERRO_1, ix);
}

//Gerar um START Repetido no TWI
void twi_start_rep (int ix){
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //Enviar START
  espera_twint(ix+1);
  if ( (TWSR & 0xF8) != TWI_START_REP_OK)  twi_erro(TWI_ERRO_2, ix);
}

//Enviar STOP
void twi_stop (void){
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);  // Enviar STOP
  delay (1); //(para CLK=1 MHz) atraso para que STOP seja percebido
}


// Enviar endereço de Escrita do Escravo (ER) e esperar ACK
// eer = endereço do escravo receptor
void twi_er (byte eer, byte ix){
  TWDR = eer;                     //Endereço de escrita no escravo
  TWCR = (1<<TWINT) | (1<<TWEN);  //Enviar endereço
  while ( !(TWCR & (1<<TWINT)));  //Esperar TWINT = 1
  if ( (TWSR & 0xF8) != TWI_SLA_WR_ACK)  twi_erro(TWI_ERRO_3, ix);
}

// Enviar endereço de Leitura do Escravo (ET) e esperar ACK
// eet = endereço do escravo transmissor
void twi_et (byte eet, byte ix){
  TWDR = eet;                     //Endereço de leitura do escravo
  TWCR = (1<<TWINT) | (1<<TWEN);  //Enviar endereço
  while ( !(TWCR & (1<<TWINT)));  //Esperar TWINT = 1
  if ( (TWSR & 0xF8) != TWI_SLA_RD_ACK)  twi_erro(TWI_ERRO_4,ix);
}

//Enviar dado para escravo previamente endereçado
void twi_dado_er (byte dado, byte ix){
  TWDR = dado;                      //dado a ser enviado
  TWCR = (1<<TWINT) | (1<<TWEN);    //Enviar dado
  espera_twint(ix+1);
  if ( (TWSR & 0xF8) != TWI_TX_DATA_ACK) twi_erro(TWI_ERRO_5,ix);     // Recebeu ACK ?
}

// Receber dado e gerar ACK
byte twi_dado_et_ack(byte ix){
  TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);  // Iniciar recepção (TWEA=1 --> ACK)
  espera_twint(ix+1);
  if ( (TWSR & 0xF8) != TWI_RX_DATA_ACK) twi_erro(TWI_ERRO_6,ix);
  return TWDR;
}

// Receber dado e gerar NACK
byte twi_dado_et_nack(byte ix){
  TWCR = (1<<TWINT) | (1<<TWEN);      // Iniciar recepção (TWEA=0 --> NACK)
  //while ( !(TWCR & (1<<TWINT))) ;   // Esperar TWINT = 1
  espera_twint(ix+1);
  if ( (TWSR & 0xF8) != TWI_RX_DATA_NACK) twi_erro(TWI_ERRO_7,ix);
  return TWDR;
}

// Esperar TWINT
byte espera_twint(byte ix){
  word tout=0;  
  char msg[50];
  while ( !(TWCR & (1<<TWINT))){    //Esperar TWINT = 1
    if (tout++==1000){
      twi_erro(TWI_ERRO_8,ix);
      //sprintf(msg,"(index=%d) Nunca chegou TWINT!",ix);
      //Serial.println(msg);
      return(FALSE);
    }
  }
  return(TRUE);
}

// ERROS no TWI (I2C)
// 1 = Erro ao gerar START
// 2 = Erro ao gerar START Repetido
// 3 = Erro Escravo Receptor endereçado (ER) não enviou ACK
// 4 = Erro Escravo Transmissor endereçado (ET) não enviou ACK
// 5 = Erro Escravo Receptor (ER) não enviou ACK após envio do dado
// 6 = Erro ao receber um dado do Escravo Transmissor (ET) e gerar um ACK
// 7 = Erro ao receber um dado do Escravo Transmissor (ET) e gerar um NACK
// 8 = Erro ao esperar TWINT - Timeout esperando TWINT ir para 1
void twi_erro(int cod, int ix){
  char msg[50];
  sprintf(msg,"Erro Nr %d TWI, index=%d e TWSR=%02X\n",cod,ix,TWSR&0xF8);
  ser_str(msg);
  ser_str("Programa interrompido!");
  /*
  Serial.print(msg); 
  Serial.print("Programa interrompido!");
  */
  while (1) ;   //prender execução
}
