
% calculate result of default function.
% Format: value = calculateFunctionValue(A, x)
% Input: A, matrix, A_50 or A_100
% Input: x, a certian point.
% Output: value, the result of f(x)

function value = calculateFunctionValue(A, x)
	
% 	value = -sum(log(1-A_50'*x)) - sum(log(1-x.^2));
% 	value = (x(1)-1)^2 + x(2)^2; 

	sum1 = sum(log(1-A'*x));
	sum2 = sum(log(1-x.^2));
	value = -sum1 -sum2;
end

