
clc;clear;close all;


epsilon = 10^(-8);

syms x1 x2;
targetFunction = (1-x1)^2 + 2*(x2-x1^2)^2;
targetFunctionGradient = gradient(targetFunction);


%% calculate l1-norm 
fprintf('calculating l1 norm...\n');
x = [0, 0];
x_l1 = x(1);
y_l1 = x(2);
value_l1 = calDefaultFunctionValue(x);

gradientError = double(subs(targetFunctionGradient,[x1,x2],[x(1),x(2)]));
while norm(gradientError, 2) > epsilon
	dir = calculateDirection(targetFunction, x, 1);
	stepSize = searchLine(targetFunction, x, dir);
	x = x + stepSize*dir;
	gradientError = double(subs(targetFunctionGradient,[x1,x2],[x(1),x(2)]));
	x_l1 = [x_l1, x(1)];
	y_l1 = [y_l1, x(2)];
	value_l1 = [value_l1, calDefaultFunctionValue(x)];
end
% fprintf('Final value of x is:[%d, %d]\n', x(1), x(2));

%% calculate l2-norm
fprintf('calculating l2 norm...\n');
x = [0, 0];
x_l2 = x(1);
y_l2 = x(2);
value_l2 = double(subs(targetFunction,[x1,x2],[x(1),x(2)]));

gradientError = double(subs(targetFunctionGradient,[x1,x2],[x(1),x(2)]));
while norm(gradientError, 2) > epsilon
	dir = calculateDirection(targetFunction, x, 2);
	stepSize = searchLine(targetFunction, x, dir);
	x = x + stepSize*dir;
	gradientError = double(subs(targetFunctionGradient,[x1,x2],[x(1),x(2)]));
	x_l2 = [x_l2, x(1)];
	y_l2 = [y_l2, x(2)];
	value_l2 = [value_l2, calDefaultFunctionValue(x)];
end
% fprintf('Final value of x is:[%d, %d]\n', x(1), x(2));

%% calculate linf-norm
fprintf('calculating l-inf norm...\n');
x = [0, 0];
x_linf = x(1);
y_linf = x(2);
value_linf = double(subs(targetFunction,[x1,x2],[x(1),x(2)]));

gradientError = double(subs(targetFunctionGradient,[x1,x2],[x(1),x(2)]));
while norm(gradientError, 2) > epsilon
	dir = calculateDirection(targetFunction, x, 999);
	stepSize = searchLine(targetFunction, x, dir);
	x = x + stepSize*dir;
	gradientError = double(subs(targetFunctionGradient,[x1,x2],[x(1),x(2)]));
	x_linf = [x_linf, x(1)];
	y_linf = [y_linf, x(2)];
	value_linf = [value_linf, calDefaultFunctionValue(x)];
end
fprintf('Final value of x is:[%d, %d]\n', x(1), x(2));


fprintf('---------  Result  ---------\n');
fprintf('l1 norm. Iter times: %d, at [%d, %d], minValue:%d.\n', size(x_l1,2),x_l1(end),y_l1(end),value_l1(end));
fprintf('l2 norm. Iter times: %d, at [%d, %d], minValue:%d.\n', size(x_l2,2),x_l1(end),y_l2(end),value_l2(end));
fprintf('l-inf norm. Iter times: %d, at [%d, %d], minValue:%d.\n', size(x_linf,2),x_linf(end),y_l1(end),value_linf(end));

save result x_l1 x_l2 x_linf y_l1 y_l2 y_linf value_l1 value_l2 value_linf;


%% draw result
drawError;
drawContour;




