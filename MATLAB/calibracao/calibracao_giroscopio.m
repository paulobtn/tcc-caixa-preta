% Realiza a calibra��o do Girosc�pio
% Utiliza o teste 12 para coletar os dados
% Apenas calcula m�dias e escreve num arquivo
% Escreve no arquivo calib_giro.txt
% O arquivo mil_leituras.m mostra como ler e converter os dados de calibragem

% limpa tela e vari�veis
close all;                          
clear;                              
clc;                                

% diret�rio do script
if(~isdeployed)
  cd(fileparts(which(mfilename)));
end
directory = pwd;

fprintf(1,'Teste 12\n');
fprintf(1,'O Matlab recebe dados da da Caixa Preta.\n');
fprintf(1,'Deixe o sensor parado por alguns segundos e a calibra��o ser� feita automaticamente.\n');
fprintf(1,'Em caso de erro, a porta serial pode estar travada.\n');
fprintf(1,'Neste caso use "fclose(instrfind)" para fechar porta serial.\n');


%Par�metros
fa=100;         %Frequ�ncia de amostragem em Hz
ta=1/fa;        %Intervalo entre amostras (periodo)

%Escalas
esc_ac=2;
esc_giro=250;

%Amostras
qtd_amostras = 1000;

%Abre porta serial (n�o pode estar aberta na IDE do arduino
sid=serial('COM6','Baudrate',115200);
% sid=serial('/dev/ttyACM0','Baudrate',115200);

fopen(sid);
if (sid==-1)
    fprintf(1,'Nao abriu COM6.\n');
%     fprintf(1,'Nao abriu /dev/ttyACM0.\n');
    return;
end

x1=0;
x2=0;

gx = zeros(1000,1);
gy = gx;
gz = gx;

pause(2);

%acessa o teste 12 mandando o c�digo pelo serial
fprintf(sid,'t12\r\n');

while x1~='[' || x2~='m'
    x1=x2;
    x2=fread(sid,1);
end

fprintf(1,'\nIniciando recep��o de dados...\n');
pause(1);

t=fread(sid,1);
t=fscanf(sid,'%d');
t=fscanf(sid,'%f');

while t~=55555
    t=fscanf(sid,'%d');
end

uax=0;  %�ltimo ax
uay=0;  %�ltimo ay
uaz=0;  %�ltimo az

ugx=0;
ugy=0;
ugz=0;

uhx=0;
uhy=0;
uhz=0;
ix=1;

ch = 0;
% while uax~=22222 || uay~=22222

cnt = 10;
for i = 1:qtd_amostras
    
    if mod(i,100) == 0
       fprintf(1,'%d\n',cnt);
        cnt=cnt-1; 
    end
     
    %endere�o e indice
    ch = fscanf(sid,'%d');
    ch = fscanf(sid,'%d');
  
    %aceleracao
    ch=fscanf(sid,'%d');
    ch=fscanf(sid,'%d');
    ch=fscanf(sid,'%d');
      
    %giro   
    ugx = fscanf(sid,'%d');
    ugy = fscanf(sid,'%d');
    ugz = fscanf(sid,'%d');
    
    %campo magnetico
    ch = fscanf(sid,'%d');
    ch = fscanf(sid,'%d');
    ch = fscanf(sid,'%d');
    
    gx(ix)=ugx;
    gy(ix)=ugy;
    gz(ix)=ugz;
    
    ix=ix+1;
end

ix=ix-1;
fprintf(1,'\nTeminou recep��o de dados.\n');
fprintf(sid,'x\r\n');
fclose(sid);
fprintf(1,'Recebidas %d leituras por eixo.\n',ix);
fprintf(1,'Dura��o %.2f segundos.\n',ix/fa);
%close all;

total_leituras = size(gx,1);
fprintf('Total de leituras: %d\n', total_leituras);

% Converter giros em "graus/seg"
% gx=esc_giro*(gx/32767);
% gy=esc_giro*(gy/32767);
% gz=esc_giro*(gz/32767);

intervalo = 0.01; %10ms
eixoX = 0:length(gx)-1;
eixoX = eixoX * intervalo;


% Offset de calibra��o do giro
gx_offset = mean(gx);
gy_offset = mean(gy);
gz_offset = mean(gz);

% Escreve no arquivo os dados de calibra��o
fileID  = fopen([directory '\calib_giro.txt'],'w');
fprintf(fileID , '%f\n', gx_offset);
fprintf(fileID , '%f\n', gy_offset);
fprintf(fileID , '%f\n', gz_offset);
fclose(fileID);

fprintf(1,"Pronto!\n")