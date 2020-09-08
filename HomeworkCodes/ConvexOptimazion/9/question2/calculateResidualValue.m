
% calculate residual value, automatic load A and b for equation Ax=b.
% Format: [r_dual, r_pri] = calculateResidualValue(gra, x, v)
% Input: gra, gradient of function;
% Input: x, v: current x and v;
% Output: r_dual, r_pri: residual results.


function [r_dual, r_pri] = calculateResidualValue(gra, x, v)
	load('b.mat');
	load('A_m=30_n=100.mat');		% load matrix A
	
	r_dual = gra + A'*v;
	r_pri = A*x - b;
end

