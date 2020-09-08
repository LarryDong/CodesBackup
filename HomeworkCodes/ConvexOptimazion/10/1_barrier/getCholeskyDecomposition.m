
% My cholesky decomposition function.
% Input: hermitian and positive definite matrix;
% Ouput: L, for A = LL'.

function L = getCholeskyDecomposition(A)

	if ishermitian(A)==0
		error('Input matrix is not a Hermitian matrix.');
	end
	
	eig_value = eig(A);
	if (size(find(eig_value > 0),1)) < size(A,1)
% 		error('Input matrix is not positive definite.');
		fprintf('[Warning] Input matrix is not positive definite.\n');
	end
	

	% create a new L to save value.
	L = zeros(size(A));		% L is not necessary
	Columns = size(A,2);
	
	for col = 1:Columns
		
		diagSum = 0;
		for i = 1:col-1
			diagSum = diagSum + L(col,i)^2;		
		end
		L(col,col) = sqrt(A(col,col) - diagSum);% first calculate L(i,i)
		
		recursiveValue = 0;
		for i = 1:col-1
			recursiveValue = recursiveValue + L(col,i).*L(:,i);
		end
		L(:,col) = (A(:,col) - recursiveValue) ./L(col,col);
	end
	
	epsilon = 10^(-8);
	L = L.*(abs(L)>epsilon);		% elimate small item, to make L a lower triangluar matrix.
	
end





