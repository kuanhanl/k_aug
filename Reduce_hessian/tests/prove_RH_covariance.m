clear
clc

syms A1 A2 A3 V1 V2 D1 D2

A1 = 5;
A2 = 2;
A3 = 1;

V1 = 3;
V2 = 2;

D1 = 4;
D2 = 7;

Z = [1,0;0,1;-A1*A2,-A1*A3];
HR = Z'*[V1,0,0;0,0,0;0,0,V2]*Z;
inv_HR = inv(HR);

D = [V1*D1;0;V2*D2];



V = inv_HR*Z'*D*D'*Z*inv_HR'

