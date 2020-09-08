
% calculate forward direction in different types. 
% @para:
% targetFunction: any function, in f(x1,x2) style.
% x0: initial position;
% type: 1,2,999, L1, L2, L-inf norm

function dir = calculateDeirction(targetFunction, x0, type)

	dir = [0, 0];
	syms x1 x2;
	f_x = targetFunction;
% 	f_x = (1-x1)^2 + 2*(x2-x1^2)^2;
	syms f_gradient;
	f_gradient(1) = gradient(f_x,x1);
	f_gradient(2) = gradient(f_x,x2);

	gra = double(subs(f_gradient, [x1, x2], x0));
	
	if type == 1
		if  abs(gra(1)) > abs(gra(2))
			dir = [-sign(gra(1)), 0];
		else
			dir = [0, -sign(gra(2))];
		end
	elseif type == 2
		dir = -gra;
	elseif type == 999
		dir = sign(-gra);
	end
	
% 	fprintf('Direction is: [%d, %d]\n', dir(1), dir(2));
	dir = dir ./ norm(dir,2);
% 	fprintf('Change to unit vector, is: [%d, %d]\n', dir(1), dir(2));
	
end