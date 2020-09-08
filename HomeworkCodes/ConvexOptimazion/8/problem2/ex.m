clc;clear;

syms x1 x2;

f = (1-x1)^2+2*(x2-x1^2)^2;

gra = gradient(f,[x1,x2]);
pretty(gra)

gra2 = zeros(2,2);
gradient(gra(1), [x1,x2])
gradient(gra(2), [x1,x2])


