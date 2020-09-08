
% use deafault function instead of 'subs' method to increase speed.

function value = calculateFunctionValue(x)
% 	value = 1/2*(x(1)^2+x(2)^2);
	value = (1-x(1))^2 + 2*(x(2)-x(1)^2)^2;
end
