
% calculate newton direction.
% Format: delta_Xnt = calculateNewtonDirection(H, g);
% Input: H, hermitian matrix, at point current point;
% Input: g, gradient, at current point.

function deltaXnt = calculateNewtonDirection(hermitianMatrix, gradientValue)
	
	H = hermitianMatrix;
	gra = gradientValue;
	
	if ishermitian(H) == 0
		error('Input matrix ''H'' must be a hermitian matrix.');
	end

	L = getCholeskyDecomposition(H);			% LL' = H;
	Lt_times_x = forwardElimation(L, -gra);		% L*L'*x = -g --> L'x = forwad(-g)
	deltaXnt = backwardElimation(L', Lt_times_x);	% L'x = L'x --> x = back(L')
	
% 	deltaXnt = inv(H)*(-gra);

% 	norm(deltaXnt- deltaXnt2,2);
end