clc
close all
clear all

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