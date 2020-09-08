
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
	
			
	% Step I: inv(H)*A' and inv(H)*g
	%  M = inv(H) * A' --> H*M = A' --> L*L'*M = A'
	
	g = r_dual;
	h = r_pri;
	
	Lh = getCholeskyDecomposition(H);
	invH = inv(Lh')*inv(Lh);
	
	S = -A*invH*A';
% 	S = -A*inv(H)*A';
	S = (S+S')/2;
	
	Ls = getCholeskyDecomposition(-S);
	Lst_times_w = forwardElimation(Ls, -(A*invH*g - h));	% L*L'*x = b --> L'x = forward(b)
	w = backwardElimation(Ls', Lst_times_w);			% L'x = L'x --> x = back(L')
	
	
	Lht_times_v = forwardElimation(Lh, -A'*w - g);
	v = backwardElimation(Lh', Lht_times_v);	
	
% 	x = v;
% 	v = w;


	rVector = inv(KKT_Matrix) * (-[r_dual; r_pri]);
	x = rVector(1:size(H,1));
	v = rVector(size(H,1)+1: end);

end
