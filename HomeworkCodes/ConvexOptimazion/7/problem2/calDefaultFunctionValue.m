
function value = calDefaultFunctionValue(A_50, x)
	
% 	value = -sum(log(1-A_50'*x)) - sum(log(1-x.^2));
% 	value = (x(1)-1)^2 + x(2)^2; 

	sum1 = sum(log(1-A_50'*x));
	sum2 = sum(log(1-x.^2));
	value = -sum1 -sum2;
end