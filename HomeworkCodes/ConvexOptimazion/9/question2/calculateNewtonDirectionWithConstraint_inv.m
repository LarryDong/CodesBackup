
% calculate newton direction. 
% Format: [x, v] = func(H, A, r_dual, r_pri);
% Input: H, hessian matrix at point x;
% Input: A, constraint matrix of Ax=b;
% Input: gra, gradient at point x;
% Output: deltaX, direction of next step;
% Output: w, omiga of A'w, not useful.

function [x, v] = calculateNewtonDirectionWithConstraint(H, A, r_dual, r_pri)

	% [] * [x;w] = [-f;0];
	
% 	x = zeros(size(H, 2), 1);
% 	w = zeros(size(A', 2), 1);
	
	KKT_Matrix = [H, A';
				A, zeros(size(A,1))];
	
	rVector = inv(KKT_Matrix) * (-[r_dual; r_pri]);
	




	x = rVector(1:size(H,1));
	v = rVector(size(H,1)+1: end);

	
	
	
end
