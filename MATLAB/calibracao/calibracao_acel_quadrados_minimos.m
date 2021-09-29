% Realiza a calibração do acelerometro.
% Mais complexa que a calibração de 6 pontos
% Tenta aproximar o módulo do acelerômetro para 1 usando
% um problema de mínimos quadrados não linear
% Utiliza o teste 17.
% Escreve no arquivo calib_acel.txt
% O arquivo mil_leituras.m mostra como ler e converter os dados de calibragem

% Limpa tela e apaga variáveis
close all;                          
clear;                              
clc;                               

% Diretório do script
if(~isdeployed)
  cd(fileparts(which(mfilename)));
end
directory = pwd;

fprintf(1,'Teste 17\n');
fprintf(1,'O Matlab recebe dados da da Caixa Preta.\n');
fprintf(1,'Siga as instruções para a realização da calibração.\n');
fprintf(1,'Em caso de erro, a porta serial pode estar travada.\n');
fprintf(1,'Neste caso use "fclose(instrfind)" para fechar porta serial.\n');

%Parâmetros
fa=100;         %Freq de amostragem em Hz
ta=1/fa;        %Intervalo entre amostras (periodo)

%Amostras
qtd_amostras = 50;

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

ax = zeros(qtd_amostras*6,1);
ay=ax;
az=ax;

pause(2);

%acessa o teste 17 mandando o codigo pelo serial
fprintf(sid,'t17\r\n');

fprintf(1,'\nIniciando recepção de dados\n');
pause(1);

ix=1;

msgs = ["X para cima"
        "X a 45º"
        "X para baixo"
        "Y para cima"
        "Y a 45º"
        "Y para baixo"
        "Z para cima"
        "Z a 45º"
        "Z para baixo"
        ];

for j = 1:9
    x1=0;
    x2=0;
    
    %print da instrução de calibração
    fprintf(1,"%s\n",msgs(j))
    
    %Procura início dos dados
    while x1~='[' || x2~='m'
        x1=x2;
        x2=fread(sid,1);
    end
    ch=fread(sid,1);
    
    uax=0;  %último ax
    uay=0;  %último ay
    uaz=0;  %último az

    ch = 0;

    for i = 1:(qtd_amostras)
        ch = fscanf(sid,'%d');
        fprintf(1,'%d\n',ch);
        ch = fscanf(sid,'%d');
        fprintf(1,'%d\n',ch);
        
        %aceleracao
        uax=fscanf(sid,'%d');
        uay=fscanf(sid,'%d');
        uaz =fscanf(sid,'%d');
        fprintf(1,'%d\n',uax);
        fprintf(1,'%d\n',uay);
        fprintf(1,'%d\n',uaz);

        ax(ix,1)=uax;
        ay(ix,1)=uay;
        az(ix,1)=uaz;
        
        ix=ix+1;
    end
end

ix=ix-1;
fprintf(1,'\nTerminou recepção de dados.\n');

%finaliza transmissão
fprintf(sid,'x\r\n');
fclose(sid);

%pega a média dos dados de cada posição (TODO: atualizar isso pra um for)
axplus    = [ mean(ax(1:qtd_amostras)) mean(ay(1:qtd_amostras)) mean(az(1:qtd_amostras))];
ax45      = [ mean(ax(1*qtd_amostras+1:2*qtd_amostras)) mean(ay(1*qtd_amostras+1:2*qtd_amostras)) mean(az(1*qtd_amostras+1:2*qtd_amostras))];
axminus   = [ mean(ax(2*qtd_amostras+1:3*qtd_amostras)) mean(ay(2*qtd_amostras+1:3*qtd_amostras)) mean(az(2*qtd_amostras+1:3*qtd_amostras))];
ayplus    = [ mean(ax(3*qtd_amostras+1:4*qtd_amostras)) mean(ay(3*qtd_amostras+1:4*qtd_amostras)) mean(az(3*qtd_amostras+1:4*qtd_amostras))];
ay45      = [ mean(ax(4*qtd_amostras+1:5*qtd_amostras)) mean(ay(4*qtd_amostras+1:5*qtd_amostras)) mean(az(4*qtd_amostras+1:5*qtd_amostras))];
ayminus   = [ mean(ax(5*qtd_amostras+1:6*qtd_amostras)) mean(ay(5*qtd_amostras+1:6*qtd_amostras)) mean(az(5*qtd_amostras+1:6*qtd_amostras))];
azplus    = [ mean(ax(6*qtd_amostras+1:7*qtd_amostras)) mean(ay(6*qtd_amostras+1:7*qtd_amostras)) mean(az(6*qtd_amostras+1:7*qtd_amostras))];
az45      = [ mean(ax(7*qtd_amostras+1:8*qtd_amostras)) mean(ay(7*qtd_amostras+1:8*qtd_amostras)) mean(az(7*qtd_amostras+1:8*qtd_amostras))];
azminus   = [ mean(ax(8*qtd_amostras+1:9*qtd_amostras)) mean(ay(8*qtd_amostras+1:9*qtd_amostras)) mean(az(8*qtd_amostras+1:9*qtd_amostras))];

% samples=[axplus;ax45;axminus;ax45minus;ayplus;ay45;ayminus;ay45minus;azplus;az45;azminus;az45minus];
samples=[axplus;ax45;axminus;ayplus;ay45;ayminus;azplus;az45;azminus];

% Valores inicial para calibração de offsetx, offsety, offsetz, scalex, scaley, scalez
x0 = [0 0 0 1 1 1];

fh=@(x)leastSquareFun(x,samples);
[x,resnorm] = lsqnonlin(fh,x0);

accel_offset(1)=x(1);
accel_offset(2)=x(2);
accel_offset(3)=x(3);

accel_scale(1)=1/x(4);
accel_scale(2)=1/x(5);
accel_scale(3)=1/x(6);

% Escreve no arquivo os dados de calibração
fileID  = fopen([directory '\calib_acel.txt'],'w');
fprintf(fileID , '%f\n', accel_offset(1));
fprintf(fileID , '%f\n', accel_offset(2));
fprintf(fileID , '%f\n', accel_offset(3));
fprintf(fileID , '%f\n', accel_scale(1));
fprintf(fileID , '%f\n', accel_scale(2));
fprintf(fileID , '%f\n', accel_scale(3));
fclose(fileID);

function F = leastSquareFun(x,samples)
k = 1:9;
F = ((samples(k,1)-x(1))/x(4)).^2 + ((samples(k,2)-x(2))/x(5)).^2 + ((samples(k,3)-x(3))/x(6)).^2 -1;
end
