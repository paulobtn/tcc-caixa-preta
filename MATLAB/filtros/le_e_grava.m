% Faz n leituras de aceleração, giro e magnetometro plotando gráficos,
% e escreve no arquivo leituras.txt
% Formato:
% axn
% ayn
% azn
% gxn
% gyn
% gzn
% hxn
% hyn
% hzn
% ...

% São os dados puros dos sensores, devem ser calibrados e convertidos para
% a escala correta em outro script
% Utiliza o teste 12 para coletar os dados
% Exemplifica como ler os arquivos de calibragem e converter os dados
% Altere a quantidade de leituras alterando a variavel qtd_amostras

% Limpa dados
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
fprintf(1,'Em caso de erro, a porta serial pode estar travada.\n');
fprintf(1,'Neste caso use "fclose(instrfind)" para fechar porta serial.\n');

%Parâmetros
fa=100;         %Frequï¿½ncia de amostragem em Hz
ta=1/fa;        %Intervalo entre amostras (periodo)

%Escalas
esc_ac=2;
esc_giro=250;

%Amostras
qtd_amostras = 9000;

%teste 1
%20 segundos apontado para oeste
%20 segundos para o norte
%20 segundos apontando para leste
%20 segundos apontando para oeste
%20 segundos apontando para norte
%20 segundos apontando por oeste

%teste 2
%rodar aleatoriamente por 20 seg
%apontar pra norte por 20 seg
%rodar aleatoriamente por 20 seg
%apontar pra norte por 20 seg
%rodar aleatoriamente por 20 seg
%apontar pra norte por 20 seg


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

ax = zeros(qtd_amostras,1);
ay=ax;
az=ax;

gx = zeros(qtd_amostras,1);
gy = gx;
gz = gx;

hx = zeros(qtd_amostras,1);
hy = hx;
hz = hx;

pause(2);

fprintf(1,'\nIniciando recepção de dados...\n');

%ativa o teste 12
fprintf(sid,'t12\r\n');

% Procura o início dos dados
while x1~='[' || x2~='m'
    x1=x2;
    x2=fread(sid,1);
end
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

cnt = uint16(qtd_amostras/fa);
for i = 1:qtd_amostras
    
    if mod(i,100) == 0
       fprintf(1,'%d\n',cnt);
       cnt=cnt-1; 
    end

    %endereço e indice
    ch = fscanf(sid,'%d');
    ch = fscanf(sid,'%d');
  
    %aceleracao
    uax=fscanf(sid,'%d');
    uay=fscanf(sid,'%d');
    uaz =fscanf(sid,'%d');
      
    %giro   
    ugx = fscanf(sid,'%d');
    ugy = fscanf(sid,'%d');
    ugz = fscanf(sid,'%d');
    
    %campo magnetico
    uhx = fscanf(sid,'%d');
    uhy = fscanf(sid,'%d');
    uhz = fscanf(sid,'%d');
     
    ax(ix)=uax;
    ay(ix)=uay;
    az(ix)=uaz;
    
    gx(ix)=ugx;
    gy(ix)=ugy;
    gz(ix)=ugz;
    
    hx(ix)=uhx;
    hy(ix)=uhy;
    hz(ix)=uhz;
    
    ix=ix+1;
end

ix=ix-1;
%Termina recepção
fprintf(1,'\nTeminou recepção de dados.\n');
fprintf(sid,'x\r\n');
fclose(sid);
fprintf(1,'Recebidas %d leituras por eixo.\n',ix);
fprintf(1,'Duração %.2f segundos.\n',ix/fa);

total_leituras = size(ax,1);
fprintf('Total de leituras: %d\n', total_leituras);

% escreve no arquivo
fileID  = fopen([directory '\leituras.txt'],'w');

for i = 1:total_leituras
    % Aceleração
    fprintf(fileID , '%d\n', ax(i));
    fprintf(fileID , '%d\n', ay(i));
    fprintf(fileID , '%d\n', az(i));
    
    % Giro
    fprintf(fileID , '%d\n', gx(i));
    fprintf(fileID , '%d\n', gy(i));
    fprintf(fileID , '%d\n', gz(i));
    
    % Campo magnético
    fprintf(fileID , '%d\n', hx(i));
    fprintf(fileID , '%d\n', hy(i));
    fprintf(fileID , '%d\n', hz(i));
end

fclose(fileID);

fprintf(1, "pronto!");
