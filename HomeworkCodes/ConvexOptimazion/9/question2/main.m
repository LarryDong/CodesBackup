
clc;clear;close all;

load('x_1.mat');
load('b.mat');
load('A_m=30_n=100.mat');		% load matrix A

epsilon = 10^(-12);
x = x_1;
v = zeros(size(A ,1), 1);

fprintf('Newton Method, with invalid initial point x0\n');
fprintf('Choosing initial dual parameter v0 to s.t. A''*v > 0...\n');

times = 0;
lambdaX = 0;

save_x = x;
save_value = sum(x.*log(x));

while 1
	
	hessianMatrix = calculateHessianMatrix(x);
	r_dual = calculateGradient(x) + A'*v;
	r_pri = A*x - b;
	[deltaX, deltaV] = calculateNewtonDirectionWithConstraint(hessianMatrix, A, r_dual, r_pri);
	
	error1 = norm(A*x-b,2);
	
	norm(r_dual);
	norm(r_pri);
	
	error2 = norm([r_dual;r_pri],2);
	
	if  error1<epsilon && error2<epsilon
		break;
	end
		
	stepSize = backtrackWithConstraints(x, v, deltaX, deltaV);
	x = x + stepSize*deltaX;
	v = v + stepSize*deltaV;
% 	
	times = times + 1;
	save_x = [save_x, x];
	save_value = [save_value, sum(x.*log(x))];
end


% 
fprintf('------------------ Result ------------------\n');
fprintf('Iteration times: %d\n', times);
fprintf('value sequence:\n');
vpa(save_value')

save 'result.mat' save_x save_value;
showResult;





	
	