
% calculate gradient at point x, for default function.
% Format: gra = calculateGradient(x)
% Input: x, current point.
% Output: gradient.

function gra = calculateGradient(x)

	gra = [8*x(1)^3 - 8*x(1)*x(2) + 2*x(1) - 2;
			-4*x(1)^2 + 4*x(2)];

end