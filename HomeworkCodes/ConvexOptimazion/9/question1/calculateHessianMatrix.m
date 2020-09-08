
% calculate hessian matrix of f(x) = sum x_i*log(x_i), at point x;
% Format: H = calculateHermitianMatrix(x)
% Input: x, current point.
% Output: hermitian matrix.

function H = calculateHessianMatrix(x)
	
	H = diag(x.^(-1));
	
	% because of small error, use this function to make it hermitian.
	H = (H + H') / 2;
end
