
% use backward elimation to find x of x=Ab;
function x = backwardElimation(A, b)

	if istriu(A) ~= 1
		error('Matrix must be a upper trianglular matrix.');
	end
	if rank(A) ~= size(A,1)
		error('Matrix must be full-rank.');
	end
	
	dim = size(A,1);
	x = zeros(dim,1);
	for i = dim:-1:1
		crossSum = 0;
		for j = dim:-1:i
			crossSum = crossSum + A(i,j)*x(j);
		end
		x(i) = (b(i) - crossSum) ./ A(i,i);
	end
	
end
