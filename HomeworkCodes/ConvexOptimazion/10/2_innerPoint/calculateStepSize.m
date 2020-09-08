
% t = fun((x, deltaX, lambda, deltaLambda)
% calculate step length use 't=0.99max{}' method.

function t = calculateStepSize(x, deltaX, lambda, deltaLambda)

	% attention that x and lambda is always positive.
	detlaX_negative_index = find(deltaX<0);
	tao1 = -max( x(detlaX_negative_index) ./ deltaX(detlaX_negative_index));
	
	detlaLambda_negative_index = find(deltaLambda<0);
	tao2 = -max( lambda(detlaLambda_negative_index) ./ deltaLambda(detlaLambda_negative_index));
	
	% attention that max(tao) for both at the same time 
	% is min(max(t1),max(t2)) for each one.
	t = 0.99 * min(tao1, tao2);
	
	% if both deltaX and deltaLambda is positive, t is free, set to 1.
% 	if isempty(t) || t>1
	if isempty(t)
% 		t = 1;
		error('Error accure...');
	end
	
	% check.
% 	t
% 	min(x+tao1*deltaX)
% 	min(lambda + tao2*deltaLambda)
% 	
% 	min(x+t*deltaX)
% 	min(lambda + t*deltaLambda)
	
end


