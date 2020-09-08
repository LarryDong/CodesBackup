
% calculate step length at current t-value.

function stepSize = backtrackWithConstraints(input_t, x, mu, dirX, dirMu)

	load A.mat;
	load b.mat;
	
	epsilon = 10^(-20);
	alpha = 0.01;
	belta = 0.8;
	
	gra = calculateGradient(input_t, x);
	
	if norm(gra,2) < epsilon
		stepSize = 0;
		return;
	end
	
	t = 1;		% Attention that variable 't' is conflict.
	
	% the x and v must be in domain, at the beginning of calculation.
	[r_dual, r_pri] = calculateResidualValue(input_t, x, mu);
	
	residualValueOld = norm([r_dual; r_pri], 2);
	
	potensialDeltaX = x + t*dirX;
	potensialDeltaMu = mu + t*dirMu;
	
	% check whether new point is in Dom f.
	xIsInDomain = 0;
	if sum(potensialDeltaX<0) == 0		% log(x) requires x>0.
		xIsInDomain = 1;
	else
		xIsInDomain = 0;
	end
	% since for dual-problem, there is no constraints for v,
	% so do not check v in Dom f.
	
	[r_dual_new, r_pri_new] = calculateResidualValue(input_t, potensialDeltaX, potensialDeltaMu);
	residualValueNew = norm([r_dual_new; r_pri_new], 2);
	

	while (residualValueNew > (1-alpha*t)*residualValueOld) || (xIsInDomain == 0)
		
% 		debug_error = residualValueNew-(1-alpha*t)*residualValueOld;

		t = t*belta;
		potensialDeltaX = x + t*dirX;
		potensialDeltaMu = mu + t*dirMu;
		
		% avoid out of Domf
		num  = sum(potensialDeltaX<0);
		if sum(potensialDeltaX<0) == 0
			xIsInDomain = 1;
		else
			xIsInDomain = 0;
		end
		
		[r_dual_new, r_pri_new] = calculateResidualValue(input_t, potensialDeltaX, potensialDeltaMu);
		residualValueNew = norm([r_dual_new; r_pri_new], 2);
	end
	
	
	
	stepSize = t;
end


	