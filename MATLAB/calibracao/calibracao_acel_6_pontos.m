% Realiza uma calibração simples do acelerometro.

% Coleta medidas com a caixa preta rotacionada de 6 formas:
%
%   eixo X para cima
%   eixo X para baixo
%   eixo Y para cima
%   eixo Y para baixo
%   eixo Z para cima
%   eixo Z para baixo
% 
% A ideia é que a médias das medidas coletadas em posições opostas deve dar
% o ponto zero
% E a diferença entre as medidas de eixos opostos devem dar 2g
% Com esses dois dados é possível obter o offset e a escala para a
% calibragem
% Utiliza o teste 17.
% escreve em calib_acel.txt
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
% sid=serial('COM3','Baudrate',115200);
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

msgs = ["Z para cima"
        "Y para cima"
        "Y para baixo"
        "X para baixo"
        "X para cima"
        "Z para baixo"];

ix=1;

for j = 1:6
    x1=0;
    x2=0;
    
    %print da instrução de calibração
    fprintf(1,"%s\n",msgs(j))
    
    %procura o inicio dos dados
    while x1~='[' || x2~='m'
        x1=x2;
        x2=fread(sid,1);
    end
    ch=fread(sid,1);
%     fprintf(1,"%c\n",ch);

    uax=0;  %último ax
    uay=0;  %último ay
    uaz=0;  %último az
    ch = 0;
    for i = 1:(qtd_amostras)
 
        %endereco e indice
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

%termina transmição
fprintf(sid,'x\r\n');
fclose(sid);

%pega a média dos dados de cada posição
azplus  = [ mean(ax(1:qtd_amostras)) mean(ay(1:qtd_amostras)) mean(az(1:qtd_amostras))];
ayplus  = [ mean(ax(qtd_amostras+1:2*qtd_amostras)) mean(ay(qtd_amostras+1:2*qtd_amostras)) mean(az(qtd_amostras+1:2*qtd_amostras))];
ayminus = [ mean(ax(2*qtd_amostras+1:3*qtd_amostras)) mean(ay(2*qtd_amostras+1:3*qtd_amostras)) mean(az(2*qtd_amostras+1:3*qtd_amostras))];
axminus = [ mean(ax(3*qtd_amostras+1:4*qtd_amostras)) mean(ay(3*qtd_amostras+1:4*qtd_amostras)) mean(az(3*qtd_amostras+1:4*qtd_amostras))];
axplus  = [ mean(ax(4*qtd_amostras+1:5*qtd_amostras)) mean(ay(4*qtd_amostras+1:5*qtd_amostras)) mean(az(4*qtd_amostras+1:5*qtd_amostras))];
azminus = [ mean(ax(5*qtd_amostras+1:6*qtd_amostras)) mean(ay(5*qtd_amostras+1:6*qtd_amostras)) mean(az(5*qtd_amostras+1:6*qtd_amostras))];

samples=[axplus;ayplus;ayminus;azminus;azplus;axminus];



%dados iniciais
accel_offset=[0 0 0];
accel_scale=[1 1 1];
GRAVITY=9.80665;

%Realiza a calibração
% [accel_offset, accel_scale] = calib_accel_6_points(axplus, axminus, ayplus, ayminus, azplus, azminus);
[accel_offset, accel_scale] = calib_accel_6_points(axplus, axminus, ayplus, ayminus, azplus, azminus);

% Escreve no arquivo os dados de calibração
fileID  = fopen([directory '\calib_acel.txt'],'w');
fprintf(fileID , '%f\n', accel_offset(1));
fprintf(fileID , '%f\n', accel_offset(2));
fprintf(fileID , '%f\n', accel_offset(3));
fprintf(fileID , '%f\n', accel_scale(1));
fprintf(fileID , '%f\n', accel_scale(2));
fprintf(fileID , '%f\n', accel_scale(3));
fclose(fileID);


function [accel_offset, accel_scale] = calib_accel_6_points(axplus,axminus,ayplus,ayminus,azplus,azminus)
    accel_offset(1)=(axplus(1)+axminus(1))/2;
    accel_offset(2)=(ayplus(2)+ayminus(2))/2;
    accel_offset(3)=(azplus(3)+azminus(3))/2;

    accel_scale(1)=1/((axplus(1)-axminus(1))/2);
    accel_scale(2)=1/((ayplus(2)-ayminus(2))/2);
    accel_scale(3)=1/((azplus(3)-azminus(3))/2);
end