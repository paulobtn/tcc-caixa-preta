% Calibração Magnetometro - Método retornando a matriz de escala completa
% Utiliza a função do matlab magcal
% Lê dados do Magnetômetro pela porta serial usando o modo opera6
% fclose(instrfind) --> fechar porta
% Os dados escritos no arquivo são
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% offsetx
% offsety
% offsetz
% escalaxx
% escalaxy
% escalaxz
% escalayx
% escalayy
% escalayz
% escalazx
% escalazy
% escalazz
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Para obter o valor, use a expressão
% sendo H = [ 
%             hx1 hy1 hz1
%             hx2 hy2 hz2
%             ...
%             hxn hyn hzn           
%           ]
% H_Calibrado = (H - offset)*escala 
%
% UNIDADES
% Para converter para alguma unidade:
% uT: Hx * 4912.0f / 32760.0
% G:  Hx * (4912.0f / 32760.0)/100
%
% O arquivo mil_leituras.m exemplifica como ler e converter os dados de calibragem
% -------------------------------------

clear all;
clear;
clc;

% diretório do script
if(~isdeployed)
  cd(fileparts(which(mfilename)));
end
directory = pwd;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Usuário selecionar sua janela
janela=5;       %Tamanho da janela em segundos
passo=0.5;      %Passo da janela em segundos
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf(1,'Opera 6\n');
fprintf(1,'O Matlab vai receber dados do Magnetômetro da Caixa Preta.\n');
fprintf(1,'Rotacione lentamente para todas as direções possíveis.\n');
fprintf(1,'Em caso de erro, a porta serial pode estar travada.\n');
fprintf(1,'Neste caso use "fclose(instrfind)" para fechar porta serial.\n');

%Parâmetros
fa=100;         %Frequência de amostragem em Hz
ta=1/fa;        %Intervalo entre amostras (periodo)


%Cálculos para a janela, em amostras
tam=janela*fa;   %Tamanho da janela em nr de amostras
pa=fa*passo;    %Passo em nr de amostras

sid=serial('COM6','Baudrate',115200);
fopen(sid);
if (sid==-1)
    fprintf(1,'Nao abriu COM6.\n');
    return;
end

x1=0;
x2=0;

hx=zeros(1,tam);
hy=hx;
hz=hx;
eixo=0:ta:janela-ta;

%teste2
% plot(eixo,hx,'b');
pause(2);

% Inicia o modo opera 6
fprintf(sid,'o6\r\n');

% Esperar sinal da Caixa Preta
fprintf(1,'\nPor favor, selecione opera 6 na Caixa Preta.\n');
while x1~='#' | x2~='['
    x1=x2;
    x2=fread(sid,1);
    fprintf(1,'%c',x2);
end

%Chegou sinal, iniciar recepção de dados
fprintf(1,'==> Padrão esperado.');
fprintf(1,'\nIniciando recepção de dados...\n');
pause(1);

asax=fscanf(sid,'%d',10);
asay=fscanf(sid,'%d',10);
asaz=fscanf(sid,'%d',10);

% ASA precisa ser sem sinal, mas a caixa preta está enviando com sinal
asax=typecast(int8(asax),'uint8');
asay=typecast(int8(asay),'uint8');
asaz=typecast(int8(asaz),'uint8');

asaxC = 1+ ((double(asax) - 128.0)*0.5)/128.0;
asayC = 1+ ((double(asay) - 128.0)*0.5)/128.0;
asazC = 1+ ((double(asaz) - 128.0)*0.5)/128.0;

figure(1);
uhx=0;  %Último hx
uhy=0;  %Último hy
uhz=0;  %Último hz
ix=1;

hold on;
scatter(hx,hy,'Or') ;
scatter(hx,hz,'Og') ;
scatter(hy,hz,'Ob') ;
grid;
while true
    uhx = fscanf(sid,'%d',10);
    uhy = fscanf(sid,'%d',10);
    uhz = fscanf(sid,'%d',10);
    hx(ix)=uhx;
    hy(ix)=uhy;
    hz(ix)=uhz; 
    if hx(1,ix) == 22222 && hx(1,ix) == 22222
        ix=ix+1;
        break;
    end   
    if mod(ix,pa) == 0       
        scatter(hx, hy, 'r');
        scatter(hx, hz, 'g');
        scatter(hy, hz, 'b');    
        title('magnetometro');
        legend('xy', 'xz', 'yz');       
        drawnow;    
    end   
    ix=ix+1;
end
hold off;

%por algum motivo o primeiro hz está sempre dando zero. Ignorar ele
hz(1,1) = hz(1,2); 

ix=ix-1;
fprintf(1,'\nTeminou recepção de dados.\n');
fprintf(sid,'x\r\n');
fclose(sid);
fprintf(1,'Recebidas %d leituras por eixo.\n',ix);
fprintf(1,'Duração %.2f segundos.\n',ix/fa);
%close all;

% Remover a marca final "22222" de todos os eixos
% Repete a penúltima leitura
hx(1,ix)=hx(1,ix-1);
hy(1,ix)=hy(1,ix-1);
hz(1,ix)=hz(1,ix-1);

%h corrigido com o ajuste da sensibilidade
hxASA = hx*asaxC;
hyASA = hy*asayC;
hzASA = hz*asazC;

magData = [hxASA' hyASA' hzASA'];

%Faz a calibração
[A,b,expMFS]  = magcal(magData);
magDataCorrected = (magData-b)*A;

%escreve em arquivo offsets e escalas
fName = [directory '\calib_mag.txt'];
fid=fopen(fName,'w');
%Verificar se abriu o arquivo
if (fid==-1)
    fprintf(1,'Nao abriu arquivo [%s]. Parar!\n',fName);
    return;
end
fprintf(fid, '%f\n', b(1));
fprintf(fid, '%f\n', b(2));
fprintf(fid, '%f\n', b(3));
fprintf(fid, '%f\n', A(1));
fprintf(fid, '%f\n', A(2));
fprintf(fid, '%f\n', A(3));
fprintf(fid, '%f\n', A(4));
fprintf(fid, '%f\n', A(5));
fprintf(fid, '%f\n', A(6));
fprintf(fid, '%f\n', A(7));
fprintf(fid, '%f\n', A(8));
fprintf(fid, '%f\n', A(9));
fclose(fid);

% Plota convertendo para micro Tesla
escala = 4912.0 / 32760;
magData = magData*escala;
[A,b,expMFS]  = magcal(magData);
magDataCorrected = (magData-b)*A;

% precisa adicionar a biblioteca de exemplo do matlab ao path para usar HelperDrawEllipsoid 
% de = HelperDrawEllipsoid;
% de.plotCalibrated(A,b,expMFS,magData,magDataCorrected,'Auto');
