
% calculate min point on current direction, with initial point x0
% Format: stepSize = searchLine(x0, dir)
% Input: x0, initial point
% Input: dir, search direction
% Output: step length at current direction, a scalar.

function stepSize = searchLine(x0, dir)


%%  calculate sinlge valley range, on current direction.
gamma = 1.1;
delta = 1;
i = 1;
f_a = 1;
f_b = 0;

while f_a > f_b			% left is larger than right, continue to find
	a = gamma^(i-1) * delta;
	b = gamma^i * delta;
	i = i+1;
	f_a = calculateFunctionValue(x0 + a*dir);
	f_b = calculateFunctionValue(x0 + b*dir);
% 	fprintf('a and b is: %d, %d\n', a, b);
end
range = [min(0,b), max(0,b)];
% fprintf('Single valley is :[%d, %d]\n', range(1),range(2));


%% find valley point, using 0.618 method.
a = range(1);
b = range(2);
c = (sqrt(5)-1)/2;
epsilon = 10^(-9);

s1 = a + (1-c)*(b-a);
s2 = a + c*(b-a);


while b-a > epsilon 
	value1 = calculateFunctionValue(x0 + s1*dir);
	value2 = calculateFunctionValue(x0 + s2*dir);
	if value1 > value2		% left>right, reserve [s1, b]
		a = s1;
		s1 = s2;
		s2 = a + c*(b-a);
% 		fprintf('remain right part. [%d, %d]\n', a,b);
	else			% left <= right, reserve [a, s2]
		b = s2;
		s2 = s1;
		s1 = a + (1-c)*(b-a);
% 		fprintf('remain left part. [%d, %d]\n', a,b);
	end
end



%%
stepSize = (b+a)/2;


