
function gra = calculateGradient(t, x)
	
	load P.mat;
	load q.mat;
	
	% gradient concludes: t*f0 and fai(x)
	gra = t*(P*x + q) + (- 1./x);
	
end