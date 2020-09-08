
load('result.mat');

x1 = linspace(-0.1,1.1,100);
x2 = linspace(-0.1,1.1,100);

figure('Name','Trace');

[X1, X2] = meshgrid(x1 ,x2);
Y = (1-X1).^2 + 2*(X2 - X1.^2).^2;

contour(X1, X2, Y, 20); hold on;
plot(save_x1, save_x2, '-bo', 'LineWidth', 2);
title('Newton method'); xlabel('x'); ylabel('y');

