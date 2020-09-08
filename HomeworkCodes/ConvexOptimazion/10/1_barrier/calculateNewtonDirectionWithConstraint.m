
% calculate newton direction. 
% Format: [x, mu] = func(H, A, r_dual, r_pri);
% Input: H, hessian matrix at point x;
% Input: A, constraint matrix of Ax=b;
% Input: gra, gradient at point x;
% Output: deltaX, direction of next step;
% Output: deltaMu, direction of 


function [x, mu] = calculateNewtonDirectionWithConstraint(H, A, r_dual, r_pri)

	KKT_Matrix = [H, A';
				 A, zeros(size(A,1))];
			 
%% use inv to calculate. Nor recommanded...			 

	residualVector = inv(KKT_Matrix) * (-[r_dual; r_pri]);
	
	x = residualVector(1:size(H,1));
	mu = residualVector(size(H,1)+1: end);

%% Use cholesky elimation method to calculate.
	% the following elimation from P521 in homework3.
	
%%

	g = r_dual;
	h = r_pri;
	
	Lh = getCholeskyDecomposition(H);
	invH = inv(Lh')*inv(Lh);
	
	S = -A*invH*A';
	S = (S+S')/2;
	
	Ls = getCholeskyDecomposition(-S);
	Lst_times_w = forwardElimation(Ls, -(A*invH*g - h));	% L*L'*x = b --> L'x = forward(b)
	w = backwardElimation(Ls', Lst_times_w);			% L'x = L'x --> x = back(L')
	
	Lht_times_v = forwardElimation(Lh, -A'*w - g);
	v = backwardElimation(Lh', Lht_times_v);	
	
%%
	% v and w is not changed because last time use these two names
	x = v;
	mu =w;
	
	% a trick here. To deal with H when is not positive definite.
	
	x = residualVector(1:size(H,1));
	mu = residualVector(size(H,1)+1: end);
	
	
end