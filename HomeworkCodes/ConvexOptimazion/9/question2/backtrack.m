

% find the step size of current dir.
function stepSize = backtrack(x0, dir)

	epsilon = 10^(-10);
	alpha = 0.01;
	belta = 0.5;
	
	gra = calculateGradient(x0);
	
	if norm(gra,2) < epsilon
		stepSize = 0;
		return;
	end
	
	t = 1;
	times = 0;
	
	f_now = calculateFunctionValue(x0);	% x0 must be valid point, in Dom F.
	potentialDeltaX = x0 + t*dir;		% to see whether new point is in Dom F.
	inRange = 0;
	if sum(sum(potentialDeltaX<=0)) == 0		% if has no negative values, in Dom F.
			inRange = 1;
	else
			inRange = 0;
	end
	f_new = calculateFunctionValue(x0 + t*dir);
	
	% to find deltaX, and to make sure that deltaX is also in DomF
	while (f_new > f_now + alpha*t*gra'*dir) || (inRange == 0)
		
		times = times + 1;
		t = t*belta^times;
		deltaX = x0 + t*dir;
		
		% avoid out of Domf
		negativeNumber = sum(sum(deltaX<=0));
		if negativeNumber == 0
				inRange = 1;
		else
				inRange = 0;
		end
		f_new = calculateFunctionValue(A,deltaX);
	end
	
	stepSize = t;
	
end

