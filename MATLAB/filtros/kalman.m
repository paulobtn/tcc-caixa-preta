% Aplica o filtro de kalman a partir dos dados gerados pelo le_e_grava.m

% Limpa dados
close all;                          
clear;                              
clc;     

% diretório do script
if(~isdeployed)
  cd(fileparts(which(mfilename)));
end
directory = pwd;

fprintf(1,'Os dados serão lidos de leituras.txt\n');

%Parâmetros
fa=100;         %Frequência de amostragem em Hz
ta=1/fa;        %Intervalo entre amostras (periodo)

%Escalas
esc_giro = 250/32767; % transformar giro em º/s
esc_mag  = 4912.0/32760.0; % transformar mag em uT
esc_ac   = 9.80665; %transformar acel em m/s2

%Lê do arquivo
fid  = fopen([directory '\leituras.txt'],'r');
leituras=fscanf(fid,'%f');
fclose(fid);
qtd_leituras = size(leituras,1)/9;

%coloca as leituras nas variáveis
ax = zeros(qtd_leituras,1);
ay = zeros(qtd_leituras,1);
az = zeros(qtd_leituras,1);

gx = zeros(qtd_leituras,1);
gy = zeros(qtd_leituras,1);
gz = zeros(qtd_leituras,1);

hx = zeros(qtd_leituras,1);
hy = zeros(qtd_leituras,1);
hz = zeros(qtd_leituras,1);

for i=1:qtd_leituras
    ax(i) = leituras(1+9*(i-1));
    ay(i) = leituras(2+9*(i-1));
    az(i) = leituras(3+9*(i-1));

    gx(i) = leituras(4+9*(i-1));
    gy(i) = leituras(5+9*(i-1));
    gz(i) = leituras(6+9*(i-1));

    hx(i) = leituras(7+9*(i-1));
    hy(i) = leituras(8+9*(i-1));
    hz(i) = leituras(9+9*(i-1));
end

% Calibra dados e atribui escalas 

% Acelerometro m/s²
fid  = fopen([directory '\calib_acel.txt'],'r');
calibAccel=fscanf(fid,'%f');
fclose(fid);
acc = [ 
         (ax-calibAccel(1))*calibAccel(4)*esc_ac ...
         (ay-calibAccel(2))*calibAccel(5)*esc_ac ...
         (az-calibAccel(3))*calibAccel(6)*esc_ac ...
         ];

% Giroscopio º/s
fid  = fopen([directory '\calib_giro.txt'],'r');
calibGiro=fscanf(fid,'%f');
fclose(fid);
gyro=[ ...
    (gx-calibGiro(1))*esc_giro ...
    (gy-calibGiro(2))*esc_giro ...
    (gz-calibGiro(3))*esc_giro ...
];

% Magnetômetro uT
fid = fopen([directory '\calib_mag.txt'],'r');
calibMag=fscanf(fid,'%f');
fclose(fid);
h_off = calibMag(1:3)';
h_sc  = reshape(calibMag(4:12),3,3);
magData=[hx hy hz];
mag= (magData-h_off)*h_sc*esc_mag;

% Configura eixo X
% intervalo = 0.01; %10ms
% eixoX = 0:length(ax)-1;
% eixoX = eixoX * intervalo;

%inverter os eixos

%Acelerometro, giro e mag
g = gyro(:,1);
gyro(:,1)=gyro(:,2);
gyro(:,2)=g;

a = acc(:,1);
acc(:,1)=acc(:,2);
acc(:,2)=a;

acc(:,1)=acc(:,1)*-1;
acc(:,2)=acc(:,2)*-1;

gyro(:,3) = gyro(:,3)*-1;
% gyro(:,1)=gyro(:,1)*-1;
% gyro(:,2)=gyro(:,2)*-1;

% Cria o filtro

GyroscopeNoiseMPU9250 = 3.0462e-06; % GyroscopeNoise (variance) in units of rad/s
AccelerometerNoiseMPU9250 = 0.0061;
viewer = HelperOrientationViewer('Title',{'IMU Filter'});
FUSE = imufilter('SampleRate',100, 'GyroscopeNoise',GyroscopeNoiseMPU9250,'AccelerometerNoise', AccelerometerNoiseMPU9250);
for ii=1:size(acc,1)
    rotators = FUSE(acc(ii,:), gyro(ii,:));
    for j = numel(rotators)
        viewer(rotators(j));
    end
    pause(0.01);
end





