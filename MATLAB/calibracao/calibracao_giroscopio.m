% Realiza a calibração do Giroscópio
% Utiliza o teste 12 para coletar os dados
% Apenas calcula médias e escreve num arquivo
% Escreve no arquivo calib_giro.txt
% O arquivo mil_leituras.m mostra como ler e converter os dados de calibragem

% limpa tela e variáveis
close all;                          
clear;                              
clc;                                

% diretório do script
if(~isdeployed)
  cd(fileparts(which(mfilename)));
end
directory = pwd;

fprintf(1,'Teste 12\n');
fprintf(1,'O Matlab recebe dados da da Caixa Preta.\n');
fprintf(1,'Deixe o sensor parado por alguns segundos e a calibração será feita automaticamente.\n');
fprintf(1,'Em caso de erro, a porta serial pode estar travada.\n');
fprintf(1,'Neste caso use "fclose(instrfind)" para fechar porta serial.\n');


%Parâmetros
fa=100;         %Frequência de amostragem em Hz
ta=1/fa;        %Intervalo entre amostras (periodo)

%Escalas
esc_ac=2;
esc_giro=250;

%Amostras
qtd_amostras = 1000;

%Abre porta serial (não pode estar aberta na IDE do arduino
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

%acessa o teste 12 mandando o cï¿½digo pelo serial
fprintf(sid,'t12\r\n');

while x1~='[' || x2~='m'
    x1=x2;
    x2=fread(sid,1);
end

fprintf(1,'\nIniciando recepção de dados...\n');
pause(1);

t=fread(sid,1);
t=fscanf(sid,'%d');
t=fscanf(sid,'%f');

while t~=55555
    t=fscanf(sid,'%d');
end

uax=0;  %último ax
uay=0;  %último ay
uaz=0;  %último az

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
     
    %endereço e indice
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
fprintf(1,'\nTeminou recepção de dados.\n');
fprintf(sid,'x\r\n');
fclose(sid);
fprintf(1,'Recebidas %d leituras por eixo.\n',ix);
fprintf(1,'Duração %.2f segundos.\n',ix/fa);
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


% Offset de calibração do giro
gx_offset = mean(gx);
gy_offset = mean(gy);
gz_offset = mean(gz);

% Escreve no arquivo os dados de calibração
fileID  = fopen([directory '\calib_giro.txt'],'w');
fprintf(fileID , '%f\n', gx_offset);
fprintf(fileID , '%f\n', gy_offset);
fprintf(fileID , '%f\n', gz_offset);
fclose(fileID);

fprintf(1,"Pronto!\n")