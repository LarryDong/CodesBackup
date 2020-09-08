
% calculate gradient at point x, for default function.
% Format: gra = calculateGradient(x)
% Input: A, matrix, A_50 or A_100
% Input: x, current point.
% Output: gradient.

% calculate the gradient of a certain function.

function gra = calDefaultFunctionGradient(A, x)

	sum1 = 0;
	sum2 = 0;
	for i=1:size(A,1)
		sum1 = sum1 + (-A(:,i)) / (1 - A(:,i)'*x);
		nom = zeros(size(A,1),1);
		nom(i) = 1;
		sum2 = sum2 + (-2*(nom.*x)/(1-x(i)^2));
		gra = -sum1 - sum2;
	end
end
