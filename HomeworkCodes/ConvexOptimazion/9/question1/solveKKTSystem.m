
function [v, w] = solveKKTSystem(H, A, g, h)
	
	if (size(H,1) ~= size(g,1))
		error('Dimension Error 1!');
	end
	if (size(A,1) ~= size(h,1))
		error('Dimension Error 2!');
	end
	
	v = zeros(size(H,2));
	w = zeros(size(A', 2));
	
	invH_AT = inv(H)*A';
	invH_g = inv(H)*g;
	S=  -A*inv(H)*

end