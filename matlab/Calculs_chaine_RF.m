clc;
close all;
clear all;
%Projet - Chaîne RF
%Sébastien Courtois 2018-10-16

%Calculs du gain total, température équivalente, IP3, pour le récepteur et
%l'émetteur. Calcul du Si_min et de la plage dynamique.

T0 = 290;
B = 2.5e9 - 2.4e9;
Ta = 300;
k = 1.38e-23;
%Loss dB
L_SPDT = 1.4;
L_SPDT_max = 1.7;
L_filtreBP_2441 = 3;
L_filtreBP_2441_max = 5;
L_mixer_RF = 7.94;
L_filtrePB_70 = 1.11;
L_mixer_IF = 4.95;
L_filtreLP_10 = 0.47;

%Gain dB
G_SPDT = -L_SPDT;
G_SPDT_min = -L_SPDT_max;
G_filtrePB_2441 = -L_filtreBP_2441;
G_filtrePB_2441_min = -L_filtreBP_2441_max;
G_LNA = 14;
G_LNA_min = 11.5;
G_mixer_RF = -L_mixer_RF;
G_filtrePB_70 = -L_filtrePB_70;
G_mixer_IF = -L_mixer_IF;
G_filtreLP_10 = -L_filtreLP_10;
G_AGC_max = 20;
G_AGC_min = -20;

%Gain naturelle
Gnat_SPDT = 10^(G_SPDT/10);
Gnat_SPDT_min = 10^(G_SPDT_min/10);
Gnat_filtrePB_2441 = 10^(G_filtrePB_2441/10);
Gnat_filtrePB_2441_min = 10^(G_filtrePB_2441_min/10);
Gnat_LNA = 10^(G_LNA/10);
Gnat_LNA_min = 10^(G_LNA_min/10);
Gnat_mixer_RF = 10^(G_mixer_RF/10);
Gnat_filtrePB_70 = 10^(G_filtrePB_70/10);
Gnat_mixer_IF = 10^(G_mixer_IF/10);
Gnat_filtreLP_10 = 10^(G_filtreLP_10/10);
Gnat_AGC_max = 10^(G_AGC_max/10);
Gnat_AGC_min = 10^(G_AGC_min/10);
Gnat_col_Rx = [Gnat_SPDT ...
    Gnat_SPDT*Gnat_filtrePB_2441 ...
    Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA ...
    Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA ...
    Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF ...
    Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70 ...
    Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF ...
    Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF*Gnat_filtreLP_10 ...
    Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF*Gnat_filtreLP_10*Gnat_AGC_max]';

Gnat_col_Tx = [Gnat_filtreLP_10 ...
    Gnat_filtreLP_10*Gnat_mixer_IF ...
    Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70 ...
    Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF ...
    Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441 ...
    Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA ...
    Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA ...
    Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_SPDT]';
%Figure de bruit dB
F_SPDT = L_SPDT;
F_SPDT_max = L_SPDT_max;
F_filtreBP_2441 = L_filtreBP_2441;
F_filtreBP_2441_max = L_filtreBP_2441_max;
F_LNA = 0.8;
F_LNA_max = 1.1;
F_mixer_RF = L_mixer_RF;
F_filtrePB_70 = L_filtrePB_70;
F_mixer_IF = L_mixer_IF;
F_filtreLP_10 = L_filtreLP_10;
F_AGC = 7.2;

%Figure de bruit naturelle
Fnat_SPDT = 10^(F_SPDT/10);
Fnat_SPDT_max = 10^(F_SPDT_max/10);
Fnat_filtreBP_2441 = 10^(F_filtreBP_2441/10);
Fnat_filtreBP_2441_max = 10^(F_filtreBP_2441_max/10);
Fnat_LNA = 10^(F_LNA/10);
Fnat_LNA_max = 10^(F_LNA_max/10);
Fnat_mixer_RF = 10^(F_mixer_RF/10);
Fnat_filtrePB_70 = 10^(F_filtrePB_70/10);
Fnat_mixer_IF = 10^(F_mixer_IF/10);
Fnat_filtreLP_10 = 10^(F_filtreLP_10/10);
Fnat_AGC = 10^(F_AGC/10);

%Calcul gain total (maximum)
G_tot_Rx_min = G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA + G_mixer_RF + G_filtrePB_70 + G_mixer_IF + G_filtreLP_10 + G_AGC_min;
G_tot_Rx_max = G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA + G_mixer_RF + G_filtrePB_70 + G_mixer_IF + G_filtreLP_10 + G_AGC_max;
G_tot_Tx = G_filtreLP_10 + G_mixer_IF + G_filtrePB_70 + G_mixer_RF + G_LNA + G_LNA + G_filtrePB_2441 + G_SPDT;

%Calcul figure de bruit (maximum)
F_Rx = Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT ...
    + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) ...
    + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA) ...
    + (Fnat_mixer_RF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA) ...
    + (Fnat_filtrePB_70 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF) ...
    + (Fnat_mixer_IF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70) ...
    + (Fnat_filtreLP_10 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF) ...
    + (Fnat_AGC - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF*Gnat_filtreLP_10);
Fnat_col_Rx = [Fnat_SPDT ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA) ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA) + (Fnat_mixer_RF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA) ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA) + (Fnat_mixer_RF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA) + (Fnat_filtrePB_70 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF) ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA) + (Fnat_mixer_RF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA) + (Fnat_filtrePB_70 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF) + (Fnat_mixer_IF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70) ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA) + (Fnat_mixer_RF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA) + (Fnat_filtrePB_70 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF) + (Fnat_mixer_IF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70) + (Fnat_filtreLP_10 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF) ...
    Fnat_SPDT + (Fnat_filtreBP_2441- 1)/Gnat_SPDT + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA) + (Fnat_mixer_RF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA) + (Fnat_filtrePB_70 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF) + (Fnat_mixer_IF - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70) + (Fnat_filtreLP_10 - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF) + (Fnat_AGC - 1)/(Gnat_SPDT*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA*Gnat_mixer_RF*Gnat_filtrePB_70*Gnat_mixer_IF*Gnat_filtreLP_10)]';

F_Tx = Fnat_filtreLP_10 ...
    + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) ...
    + (Fnat_filtrePB_70 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF) ...
    + (Fnat_mixer_RF - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70) ...
    + (Fnat_filtreBP_2441 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF) ...
    + (Fnat_LNA - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441) ...
    + (Fnat_LNA - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA) ...
    + (Fnat_SPDT - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA);
Fnat_col_Tx = [Fnat_filtreLP_10 ...
    Fnat_filtreLP_10 + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) ...
    Fnat_filtreLP_10 + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) + (Fnat_filtrePB_70 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF) ...
    Fnat_filtreLP_10 + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) + (Fnat_filtrePB_70 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF) + (Fnat_mixer_RF - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70) ...
    Fnat_filtreLP_10 + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) + (Fnat_filtrePB_70 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF) + (Fnat_mixer_RF - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70) + (Fnat_filtreBP_2441 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF) ...
    Fnat_filtreLP_10 + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) + (Fnat_filtrePB_70 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF) + (Fnat_mixer_RF - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70) + (Fnat_filtreBP_2441 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF) + (Fnat_LNA - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441) ...
    Fnat_filtreLP_10 + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) + (Fnat_filtrePB_70 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF) + (Fnat_mixer_RF - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70) + (Fnat_filtreBP_2441 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF) + (Fnat_LNA - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA) ...
    Fnat_filtreLP_10 + (Fnat_mixer_IF - 1)/(Gnat_filtreLP_10) + (Fnat_filtrePB_70 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF) + (Fnat_mixer_RF - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70) + (Fnat_filtreBP_2441 - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF) + (Fnat_LNA - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441) + (Fnat_LNA - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA) + (Fnat_SPDT - 1)/(Gnat_filtreLP_10*Gnat_mixer_IF*Gnat_filtrePB_70*Gnat_mixer_RF*Gnat_filtrePB_2441*Gnat_LNA*Gnat_LNA) ...
    ]';
%Calcul de la Température équivalent du bruit en entrée
Te_Rx = (F_Rx-1)*T0;
Te_col_Rx = (Fnat_col_Rx - 1).*T0;
Te_Tx = (F_Tx-1)*T0;    
Te_col_Tx = (Fnat_col_Tx - 1).*T0;

%Calcul de Si_min
%Récepteur
No_Rx = 10*log10(k*B*Gnat_col_Rx.*(Ta+Te_col_Rx))+30;
Ni_Rx = No_Rx(1) - G_SPDT;
Si_min_Rx = 10*log10(k*B*(Ta+Te_Rx)*10 ) + 30;
Si_min_col_Rx = [Si_min_Rx ...
    Si_min_Rx + G_SPDT ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 + G_LNA ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA + G_mixer_RF ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA + G_mixer_RF + G_filtrePB_70 ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA + G_mixer_RF + G_filtrePB_70 + G_mixer_IF ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA + G_mixer_RF + G_filtrePB_70 + G_mixer_IF + G_filtreLP_10 ...
    Si_min_Rx + G_SPDT + G_filtrePB_2441 + G_LNA + G_LNA + G_mixer_RF + G_filtrePB_70 + G_mixer_IF + G_filtreLP_10 + G_AGC_min
% Si_min_Rx = Ni_Rx + 10;               %Si_min à utiliser
% Si_min3 = 10*log10(k*B*T0*10)+30;
% Si_min_col_Rx = No_Rx + 10;
%Émetteur
No_Tx = 10*log10(k*B*Gnat_col_Tx.*(Ta+Te_col_Tx))+30;
Ni_Tx = No_Tx(1) - G_filtreLP_10;
Si_min_Tx = Ni_Tx + 10;
Si_min_col_Tx = No_Tx + 10;

%Calcul de IP3
IP3_SPDT = 44;
IP3_LNA = 32.98;
IP3_mixer_RF = 12;
IP3_mixer_IF = 14.03;
IP3_AGC = 28;
%Récepteur
IP3_SPDT_Rx = IP3_SPDT;
IP3_LNA1_Rx = IP3_LNA - G_LNA - G_filtrePB_2441 - G_SPDT;
IP3_LNA2_Rx = IP3_LNA - G_LNA - G_LNA - G_filtrePB_2441 - G_SPDT;
IP3_mixer_RF_Rx = IP3_mixer_RF - G_LNA - G_LNA - G_filtrePB_2441 - G_SPDT;
IP3_mixer_IF_Rx = IP3_mixer_IF - G_filtrePB_70 - G_mixer_RF - G_LNA - G_LNA - G_filtrePB_2441 - G_SPDT;
IP3_AGC_Rx = IP3_AGC - G_AGC_min - G_filtreLP_10 - G_mixer_IF - G_filtrePB_70 - G_mixer_RF - G_LNA - G_LNA - G_filtrePB_2441 - G_SPDT;

IP3_SPDT_Rx_nat = 10^((IP3_SPDT_Rx-30)/10);
IP3_LNA1_Rx_nat = 10^((IP3_LNA1_Rx-30)/10);
IP3_LNA2_Rx_nat = 10^((IP3_LNA2_Rx-30)/10);
IP3_mixer_RF_Rx_nat = 10^((IP3_mixer_RF_Rx-30)/10);
IP3_mixer_IF_Rx_nat = 10^((IP3_mixer_IF_Rx-30)/10);
IP3_AGC_Rx_nat = 10^((IP3_AGC_Rx-30)/10);

IP3_eq_Rx_nat = 1/(1/IP3_SPDT_Rx_nat + 1/IP3_LNA1_Rx_nat + 1/IP3_LNA2_Rx_nat + 1/IP3_mixer_RF_Rx_nat + 1/IP3_mixer_IF_Rx_nat + 1/IP3_AGC_Rx_nat);
IP3_eq_Rx = 10*log10(IP3_eq_Rx_nat)+30;
%Émetteur
IP3_SPDT_Tx = IP3_SPDT - G_LNA - G_LNA - G_filtrePB_2441 - G_mixer_RF - G_filtrePB_70 - G_mixer_IF - G_filtreLP_10;
IP3_LNA1_Tx = IP3_LNA - G_LNA - G_LNA - G_filtrePB_2441 - G_mixer_RF - G_filtrePB_70 - G_mixer_IF - G_filtreLP_10;
IP3_LNA2_Tx = IP3_LNA - G_LNA - G_filtrePB_2441 - G_mixer_RF - G_filtrePB_70 - G_mixer_IF - G_filtreLP_10;
IP3_mixer_RF_Tx = IP3_mixer_RF - G_filtrePB_70 - G_mixer_IF - G_filtreLP_10;
IP3_mixer_IF_Tx = IP3_mixer_IF - G_filtreLP_10;

IP3_SPDT_Tx_nat = 10^((IP3_SPDT_Tx-30)/10);
IP3_LNA1_Tx_nat = 10^((IP3_LNA1_Tx-30)/10);
IP3_LNA2_Tx_nat = 10^((IP3_LNA2_Tx-30)/10);
IP3_mixer_RF_Tx_nat = 10^((IP3_mixer_RF_Tx-30)/10);
IP3_mixer_IF_Tx_nat = 10^((IP3_mixer_IF_Tx-30)/10);

IP3_eq_Tx_nat = 1/(1/IP3_SPDT_Tx_nat + 1/IP3_LNA1_Tx_nat + 1/IP3_LNA2_Tx_nat + 1/IP3_mixer_RF_Tx_nat + 1/IP3_mixer_IF_Tx_nat);
IP3_eq_Tx = 10*log10(IP3_eq_Tx_nat)+30;

%Calcul Si_max
P1_SPDT = 27;
P1_LNA = 18.24;
P1_mixer_RF = IP3_mixer_RF;         %12 dBm
P1_mixer_IF = IP3_mixer_IF;         %14.03 dBm
P1_AGC = 12;
%Récepteur
Si_max_Rx = [P1_mixer_RF - G_LNA - G_LNA - G_filtrePB_2441 - G_SPDT ...
    P1_mixer_RF - G_LNA - G_LNA - G_filtrePB_2441 ...
    P1_mixer_RF - G_LNA - G_LNA ...
    P1_mixer_RF - G_LNA ...
    P1_mixer_RF ...
    P1_mixer_RF + G_mixer_RF ...
    P1_mixer_RF + G_mixer_RF+ G_filtrePB_70 ...
    P1_mixer_RF + G_mixer_RF+ G_filtrePB_70 + G_mixer_IF ...
    P1_mixer_RF + G_mixer_RF + G_filtrePB_70 + G_mixer_IF + G_filtreLP_10 ...
    P1_mixer_RF + G_mixer_RF + G_filtrePB_70 + G_mixer_IF + G_filtreLP_10 + G_AGC_min]';

%Émetteur
Si_max_Tx = [P1_LNA - G_LNA - G_LNA - G_filtrePB_2441 - G_mixer_RF - G_filtrePB_70 - G_mixer_IF - G_filtreLP_10 ...
    P1_LNA - G_LNA - G_LNA - G_filtrePB_2441 - G_mixer_RF - G_filtrePB_70 - G_mixer_IF ...
    P1_LNA - G_LNA - G_LNA - G_filtrePB_2441 - G_mixer_RF - G_filtrePB_70 ...
    P1_LNA - G_LNA - G_LNA - G_filtrePB_2441 - G_mixer_RF ...
    P1_LNA - G_LNA - G_LNA - G_filtrePB_2441 ...
    P1_LNA - G_LNA - G_LNA ...
    P1_LNA - G_LNA ...
    P1_LNA]';

%Calcul de la porté de l'antenne
c = 3*10^8;
f = 2.45*10^9;
%Gt = 6.31;
%Pt = 0;
GtPt = 4;
Gr1 = 2.69; %Antenne planaire 1
Gr2 = 1.56; %Antenne planaire 2 (T)
Gr3 = 6.31;

Pr = 4.01*10^-12;
lambda = c/f;
ind = 2.7;

R1 = ((GtPt*Gr1*lambda^2)/(((4*pi)^ind)*Pr))^(1/ind);
R2 = ((GtPt*Gr2*lambda^2)/(((4*pi)^ind)*Pr))^(1/ind);
R3 = ((GtPt*Gr3*lambda^2)/(((4*pi)^ind)*Pr))^(1/ind);