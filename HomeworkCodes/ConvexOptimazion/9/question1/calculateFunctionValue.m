
% calculate result of default function.
% Format: value = calculateFunctionValue(x)
% Input: x, a certian point.
% Output: value, the result of f(x)

function value = calculateFunctionValue(x)
	
	value =  x' * log(x);

end

