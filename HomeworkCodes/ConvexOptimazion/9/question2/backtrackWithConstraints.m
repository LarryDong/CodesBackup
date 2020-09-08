

% find the step size of current dir.
function stepSize = backtrackWithConstraints(x, v, dirX, dirV)

	epsilon = 10^(-12);
	alpha = 0.01;
	belta = 0.5;
	
	gra = calculateGradient(x);
	
	if norm(gra,2) < epsilon
		stepSize = 0;
		return;
	end
	
	t = 1;
	times = 0;
	
	% the x and v must be in domain, at the beginning of calculation.
	[r_dual, r_pri] = calculateResidualValue(gra, x, v);
	residualValueOld = norm([r_dual; r_pri], 2);
	
	potensialDeltaX = x + t*dirX;
	potensialDeltaV = v + t*dirV;
	
	% check whether new point is in Dom f.
	xIsInDomain = 0;
	if sum(potensialDeltaX<0) == 0		% log(x) requires x>0.
		xIsInDomain = 1;
	else
		xIsInDomain = 0;
	end
	% since for dual-problem, there is no constraints for v,
	% so do not check v in Dom f.
	
	graNew = calculateGradient(potensialDeltaX);
	[r_dual_new, r_pri_new] = calculateResidualValue(graNew, potensialDeltaX, potensialDeltaV);
	residualValueNew = norm([r_dual_new; r_pri_new], 2);
	

	% 
	while (residualValueNew > (1-alpha*t)*residualValueOld) || (xIsInDomain == 0)
		
% 		times = times + 1;
		t = t*belta;
		
		potensialDeltaX = x + t*dirX;
		potensialDeltaV = v + t*dirV;
		
		% avoid out of Domf
		num  = sum(potensialDeltaX<0);
		if sum(potensialDeltaX<0) == 0
			xIsInDomain = 1;
		else
			xIsInDomain = 0;
		end
		
		% update the value of r_old, r_new.
		residualValueOld = residualValueNew;

		graNew = calculateGradient(potensialDeltaX);
		[r_dual_new, r_pri_new] = calculateResidualValue(graNew, potensialDeltaX, potensialDeltaV);
		residualValueNew = norm([r_dual_new; r_pri_new], 2);
	end
	
	stepSize = t;
	
end

