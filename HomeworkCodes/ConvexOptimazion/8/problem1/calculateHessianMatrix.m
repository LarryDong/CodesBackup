
% calculate hessian matrix of default function, at point x;
% Format: H = calculateHessianMatrix(x)
% Input: x, current point.
% Output: hermitian matrix.

function H = calculateHessianMatrix(x)
	
	H = [24*x(1)^2 - 8*x(2) + 2, -8*x(1);
		 -8*x(1)			   , 4		];
	 
end
