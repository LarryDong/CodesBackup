
% calculate the gradient of a certain function.

function gra = calDefaultFunctionGradient(A, x)

	sum1 = 0;
	sum2 = 0;
	for i=1:size(A,1)
		sum1 = sum1 + (-A(:,i)) / (1 - A(:,i)'*x);
		nom = zeros(size(A,1),1);
		nom(i) = x(i);
		sum2 = sum2 + (-2*nom/(1-x(i)^2));
		gra = -sum1 - sum2;
	end
end
