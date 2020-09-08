
% calculate gradient at point x, for function f(x) = sum x_i*log(x_i)
% Format: gra = calculateGradient(x)
% Input: x, current point, in [x1, x2, ... xn]' form.
% Output: gradient.

% calculate the gradient of a certain function.

function gra = calDefaultFunctionGradient(x)
	gra = log(x) + 1;
% 	gra = gra./norm(gra,2); % normalize gradients.
end
