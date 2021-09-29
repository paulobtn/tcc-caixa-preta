% Aplica o filtro de madgwick a partir dos dados gerados pelo le_e_grava.m

% Limpa dados
close all;                          
clear;                              
clc;     

% diretório do script
if(~isdeployed)
  cd(fileparts(which(mfilename)));
end
directory = pwd;

madgwickPath = [directory '\quaternion_library'];
addpath(madgwickPath);

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


% Calibra dados

% Acelerometro m/s²
fid  = fopen([directory '\calib_acel.txt'],'r');
calibAccel=fscanf(fid,'%f');
fclose(fid);
accel = [ 
         (ax-calibAccel(1))*calibAccel(4)*esc_ac ...
         (ay-calibAccel(2))*calibAccel(5)*esc_ac ...
         (az-calibAccel(3))*calibAccel(6)*esc_ac ...
         ];

% Giroscopio º/s
fid  = fopen([directory '\calib_giro.txt'],'r');
calibGiro=fscanf(fid,'%f');
fclose(fid);
% gyro=[ ...
%     (gx)*esc_giro ...
%     (gy)*esc_giro ...
%     (gz)*esc_giro ...
% ];
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
% mag= magData*esc_mag;

% Configura eixo X
intervalo = 0.01; %10ms
eixoX = 0:length(ax)-1;
eixoX = eixoX * intervalo;

%corrige eixos para NED eixo X do giro é N
% -Ax, Ay, Az, Gx, -Gy, -Gz, My, -Mx, Mz
% North along the accel +x-axis, East along the accel -y-axis, and Down along the accel -z-axis.
% accel(:,1)=accel(:,1)*-1;
% 
% gyro(:,2)=gyro(:,2)*-1;
% gyro(:,3)=gyro(:,3)*-1;
% 
% a = mag(:,1);
% mag(:,1)=mag(:,2);
% mag(:,2)=a*-1;

accel(:,1)=accel(:,1)*-1;
accel(:,2)=accel(:,2)*-1;

gyro(:,1)=gyro(:,1)*-1;
gyro(:,2)=gyro(:,2)*-1;

a = mag(:,1);
mag(:,1)=mag(:,2);
mag(:,2)=a;
mag(:,1)=mag(:,1)*-1;
mag(:,2)=mag(:,2)*-1;
mag(:,3)=mag(:,3)*-1;

% int16_t ax=-(Buf[0]<<8 | Buf[1]);
%   int16_t ay=-(Buf[2]<<8 | Buf[3]);
%   int16_t az=Buf[4]<<8 | Buf[5];
% 
%   // Gyroscope
%   int16_t gx=-(Buf[8]<<8 | Buf[9]);
%   int16_t gy=-(Buf[10]<<8 | Buf[11]);
%   int16_t gz=Buf[12]<<8 | Buf[13];
%   
% int16_t mx=-(Mag[3]<<8 | Mag[2]);
%   int16_t my=-(Mag[1]<<8 | Mag[0]);
%   int16_t mz=-(Mag[5]<<8 | Mag[4]);



% Cria o filtro
% mag = mag/100;
% filtro de madgwick (imu)
AHRS_IMU = MadgwickAHRS('SamplePeriod', 0.01, 'Beta', 0.1);
quaternion_imu = zeros(length(eixoX), 4);
for t = 1:length(eixoX)
    AHRS_IMU.UpdateIMU(gyro(t,:) * (pi/180), accel(t,:));	% as unidades do giroscopio devem ser em radiano
    quaternion_imu(t, :) = AHRS_IMU.Quaternion;
end

euler = quatern2euler(quaternConj(quaternion_imu)) * (180/pi);	% use conjugate for sensor frame relative to Earth and convert to degrees.
figure('Name', 'Angulos');
hold on;
plot(eixoX, euler(:,1), 'r');
grid;
plot(eixoX, euler(:,2), 'g');
plot(eixoX, euler(:,3), 'b');
title('Rotação - Filtro de Madgwick IMU');
xlabel('Tempo (s)');
ylabel('Angulos (graus)');
legend('\phi', '\theta', '\psi');
hold off;

% %filtro de madgwick (completo)



AHRS_COMPLETO =  MadgwickAHRS('SamplePeriod', 0.01, 'Beta', 0.1);
quaternion_completo = zeros(length(eixoX), 4);

for t = 1:length(eixoX)
    AHRS_COMPLETO.Update(gyro(t,:) * (pi/180), accel(t,:), mag(t,:))% as unidades do giroscopio devem ser em radiano
    quaternion_completo(t, :) = AHRS_COMPLETO.Quaternion;
end
euler = quatern2euler(quaternConj(quaternion_completo)) * (180/pi);	
figure('Name', 'Angulos2');
hold on;
plot(eixoX, euler(:,1), 'r');
grid;
plot(eixoX, euler(:,2), 'g');
plot(eixoX, euler(:,3), 'b');
title('Rotação - Filtro de Madgwick Completo');
xlabel('Tempo (s)');
ylabel('Angulos (graus)');
legend('\phi', '\theta', '\psi');
hold off;

