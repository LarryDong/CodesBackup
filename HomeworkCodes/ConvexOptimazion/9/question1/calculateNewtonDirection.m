
% calculate newton direction. 
% Format: [deltaX, w] = calculateNewtonDirection(H, A, gra);
% Input: H, hessian matrix at point x;
% Input: A, constraint matrix of Ax=b;
% Input: gra, gradient at point x;
% Output: deltaX, direction of next step;
% Output: w, omiga of A'w, not useful.

function [deltaX, w] = calculateNewtonDirection(H, A, gra)

	% [] * [x;w] = [-f;0];
	
	KKT_Matrix = [H, A';
				A, zeros(size(A,1))];
	
	R = inv(KKT_Matrix) * ([-gra; zeros(size(A,1),1)]);
	deltaX = R(1:size(H,1));
	
	w = 0;	% w is not used.
end
