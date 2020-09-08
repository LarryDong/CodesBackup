
% use forward elimation to find M of LX=A;
function X = forwardElimation_MatrixForm(L, A)

	if istril(L) ~= 1
		error('Matrix must be a lower trianglular matrix.');
	end
	if rank(L) ~= size(A,1)
		error('Matrix must be full-rank.');
	end
	
	Rows = size(L,1);
	Columns = size(A,2);
	
	X = zeros(Rows, Columns);
	
	for i = 1:Rows
		crossSum = 0;
		for j = 1:i-1
			crossSum = crossSum + L(i,j)*X(j,:);
		end
		% sum
		X(i,:) = (A(i,:) - crossSum) ./ L(i, i);
	end
	
end
