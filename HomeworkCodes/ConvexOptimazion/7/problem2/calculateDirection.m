
% calculate forward direction in different types. 
% @para:
% A: A_50 or A_100
% x0: initial position;
% type: 1,2,999, L1, L2, L-inf norm

function dir = calculateDirection(A, x0, type)

	gra = calDefaultFunctionGradient(A, x0);
	
	if type == 1	% l1-norm should be [0,0,...,-1(No.i^th),0,0..]
		[useless,index] = max(abs(gra));
		dir = zeros(size(A,1),1);
		dir(index) = -sign(gra(index));
	elseif type == 2
		dir = -gra;
	elseif type == 999
		dir = sign(-gra);
	else
		error('[Error]! Type of l-norm is not correct!');
	end
	
	dir = dir ./ norm(dir,2);	% change to unit-vector.
end
