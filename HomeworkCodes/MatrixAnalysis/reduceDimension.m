
% [Y,W] = fun(input, labels, dim, para)

function [Y, W] = reduceDimension(inputData, labels, reducedDimension, parameters)

	fprintf('--> reducing dimension from %d to %d...\n', size(inputData,2),reducedDimension);
	X = inputData;

	[n, ~] = size(X);
	
	neighborNumber = parameters(1);
	constT = parameters(2);
	constB = parameters(3);
	
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


% matlab2014b
% [V, eigenvalues, exitFlag] = eigs(C, 2 + 1,'sm');
% funciont 'eigs' in matlab2014 may not accurate.

%% compute eigen values.
% 	fprintf('--> computating 3 EigenValues ....\n');
% 	[V, eigenvalues, exitFlag] = eigs(C, 3 + 1,'smallestabs','Display', 1);
% 	fprintf('--> Eigen values are:\n'); diag(eigenvalues)
% 
% 	% save('Result.mat', 'eigenvalues','V');
% 
% 	% Sort eigenvectors in ascending order
% 	eigenvalues = diag(eigenvalues);
% 	[eigenvalues, ind] = sort(eigenvalues, 'ascend');
% 	eigenvalues = eigenvalues(2:4);
% 	V = V(:,ind(2:4));
% 	X3d = X*V;

% 	[V, eigenvalues, exitFlag] = eigs(C, reducedDimension + 1,'smallestabs','Display', 1);

% change to new ones
% 	[V, eigenvalues, exitFlag] = eigs(C, reducedDimension + 1,'smallestabs','Display', 0);
% 
% 	% Sort eigenvectors in ascending order
% 	eigenvalues = diag(eigenvalues);
% 	[eigenvalues, ind] = sort(eigenvalues, 'ascend');
% 	eigenvalues = eigenvalues(2:reducedDimension+1);
% 	W = V(:,ind(2:reducedDimension+1));
% 	Y = X*W;
% 	

% 	[V, eigenvalues, exitFlag] = eigs(C, 3,'smallestabs','Display', 0);
	[V, D] = eig(C);
	eigenvalues = diag(D);
	% Sort eigenvectors in ascending order
% 	eigenvalues = diag(eigenvalues);
	[eigenvalues, ind] = sort(eigenvalues, 'ascend');
	eigenvalues = eigenvalues(1: reducedDimension);
	W = V(:, ind(1: reducedDimension));
	Y = X * W;
end

