
% train projection matrix.
% P = tranProjectionMatrix(X, labels, targetDimension, options)
% @X, n-by-d input data. -n is number of data, -d is dimension
% @labels, n-by-1 label, corresponding to X.
% @targetDimension, to which dimension the projection matrix to.
% @options, 1-by-3 matrix. [K, t, b], three constant. [4,5,1] by default.
% @P, n-by-targetDemension projection matrix.

function P = trainProjectionMatrix(X, labels, targetDimension, options, showVerbose)
% 	fprintf('[Project] Training projection matrix...\n');
	P = [];
	if (size(X,1) ~= size(labels,1))
		error('Size of data and label should be the same.');
	end
	
	if nargin == 3
		options = [4, 5, 1];
	end
	
	neighborNumber = options(1);
	constT = options(2);
	constB = options(3);
	
	n= size(X, 1);
	[ind, A] = knnsearch(X, X, 'K', neighborNumber + 1, 'Distance', 'Euclidean');
	
	A = sparse(repmat((1:n)', 1, neighborNumber + 1), ind, A, n, n);
	A = full(A);
	A = ((A+A')/2)/max(max(A));	% make it symmetric and 0-1
	
	eps = 10^(-8);
	S = zeros(n,n);
	for i = 1:n
		for j = 1:n
			if(A(i,j)>eps)
				if(labels(i)==labels(j))
					S(i, j) = exp(-A(i,j)^2/constT) * (1+exp(-A(i,j)^2/constT));
				else
					S(i, j) = exp(-A(i,j)^2/constT) * (1-exp(-A(i,j)^2/constT));
				end
			else
				S(i,j) = 0;
			end
		end
	end
	B = zeros(n,n);
	for i = 1:n
		for j = 1:n
			if(A(i,j)>eps)
				if(labels(i)==labels(j))
					B(i, j) = exp(-constB/A(i,j)^2) * (1-exp(-constB/A(i,j)^2));
				else
					B(i, j) = exp(-constB/A(i,j)^2) * (1+exp(-constB/A(i,j)^2));
				end
			else
				B(i,j) = 0;
			end
		end
	end
	
	
	d = sum(S);
	D = diag(d);
	L = D-S;
	GL = X'*L*X;
 
	d = sum(B);
	Dbar = diag(d);
	Lbar = Dbar-B;
	GN = X'*Lbar*X;

	C = GL - GN;
	C = (C+C')/2;

	[V, D] = eig(C);
	eigenvalues = diag(D);
	% Sort eigenvectors in ascending order
	[eigenvalues, ind] = sort(eigenvalues, 'ascend');
	% eigenvalues = eigenvalues(1:reducedDimension);
	P = V(:, ind(1: targetDimension));

% 	fprintf('t1-%d,t2-%d,t3-%d,t4-%d,t5-%d\n',t1,t2-t1,t3-t2,t4-t3,t5-t4)
end







