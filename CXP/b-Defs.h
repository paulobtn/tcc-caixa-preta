// Defs.h
// Defines usados na Caixa Preta

#define TRUE  1
#define FALSE 0

#define CR 0xD    //Carriage Return
#define LF 0xA    //Line Feed
#define SPC 0x20  //Espaço

// BITS - definições das posições
#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08
#define BIT4  0x10
#define BIT5  0x20
#define BIT6  0x40
#define BIT7  0x80

// Endereços na SRAM
#define MPU_ADR_INI 0x00000L  //Início área MPU (12.720 msg x 18 = 228.960 Bytes) = 12,7 seg
#define MPU_ADR_FIM 0x37E60L  //Fim    área MPU (228.960)
#define CXP_ADR_INI 0x37E60L  //Início área de conficuração da Caixa Preta (sobra 416 bytes)
#define CXP_ADR_FIM 0x38000L  //Fim    área de conficuração da Caixa Preta
#define GPS_ADR_INI 0x38000L  //Início área GPS (256 msg x 128 = 32.768) = 25,6 seg
//#define GPS_ADR_FIM 0x40000L  //Fim    área GPS
#define GPS_ADR_FIM 0x40000L-GPS_PASSO  //Fim    área GPS
#define MPU_PASSO   18        //Tamanho de uma mensagem do MPU (Acel, Giro, Mag)
#define GPS_PASSO   128       //Tamanho da mensagem do GPS (gps_dados[])

// OPERAÇÃO - Principais parâmetros 
#define OP_FREQ       SAMPLE_RT_100Hz //Freq de operação do MPU e tb para Calibração ao Ligar: 
#define OP_ESC_ACEL   ACEL_FS_8G      //Acel: escala para operação
#define OP_ESC_GIRO   GIRO_FS_2000    //Giro: escala para operação

#define OP_QTD_MED_AG   8             //Calibração ao Ligar: quantidade de medidas por eixo acel e giro
#define OP_QTD_MED_MG  64             //Calibração ao Ligar: quantidade de medidas por eixo mag

// Códigos usados para indicar sim/não, passou/falhou
#define COD_SIM  0x5353     //(21.331) 2x ASCII(S) Afirmativo
#define COD_NAO  0x4E4E     //(20.046) 2x ASCII(N) Negativo

// Limiares para indicar Acidente
#define LIMIAR_AX  4     //4g
#define LIMIAR_AY  4     //4g
#define LIMIAR_AZ  4     //4g
#define LIMIAR_GX  1000 //1.000 gr/s
#define LIMIAR_GY  1000 //1.000 gr/s
#define LIMIAR_GZ  1000 //1.000 gr/s

// SRAM (FLASH) - Uso na Operação - Posições para guardar parâmetros
#define OP_OK      CXP_ADR_INI  //Fez calibração ao ligar? 0x4E4E=NN=Não (pronta) 0x5353=SS=Sim - Repetição do ASCII
#define OP_BATEU   OP_OK+2      //Bateu? 0x4E4E=NN=Não (pronta) 0x5353=SS=Sim - Repetição do ASCII
#define OP_ST_OK   OP_BATEU+2   //Passou no Self Test 0x4E4E=não e 0x5353=sim
#define OP_STH_OK  OP_ST_OK+2   //Magnetômetro Passou no Self Test 0x4E4E=não e 0x5353=sim
#define OP_CF_OK   OP_STH_OK+2  //Existe calibração de Fábrica (CF) 0x4E4E=não e 0x5353=sim
#define OP_CFH_OK  OP_CF_OK+2   //Magentômetro fez calibração de Fábrica (CF) 0x4E4E=não e 0x5353=sim

// Calibração Aceleração e Giro ao ligar o carro
#define OPC_FREQ_AG OP_CFH_OK+2   //Freq de amostragem usada na calibração Acel e Giro
#define OPC_BW_AG   OPC_FREQ_AG+2 //Banda passante usada na calibração Acel e Giro
#define OPC_ESC_AC  OPC_BW_AG+2   //Escala do Acelerômetro usada na calibração
#define OPC_ESC_GI  OPC_ESC_AC+2  //Escala do Giroscópio usada na calibração
#define OPC_QTD_AG  OPC_ESC_GI+2  //QTd de medidas para a calibração Acel e Giro
#define OPC_AX      OPC_QTD_AG+2  //Calibração AX
#define OPC_AY      OPC_AX+2      //Calibração AY
#define OPC_AZ      OPC_AY+2      //Calibração AZ
#define OPC_TP      OPC_AZ+2      //Calibração TP
#define OPC_GX      OPC_TP+2      //Calibração GX
#define OPC_GY      OPC_GX+2      //Calibração GY
#define OPC_GZ      OPC_GY+2      //Calibração GZ
// Calibração Magnetômetro ao ligar o carro
#define OPC_QTD_MG  OPC_GZ+2      //QTd de medidas para a calibração Magn
#define OPC_ESC_MG  OPC_QTD_MG+2  //Escalas acel (MSB) e giro (LSB)
#define OPC_HX      OPC_ESC_MG+2  //Calibração HX
#define OPC_HY      OPC_HX+2      //Calibração HY
#define OPC_HZ      OPC_HY+2      //Calibração HZ

// Parâmetros para operação: escala e limiar
#define OP_FREQ_AG  OPC_HZ+2      //Freq de amostragem usada na operação Acel e Giro
#define OP_BW_AG    OP_FREQ_AG+2  //Banda passante usada na operação Acel e Giro
#define OP_ESC_AC   OP_BW_AG+2    //Escala do Acelerômetro usada na operação
#define OP_ESC_GI   OP_ESC_AC+2   //Escala de giro para operaçao
#define OP_ESC_MG   OP_ESC_GI+2   //Escala de magnet para operaçao
#define OP_LIM_AX   OP_ESC_MG+2   //AX - Limiar para disparo (valor absoluto)
#define OP_LIM_AY   OP_LIM_AX+2   //AY - Limiar para disparo (valor absoluto)
#define OP_LIM_AZ   OP_LIM_AY+2   //AZ - Limiar para disparo (valor absoluto)
#define OP_LIM_GX   OP_LIM_AZ+2   //GX - Limiar para disparo (valor absoluto)
#define OP_LIM_GY   OP_LIM_GX+2   //GY - Limiar para disparo (valor absoluto)
#define OP_LIM_GZ   OP_LIM_GY+2   //GZ - Limiar para disparo (valor absoluto)
// Quem disparou
#define OP_MPU_ADR  OP_LIM_GZ+2   //(32 bits) Endereço MPU no momento do disparo
#define OP_GPS_ADR  OP_MPU_ADR+4  //(32 bits) Endereço GPS no momento do disparo
#define OP_DISP_TP  OP_GPS_ADR+4  //Temperatura no instante do disparo
#define OP_DISP_AX  OP_DISP_TP+2  //AX disparou (SS=Sim e NN=Não)
#define OP_DISP_AY  OP_DISP_AX+2  //AY disparou (SS=Sim e NN=Não)
#define OP_DISP_AZ  OP_DISP_AY+2  //AZ disparou (SS=Sim e NN=Não)
#define OP_DISP_GX  OP_DISP_AZ+2  //GX disparou (SS=Sim e NN=Não)
#define OP_DISP_GY  OP_DISP_GX+2  //GY disparou (SS=Sim e NN=Não)
#define OP_DISP_GZ  OP_DISP_GY+2  //GZ disparou (SS=Sim e NN=Não)
#define OP_BRK      OP_DISP_GZ+2  //Aquisição foi interrompida (S=Sim e NN=Não)
#define OP_ULT_ADR  OP_BRK+2      //Último endereço gravado pelo MPU

// Data e hora do acidente
#define OP_AC_DATA  OP_ULT_ADR+4    //ddmmyy0 --> Data do acidente, vem do GPS
#define OP_AC_HORA  OP_AC_DATA+8    //hhmmss.sss0 --> hora do acidente, vem do GPS
#define OP_VAZIO    OP_AC_HORA+12   //Vazio

// CALIBRAÇÃO DE FÁBRICA
const char *CF_HOJE = "24/07/20"; //Data para Configuração de Fábrica
const char *CF_BSB = "Brasilia";  //Data para Configuração de Fábrica
#define G_PADRAO    9.80665       //1g padrão
#define G_BSB       9.7808439     //Ac. gravidade em Brasília
#define CF_ESPERA   10          //Tempo de espera (seg) antes de calibrar o MPU
#define CF_FREQ     100           //Calibração de Fábrica: Freq de amostragem do MPU
#define CF_QTD_MED  256         //Calibração de Fábrica: quantidade de medidas por eixo

// Endereços da EEPROM
// Usada para guardar Calibraçao de Fábrica
// Posições para Data, Local e Aceleração
#define EEPROM_TAM      4096  //Tamanho da EEPROM (12 bits de endereços)
#define CF_COD_OK       0x5353            //Código indica calibração já fita, SIM = SS = 0x5353

#define CF_OK           0                 //0 - Já fez calibração? SS = SIM
#define CF_DATA         CF_OK+2           //2 - Data da configuração
#define CF_LOCAL        CF_DATA+14        //10 - String com Local da configuração
#define CFG_PADRAO      CF_LOCAL+32       //30 - String com Aceleração da gravidade padrão
#define CFG_LOCAL       CFG_PADRAO+16     //40 - String com Aceleração da gravidade local
#define CFG_PADRAO_BIN  CFG_LOCAL+16      //50 - Inteiro com Aceleração da gravidade padrão
#define CFG_LOCAL_BIN   CFG_PADRAO_BIN+2  //52 - Inteiro com Aceleração da gravidade local
#define CF_WHO          CFG_LOCAL_BIN+2   //54 - Resposta ao Who am I em decimal
//////// Calibração --- Posições para guardar parâmetros e resultados das médias das medidas
#define CF_FA           CF_WHO+2          //56 - Freq de amostragem usada na calibração
#define CF_BW           CF_FA+2           //58 - Banda passante usada na calibração
#define CF_QTD          CF_BW+2           //5A - Quantidade de medidas por eixo
#define CF_ESC_AC       CF_QTD+2          //5C - Escala usada para o acelerômetro
#define CF_ESC_GI       CF_ESC_AC+2       //5E - Escala usada para o giroscópio
#define CF_AX           CF_ESC_GI+2       //60
#define CF_AY           CF_AX+2           //62
#define CF_AZ           CF_AY+2           //64
#define CF_TP           CF_AZ+2           //66
#define CF_GX           CF_TP+2           //68
#define CF_GY           CF_GX+2           //6A
#define CF_GZ           CF_GY+2           //6C
//////// Somatórios --- Posições para guardar a média das medidas
#define CF_AX_SOMA     CF_GZ+2            //6E
#define CF_AY_SOMA     CF_AX_SOMA+4       //70
#define CF_AZ_SOMA     CF_AY_SOMA+4       //74
#define CF_TP_SOMA     CF_AZ_SOMA+4       //78
#define CF_GX_SOMA     CF_TP_SOMA+4       //7C
#define CF_GY_SOMA     CF_GX_SOMA+4       //80
#define CF_GZ_SOMA     CF_GY_SOMA+4       //84
//////// Calibração --- Posições para guardar a Primeira e última das medidas e a média
#define CF_AX_PRI      CF_GZ_SOMA+4       //88
#define CF_AY_PRI      CF_AX_PRI+2        //8A
#define CF_AZ_PRI      CF_AY_PRI+2        //8C
#define CF_TP_PRI      CF_AZ_PRI+2        //8E
#define CF_GX_PRI      CF_TP_PRI+2        //90
#define CF_GY_PRI      CF_GX_PRI+2        //92
#define CF_GZ_PRI      CF_GY_PRI+2        //94
#define CF_AX_ULT      CF_GZ_PRI+2        //96
#define CF_AY_ULT      CF_AX_ULT+2        //98
#define CF_AZ_ULT      CF_AY_ULT+2        //9A
#define CF_TP_ULT      CF_AZ_ULT+2        //9C
#define CF_GX_ULT      CF_TP_ULT+2        //9E
#define CF_GY_ULT      CF_GX_ULT+2        //A0
#define CF_GZ_ULT      CF_GY_ULT+2        //A2
//////// Self Test --- Posições para guardar resultados do self-test
#define CF_ST_OK       CF_GZ_ULT+2        //A4 - Passou no self test? TRUE/FALSE
#define CF_ST_OFF_AX   CF_ST_OK+2
#define CF_ST_OFF_AY   CF_ST_OFF_AX+2
#define CF_ST_OFF_AZ   CF_ST_OFF_AY+2
#define CF_ST_OFF_GX   CF_ST_OFF_AZ+2
#define CF_ST_OFF_GY   CF_ST_OFF_GX+2
#define CF_ST_OFF_GZ   CF_ST_OFF_GY+2

#define CF_ST_ON_AX    CF_ST_OFF_GZ+2
#define CF_ST_ON_AY    CF_ST_ON_AX+2
#define CF_ST_ON_AZ    CF_ST_ON_AY+2
#define CF_ST_ON_GX    CF_ST_ON_AZ+2
#define CF_ST_ON_GY    CF_ST_ON_GX+2
#define CF_ST_ON_GZ    CF_ST_ON_GY+2

#define CF_ST_REG_AX   CF_ST_ON_GZ+2    //ax - Reg de self-test (16 bits mas usa apenas 8 bits)
#define CF_ST_REG_AY   CF_ST_REG_AX+2   //ay - Reg de self-test (16 bits mas usa apenas 8 bits)
#define CF_ST_REG_AZ   CF_ST_REG_AY+2   //az - Reg de self-test (16 bits mas usa apenas 8 bits)
#define CF_ST_REG_GX   CF_ST_REG_AZ+2   //gx - Reg de self-test (16 bits mas usa apenas 8 bits)
#define CF_ST_REG_GY   CF_ST_REG_GX+2   //gy - Reg de self-test (16 bits mas usa apenas 8 bits)
#define CF_ST_REG_GZ   CF_ST_REG_GY+2   //gz - Reg de self-test (16 bits mas usa apenas 8 bits)

#define CF_ST_TOL_AX   CF_ST_REG_GZ+2   //ax - Resultado self-test (<14%) (16 bits mas usa apenas 8 bits)
#define CF_ST_TOL_AY   CF_ST_TOL_AX+2   //ay - Resultado self-test (<14%) (16 bits mas usa apenas 8 bits)
#define CF_ST_TOL_AZ   CF_ST_TOL_AY+2   //az - Resultado self-test (<14%) (16 bits mas usa apenas 8 bits)
#define CF_ST_TOL_GX   CF_ST_TOL_AZ+2   //gx - Resultado self-test (<14%) (16 bits mas usa apenas 8 bits)
#define CF_ST_TOL_GY   CF_ST_TOL_GX+2   //gy - Resultado self-test (<14%) (16 bits mas usa apenas 8 bits)
#define CF_ST_TOL_GZ   CF_ST_TOL_GY+2   //gz - Resultado self-test (<14%) (16 bits mas usa apenas 8 bits)

#define CF_MAG_OK     CF_ST_TOL_GZ+2   //Já fez calibração do Magnetômetro? COD_SIM ou COD_NAO
#define CF_STH_OK     CF_MAG_OK+2      //Magnetômetro passou no Self Test?
#define CF_STH_HX     CF_STH_OK+2      //Leitura de HX durante o Self Test?
#define CF_STH_HY     CF_STH_HX+2      //Leitura de HY durante o Self Test?
#define CF_STH_HZ     CF_STH_HY+2      //Leitura de HZ durante o Self Test?
#define CF_HX_ASA     CF_STH_HZ+2      //hx - ASA = Ajuste gravado na ROM (16 bits mas usa apenas 8 bits) Negativo?
#define CF_HY_ASA     CF_HX_ASA+2      //hy - ASA = Ajuste gravado na ROM (16 bits mas usa apenas 8 bits) Negativo?
#define CF_HZ_ASA     CF_HY_ASA+2      //hz - ASA = Ajuste gravado na ROM (16 bits mas usa apenas 8 bits) Negativo?
#define CF_HX_OFF     CF_HZ_ASA+2      //hx - Offset multiplicado por 10, para dar precisão
#define CF_HY_OFF     CF_HX_OFF+2      //hy - Offset multiplicado por 10, para dar precisão
#define CF_HZ_OFF     CF_HY_OFF+2      //hz - Offset multiplicado por 10, para dar precisão
#define CF_HX_ESC     CF_HZ_OFF+2      //hx - Escala multiplicada por 10, para dar precisão
#define CF_HY_ESC     CF_HX_ESC+2      //hx - Escala multiplicada por 10, para dar precisão
#define CF_HZ_ESC     CF_HY_ESC+2      //hx - Escala multiplicada por 10, para dar precisão
//dados usados na calibração do acelerometro
#define CF_AX_X_CIMA     CF_HZ_ESC+2     //valor de ax quando o eixo X aponta para cima
#define CF_AX_X_BAIXO    CF_AX_X_CIMA+2   //valor de ax quando o eixo X aponta para baixo
#define CF_AX_Y_CIMA     CF_AX_X_BAIXO+2  //valor de ay quando o eixo Y aponta para cima
#define CF_AX_Y_BAIXO    CF_AX_Y_CIMA+2   //valor de ay quando o eixo Y aponta para baixo
#define CF_AX_Z_CIMA     CF_AX_Y_BAIXO+2  //valor de az quando o eixo Z aponta para cima
#define CF_AX_Z_BAIXO    CF_AX_Z_CIMA+2   //valor de az quando o eixo Z aponta para baixo
//dados usados na calibração do magnetômetro
#define CF_HX_MIN    CF_AX_Z_BAIXO+2      // valor mínimo de hx
#define CF_HX_MAX    CF_HX_MIN+2          // valor máximo de hx
#define CF_HY_MIN    CF_HX_MAX+2          // valor mínimo de hy
#define CF_HY_MAX    CF_HY_MIN+2          // valor mínimo de hy
#define CF_HZ_MIN    CF_HY_MAX+2          // valor mínimo de hz
#define CF_HZ_MAX    CF_HZ_MIN+2          // valor máximo de hz



// LCD Constantes para os bits de controle
#define LCD_BL  8 //Back Light
#define LCD_RW  4 //R/#W
#define LCD_RS  2 //RS
#define LCD_E   1 //Enable

// LCD - Bits para indicar qual linha mudou
#define LCD_LINHA0 BIT0 //Linha 0 foi alterada
#define LCD_LINHA1 BIT1 //Linha 1 foi alterada
#define LCD_LINHA2 BIT2 //Linha 0 foi alterada
#define LCD_LINHA3 BIT3 //Linha 1 foi alterada

// LCD - Setas Esqeuerda e Direita
#define LCD_SETA_DIR  0x7E  //
#define LCD_SETA_ESQ  0x7F  //

// Timer 1 
#define FREQ_T1 100  //Freq de interrupção do timer 1

// Timer 2
#define FREQ_T2 1000  //Freq de interrupção do timer 1
//#define FREQ_T2 5000  //Freq de interrupção do timer 1
//#define FREQ_T2 25000  //Freq de interrupção do timer 1

// TESTE
#define TESTE_TOT 17     //Modos de teste: 1, 2 , ..., 17
#define TESTE_0    0     //Opera
#define TESTE_1    1     //LEDs
#define TESTE_2    2     //LCD
#define TESTE_3    3     //Teclado
#define TESTE_4    4     //TWI
#define TESTE_5    5     //Acel e giro
#define TESTE_6    6     //Magnetometro
#define TESTE_7    7     //SRAM
#define TESTE_8    8     //FLAH
#define TESTE_9    9     //GPS Tudo
#define TESTE_10  10     //GPS RMC GSA
#define TESTE_11  11     //GPS U-Center
#define TESTE_12  12     //MPU-->Matlab
#define TESTE_13  13     //BlueTooth
#define TESTE_14  14     //Livre
#define TESTE_15  15     //Livre
#define TESTE_16  16     //Livre
#define TESTE_17  17     //Livre

// OPERA
#define OPERA_TOT  9     //Modos de teste: 1, 2 , ..., 9
#define OPERA_0    0     //Teste
#define OPERA_1    1     //Aqusição de Dados
#define OPERA_2    2     //Livre
#define OPERA_3    3     //Livre
#define OPERA_4    4     //Livre
#define OPERA_5    5     //Calibração de Fábrica
#define OPERA_6    6     //Calibração do Magnetômetro
#define OPERA_7    7     //Livre
#define OPERA_8    8     //Livre
#define OPERA_9    9     //Livre

// TECLADO - Parametros para leitura das chaves
#define  SW_FILA_TAM  10  // tamanho da fila do teclado
#define  SW_TOL     20    // tolerancia para identificar chave
// Codigos para as chaves
#define  SW_NADA    7
#define  SW_INF     6
#define  SW_DIR     4
#define  SW_SUP     3
#define  SW_ESQ     2
#define  SW_SEL     0
#define  SW_SEQ1    8
#define  SW_SEQ2    9
#define  SW_NAOSEI 10

// Aleat - Constantes para o gerador pseudo-aleatório
#define ALEAT_SUGEST_M   53  //m sugerido
#define ALEAT_SUGEST_D   109 //d sugerido
#define ALEAT_SUGEST_U   13  //u sugerido (semente)


// GPS
// Identificar tipos de mensagens
#define GPS_NADA  0
#define GPS_RMC   1
#define GPS_VTG   2
#define GPS_GGA   3
#define GPS_GSA   4
#define GPS_GSV   5
#define GPS_GLL   6
#define GPS_MSG_TAM 200   //Tamanho max de uma msg do GPS
#define GPS_DADOS_TAM 128   //Tamanho do vetor para os dados extraídos do GPS

// Marcar posição de cada um dos parâmetros guardados em gps_dados[GPS_DADOS_TAM]
#define GPS_STATUS     0                 //2 bytes
#define GPS_HORA      (GPS_STATUS+2)    //11 bytes
#define GPS_DATA      (GPS_HORA+11)     //7 bytes
#define GPS_LAT       (GPS_DATA+7)      //11 bytes
#define GPS_NS        (GPS_LAT+11)      //2 bytes
#define GPS_LONG      (GPS_NS+2)        //12 bytes
#define GPS_EW        (GPS_LONG+12)     //2 bytes
#define GPS_VEL_NOS   (GPS_EW+2)        //8 bytes
#define GPS_CURSO     (GPS_VEL_NOS+8)   //8 bytes
#define GPS_PDOP      (GPS_CURSO+8)     //6 bytes
#define GPS_HDOP      (GPS_PDOP+6)      //6 bytes
#define GPS_VDOP      (GPS_HDOP+6)      //6 bytes
#define GPS_VEL_KPH   (GPS_VDOP+6)      //7 bytes
#define GPS_VEL_UN    (GPS_VEL_KPH+7)   //2 bytes
#define GPS_FIX       (GPS_VEL_UN+2)    //2 bytes ?sem uso? - não me lembro por que
#define GPS_QTD_SAT   (GPS_FIX+2)       //3 bytes
#define GPS_ALT       (GPS_QTD_SAT+3)   //7 bytes
#define GPS_ALT_UN    (GPS_ALT+7)       //2 bytes
#define GPS_ADR_SRAM  (GPS_ALT_UN+2)    //5 bytes

/////////////// MPU 6050 - Constantes
#define MPU_ADR  0x68  //Endereço MPU-6050
#define MPU_EWR  0xD0  //MPU para escrita (0x68<<1)
#define MPU_ERD  0xD1  //MPU para leitura (0x68<<1 + 1)
#define MPU9250_WHO 0x73  //MPU Who am I

//Escalas para Giroscópio
#define GIRO_FS_250  0   // +/- 250 graus/seg
#define GIRO_FS_500  1   // +/- 500 graus/seg
#define GIRO_FS_1000 2   // +/- 1000 graus/seg
#define GIRO_FS_2000 3   // +/- 2000 graus/seg

//Escalas para Acelerômetro
#define ACEL_FS_2G  0   // +/- 2g
#define ACEL_FS_4G  1   // +/- 4g
#define ACEL_FS_8G  2   // +/- 8g
#define ACEL_FS_16G 3   // +/- 16g

// Valores para o Sample Rate, Registrador SMPLRT_DIV
// Considerando Taxa = 1kHz (Registrador CONFIG)
#define SAMPLE_RT_1kHz     0   // 1.000/(0+1) = 1000
#define SAMPLE_RT_500Hz    1   // 1.000/(1+1) = 500
#define SAMPLE_RT_333Hz    2   // 1.000/(2+1) = 333,33
#define SAMPLE_RT_250Hz    3   // 1.000/(3+1) = 250
#define SAMPLE_RT_200Hz    4   // 1.000/(4+1) = 200
#define SAMPLE_RT_166Hz    5   // 1.000/(5+1) = 166,66
#define SAMPLE_RT_142Hz    6   // 1.000/(6+1) = 142,85
#define SAMPLE_RT_125Hz    7   // 1.000/(7+1) = 125
#define SAMPLE_RT_111Hz    8   //1.000 /(8+1) = 111,11
#define SAMPLE_RT_100Hz    9   //1.000 /(9+1) = 100

// Registradores do MPU-9250 que foram usados

//adicionados 26 set
#define SELF_TEST_X_GYRO      0x00
#define SELF_TEST_Y_GYRO      0x01
#define SELF_TEST_Z_GYRO      0x02
#define SELF_TEST_X_ACCEL     0x0D
#define SELF_TEST_Y_ACCEL     0x0E
#define SELF_TEST_Z_ACCEL     0x0F
#define XG_OFFSET_H           0x13
#define XG_OFFSET_L           0x14
#define YG_OFFSET_H           0x15
#define YG_OFFSET_L           0x16
#define ZG_OFFSET_H           0x17
#define ZG_OFFSET_L           0x18


#define SELF_TEST_X      0x0D
#define SELF_TEST_Y      0x0E
#define SELF_TEST_Z      0x0F
#define SELF_TEST_A      0x10
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG_2   0x1D //adicionado 26 set
#define FIFO_EN          0x23
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define TEMP_OUT_H       0x41
#define USER_CTRL        0x6A
#define PWR_MGMT_1       0x6B
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I         0x75

/////////////// MPU 9250 - Magnetômetro
#define MAG_I2C_ADDR    0x0C //endereço i2c do magnetometro
#define MAG_I2C_ADDR_WR 0x18 //0x0c << 1
#define MAG_I2C_ADDR_RD 0x19 //(0x0c << 1) + 1
#define MAG_WHO         0x48  //MAG Who am I
// Registradores
#define MAG_CNTL_1      0x0A  //Controle 1
#define MAG_CNTL_2      0x0B  //(RSV) Controle 2
#define MAG_ASTC        0x0C  //Self Test
#define MAG_XOUT_L      0x03  //MAG XL seq:[XL XH YL YH ZL ZH]
#define MAG_ASAX        0x10 //end. reg. ASAX do magnetometro
#define MAG_ASAY        0x11 //end. reg. ASAY do magnetometro  
#define MAG_ASAZ        0x12 //end. reg. ASAZ do magnetometro  
#define MAG_ST1         0x02 //end. reg. ST1 do magnetometro (DRDY)
#define MAG_ST2         0x09 //end. reg. ST2 do magnetometro (HOFL)

//////////////////////////////////////////////////////////////////////////////////
////////////// CONSTANTES DE BAIXO NÍVEL /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// SRAM 23LC1024

#define SRAM_TAM_CHIP 0x20000L  //Máximo por chip

// Instruções da SRAM
#define SRAM_READ    3   //Ler dado da memória
#define SRAM_WRITE   2   //Escrever dado da memória
#define SRAM_RDMR    5   //Ler Registrador de Modo
#define SRAM_WRMR    1   //Escrever no Registrador de Modo

// Registrador de Modo: Modos de Operação
#define SRAM_MODO_BYTE   0x00  //Modo Byte
#define SRAM_MODO_PAG    0x80  //Modo Página
#define SRAM_MODO_SEQ    0x40  //Modo Sequencial

// Velocidades SPI, verificar o dobrador (SPI2X)
#define SPI_125K   0 //SCL=125KHz, SPI2X=0
#define SPI_250K   1 //SCL=250KHz, SPI2X=0
#define SPI_500K   2 //SCL=500KHz, SPI2X=1
#define SPI_1M     3 //SCL=1MHz,   SPI2X=0
#define SPI_2M     4 //SCL=2MHz    SPI2X=1
#define SPI_4M     5 //SCL=4MHz    SPI2X=0
#define SPI_8M     6 //SCL=8MHz    SPI2X=1

#define MISO  50  //Master Input
#define MOSI  51  //Master Output      
#define SCK   52  //Saída do relógio
#define CS0   49  //(PL0) Controla o estado do #CS0 (0x0 0000 -> 0x1 FFFF)
#define CS1   48  //(PL1) Controla o estado do #CS1 (0x2 0000 -> 0x3 FFFF)      
#define CS2   47  //(PL2) Controla o estado do #CS2      
#define SS    53  //Para Mestre é preciso SS como saída      

// LCD
#define NRL 4   //Qtd de linhas do LCD
#define NRC 20  //Qtd de colunas do LCD


////////////////////////////////////////////////////////////
/////////////////////// FILAS //////////////////////////////
////////////////////////////////////////////////////////////

// UART0(Arduini) e UART2(Bluetooth) integradas
#define SERI_FILA_TAM 50   //Tamanho da fila circular de entrada serial
#define SERO_FILA_TAM 200   //Tamanho da fila circular de saída

// GPS - Serial 3
#define GPS_TX_FILA_TAM 10 //Tamanho da fila circular de TX
#define GPS_RX_FILA_TAM 300 //Tamanho da fila circular de RX

// Endereços da FLASH 24LC1025 (128 KB) TWI
#define FLASH1_ADR     0x50  //FLASH1
#define FLASH2_ADR     0x51  //FLASH2
#define FLASH1_ADR_B0  FLASH1_ADR+0  //FLASH1, Bloco 0: 64KB (0x00000 -> 0x0FFFF)
#define FLASH1_ADR_B1  FLASH1_ADR+4  //FLASH1, Bloco 1: 64KB (0x10000 -> 0x1FFFF)
#define FLASH2_ADR_B0  FLASH2_ADR+0  //FLASH2, Bloco 0: 64KB (0x00000 -> 0x0FFFF)
#define FLASH2_ADR_B1  FLASH2_ADR+4  //FLASH2, Bloco 1: 64KB (0x10000 -> 0x1FFFF)
#define FLASH_PAG   128              //Tamanho da página para gravação         

/////////////// TWI - Códigos de Status
#define TWI_START_OK      8     //Start OK
#define TWI_START_REP_OK  0x10  //Start Repetido OK
#define TWI_SLA_WR_ACK    0x18  //EET enviado e ACK recebido
#define TWI_SLA_WR_NACK   0x20  //EET enviado e NACK recebido
#define TWI_TX_DATA_ACK   0x28  //Dado enviado e ACK recebido
#define TWI_SLA_RD_ACK    0x40  //EER enviado e ACK recebido
#define TWI_SLA_RD_NACK   0x48  //EER enviar e NACK recebido 
#define TWI_RX_DATA_NACK  0x58  //Dado recebido e NACK gerado
#define TWI_RX_DATA_ACK   0x50  //Dado recebido e ACK gerado
#define TWI_TMI_OUT       10000 //Time out

// Códigos de erro no trabalho com TWI
#define TWI_ERRO_1  1   //Erro ao gerar START
#define TWI_ERRO_2  2   //Erro ao gerar START Repetido
#define TWI_ERRO_3  3   //Erro Escravo Receptor endereçado (ER) não enviou ACK
#define TWI_ERRO_4  4   //Erro Escravo Transmissor endereçado (ET) não enviou ACK
#define TWI_ERRO_5  5   //Erro Escravo Receptor (ER) não enviou ACK após envio do dado
#define TWI_ERRO_6  6   //Erro ao receber um dado do Escravo Transmissor (ET) e gerar um ACK
#define TWI_ERRO_7  7   //Erro ao receber um dado do Escravo Transmissor (ET) e gerar um NACK
#define TWI_ERRO_8  8   //Erro ao esperar TWINT - Timeout esperando TWINT ir para 1
