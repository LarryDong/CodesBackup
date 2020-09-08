
% [dx, dlamuda, dmu] = fun(t, x, lambda, mu)
% automatic load function parameter.

function [deltaX, deltaLambda, deltaMu] = calculateNewtonDirectionInQuadraticform(t, x, lambda, mu)

	load P.mat;
	load q.mat;
	load A.mat;
	load b.mat;
	
	
%%	Use inv to calculate method. Not recommanded.
	KKT_Matrix = [P, -eye(size(P,1)), A';
				diag(lambda), diag(x), zeros(size(x,1), size(A', 2));
				A, zeros(size(A,1), size(x, 1)), zeros(size(A, 1), size(A', 2))];
	r_dual = q + P*x - lambda + A'*mu;
	r_cent = diag(lambda)*x - 1/t*ones(size(x));
	r_pri = A*x - b;
	
	deltaY = - inv(KKT_Matrix)*[r_dual; r_cent; r_pri];
	deltaX = deltaY(1 : size(P,1));
	deltaLambda = deltaY( size(P,1)+1 : size(P,1) + size(lambda,1) );
	deltaMu = deltaY( size(P,1) + size(lambda,1) + 1 : end);
	
	
%% Use cholesky elimation method to calculate.

	alpha = (q + P*x - lambda + A'*mu);
	beta = (diag(lambda)*x - 1/t*ones(size(x)));
	gamma = (A*x - b);
	
	HBar = P + diag(1./x)*diag(lambda);
	
%%	the following elimation from P521 in homework3.
	% 
	H = HBar;
	
	g = alpha + diag(1./x)*beta;
	h = gamma;
	
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
	deltaX = v;
	deltaMu =w;
	deltaLambda = diag(1./x)*( - beta - diag(lambda)*deltaX);

end


