
function value = calFunctionValue(function_fx, x)

	defaultVariables = symvar(function_fx);
	if size(defaultVariables, 2) ~= size(x,2)
		% this happens when the function and input variables have
		% differernt number.
		error('[Error]! Number of variables not match.');
	end
	value = double(subs(function_fx, defaultVariables, x));
end

