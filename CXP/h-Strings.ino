// Strings - Gerar strings para a impressão LCD e Serial
// CXP - Caixa Preta
// 10/01/2019

// Retorna String com a LONGITUDE para Googlemaps
// Recebe = dddmm.mmmmm (degraus and minutes)
// ns = 'E' ou 'W'
// msg = +/-ddd.ddddddd
void str_long(byte *longi, byte ew, byte *msg){
  byte i;
  byte g;
  long m;
  float f;
  g=100*(longi[0]-0x30);  //graus Centena
  g+=10*(longi[1]-0x30);  //graus Dezena
  g+=   (longi[2]-0x30);  //graus Unidade
  m=10*(longi[3]-0x30)+(longi[4]-0x30);
  for (i=0; i<5; i++){
    m = 10*m;
    m = m+(longi[6+i]-0x30);
  }
  f=g+((float)m/6000000L);
  if (ew=='W')  f=-f;
  str_float(f,9,msg);

  /*  
  ser_str("\n\nstr_longi(): graus=");
  ser_dec8unz(g);
  ser_str(" minutos=");
  ser_dec32unz(m);
  ser_str(" float=");
  ser_float(f,8);
  ser_crlf(1);
  */
  
}

// Retorna String com a LATITUDE para Googlemaps
// Recebe = ddmm.mmmmm (degraus and minutes)
// ns = 'N' ou 'S'
// msg = +/-dd.ddddddd
void str_lat(byte *lati, byte ns, byte *msg){
  byte i;
  byte g;
  long m;
  float f;
  g=10*(lati[0]-0x30)+(lati[1]-0x30);
  m=10*(lati[2]-0x30)+(lati[3]-0x30);
  for (i=0; i<5; i++){
    m = 10*m;
    m = m+(lati[5+i]-0x30);
  }
  f=g+((float)m/6000000L);
  if (ns=='S')  f=-f;
  str_float(f,9,msg);

  /*
  ser_str("\n\n str_lat: graus=");
  ser_dec8unz(g);
  ser_str(" minutos=");
  ser_dec32unz(m);
  ser_str(" float=");
  ser_float(f,8);
  ser_crlf(1);
  */
}

// Escala Aceleração (Fundo de Escala) - Retorna string com a escala
// Precisa de espaço de 8 caracteres incluindo o zero final
void str_fs_acel(byte esc, char *vt){
  switch(esc){
    case ACEL_FS_2G:  str_copia_zf("+/- 2g",vt); break;
    case ACEL_FS_4G:  str_copia_zf("+/- 4g",vt);   break;
    case ACEL_FS_8G:  str_copia_zf("+/- 8g",vt);   break;
    case ACEL_FS_16G: str_copia_zf("+/- 16g",vt);  break;
    default:          str_copia_zf("??????g",vt);  break;
  }
}

// Escala Giro (Fundo de Escala) - Retorna string com a escala
// Precisa de espaço de 14 caracteres incluindo o zero final
void str_fs_giro(byte esc, char *vt){
  switch(esc){
    case GIRO_FS_250:   str_copia_zf("+/- 250 gr/s",vt); break;
    case GIRO_FS_500:   str_copia_zf("+/- 500 gr/s",vt); break;
    case GIRO_FS_1000:  str_copia_zf("+/- 1000 gr/s",vt); break;
    case GIRO_FS_2000:  str_copia_zf("+/- 2000 gr/s",vt); break;
    default:            str_copia_zf("???????? gr/s",vt); break;
  }
}

// Faz cópia de uma string e retorna tamanho string copiada, incluindo o zero final
// Nome criado para não confundir com strcpy()
byte str_copia_zf(byte *ft, byte *dest){
  byte i=0;
  while(ft[i] != '\0'){
    dest[i]=ft[i];
    i++;
  }
  dest[i]=ft[i];
  i++;
  return i;
}


// Faz cópia de uma string e retorna tamanho string copiada, não conta o zero
// Nome criado para não confundir com strcpy()
byte str_copia(byte *ft, byte *dest){
  byte i=0;
  while(ft[i] != '\0'){
    dest[i]=ft[i];
    i++;
  }
  return i;
}

// Retorna tamanho string, não conta o zero
byte str_tam(byte *ft){
  byte iy=0;
  while(ft[iy] != '\0') iy++;
  return iy;
}

// Remove zeros à esquerda, sem sinal
void str_rmvz_u(char *msg){
  int i,j;
  j=0;
  while(msg[j] == '0')  j++;  //Onde acabam os zeros
  i=0;
  while(msg[j] != '\0') msg[i++]=msg[j++];
  msg[i]=msg[j];
  if (msg[0]=='\0'){
    msg[0]='0'; //Só tem zeros?
    msg[1]='\0';
  }
}

// Remove zeros à esquerda, com sinal
void str_rmvz_s(char *msg){
  int i,j;
  j=1;
  while(msg[j] == '0')  j++;  //Onde acabam os zeros
  i=1;
  while(msg[j] != '\0') msg[i++]=msg[j++];
  msg[i]=msg[j];
  if (msg[1]=='\0') msg[0]='0'; //Só tem zeros?
}

////////////////////////////////////////////////////
/////////// float (double) /////////////////////////
////////////////////////////////////////////////////

// Imprimir float = + xxx xxx xxx , ddd ddd ddd ddd (usar char msg[24])
// Não imprime zeros à esquerda, exceto para 0.xxxx
//  9 posições = limite da parte inteira
// 12 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
// No Arduino, double e float têm a mesma precisão
void str_float(float f, byte prec, char *msg){
  byte i,ix,aux;
  float fx;
  long dv=1;
  if ((abs(f) >= 1E9) || (prec>12)){  //Ultrapassou limite?
    for (i=0; i<7; i++)
      msg[i]='#';
    msg[3]='.';
    msg[7]='\0'; 
  }
  else{
    ix=0;
    //Imprimir sinal
    if (f<0)  msg[ix++]='-';
    else      msg[ix++]='+';
    //Tamanho da parte inteira
    fx=abs(f);
    while( (fx-dv) >0){
      dv*=10;
    }
    dv/=10;
    
    //Imprimir parte inteira
    while(fx>=1){
      aux=fx/dv;
      msg[ix++]=aux+0x30;
      fx=fx-(aux*dv);
      dv=dv/10;
    }
    //Caso com zeros, ex: 10,1
    while(dv!=0){
      msg[ix++]='0';
      dv = dv/10;
    }
    //Caso +.123 (falta um zero)
    if (ix==1)  msg[ix++]='0';
    //msg[ix++]=',';    //Vírgula
    msg[ix++]='.';      //Ponto
    
    //Parte fracionária
    for(i=0; i<prec; i++){
      fx*=10;
      aux=fx;
      msg[ix++]=aux+0x30;
      fx=fx-aux;
    }
    msg[ix]='\0';
  }
}

////////////////////////////////////////////////////
//////////////// 32 bits ///////////////////////////
////////////////////////////////////////////////////

// Transforma uma string em long
long str_to_dec32(char *vt){
  long nr=0,sinal;
  char c,i=0;
  if (vt[i]=='+') {sinal=1;   i++;}
  if (vt[i]=='-') {sinal=-1;  i++;}
  while(vt[i] != '\0')
    nr = 10*nr+vt[i++]-0x30;    
  return sinal*nr;
}

// dec32 - Decinal 32 bits com sinal e com zeros à esquerda
// msg = +4 294 967 295 \0 - 12 posições
void str_dec32(long c, char *msg){
  byte i,aux;
  long dv=1000000000L;
  unsigned long x;
  if (c<0)  msg[0]='-';
  else      msg[0]='+';
  x=abs(c);
  for (i=1; i<10; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// dec32u - Decinal 8 bits sem sinal e com zeros à esquerda
// msg = 4 294 967 295 \0 - 11 posições
void str_dec32u(long c, char *msg){
  byte i,aux;
  unsigned long x,dv=1000000000L;
  x=c;
  for (i=0; i<9; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// Gerar string hexa 32 bits sem sinal
// msg = xxxx xxxx \0 - 9 posições
void str_hex32(long c, char *msg){
    byte i,aux;
    for (i=8; i>0; i--){
        aux=c&0xF;
        if (aux>9)  msg[i-1]=aux+0x37;
        else        msg[i-1]=aux+0x30;
        c = c>>4;
    }
    msg[8]='\0';
}

////////////////////////////////////////////////////
//////////////// 16 bits ///////////////////////////
////////////////////////////////////////////////////

// dec16 - Decinal 16 bits com sinal e zeros à esquerda
// msg = +12345\0 - 7 posições
void str_dec16(int c, char *msg){
  byte i;
  word x,aux,dv=10000;
  if (c<0)  msg[0]='-';
  else      msg[0]='+';
  x=abs(c);
  for (i=1; i<5; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// dec16u - Decinal 8 bits sem sinal e zeros à esquerda
// msg = 65535\0 - 6 posições
void str_dec16u(word c, char *msg){
  byte i;
  word x,aux,dv=10000;
  x=c;
  for (i=0; i<4; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// Gerar string hexa 16 bits sem sinal
void str_hex16(word c, char *msg){
    byte i,aux;
    for (i=4; i>0; i--){
        aux=c&0xF;
        if (aux>9)  msg[i-1]=aux+0x37;
        else        msg[i-1]=aux+0x30;
        c = c>>4;
    }
    msg[4]='\0';
}

////////////////////////////////////////////////////
///////////////// 8 bits ///////////////////////////
////////////////////////////////////////////////////

// dec8 - Decinal 8 bits com sinal e zeros à esquerda
// msg = +123\0 - 5 posições
void str_dec8(char c, char *msg){
  byte i,x,aux,dv=100;
  if (c<0)  msg[0]='-';
  else      msg[0]='+';
  x=abs(c);
  for (i=1; i<3; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// dec8u - Decinal 8 bits sem sinal e zeros à esquerda
// msg = 123\0 - 4 posições
void str_dec8u(byte c, char *msg){
  byte i,x,aux,dv=100;
  x=c;
  for (i=0; i<2; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// Gerar string hexa 8 bits sem sinal
void str_hex8(byte c, char *msg){
    byte i,aux;
    for (i=2; i>0; i--){
        aux=c&0xF;
        if (aux>9)  msg[i-1]=aux+0x37;
        else        msg[i-1]=aux+0x30;
        c = c>>4;
    }
    msg[2]='\0';
}

// Gerar string com qtd de espaços = 0x20 (Espaço em Branco)
void str_spc(char qtd, char *msg){
    word i;
    for(i=0; i<qtd; i++)
        msg[i]=' ';   // ' ' = 0x20
        msg[i]='\0';
}

// Gerar string com qtd CRLF = 0xD 0xA (conta os pares)
void str_crlf(char qtd, char *msg){
    word i=0;
    while(i<2*qtd){
        msg[i++]='\r';  // \r = Carriage Return
        msg[i++]='\n';  // \n = New Line
    }
    msg[i]='\0';
}

// Gerar string com qtd CR = 0xD (Carriage Return)
void str_cr(char qtd, char *msg){
    word i;
    for(i=0; i<qtd; i++)
        msg[i]='\r';   // \r = Carriage Return
    msg[i]='\0';
}

// Gerar string com qtd LF = 0xA (Line Feed)
void str_lf(char qtd, char *msg){
    word i;
    for(i=0; i<qtd; i++)
        msg[i]='\n';   // \r = Carriage Return
    msg[i]='\0';
}

// Converter ASCII em nibble
byte asc_nib(byte asc){
  if (asc<'A')  return asc-0x30;
  else          return asc-0x37;
}
