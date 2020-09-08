
% use forward elimation to find x of x=Ab;
function x = forwardElimation(A, b)

	if istril(A) ~= 1
		error('Matrix must be a lower trianglular matrix.');
	end
	if rank(A) ~= size(A,1)
		error('Matrix must be full-rank.');
	end
	
	dim = size(A,1);
	x = zeros(dim,1);
	for i = 1:dim
		crossSum = 0;
		for j = 1:i-1
			crossSum = crossSum + A(i,j)*x(j);
		end
		x(i) = (b(i) - crossSum) ./ A(i,i);
	end
	
end
