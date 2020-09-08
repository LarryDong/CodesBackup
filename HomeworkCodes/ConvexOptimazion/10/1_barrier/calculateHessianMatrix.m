
function H = calculateHessianMatrix(t, x)
	
	load P.mat;
	H = t*P + diag(1./(x.*x));
	
end
