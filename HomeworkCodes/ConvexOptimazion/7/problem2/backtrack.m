

% find the step size of current dir.
function stepSize = backtrack(A, x0, dir)

	epsilon = 10^(-6);
	alpha = 0.1;
	belta = 0.8;
	
	gra = calDefaultFunctionGradient(A, x0);
	
	if norm(gra,2) < epsilon
		stepSize = 0;
		return;
	end
	
	t = 1;
	times = 0;
	
	f_now = calDefaultFunctionValue(A, x0);
	f_new = calDefaultFunctionValue(A, x0 + t*dir);
	
	inRange = 0;
	
	while (f_new > f_now + alpha*t*gra'*dir) || (inRange == 0)
		
		times = times + 1;
		t = t*belta^times;
		deltaX = x0 + t*dir;
% 		check = find(A'*deltaX>=1);
		% avoid out of Domf
		st1 = size(find(A'*deltaX>=1), 1);
		st2 = size(find(deltaX.*deltaX>=1), 1);
		if st1<=0.5 && st2<=0.5		% no value larger than 1
			inRange = 1;
		else
			inRange = 0;
		end
		f_new = calDefaultFunctionValue(A,deltaX);
	end
	
	stepSize = t;
	
	st1 = size(find(A'*deltaX>=1), 1);
	st2 = size(find(deltaX.*deltaX>=1), 1);
	if st1<=0.5 && st2<=0.5		% no value larger than 1
			inRange = 1;
	else
			inRange = 0;
	end
		
% 	fprintf('t = %d\n', t);
	
end