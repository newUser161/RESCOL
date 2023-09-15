
set Nodos;
param NS:=1000;
param KK;

#coordenadas
set coord;
set coordenadas within coord cross coord;
param nodo{coordenadas};

set Arcos within {Nodos,Nodos} default {};
set S{1..NS} within Nodos default {};
set I within {Nodos};

set AM within Arcos default {}; 
set E within Arcos default {};

param w{(i,j) in Arcos};

var x{(i,j) in Arcos} >=0, integer;
var s{i in Nodos} binary;
var t{i in Nodos} binary;

minimize Z: sum{(i,j) in Arcos}w[i,j]*x[i,j];

subject to

R1{(i,j) in AM}: x[i,j] >= 1;

R2{(i,j) in E}: x[i,j]+x[j,i] >=1;

R3{i in I}: s[i] + sum{j in Nodos: (j,i) in Arcos}x[j,i] = sum{j in Nodos: (i,j) in Arcos}x[i,j] + t[i];

R4{i in Nodos diff I}: sum{j in Nodos: (j,i) in Arcos}x[j,i] = sum{j in Nodos: (i,j) in Arcos} x[i,j] + t[i];

R5: sum{i in I}s[i]= 1;

R6: sum{i in Nodos}t[i]=1;

R7{g in 1..KK}: sum{i in S[g], j in Nodos diff S[g]: (i,j) in Arcos}x[i,j] >= 1;
