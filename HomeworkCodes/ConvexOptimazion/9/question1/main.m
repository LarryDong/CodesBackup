
clc;clear;close all;

% load('x_1.mat');
load('x_0.mat');
load('b.mat');
load('A_m=30_n=100.mat');		% load matrix A

fprintf('Calculating optimal solution by Newton method ...\n');

epsilon = 10^(-10);
x = x_0;

times = 0;
lambdaX = 0;

save_x = x;
save_value = sum(x.*log(x));
while 1

	hessianMatrix = calculateHessianMatrix(x);
	gra = calculateGradient(x);
	
	deltaX = calculateNewtonDirection(hessianMatrix, A, gra);
	
	lambdaX = norm(deltaX'*hessianMatrix*deltaX);
	
	if lambdaX^2/2 <= epsilon
		fprintf('lambda X is small enough.\n');
		break;
	end
	
	stepSize = backtrack(x, deltaX); 
	x = x + stepSize*deltaX;
	times = times + 1;
	save_x = [save_x, x];
	save_value = [save_value, sum(x.*log(x))];
end

fprintf('------------------ Result ------------------\n');
fprintf('Iteration times: %d\n', times);
fprintf('value sequence:\n');
vpa(save_value')

save 'result.mat' save_x save_value;
showResult;





	
	