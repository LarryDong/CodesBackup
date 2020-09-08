
clc;clear;close all;


epsilon = 10^(-8);

%% calculate l1-norm 
fprintf('Using Newton method to find optimal value ...\n');
x0 = [0, 0]';

save_x1 = x0(1);
save_x2 = x0(2);
save_value = calculateFunctionValue(x0);
save_gradient = calculateGradient(x0);

x = x0;
gradientValue = calculateGradient(x);
while norm(gradientValue, 2) > epsilon
	hermitianMatrix = calculateHessianMatrix(x);
	
	% if at point x, hermitian is not positive-definite, use l2-norm
	eig_value = eig(hermitianMatrix);
	if (size((find(eig_value > 0)),1)) < size(hermitianMatrix, 1)
		dir = -gradientValue;
	else		% use Newton way to find direction.
		dir = calculateNewtonDirection(hermitianMatrix, gradientValue);
	end
	
	stepSize = searchLine(x, dir);
	x = x + stepSize*dir;
	gradientValue = calculateGradient(x);	% calculate gradient after changing new point.
	
	save_x1 = [save_x1, x(1)];
	save_x2 = [save_x2, x(2)];
	save_value = [save_value, calculateFunctionValue(x)];
	save_gradient = [save_gradient, gradientValue];
end

fprintf('sequence of x1:\n');
vpa(save_x1)
fprintf('sequence of x2:\n');
vpa(save_x2)

save 'result.mat' save_x1 save_x2 save_value save_gradient;


drawContour;
drawError;




