
clc;clear

load A.mat;
load b.mat; 
load lambda.mat;
load mu.mat;
load P.mat;
load q.mat;
load x_0.mat;

% epsilon is the tolerance of Newton Iteration.
epsilon = 10^(-10);
epsilon2 = 10^(-10);	% eps2 is scaling to make r_dual reasonable.

yita = 10^(-10);

x = x_0;
t = 1;
scalar_mu = 10;

m = 30;
n = 100;

log_n_over_t = [];	% parameter for plot.

newtonIterationTimes = 0;

while(1)
	
% 	innerCounter = 0;
	
	r_dual = 0;
	r_pri = 0;
	
	while 1
		hessianMatrix = calculateHessianMatrix(t, x);
		
		r_dual = calculateGradient(t, x) + A'*mu;
		r_pri = A*x - b;
		[deltaX, deltaMu] = calculateNewtonDirectionWithConstraint(hessianMatrix, A, r_dual, r_pri);
	
		% save values to plot.
		log_n_over_t = [log_n_over_t, log(n/t)];
	
		error1 = norm(A*x-b,2);
		error2 = norm([r_dual;r_pri],2);
		if  error1<epsilon && error2<epsilon2
% 			fprintf('inner iter times: %d\n', innerCounter);
			break;
		end
		
		stepSize = backtrackWithConstraints(t, x, mu, deltaX, deltaMu);

		x = x + stepSize*deltaX;
		mu = mu + stepSize*deltaMu;
% 		innerCounter = innerCounter+1;
		newtonIterationTimes = newtonIterationTimes + 1;
		
	end
	
	epsilon2 = epsilon2 * 9;	% scaling eps2 because x*(t) is not very accurate.
	t = scalar_mu*t;
	
	if ( norm(scalar_mu*m/t, 2) <= yita)
		break;
	end
	
end



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
figure('Name', 'log(n/t) - k');
plot(log_n_over_t,'-ro');

