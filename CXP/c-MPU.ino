// MPU - Rotinas para acesso
// CXP - Caixa Preta

// Ler Aceleração, Giro e Mag
// Retorna vetor de 18 bytes
// [axh axl ayh ayl azh azl gxh gxl gyh gyl gzh gzl hxh hxl hyh hyl hzh hzl]
void mpu_rd_ac_gi_mg(byte *vetor){
  byte x,vet[14],st1,st2;
  // Acel e Giro
  mpu_rd_blk(ACCEL_XOUT_H, vet, 14);
  vetor[ 0] = vet[ 0];  //axh
  vetor[ 1] = vet[ 1];  //axl
  vetor[ 2] = vet[ 2];  //ayh
  vetor[ 3] = vet[ 3];  //ayl
  vetor[ 4] = vet[ 4];  //azh
  vetor[ 5] = vet[ 5];  //azl
  // Pular temperatura vet[6] e vet[7]
  vetor[ 6] = vet[ 8];  //gxh
  vetor[ 7] = vet[ 9];  //gxl
  vetor[ 8] = vet[10];  //gyh
  vetor[ 9] = vet[11];  //gyl
  vetor[10] = vet[12];  //gzh
  vetor[11] = vet[13];  //gzl

  // Magnetômetro
  // Data pronto (DRDY=1) ?
  st1=mpu_rd_mg_reg(MAG_ST1);
  if ( (st1&1) == 0)
    st1=mpu_rd_mg_reg(MAG_ST1); //Ler novamente
  mpu_rd_mg_blk(MAG_XOUT_L, vet, 6);

  vetor[12] = vet[1];  //hxh
  vetor[13] = vet[0];  //hxl
  vetor[14] = vet[3];  //hyh
  vetor[15] = vet[2];  //hyl
  vetor[16] = vet[5];  //hzh
  vetor[17] = vet[4];  //hzl
  vetor[17] = (vetor[17] & 0xFE) | (st1&1); //LSbit de hzl = DRDY
  
  // Overflow (HOFL) ==> 9.999
  st2=mpu_rd_mg_reg(MAG_ST2);
  if ((st2&8) == 8){
    vetor[12] = vetor[14] = vetor[16] = 9999>>8;
    vetor[13] = vetor[15] = vetor[17] = 9999&0xFF;
  }
}


////////////////////////////////////////////////////
/////////////// Magnetômetro   /////////////////////
////////////////////////////////////////////////////

// MAG: Realizar Self-Test (ST), prn = imprimir resultados?
// Retorna: TRUE  se passou no teste
//          FALSE se falhou no teste
// vetor[ hx hy hz ] --> espaço para 3 inteiros
byte mpu_mag_self_test(int *vetor, byte prn) {
  byte vet[6],ok;
  int aux[3];
  byte asa[3];
  
  mpu_wr_mg_reg(MAG_CNTL_1, 0x00);  //(1) MODE=0, Magnetometro Power Down
  delay(100);
  mpu_wr_mg_reg(MAG_ASTC, 0x64);    //(2) SELF=1
  delay(100);
//  mpu_wr_mg_reg(MAG_CNTL_1, 0xC);   //(3) BIT=1 (16 bits) e MODE=8 (self test)
  mpu_wr_mg_reg(MAG_CNTL_1, 0x18);   //(3) BIT=1 (16 bits) e MODE=8 (self test)
  delay(100);

  byte r = 0;
  r = mpu_rd_mg_reg(MAG_ST1) &1;
  while(r&1 == 0){
    r = mpu_rd_mg_reg(MAG_ST1) &1;
  }
  
  
  mpu_rd_mg_blk(MAG_XOUT_L, vet, 6);
  aux[0] = (int)((int)(vet [1] << 8) | vet[0]);    //Montar Mag X
  aux[1] = (int)((int)(vet [3] << 8) | vet[2]);    //Montar Mag Y
  aux[2] = (int)((int)(vet [5] << 8) | vet[4]);    //Montar Mag Z
  mpu_wr_mg_reg(MAG_ASTC, 0);       //(2) SELF=0
  mpu_wr_mg_reg(MAG_CNTL_1, 0x00);  //(1) MODE=0, Magnetometro Power Down

  // ASA: Ajuste de sensibilidade
  mpu_mag_rd_rom(asa);    //asa[0]=ASAx, asa[1]=ASAy, asa[2]=ASAz,

 
  vetor[0]=(float)aux[0]*( 1+((float)asa[0]-128)/256.);
  vetor[1]=(float)aux[1]*( 1+((float)asa[1]-128)/256.);
  vetor[2]=(float)aux[2]*( 1+((float)asa[2]-128)/256.);

  
  ok=TRUE;
  if ( (vetor[0] <=  -200) || (vetor[0] >=  200))  ok=FALSE;  //hx
  if ( (vetor[1] <=  -200) || (vetor[1] >=  200))  ok=FALSE;  //hy
  if ( (vetor[2] <= -3200) || (vetor[2] >= 3200))  ok=FALSE;  //hz

  if (prn==TRUE){ //Imprimir resultados ?
    ser_crlf(1);
    ser_str("\n--- Resultados MAG Self Test ---\n");
    ser_str("hx=");         ser_dec16(aux[0]);
    ser_str("  hy=");       ser_dec16(aux[1]);
    ser_str("  hz=");       ser_dec16(aux[2]);
    ser_str("\nASAx=");     ser_dec8u(asa[0]);
    ser_str("  ASAy=");     ser_dec8u(asa[1]);
    ser_str("  ASAz=");     ser_dec8u(asa[2]);
//    ser_str("\nhx=");       ser_dec16(vetor[0]);  //Após ajuste
//    ser_str("  hy=");       ser_dec16(vetor[1]);
//    ser_str("  hz=");       ser_dec16(vetor[2]);
    ser_str(" ==> ");
    if (ok==TRUE)   ser_str("OK");
    else            ser_str("NOK");     
    ser_str("\n--- Fim Funcao MAG Self Test ---\n\n");
  }
  return ok;
}



// Inicializar Magnetômetro
void mpu_mag_config(void){
  mpu_wr(USER_CTRL, 0x00);          //Desab. modo mestre no mpu
  mpu_wr(INT_PIN_CFG, 0x02);        //Hab.o bypass I2C
  mpu_wr_mg_reg(MAG_CNTL_1, 0x00);  //Magnetometo Power Down
  delay(100);                       //Espera trocar de modo
  mpu_wr_mg_reg(MAG_CNTL_1, 0x16);  //??? Mag. Modo Continuo, 100Hz e 16 bits
  delay(100);                       //espera trocar de modo
}

// Ler Fuse ROM Magnetômetro
// vet = vetor de 3 posições
void mpu_mag_rd_rom(byte *vet){
  mpu_mag_config();                 //Inicializar Mag   
  mpu_wr_mg_reg(MAG_CNTL_1, 0x1F);  //Modo FUSE ROM
  delay(100);                       //Esperar trocar de modo
  mpu_rd_mg_blk(MAG_ASAX, vet, 3);  //Ler ajustes sensiblidade ASAX, ASAY, ASAZ
  mpu_mag_config();                 //Inicializar Mag  
}

//Mag Who am I, deve retornar 0x48
byte mag_whoami(){
  byte who = 0;
  who = mpu_rd_mg_reg(0x00);
  return who;
}

// Ler Magnetômetro
// Retorna vetor de 3 words [hx hy hz]
// Copia DRDY para o último bit de hz
// retorna 0: dado não pronto (DRDY = 0)
// retorna 1: Tudo bem
// retorna 2: overflow (HOFL)
byte mpu_rd_mg_out(int *vetor){
  byte vet[6],st1,st2;
  
  // Data pronto (DRDY=1) ?
  st1=mpu_rd_mg_reg(MAG_ST1);
  if ( (st1&1) == 0)
    st1=mpu_rd_mg_reg(MAG_ST1); //Ler novamente
  
  mpu_rd_mg_blk(MAG_XOUT_L, vet, 6);
  vetor[0] = (int)((int)(vet [1] << 8) | vet[0]);    //Montar Mag X
  vetor[1] = (int)((int)(vet [3] << 8) | vet[2]);    //Montar Mag Y
  vetor[2] = (int)((int)(vet [5] << 8) | vet[4]);    //Montar Mag Z

  // Coloca DRDY no último bit de HZ
  vetor[2] = (vetor[2] & 0xFFFE) | (st1&1); //LSbit de hz = DRDY
  
  // Overflow (HOFL) ==> 9.999
  st2=mpu_rd_mg_reg(MAG_ST2);
  if ((st2&8) == 8){
    vetor[0] = vetor[1] = vetor[2] = 9999;
  }

  if ( (st1&1) == 0)  return 0;   //0=Dado Não pronto
  if ( (st2&8) == 8)  return 2;   //2=Sensor overflow
  return TRUE;                    //1=Tudo certo
}

// (50) Ler registrador do magnetometro
byte mpu_rd_mg_reg(byte reg){
  byte dado;
  twi_start(50);                //START
  twi_er(MAG_I2C_ADDR_WR, 51);  //Endereçar Magnetometro para escrita
  twi_dado_er(reg, 52);         //Informar registrador
  twi_start_rep(53);            //START Repetido
  twi_et(MAG_I2C_ADDR_RD, 54);  //Endereçar Magnetometro para leitura
  dado = twi_dado_et_nack(55);  //Receber dado do magnetometro com NACK
  twi_stop();                   //Gerar STOP para finalizar
  return dado;
}

//(60) lê em bloco os registradores do magnetometro
void mpu_rd_mg_blk(byte reg, byte *dado, byte qtd){
  byte i;
  twi_start(60);                    //START
  twi_er(MAG_I2C_ADDR_WR, 61);      //Endereçar MPU para escrita
  twi_dado_er(reg, 62);             //Informar registrador
  twi_start_rep(63);                //START Repetido
  twi_et(MAG_I2C_ADDR_RD, 64);      //Endereçar Magnetometro para leitura

  for (i=0; i<qtd; i++)
    dado[i] = twi_dado_et_ack(65);  //Receber dados e gerar ACK
    
  dado = twi_dado_et_nack(66);      //Receber último dado e gerar NACK
  twi_stop();                       //Gerar STOP para finalizar
}

// (70) escreve no magnetometro
byte mpu_wr_mg_reg(byte reg, byte dado){
  twi_start(70);                //start
  twi_er(MAG_I2C_ADDR_WR, 71);  //endereça magnetometro para escrita
  twi_dado_er(reg, 72);         //informa o registrador
  twi_dado_er(dado, 73);        //informa o dado
  twi_stop();                   //stop
}


////////////////////////////////////////////////////
/////////// Aceleração e Giro      /////////////////
////////////////////////////////////////////////////

// Ler no MPU a banda do filtro
// Retorna 5, 10, 21, ..., 260 (Escala do Acelerômetro)
int mpu_rd_bw(void){
  byte x;
  x=mpu_rd(CONFIG);
  switch(x){
    case 0:   return 260;
    case 1:   return 184;
    case 2:   return  94;
    case 3:   return  44;
    case 4:   return  21;
    case 5:   return  10;
    case 6:   return   5;
    case 7:   return 999; //Indicar Valor reservado
  }
}

// Ler no MPU a freq de amostragem
// Retorna 100, 200, ..., 1000
// Considera Gyro Rate = 1.000 (DLPF=0)
int mpu_rd_freq(void){
  byte x;
  x=mpu_rd(SMPLRT_DIV);
  x=1000/(1+x);
  return x;
}

// Ler no MPU a escala usada para o acelerômetro
// Retorna 2, 4, 8 ou 16
int mpu_rd_esc_acel(void){
  byte x;
  x=mpu_rd(ACCEL_CONFIG);
  x=(x>>3)&0x7;
  x=2<<x;
  return x;
}

// Ler no MPU a escala usada para o giroscópio
// Retorna 250, 500, 1000 ou 2000 graus/seg
int mpu_rd_esc_giro(void){
  byte x;
  x=mpu_rd(GYRO_CONFIG);
  x=(x>>3)&0x7;
  x=250<<x;
  return x;
}

// Preparar para MPU usar INT4
// Pino PE4 entrada com pullup
// Habilitar INT4 para flanco de descida
// MPU: interrup em baixo com push-pull, pulso de 50 useg
// MPU: Habilitar interrup dado pronto
void mpu_int(void){

  // INT4 = PE4 = Pino 2 --> entrada com pullup 
  DDRE = DDRE & ~(1 << DDE4); //DDD4=0, entrada          
  PORTE = PORTE | (1 << PE4); //Pull-up ligado (PORTE4=1)
  //pinMode(2,INPUT_PULLUP);

  // Preparar interrupção INT4 por flanco de descida
  EICRB = (EICRB | (1<<ISC41)) & ~(1 << ISC40); //INT4 = flanco descida
  EIMSK = EIMSK | (1 << INT4);                  //INT4 habilitada  

  //mpu_wr(INT_PIN_CFG,0x80);  //push-pull, pulso 50 useg
  mpu_wr(INT_PIN_CFG,0x82);  //push-pull, pulso 50 useg
  mpu_wr(INT_ENABLE,1);     //DATA_RDY_EN
}

// Desabilitar MPU que usava INT4
// Desabilitar INT4 para 
// MPU: Desabilitar interrup dado pronto
void mpu_des_int(void){
  EIMSK &= ~(1 << INT4);    //INT4 Desabilitada  
  mpu_wr(INT_ENABLE,0);     //DATA_RDY_EN = 0 (desab)
}

// Ler Temperatura
int mpu_rd_tp(void){
  byte vet[2];
  int x;
  mpu_rd_blk(TEMP_OUT_H, vet, 2);
  x = (int)((vet [0] << 8) | vet[1]);    //Montar Temperatura
  return x;
}

// Ler Aceleração, temperatura e giro
// Retorna vetor de 7 words [ax ay az tp gx gy gz]
void mpu_rd_ac_tp_gi(word *vetor){
  byte i,vet[14];
  mpu_rd_blk(ACCEL_XOUT_H, vet, 14);
  vetor[0] = (int)((vet [0] << 8) | vet[1]);    //Montar Acel X
  vetor[1] = (int)((vet [2] << 8) | vet[3]);    //Montar Acel Y
  vetor[2] = (int)((vet [4] << 8) | vet[5]);    //Montar Acel Z
  vetor[3] = (int)((vet [6] << 8) | vet[7]);    //Montar Temp
  vetor[4] = (int)((vet [8] << 8) | vet[9]);    //Montar Giro x
  vetor[5] = (int)((vet[10] << 8) | vet[11]);   //Montar Giro y
  vetor[6] = (int)((vet[12] << 8) | vet[13]);   //Montar Giro z
}

// Ler Aceleração, giro
// Retorna vetor de 6 words [ax ay az gx gy gz]
void mpu_rd_ac_gi(word *vetor){
  byte i,vet[14];
  mpu_rd_blk(ACCEL_XOUT_H, vet, 14);
  vetor[0] = (int)((vet [0] << 8) | vet[1]);    //Montar Acel X
  vetor[1] = (int)((vet [2] << 8) | vet[3]);    //Montar Acel Y
  vetor[2] = (int)((vet [4] << 8) | vet[5]);    //Montar Acel Z
  vetor[3] = (int)((vet [8] << 8) | vet[9]);    //Montar Giro x
  vetor[4] = (int)((vet[10] << 8) | vet[11]);   //Montar Giro y
  vetor[5] = (int)((vet[12] << 8) | vet[13]);   //Montar Giro z
}

// Acordar o MPU e programar para usar relógio Giro X
void mpu_acorda(void) {
  mpu_wr(PWR_MGMT_1, 1);
}

// Dormir o MPU e programar para usar relógio Giro X
void mpu_dorme(void) {
//  mpu_wr(PWR_MGMT_1, 0x21);  //SLEEP=1 e PLL com Giro X
  mpu_wr(PWR_MGMT_1, 0x41);  //SLEEP=1 e PLL com Giro X
}

// Ler o registrador WHO_AM_I
byte mpu_whoami(void) {
  return mpu_rd(WHO_AM_I);
}

// Colocar o MPU num estado conhecido
// Taxa = 1 kHz, Banda: Acel=5 Hz e Giro=5 Hz e delay=19 mseg
// Taxa de amostragem =  taxa/(1+SMPLRT_DIV) = 1k/10 = 100Hz
//Escalas acel = +/2g e giro = +/-250 gr/s
//void mpu_config(void) {
//
//  // Despertar MPU, Relógio = PLL do Giro-x
//  mpu_wr(PWR_MGMT_1, 0x01);
//  delay(200);       //200ms - Esperar PLL estabilizar
//
//  // 6 ==> Taxa = 1 kHz, Banda: Acel=5 Hz e Giro=5 Hz e delay=19 mseg
//  mpu_wr(CONFIG, 6);
//
//  // 9 ==> Taxa de amostragem =  taxa/(1+SMPLRT_DIV) = 1k/10 = 100Hz
//  mpu_wr(SMPLRT_DIV, SAMPLE_RT_100Hz);  //Taxa de amostragem = 100 Hz
//  //mpu_wr(SMPLRT_DIV, SAMPLE_RT_500Hz);  //Taxa de amostragem = 500 Hz
//
//  // Definir escalas
//  mpu_escalas(GIRO_FS_250, ACEL_FS_2G);   //Escalas acel = +/2g e giro = +/-250 gr/s
//}

// Colocar o MPU num estado conhecido
// Taxa = 1 kHz, Banda: Acel=5.05 Hz e Giro=5 Hz. Delay Acel = 32.48ms. Delay Giro = 33.48
// Taxa de amostragem =  taxa/(1+SMPLRT_DIV) = 1k/10 = 100Hz
//Escalas acel = +/2g e giro = +/-250 gr/s
void mpu_config(void) {

  // Despertar MPU, Relógio = PLL do Giro-x
  mpu_wr(PWR_MGMT_1, 0x01);
  delay(200);       //200ms - Esperar PLL estabilizar

  // Definir escalas
  mpu_escalas(GIRO_FS_250, ACEL_FS_2G);   //Escalas acel = +/2g e giro = +/-250 gr/s

  // 6 => Liga o filtro passa-baixa do giroscópio e temperatura para para 5Hz
  // Delay giro = 33.48ms, Taxa giro = 1Khz; Delay temperatura 18.6ms 
  mpu_wr(CONFIG, 6);

  // 6 => Liga o filtro passa-baixa do acelerômetro para para 5.05Hz. Delay= 32.48ms
  mpu_wr(ACCEL_CONFIG_2, 6);
  
  // 9 ==> Taxa de amostragem =  taxa/(1+SMPLRT_DIV) = 1k/10 = 100Hz
  mpu_wr(SMPLRT_DIV, SAMPLE_RT_100Hz);  //Taxa de amostragem = 100 Hz
  //mpu_wr(SMPLRT_DIV, SAMPLE_RT_500Hz);  //Taxa de amostragem = 500 Hz
}

// Selecionar Fundo de Escalas para o MPU
// Acel: 0=+/-2g, 1=+/-4g, 2=+/-8g, 3=+/-16g 
// Gyro: 0=+/-250gr/s, 1=+/-500gr/s, 2=+/-1000gr/s, 3=+/-2000gr/s,  
void mpu_escalas(byte gfs, byte afs) {
  mpu_wr(GYRO_CONFIG, gfs << 3); //FS do Giro
  mpu_wr(ACCEL_CONFIG, afs << 3); //FS do Acel
}

// Selecionar Sample Rate
// Considerando Taxa = 1kHz (Registrador CONFIG)
void mpu_sample_rt(byte sample_rate) {
  mpu_wr(SMPLRT_DIV, sample_rate);  //Taxa de amostragem
}

// MPU: Calibrar
// Faz uma série de leituras e retorna a média
void mpu_calibra(int *vt, word qtd, byte esc_ac, byte esc_gi) {
  long sum[7];  //Acumular para calcular a média
  word aux[7];  //Leituras intermediárias
  word i,j;

  mpu_escalas(esc_gi, esc_ac);          // Menores escalas para maior precisão
  mpu_sample_rt(OP_FREQ);               // Taxa de amostragem de operação
  mpu_int();                            // Habilitar interrupção
  for (i=0; i<7; i++)   sum[i]=0;       //Zerar acumulador
  mpu_dado_ok=FALSE;
  for (i=0; i<qtd; i++){
    //ser_dec16(i);
    //ser_crlf(1);
    while(mpu_dado_ok == FALSE);
    mpu_dado_ok = FALSE;
    mpu_rd_ac_tp_gi(aux);
    for (j=0; j<7; j++) sum[i] += aux[i];
  }

  // Calcular as médias
  for (i=0; i<7; i++) vt[i] = sum[i]/qtd;
}

// mag_calibra obtem os valores necessarios para calibrar o magnetometro
// asa[3] = [asax, asay, asaz] são os ajustes de sensibilidade
// h_extr[6] = [hx_min, h_max, hy_min, h_max, hz_min, hz_max]
// Esses são todos os dados para realizar uma calibração de magnetômetro simples no pós processamento:
//
// O algoritmo de matlab para calibrar é:
//     % Hard Iron - Remover offset
//     hx_off=(hx_max + hx_min)/2;
//     hy_off=(hy_max + hy_min)/2;
//     hz_off=(hz_max + hz_min)/2;
//     
//     % Soft Iron - Corrigir a escala
//     hx_avg_delta=(hx_max - hx_min)/2;
//     hy_avg_delta=(hy_max - hy_min)/2;
//     hz_avg_delta=(hz_max - hz_min)/2; 
//   
//     avg_h_delta=(hx_avg_delta + hy_avg_delta + hz_avg_delta)/3;
//
//     hx_sc=avg_h_delta/hx_avg_delta;
//     hy_sc=avg_h_delta/hy_avg_delta;
//     hz_sc=avg_h_delta/hz_avg_delta;
//
//     h_off = [ hx_off hy_off hz_off ];
//     h_sc  = [ 
//                hx_sc  0       0       ;
//                0      hy_sc   0       ; 
//                0      0       hz_sc
//             ];
//    magDataCalibrated = (magData - h_off)*h_sc;
byte mag_calibra(byte* asa, int* h_extr, byte prn ){
  
  byte mag_st,who;
  int vetor[3];

  h_extr[0] = 32767;
  h_extr[1] = -32768;
  h_extr[2] = 32767;
  h_extr[3] = -32768;
  h_extr[4] = 32767;
  h_extr[5] = -32768;
  

  mpu_config();         //MPU configurar
  mpu_mag_config();     //MAG configurar

  who = mag_whoami();
  if (who != MAG_WHO){
//    lcd_str(1,13,"whoami nao encontrado");  //MPU Não respondendo
    ser_str("Magnetômetro não encontrado\n");
    return FALSE;
  } 

  ser_str("Calibração do Magnetômetro.\n\n");
  ser_str("Quando a calibração iniciar, movimente a caixa preta lentamente em todas as direções possíveis.\n");
  ser_str("O procedimento é encerrado ao pressionar qualquer botão da caixa-preta.\n");
  ser_str("Inicia em 5 segundos...\n");

//  lcd_apaga_lin(1);
//  lcd_apaga_lin(2);
//  lcd_apaga_lin(3);
//  lcd_str(1,0,"Calibr mag.");
//  lcd_str(2,0,"Inicia em 5 seg...");
  
  delay(5000);
  ser_str("Coleta de dados iniciou...\n"); 
  
  // Iniciar coleta

  // coleta o ajuste de sensibilidade
  mpu_mag_rd_rom(asa);
  
  // Habilitar interrupção MPU (Dado Pronto)
  mpu_sample_rt(SAMPLE_RT_100Hz);
  mpu_int();

//  lcd_str(1,0,"Coletando dados...");
//  lcd_str(2,0,"Rotacione lentamente");
//  lcd_str(3,0,"e finalize com botao");
  while(TRUE){
    while (mpu_dado_ok == FALSE);   //Agaurdar MPU a 100 Hz (10 ms)
    mpu_dado_ok=FALSE;
    mag_st=mpu_rd_mg_out(vetor);

    if (mag_st==1){        //Tudo certo
//     lcd_char(2, 5,'1');  
//     lcd_char(2,15,'0'); 

     //atualiza valores maximos e minimos
     if(vetor[0] < h_extr[0]) h_extr[0] = vetor[0];
     if(vetor[0] > h_extr[1]) h_extr[1] = vetor[0];
     if(vetor[1] < h_extr[2]) h_extr[2] = vetor[1];
     if(vetor[1] > h_extr[3]) h_extr[3] = vetor[1];
     if(vetor[2] < h_extr[4]) h_extr[4] = vetor[2];
     if(vetor[2] > h_extr[5]) h_extr[5] = vetor[2];
     
   }
   else if(mag_st==0){    //Dado não pronto
//     lcd_char(2, 5,'0');  
//     lcd_char(2,15,'0');  
   }
   
   else if(mag_st==2){    //Sensor Overflow
//     lcd_char(2, 5,'1');  
//     lcd_char(2,15,'1');  
   }

    //if (sw_tira(&who))     break;
    if (fim_qqtec_x() == TRUE)  break;  //qq Tecla o letra x para finalizar
  }   
 
  mpu_des_int();
//  lcd_apaga_lin(1);
//  lcd_apaga_lin(2);
//  lcd_apaga_lin(3);
//  lcd_str(2,0,"Fim da coleta");
  ser_str("Fim da coleta de dados.\n"); 

  if(prn){
    ser_str("Valores de calibragem do magnetômetro:");  ser_crlf(1); 
    ser_str("ASA:");  ser_crlf(1); 
    ser_dec8u(asa[0]);    ser_crlf(1);
    ser_dec8u(asa[1]);    ser_crlf(1);
    ser_dec8u(asa[2]);    ser_crlf(1);
    ser_str("Hx:");  ser_crlf(1); 
    ser_dec16(h_extr[0]); ser_crlf(1);
    ser_dec16(h_extr[1]); ser_crlf(1);
    ser_str("Hy");  ser_crlf(1); 
    ser_dec16(h_extr[2]); ser_crlf(1);
    ser_dec16(h_extr[3]); ser_crlf(1);
    ser_str("Hz");  ser_crlf(1); 
    ser_dec16(h_extr[4]); ser_crlf(1);
    ser_dec16(h_extr[5]); ser_crlf(1);
  }
  
  return TRUE;
  
  
  // (Offset) Gravar Calibração na EEPROM
 /* eeprom_wr_16b(CF_MAG_OK,COD_SIM);      //Marcar que fez calibração do Magnetômetro
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

  // (ASA) Gravar Ajustes de Sensibilidade (tem que ser unsigned)
  mpu_mag_rd_rom(asa);
  eeprom_wr_16b(CF_HX_ASA,(int)asa[0]); //(ASAx) Ajuste de sensibilidade de hx
  eeprom_wr_16b(CF_HY_ASA,(int)asa[1]); //(ASAy) Ajuste de sensibilidade de hy
  eeprom_wr_16b(CF_HZ_ASA,(int)asa[2]); //(ASAz) Ajuste de sensibilidade de hz*/
  
 
}

//acel_calibra Obtem os valores necessarios para calibrar o acelerometro
//v_out[6] = [   ax_x_cima,
//               ax_x_baixo,
//               ay_y_cima,
//               ay_y_baixo,
//               az_z_cima,
//               az_z_baixo,
//             ]
//
//   % o seguinte algoritmo do matlab realiza a calibração
//    accel_offset(1)=(ax_x_cima + ax_x_baixo)/2;
//    accel_offset(2)=(ay_y_cima + ay_y_baixo)/2;
//    accel_offset(3)=(az_z_cima + az_z_baixo)/2;
//
//    accel_scale(1)=1/((ax_x_cima - ax_x_baixo)/2);
//    accel_scale(2)=1/((ay_y_cima - ay_y_baixo)/2);
//    accel_scale(3)=1/((az_z_cima - az_z_baixo)/2);
//  
//    ax_calibrado=(ax-accel_offset(1))*accel_scale(1);
//    ay_calibrado=(ay-accel_offset(2))*accel_scale(2);
//    az_calibrado=(az-accel_offset(3))*accel_scale(3);
byte acel_calibra(int* v_out, byte prn){

  int vt[7]; 
  long aux[6];

  char *msg1="Erro Who am I = ";
  char *msg_calibr[6] = { "X para cima",
                          "X para baixo",
                          "Y para cima",
                          "Y para baixo",
                          "Z para cima",
                          "Z para baixo"};
  
  byte whoami;
  int qtd_medidas=30;

  ser_str("Calibração do acelerômetro.\n");
  ser_str("Posicione o sensor de acordo com as instruções.\n");
  ser_str("Ao posicionar, pressione qualquer botão para realizar a medida.\n");

  mpu_acorda();
  mpu_config();
  whoami=mpu_whoami();                    
  if (whoami != MPU9250_WHO){
//    lcd_str(1,0,msg1); lcd_dec16unz(1,16,whoami);
    ser_str(msg1);     ser_dec16unz(whoami);  ser_crlf(1);
    delay(1000);
    return FALSE;
  }

  delay(1000);

//  lcd_apaga_lin(1);
//  lcd_apaga_lin(2);
//  lcd_apaga_lin(3);
//  lcd_str(1,0,"Calib do acel: ");

  int quit=FALSE;
  byte x=0;
  int cnt = qtd_medidas;
  for(int i = 0 ; i < 6 ; i++){ 
    //Inicia coleta de dados

    ser_str(msg_calibr[i]); ser_crlf(1);
    
    //espera usuario apertar botao para medir, ou terminar mandando X pelo serial
    while(TRUE){
      while (TRUE){
        if (seri_tira(&x)==FALSE)   break;
        if (x=='x' || x=='X')       quit=TRUE;
      }     
      if ( sw_tira(&x) == TRUE) break;
      if(quit == TRUE) break;
    }

    if(quit == TRUE) return FALSE;

    //inicia as leituras
    mpu_int();
    
    //faz algumas leituras para estabilizar e evitar o movimento de apertar o botão
    int l=160;  
    while(l--){
      while (mpu_dado_ok == FALSE);
      mpu_rd_ac_tp_gi(vt);  //Ler MPU
    }

    aux[0]=aux[1]=aux[2]=0;
    cnt = qtd_medidas;
    
    while(cnt--){
        while (mpu_dado_ok == FALSE);   //Agaurdar MPU a 100 Hz (10 ms)
        mpu_dado_ok=FALSE;
        
        mpu_rd_ac_tp_gi(vt);  //Ler MPU

        aux[0]+=vt[0];
        aux[1]+=vt[1];
        aux[2]+=vt[2];
          
      }

      aux[0]/=qtd_medidas;
      aux[1]/=qtd_medidas;
      aux[2]/=qtd_medidas;

      v_out[i] = (int)aux[(int)floor(i/2)];

      mpu_des_int();
  }

  ser_str("dados coletados.\n");
  if(prn){
    ser_str("Valores que serão usados na calibragem do acelerometro:");  ser_crlf(1); 
    ser_str("ax:");  ser_crlf(1); 
    ser_dec16(v_out[0]); ser_crlf(1);
    ser_dec16(v_out[1]); ser_crlf(1);
  
    ser_str("ay:");  ser_crlf(1); 
    ser_dec16(v_out[2]); ser_crlf(1);
    ser_dec16(v_out[3]); ser_crlf(1);
  
    ser_str("az:");  ser_crlf(1); 
    ser_dec16(v_out[4]); ser_crlf(1);
    ser_dec16(v_out[5]); ser_crlf(1);
  }
  return TRUE;
}


// MPU: Realizar Self-Test (ST), prn = imprimir resultados?
// Retorna: TRUE  se passou no teste
//          FALSE se falhou no teste
// baseado no documento AN-MPU-9250A-03 MPU-9250 Accel Gyro and Compass Self-Test Implementation v1 0_062813.pdf
//byte mpu_self_test_2(byte prn = FALSE) {
byte mpu_self_test_2(byte prn) {
  
  float ST_OPT[6];            // self test value de fábrica
  long  sum[6];               // registrador auxilar para calcular a soma  
  int   aux[6];               // registrador auxiliar para leituras
  int   mediaNoST[6];         // media das 200 medidas sem o self test
  int   mediaST[6];           // media das 200 medidas com o self test
  int   selfTestResponse[6];  // self test response (medias com self test - medias sem self test)
  byte  ST_CODE[6];           // factory self test code (usado para calcular o self test value)
  float porcentagens[6];      // porcentagem ao dividir selfTestResponse por ST_OPT (determina se st passou)
  
  int   qtd = 200;            // quantos valores coletar para calcular a média
  byte  passou = true;        // valor que retorna no final da função (passou ou não?)

  /* 3.0 Procedimento */
  // 1. Configurações necessárias para o self test 
  mpu_wr(SMPLRT_DIV, 0x00);    
  mpu_wr(CONFIG, 0x02);        // Giroscópio: mudando DLPF para config 2. taxa do giroscopio 1 kHz e DLPF  92 Hz
  mpu_wr(ACCEL_CONFIG_2, 0x02); // Acelerometro: taxa do acelerometro 1 kHz banda 92 Hz 
    
  //configura escalas do giroscopio e acelerometro para o recomendado para o self test
  // +/- 2g e +/-250gr/seg
  mpu_escalas(GIRO_FS_250,ACEL_FS_2G); 
  delay(250); //Aguardar cofiguração estabilizar

  // 2. Com o self test desligado, ler 200 medidas do giroscópio e acelerometro e armazenar as médias
  // em mediaNoST [ax, ay, az, gx, gy, gz]
  for (int i = 0; i<6; i++)   sum[i]=0; //Zerar acumulador
  mpu_int(); // Habilitar interrupção
  mpu_dado_ok=FALSE;
  //200 medidas sem self test
  for (int i=0; i<qtd; i++){
    while(mpu_dado_ok == FALSE);
    mpu_dado_ok = FALSE;
    mpu_rd_ac_gi(aux);
    for (int j=0; j<6; j++) sum[j] += aux[j]; 
  }
  // Calcular as médias sem o self test
  for (int i=0; i<6; i++) mediaNoST[i] = sum[i]/qtd;
  
  // 3. habilitar self test nos 3 eixos do acelerometro e giroscópio
  mpu_wr(ACCEL_CONFIG, 0xE0);
  mpu_wr(GYRO_CONFIG,  0xE0);

  // 4. delay para as oscilações estabilizarem
  delay(25);  

  // 5. Com o self test ligado, ler 200 medidas do giroscópio e acelerometro e armazenar as médias
  // em mediaST [ax, ay, az, gx, gy, gz]
  for (int i = 0; i<6; i++)   sum[i]=0; //Zerar acumulador 
  //200 medidas com self test
  for (int i=0; i<qtd; i++){
    while(mpu_dado_ok == FALSE);
    mpu_dado_ok = FALSE;
    mpu_rd_ac_gi(aux);
    for (int j=0; j<6; j++) sum[j] += aux[j]; 
  }
  for (int i=0; i<6; i++) mediaST[i] = sum[i]/qtd;
  
  // 6. calculando as respostas para o self test
  for(int i = 0 ; i < 6 ; i++){
    selfTestResponse[i] = mediaST[i] - mediaNoST[i];
  }
 
  /* 3.1 Configurar giro e acel para operação normal */
  mpu_wr( ACCEL_CONFIG, 0x00);
  mpu_wr( GYRO_CONFIG,  0x00);
  delay(25);  // Delay a while to let the device stabilize
  //acertar as escalas 
  mpu_escalas(GIRO_FS_250,ACEL_FS_8G); 
  //tem que rodar o mpu_config denovo?
 
  /* 3.2 critérios para passar no self test */
  
  // 1. lendo factory Self-Test Code do giroscopio e acelerometro   
  // X-axis accel self-test
  ST_CODE[0] = mpu_rd(SELF_TEST_X_ACCEL);
  // Y-axis accel self-test
  ST_CODE[1] = mpu_rd(SELF_TEST_Y_ACCEL);
  // Z-axis accel self-test
  ST_CODE[2] = mpu_rd(SELF_TEST_Z_ACCEL);
  // X-axis gyro self-test
  ST_CODE[3] = mpu_rd(SELF_TEST_X_GYRO);
  // Y-axis gyro self-tes
  ST_CODE[4] = mpu_rd(SELF_TEST_Y_GYRO);
  // Z-axis gyro self-test
  ST_CODE[5] = mpu_rd(SELF_TEST_Z_GYRO); 

  // 2. calculando factory self-test value a partir do factory self test code
  // FT[Xa]
  ST_OPT[0] = (float)(2620/1<<GIRO_FS_250)*(pow(1.01 ,((float)ST_CODE[0] - 1.0) ));
  // FT[Ya]
  ST_OPT[1] = (float)(2620/1<<GIRO_FS_250)*(pow(1.01 ,((float)ST_CODE[1] - 1.0) ));
  // FT[Za]
  ST_OPT[2] = (float)(2620/1<<GIRO_FS_250)*(pow(1.01 ,((float)ST_CODE[2] - 1.0) ));
  // FT[Xg]
  ST_OPT[3] = (float)(2620/1<<GIRO_FS_250)*(pow(1.01 ,((float)ST_CODE[3] - 1.0) ));
  // FT[Yg]
  ST_OPT[4] = (float)(2620/1<<GIRO_FS_250)*(pow(1.01 ,((float)ST_CODE[4] - 1.0) ));
  // FT[Zg]
  ST_OPT[5] = (float)(2620/1<<GIRO_FS_250)*(pow(1.01 ,((float)ST_CODE[5] - 1.0) ));
 
  for(int i = 0 ; i < 6 ; i++){
    porcentagens[i] = (float)selfTestResponse[i]/ST_OPT[i];     
  }

  // 3. Determinando a condição de passar no teste
  // X-gyro: (GXST / GXST_OTP) > 0.5
  // Y-gyro (GYST / GYST_OTP) > 0.5
  // Z-gyro (GZST / GZST_OTP) > 0.5
  // X-Accel 0.5 < (AXST / AXST_OTP) < 1.5
  // Y-Accel 0.5 < (AYST / AYST_OTP) < 1.5
  // Z-Accel 0.5 < (AZST / AZST_OTP) < 1.5
  
  for(int i = 0 ; i < 3 ; i++){
    //testando giro
    if(porcentagens[i+3] <= 0.5) passou = false;
    //testando acel
    if(!((porcentagens[i] > 0.5) && (porcentagens[i] < 1.5))) passou = false;
  }

  //printa os resultados
  if(prn == TRUE){
    
    ser_crlf(2);
    ser_str("medias sem self test");
    ser_crlf(1);
    ser_str("\t");
    for (int i =0; i<6; i++){  ser_dec16(mediaNoST[i]);   ser_spc(1); }

    ser_crlf(2);
    
    ser_str("medias com self test");
    ser_crlf(1);
    ser_str("\t");
    for (int i =0; i<6; i++){  ser_dec16(mediaST[i]);   ser_spc(1); }

    ser_crlf(2);
    
    ser_str("Self test response");
    ser_crlf(1);
    ser_str("\t");
    for (int j =0; j<6; j++){  
      ser_dec16(selfTestResponse[j]);
      ser_spc(1); 
    }
    
    ser_crlf(2);

    ser_str("Factory self test code (nos registradores)");
    ser_crlf(1);
    ser_str("\t");
    for (int j =0; j<6; j++){  
      ser_dec8(ST_CODE[j]);
      ser_spc(1); 
    }
    
    ser_crlf(2);
    
    ser_str("Self Test value (st_opt)");
    ser_crlf(1);
    ser_str("\t");
    for (int j =0; j<6; j++){  
      ser_float(ST_OPT[j],2);
      ser_spc(1); 
    }

    ser_crlf(2); 
    
    ser_str("Porcentagens self_test_response / self_test_value");
    ser_crlf(1);
    ser_str("\t");
    for(int i = 0 ; i < 6 ; i++){
      ser_float(porcentagens[i],4); ser_spc(1); 
    }
    ser_crlf(1);

  }
  
  return passou;
 
}


// MPU: Realizar Self-Test (ST), prn = imprimir resultados?
// Retorna: TRUE  se passou no teste
//          FALSE se falhou no teste
//
//     - Self test off -  - Self test on -    -Reg. Self test -    -Calculo tolerância 
// vt[ ax ay az gx gy gz  ax ay az gx gy gz   ax ay az gx gy gz    ax ay az gx gy gz]
//     0                  6                   12                   18
// vt deve ter espaço para 24 inteiros
byte mpu_self_test(int *vt, byte prn) {
  byte x,cont;
  byte aux[6];   //Leitura dos registradores de Self test
  float  gxf, gyf, gzf, axf, ayf, azf; //Factory Trim
  
  //Acertar escalas e desligar Self Test
  mpu_escalas(GIRO_FS_250,ACEL_FS_8G);  //+/- 8g e +/-250gr/seg
  delay(250);                           //Aguardar cofiguração estabilizar
  mpu_rd_ac_gi(&vt[0]);                   //aux1 guarda leitura com self-test desabilitado
  
  // Habilitar Self_Test
  mpu_wr(ACCEL_CONFIG, 0xE0|(ACEL_FS_8G << 3));  //Escala 8g, Self-test Habilitado
  mpu_wr(GYRO_CONFIG, 0xE0|(GIRO_FS_250 << 3));  //Escala 250, Self-test Habilitado
  delay(250);                           //Aguardar cofiguração estabilizar
  mpu_rd_ac_gi(&vt[6]);                   //aux2 guarda leitura com self-test desabilitado

  // Leitura dos resultados do self-test - Montar valores
  mpu_rd_blk(SELF_TEST_X, aux, 4);
  vt[12] = (0x1C&(aux[0]>>3)) | (0x3&(aux[3]>>4));  //XA_TEST
  vt[13] = (0x1C&(aux[1]>>3)) | (0x3&(aux[3]>>2));  //YA_TEST
  vt[14] = (0x1C&(aux[2]>>3)) | (0x3&(aux[3]>>0));  //ZA_TEST
  vt[15] = aux[0]&0x1F;                             //XG_TEST
  vt[16] = aux[1]&0x1F;                             //YG_TEST
  vt[17] = aux[2]&0x1F;                             //ZG_TEST

  // Calcular os Factory Trim
  axf = (4096.0*0.34) * (pow((0.92/0.34) , (((float)vt[12] - 1.0) / 30.0)));
  ayf = (4096.0*0.34) * (pow((0.92/0.34) , (((float)vt[13] - 1.0) / 30.0)));
  azf = (4096.0*0.34) * (pow((0.92/0.34) , (((float)vt[14] - 1.0) / 30.0)));
  gxf = ( 25.0 * 131.0) * (pow( 1.046 , ((float)vt[15] - 1.0) ));
  gyf = (-25.0 * 131.0) * (pow( 1.046 , ((float)vt[16] - 1.0) ));
  gzf = ( 25.0 * 131.0) * (pow( 1.046 , ((float)vt[17] - 1.0) ));

  // Se registrador = 0 --> Factory Trim = 0
  if (vt[12] == 0) axf = 0;
  if (vt[13] == 0) ayf = 0;
  if (vt[14] == 0) azf = 0;
  if (vt[15] == 0) gxf = 0;
  if (vt[16] == 0) gyf = 0;
  if (vt[17] == 0) gzf = 0;

  // Calcular as Percentagens de Alteração
  vt[18] = 100.0 * ((float)(vt[ 6] - vt[0]) - axf ) / axf;
  vt[19] = 100.0 * ((float)(vt[ 7] - vt[1]) - ayf ) / ayf;
  vt[20] = 100.0 * ((float)(vt[ 8] - vt[2]) - azf ) / azf;
  vt[21] = 100.0 * ((float)(vt[ 9] - vt[3]) - gxf ) / gxf;
  vt[22] = 100.0 * ((float)(vt[10] - vt[4]) - gyf ) / gyf;
  vt[23] = 100.0 * ((float)(vt[11] - vt[5]) - gzf ) / gzf;

  if (prn==TRUE){ //Imprimir resultados ?
    ser_crlf(1);
    ser_str("\n--- Resultados Funcao Self Test ---\n");
    ser_spc(17);
    ser_str("ax     ay     az     gx     gy     gz\n");
    // Self test off
    ser_str("Self Test off: ");
    for (x=0; x<6; x++){  ser_dec16(vt[x]);   ser_spc(1); }
    ser_crlf(1);
    // Self test on 
    ser_str("Self Test on:  ");
    for (x=6; x<12; x++){  ser_dec16(vt[x]);   ser_spc(1); }
    ser_crlf(1);
    // Reg de Self test
    ser_str("Reg Self Test: ");
    for (x=12; x<18; x++){  ser_dec16(vt[x]);   ser_spc(1); }
    ser_crlf(1);
    //Factory trim
    ser_str("Factory Trim:  ");
    ser_float(axf,4); ser_spc(1);  
    ser_float(ayf,4); ser_spc(1);  
    ser_float(azf,4); ser_spc(1);  
    ser_float(gxf,4); ser_spc(1);  
    ser_float(gyf,4); ser_spc(1);  
    ser_float(gzf,4); ser_spc(1);  
    ser_crlf(1);
    // Resultado Tolerância
    ser_str("Resultados:    ");
    for (x=18; x<24; x++){  ser_dec16(vt[x]);   ser_spc(1); }
    ser_str("\n--- Fim Funcao Self Test ---\n");
    ser_crlf(1);
  }
  
  cont=0;
  for (x=18; x<24; x++){
    if (vt[x]>14) cont++;
  }

  if (cont==0)  return TRUE;
  else          return FALSE;
}


///////////////// Rotinas Básicas para MPU

// (10) Escrever num registrador do MPU
void mpu_wr(byte reg, byte dado) {
  twi_start(10);          //START
  twi_er(MPU_EWR, 11);    //Endereçar MPU para escrita
  twi_dado_er(reg, 12);   //Informar acesso ao PWR_MGMT_1 (0x6B)
  twi_dado_er(dado, 13);  //Selecionar PLL eixo X como referência
  twi_stop();             //Gerar STOP para finalizar
}

// (20) Ler um registrador do MPU
byte mpu_rd(byte reg) {
  uint8_t dado;
  twi_start(20);                //START
  twi_er(MPU_EWR, 21);           //Endereçar MPU para escrita
  twi_dado_er(reg, 22);         //Informar registrador
  twi_start_rep(23);            //START Repetido
  twi_et(MPU_ERD, 24);           //Endereçar MPU para leitura
  dado = twi_dado_et_nack(25);  //Receber dado do MPU com NACK
  twi_stop();                    //Gerar STOP para finalizar
  return dado;
}

// (30) Escrever um bloco de dados no MPU a partir de um registrador
void mpu_wr_blk(byte reg, byte *dado, byte qtd) {
  uint8_t i;
  twi_start(30);                //START
  twi_er(MPU_EWR, 31);          //Endereçar MPU para escrita
  twi_dado_er(reg, 32);         //Informar acesso ao PWR_MGMT_1 (0x6B)
  for (i = 0; i < qtd; i++)
    twi_dado_er(dado[i], 33);   //Selecionar PLL eixo X como referência
  twi_stop();                   //Gerar STOP para finalizar
}

// (40) Ler um bloco do MPU a partir de um registrador
void mpu_rd_blk(byte reg, byte *dado, byte qtd) {
  byte i;
  twi_start(40);                //START
  twi_er(MPU_EWR, 41);          //Endereçar MPU para escrita
  twi_dado_er(reg, 42);         //Informar registrador
  twi_start_rep(43);            //START Repetido
  twi_et(MPU_ERD, 44);          //Endereçar MPU para leitura
  for (i=0; i<qtd; i++)
    dado[i] = twi_dado_et_ack(45);  //Receber dados e gerar ACK
  dado = twi_dado_et_nack(46);  //Receber último dado e gerar NACK
  twi_stop();                   //Gerar STOP para finalizar
}


// MPU: ISR para a interrupção INT4
ISR(INT4_vect){                                
  mpu_dado_ok=TRUE;
  Scp1();
}
