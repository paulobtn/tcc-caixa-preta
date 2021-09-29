% Faz 1000 leituras de aceleração, giro e magnetometro plotando gráficos
% com e sem calibração
% Utiliza o teste 12 para coletar os dados
% Exemplifica como ler os arquivos de calibragem e converter os dados

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
fprintf(1,'O Matlab recebe 1000 dados da da Caixa Preta.\n');
fprintf(1,'Em caso de erro, a porta serial pode estar travada.\n');
fprintf(1,'Neste caso use "fclose(instrfind)" para fechar porta serial.\n');

%Parâmetros
fa=100;         %Frequï¿½ncia de amostragem em Hz
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

%ativa o teste 12
fprintf(sid,'t12\r\n');

fprintf(1,'\nIniciou a recepção de dados...\n');

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
%     fprintf(1,'%d\n',uax);
%     fprintf(1,'%d\n',uay);
%     fprintf(1,'%d\n',uaz);
      
    %giro   
    ugx = fscanf(sid,'%d');
    ugy = fscanf(sid,'%d');
    ugz = fscanf(sid,'%d');
%     fprintf(1,'%d\n',ugx);
%     fprintf(1,'%d\n',ugy);
%     fprintf(1,'%d\n',ugz);
    
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

%Pega os dados de calibracao
%Acelerometro
fid  = fopen([directory '\calib_acel.txt'],'r');
calibAccel=fscanf(fid,'%f');
fclose(fid);

%test
% calibAccel(1)=(16511 - 16140)/2;
% calibAccel(2)=(16591-16122)/2;
% calibAccel(3)=(18640-14138)/2;
% calibAccel(4)=1/((16511+16140)/2);
% calibAccel(5)=1/((16591+16122)/2);
% calibAccel(6)=1/((18640+14138)/2);

axc=(ax-calibAccel(1))*calibAccel(4);
ayc=(ay-calibAccel(2))*calibAccel(5);
azc=(az-calibAccel(3))*calibAccel(6);

%giroscopio
fid  = fopen([directory '\calib_giro.txt'],'r');
calibGiro=fscanf(fid,'%f');
fclose(fid);
gxc=(gx-calibGiro(1))*esc_giro/32767;
gyc=(gy-calibGiro(2))*esc_giro/32767;
gzc=(gz-calibGiro(3))*esc_giro/32767;

%magnetometro
fid = fopen([directory '\calib_mag.txt'],'r');
calibMag=fscanf(fid,'%f');
fclose(fid);
h_off = calibMag(1:3)';
h_sc  = reshape(calibMag(4:12),3,3);
magData=[hx hy hz];
magDataCorrected= (magData-h_off)*h_sc*4912.0/32760.0;

% Converter giros descalibrados em "graus/seg"
gx=esc_giro*(gx/32767);
gy=esc_giro*(gy/32767);
gz=esc_giro*(gz/32767);

% Converter aceleracoes descalibradas em "g"
ax=esc_ac*(ax/32767);
ay=esc_ac*(ay/32767);
az=esc_ac*(az/32767);

% Converter campo magnético descalibrado em "uT"
magData=magData*4912.0/32760.0;

% configura eixo do gráfico
intervalo = 0.01; %10ms
eixoX = 0:length(ax)-1;
eixoX = eixoX * intervalo;

% Plota dados do giroscopio antes de calibrar
figure('Name', 'Velocidade angular antes de calibrar');
hold on;
plot(eixoX, gx, 'r');
plot(eixoX, gy, 'g');
plot(eixoX, gz, 'b');
grid;
title('Velocidade angular antes de calibrar');
xlabel('Tempo (s)');
ylabel('Velocidade angular (°/s)');
legend('\phi', '\theta', '\psi');
hold off;

% Plota os dados do giro depois de calibrar
figure('Name', 'Velocidade angular depois de calibrar');
hold on;
plot(eixoX, gxc, 'r');
plot(eixoX, gyc, 'g');
plot(eixoX, gzc, 'b');
grid;
title('Velocidade angular depois de calibrar');
xlabel('Tempo (s)');
ylabel('Velocidade angular (ï¿½/s)');
legend('\phi', '\theta', '\psi');
hold off;

% Plota dados do acelerometro antes de calibrar
figure('Name', 'Acelerometro antes de calibrar');
hold on;
plot(eixoX, ax, 'r');
plot(eixoX, ay, 'g');
plot(eixoX, az, 'b');
grid;
title('Acel antes de calibrar');
xlabel('Tempo (s)');
ylabel('Aceleração(g)');
legend('x', 'y', 'z');
hold off;

% Plota dados do acelerometro depois de calibrar
figure('Name', 'Acelerometro depois de calibrar');
hold on;
plot(eixoX, axc, 'r');
plot(eixoX, ayc, 'g');
plot(eixoX, azc, 'b');
grid;
title('Acel depois de calibrar');
xlabel('Tempo (s)');
ylabel('Aceleracao');
legend('x', 'y', 'z');
hold off;

% Plota os dados do magnetometro antes de calibrar
% magData=[hx hy hz];
% magDataCorrected= (magData-h_off)*h_sc*4912.0/32760.0;
figure('Name', 'Magnetômetro antes de calibrar');
hold on;
plot(eixoX, magData(:,1), 'r');
plot(eixoX, magData(:,2), 'g');
plot(eixoX, magData(:,3), 'b');
grid;
title('Magnetômetro antes de calibrar');
xlabel('Tempo (s)');
ylabel('Campo magnético uT');
legend('x', 'y', 'z');
hold off;

% Plota os dados do magnetometro depois de calibrar
figure('Name', 'Magnetômetro depois de calibrar');
hold on;
plot(eixoX, magDataCorrected(:,1), 'r');
plot(eixoX, magDataCorrected(:,2), 'g');
plot(eixoX, magDataCorrected(:,3), 'b');
grid;
title('Magnetômetro depois de calibrar');
xlabel('Tempo (s)');
ylabel('Campo magnético uT');
legend('x', 'y', 'z');
hold off;