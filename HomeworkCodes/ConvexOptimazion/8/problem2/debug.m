
clc;clear;close all;



t = 4;

A = rand(t);
A = A*A';



% 
% 


gra = rand(t,1);


% load debug.mat;
H = A;

L = getCholeskyDecomposition(H);			% LL' = H;
Lt_times_x = forwardElimation(L, -gra);		% L*L'*x = -g --> L'x = forwad(-g)
deltaXnt = backwardElimation(L', Lt_times_x);	% L'x = L'x --> x = back(L')

deltaXnt2 = inv(H)*(-gra);

deltaError = norm(deltaXnt- deltaXnt2,2)
LaError = norm(A-L*L', 2)

save debug.mat A gra 

