
clc;clear;

load A.mat;
load b.mat; 
load lambda.mat;
load mu.mat;
load P.mat;
load q.mat;
load x_0.mat;

epsilon = 10^(-10);
x = x_0;
t = 1;
scalar_mu = 10;	% Mu should be larger than 1.

% Attention that 'm' is number of constraints.
[~, m] = size(A);

logepsilon = [];
logResidual = [];

newtonIterationTimes = 0;

while(1)
	
	% first calculate yita^, then find t.
	yitaHat = lambda' * x;
	t = scalar_mu*m / yitaHat;
	
	% calculate dx, dl, dmu.
	[deltaX, deltaLambda, deltaMu] = calculateNewtonDirectionInQuadraticform(t, x, lambda, mu);
	
	% save variables to plot.
	[r_dual, r_cent, r_pri] = calculateResidualValue(t, x, lambda, mu);
	newtonIterationTimes = newtonIterationTimes + 1;
	logepsilon = [logepsilon, log(10*m/t)];
	logResidual = [logResidual, log(sqrt(norm(r_dual,2)^2 + norm(r_pri,2)^2))];

	% calculate step size, by 't=0.99max{}' method.
	stepSize = calculateStepSize(x, deltaX, lambda, deltaLambda);

	% update position.
	x = x + stepSize*deltaX;
	mu = mu + stepSize*deltaMu;
	lambda = lambda + stepSize*deltaLambda;
	
	% check conditions.
	if (norm(r_pri,2) <= epsilon) && (norm(r_dual,2) <= epsilon) && (norm(scalar_mu*m/t, 2) <= epsilon)
		break;
	end
end


%% show result.
fprintf('-------------- Result --------------\n')

xStar = x;
muStar = mu;
lambdaStar = lambda;
pStar = 1/2 * (x'*P*x) + q'*x;


save 'result.mat' xStar muStar lambdaStar pStar;
fprintf('x*, mu*, lambda* and p* have saved to ''result.mat''.\n')
fprintf('Newton iteration times: %d\n', newtonIterationTimes);
fprintf('Min value: p* = %d\n', pStar);


%% draw figures.
figure('Name', 'log(epsilon) - iter');
plot(logepsilon,'-r*');

figure('Name', 'log(residual) - iter');
plot(logResidual,'-bo');




















