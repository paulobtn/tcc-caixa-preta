// Global.h - Variáveis globais
// CXP - Caixa Preta

// MPU
volatile byte mpu_dado_ok;    //Indicar que MPU tem novo dado

// LCD - variáveis globais
volatile byte lcd_buf[NRL][NRC+1];   //Buffer para as 4 linhas do LCD
volatile byte lcd_busy;              //TRUE indica que a interrupção está atualizando LCD
volatile byte lcd_mudou;             //Indicar qual linha deve ser atualizada
volatile byte *lcd_lin;              //Ponteiro para a linha a ser atualizada
volatile byte lcd_fase;              //Timer2 - Fase da atualização, 0, 1, 2, 3, 2, 3, ...
volatile byte lcd_ix;                //Timer2 - Indexador das colunas de uma linha, 0, 1, ..., 20
volatile byte lcd_rr;                //Timer1 - Round Robin para descobrir qual linha atualizar


// Teclas - Nome das teclas com apenas 3 letras
//                  0     1      2     3     4      5      6      7      8      9      10
char *sw_nome[]={"SEL","?1?", "ESQ","SUP","DIR", "?5?", "INF", "NAD", "SQ1", "SQ2", "???"};
//volatile char sw_fila[SER_TX_FILA_TAM]; //Espaço para a fila teclado
volatile char sw_fila[SW_FILA_TAM]; //Espaço para a fila teclado
volatile byte sw_pin, sw_pout;          //Ponteiros para usar a fila
volatile byte sw_1,sw_2,sw_n,sw_v;      //Variáveis para detectar teclas acionadas
volatile byte sw_st_seq1,sw_st_seq2;    //Maq Estados para buscar sequências


// TESTE - Mensagens do modo de teste
char *teste_msg[]={ "0-Vai para Opera",        //0
                    "1-LEDs",                  //1
                    "2-LCD",                   //2
                    "3-Teclado",               //3
                    "4-TWI (I2C)",             //4
                    "5-Acel e giro",           //5
                    "6-Magnetometro",          //6
                    "7-SRAM",                  //7
                    "8-FLASH",                 //8
                    "9-GPS: Tudo",             //9
                    "10-GPS: Interpreta",      //10
                    "11-GPS:U-Center",         //11
                    "12-MPU-->MatLab",         //12
                    "13-Blue Tooth",           //13
                    "14-BT - Cmds AT",         //14
                    "15-Self test mpu",        //15
                    "16-Self Test mag",        //16
                    "17-Calibrar Acelerometro"};//17
                    
// OPERA - Mensagens do modo de Operação
char *opera_msg[]={ "0-Vai para Teste",   //0
                    "1-Adquirir Dados",   //1
                    "2-Vazio",            //2
                    "3-vazio",            //3
                    "4-Vazio",            //4
                    "5-Calibra Fab",      //5
                    "6-Vazio",      //6
                    "7-Calibra Mag",            //7
                    "8-Calibra Acel",            //8
                    "9-Vazio"};           //9
                    

// Aleat - Gerador pseudo-aleatório
volatile word rrand_m ;  //multiplicador
volatile word rrand_d;   //divisor
volatile word rrand_u;   //semente

//GPS
volatile byte gps_dados[GPS_DADOS_TAM];     //Vetor para guardar dados extraídos das msg do GPS
volatile byte gps_st;   //Estado do identificador de mensagens, usado em teste_9_gps_msg(char letra)
volatile byte gps_rx_ok;                    //Indica que terminou recepção
volatile byte gps_tx_ok;                    //Indica que terminou transmissão
volatile byte gps_msg_0[GPS_MSG_TAM];       //(fase=0) Buffer 0 usado pela interrupção para receber GPS
volatile byte gps_msg_1[GPS_MSG_TAM];       //(fase=1) Buffer 1 usado pela interrupção para receber GPS
volatile byte gps_msg_ix;                   //Indexador para escrever nos buffers;
volatile byte gps_msg_fase;                 //Fase para receber GPRMC (0=gps_rmc_0 e 1=gps_rmc_1);
volatile byte gps_msg_ok;                   //Indica que completou o recebimento de uma mensagem;
volatile byte gps_tipo;                     //Tipo de msg GPS que foi analisada


// RMC só para testar
byte *gps_t="$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A*57";

//Timer 1
volatile byte timer1_cont;       //(0, ..., 49) Contar estados do Timer 1 para ativar ADC
volatile byte flag_10ms;         //Marca 10 mseg

////////////////////////////////////////////////////////////
/////////////////////// FILAS //////////////////////////////
////////////////////////////////////////////////////////////

// SERI e SERO: UART0(Arduino) e UART2(Bluetooth) integradas
volatile char seri_fila[SERI_FILA_TAM]; //Espaço para a fila serial de entrada
volatile word seri_pin, seri_pout;      //Ponteiros para usar a fila
volatile char sero_fila[SERO_FILA_TAM]; //Espaço para a fila serial de saída
volatile word sero_pin, sero_pout;      //Ponteiros para usar a fila
volatile byte sero_seg;                 //Segundo sero - garantir que uART0 e UART2 transmitam (interrupção)
volatile byte sero_parou;               //Indica que fila sero passou pelo estado vazia (recomeçar interrupções)
volatile byte sero_schnell;             //Bloqueia a fila e imprimi direto na porta serial (grandes qtd de dados)

//ADC
volatile word adc_val;    //Valor lido pelo ADC (em 16 bits porque tem contas)
volatile byte adc_ok;     //Flag indica que terminou conversão
volatile byte sw_val;     //Teclado: valor lido pelo ADC
volatile byte vcar_val;   //VCAR: valor lido pelo ADC
volatile byte vcap_val;   //VCAP: valor lido pelo ADC

//magnetometro
volatile byte mag_asa[3]; //local para armazenar os valores dos registradores asax, asay e asaz do magnetometro
