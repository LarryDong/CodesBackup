
clear;clc;

load('Homework1107DATA.mat');

epsilon = 10^(-5);


A = A_50;	% choose parameter.


% avoid the initial point out of Domf
isBoundary = 2;
% x = rand(size(A,1),1)/50;
x=[];
while isBoundary > 1
	x = rand(size(A,1),1)/10;	% randomly choose the initial point.
% 	x = rand(size(A,1),1)/25;	% if use A_100, should be samller.
	st = size(find(A'*x>=1), 1);	% if x0 in Domf, should be no >=1 element. st = 0
	if st < 0.5
		isBoundary = 0;			% if in domain, set isBoundary to 0 to stop while-loop.
	end
end

x0 = x;		% store initial value to debug.

% x0 = zeros(size(A,1),1);	% choose a certain initial point.

% record step size and value in vector, to draw the curve.
t1 = [];
t2 = [];
tinf = [];

f1 = [];
f2 = [];
finf = [];

fprintf('Calculating optimum solution under l1 norm...\n');
x = x0;
gradientError = calDefaultFunctionGradient(A, x);
times = 0;
while norm(gradientError, 2) > epsilon
	times = times + 1;
	dir = calculateDirection(A, x, 1);
	stepSize = backtrack(A, x, dir);
	x = x + stepSize*dir;
	gradientError = calDefaultFunctionGradient(A, x);

	% store stepsize and value.
	t1 = [t1, stepSize];
	f1 = [f1, calDefaultFunctionValue(A, x)];
end
x_l1 = x;

fprintf('Calculating optimum solution under l2 norm...\n');
x = x0;
gradientError = calDefaultFunctionGradient(A, x);
times = 0;
while norm(gradientError, 2) > epsilon
	times = times + 1;
	dir = calculateDirection(A, x, 2);
	stepSize = backtrack(A, x, dir);
	x = x + stepSize*dir;
	gradientError = calDefaultFunctionGradient(A, x);
	
	t2 = [t2, stepSize];
	f2 = [f2, calDefaultFunctionValue(A, x)];
end
x_l2 = x;

fprintf('Calculating optimum solution under l-inf norm...\n');
x = x0;
gradientError = calDefaultFunctionGradient(A, x);
times = 0;
while norm(gradientError, 2) > epsilon
	times = times + 1;
	dir = calculateDirection(A, x, 999);
	stepSize = backtrack(A, x, dir);
	x = x + stepSize*dir;
	gradientError = calDefaultFunctionGradient(A, x);

	tinf = [tinf, stepSize];
	finf = [finf, calDefaultFunctionValue(A, x)];
	
end
x_linf = x;

save result t1 t2 tinf f1 f2 finf x_l1 x_l2 x_linf;

fprintf('---------  Result  ---------\n');
fprintf('l1 norm. Iter times: %d, value: %d.\n', size(f1,2),f1(end));
fprintf('l2 norm. Iter times: %d, value: %d.\n', size(f2,2),f2(end));
fprintf('l-inf norm. Iter times: %d, value: %d.\n', size(finf,2),finf(end));

drawResult;
