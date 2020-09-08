
clear;clc;
close all;	% Add this line to draw only once.

load('Homework_11_14_DATA.mat');
epsilon = 10^(-5);


A = A_50;	% choose parameter.

% avoid the initial point out of Domf
isBoundary = 2;
fprintf('Choosing valid initial point...\n');
while isBoundary > 1
% 	x0 = rand(size(A,1),1)/25;	% randomly choose the initial point.
	x0 = rand(size(A,1),1)/10;	% if use A_50, divided by 10 is enough.
% 	x0 = rand(size(A,1),1)/25;	% if use A_100, should be samller.
	st = sum(sum(A'*x0>=1));	% if x0 in Domf, should be no >=1 element. st = 0
	if st < 0.5
		isBoundary = 0;			% if in domain, set isBoundary to 0 to stop while-loop.
	end
end


fprintf('Calculating optimal solution by Newton method ...\n');
x = x0;		% randomly select value
% x = zeros(50,1);
% x = zeros(100,1);
gradientValue = calculateGradient(A, x);

% save parameters.
save_x = [];
save_value = [];
save_gradient = [];
save_stepLength = [];

times = 0;
while norm(gradientValue, 2) > epsilon
	
	hermitianMatrix = calculateHessianMatrix(A, x);
	
	gra = calculateGradient(A, x);
	
	eig_value = eig(hermitianMatrix);
	if sum(sum(eig_value > 0)) < size(hermitianMatrix, 1)
		dir = -gradientValue;
	else		% use Newton way to find direction.
		dir = calculateNewtonDirection(hermitianMatrix, gradientValue);
	end
	
	stepSize = backtrack(A, x, dir); 
	x = x + stepSize*dir;
	gradientValue = calculateGradient(A, x);
	norm(gradientValue);
	
	times = times + 1;
	
	save_x = [save_x, x];
	save_value = [save_value, calculateFunctionValue(A, x)];
	save_gradient = [save_gradient, gradientValue];
	save_stepLength = [save_stepLength, stepSize];
end

fprintf('\n ------------- Result -------------\n');
fprintf('Iter times: %d, value: %d.\n', size(save_x,2),save_value(end));

save 'result.mat' save_x save_value save_gradient save_stepLength

drawResult;
