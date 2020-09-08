

function [r_dual, r_pri] = calculateResidualValue(t, x, mu)
	
	load A.mat;
	load b.mat;
	
	r_dual = calculateGradient(t, x) + A'*mu;
	r_pri = A*x - b;
	
end