
% calculate hessian matrix of default function, at point x;
% Format: H = calculateHermitianMatrix(A, x)
% Input: A, parameter matrix, A_50 or A_100.
% Input: x, current point.
% Output: hermitian matrix.

function H = calculateHessianMatrix(A, x)
	
	% represent target function as "f = - hessian1 - hessian2".
	hessian1 = zeros(size(A));
	hessian2 = zeros(size(A));
	
	% calculate the first SUM part.
	for i = 1:size(A,1)
		ai = A(:,i);
		hessian1 = hessian1 + (ai) * ((-ai')./(1-ai'*x)^2);
	end
	
	% calculate the second SUM part.
	% after calculation, the second should be a diagnal matrix.
	diagoalItem = [];
	for i = 1:size(A,1)
		diagoalItem = [diagoalItem, (-4*x(i)^2)/(1-x(i)^2)^2 + (-2)/(1-x(i)^2)];
	end
	hessian2 = diag(diagoalItem);
	
	H = -hessian1 - hessian2;
	
	% because of small error, use this function to make it hermitian.
	H = (H + H') / 2;
end
