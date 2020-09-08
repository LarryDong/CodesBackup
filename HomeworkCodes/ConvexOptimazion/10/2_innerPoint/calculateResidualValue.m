
% [r_dual, r_cent, r_pri] = fun(t, x, lambda, mu)
% calculate residual, in quadratic form.

function [r_dual, r_cent, r_pri] = calculateResidualValue(t, x, lambda, mu)
	
	load A.mat;
	load b.mat;
	load P.mat;
	load q.mat;
	
	r_dual = q + P*x - lambda + A'*mu;
	r_cent = diag(lambda)*x - 1/t*ones(size(x));
	r_pri = A*x - b;
	
end
