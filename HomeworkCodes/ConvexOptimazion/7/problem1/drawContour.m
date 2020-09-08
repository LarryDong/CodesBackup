
load('result.mat');

x1 = linspace(-0.1,1.1,100);
x2 = linspace(-0.1,1.1,100);

figure('Name','Trace');

[X1, X2] = meshgrid(x1 ,x2);
Y = (1-X1).^2 + 2*(X2 - X1.^2).^2;
subplot(1,3,1);
contour(X1, X2, Y, value_l1); hold on;
plot(x_l1, y_l1, '-bo', 'LineWidth', 2);
title('L1'); xlabel('x'); ylabel('y');


[X1, X2] = meshgrid(x1 ,x2);
Y = (1-X1).^2 + 2*(X2 - X1.^2).^2;
subplot(1,3,2);
contour(X1, X2, Y, value_l2); hold on;
plot(x_l2, y_l2, '-go', 'LineWidth', 2);
title('L2'); xlabel('x'); ylabel('y');


[X1, X2] = meshgrid(x1 ,x2);
Y = (1-X1).^2 + 2*(X2 - X1.^2).^2;
subplot(1,3,3);
contour(X1, X2, Y, value_linf); hold on;
plot(x_linf, y_linf, '-ro', 'LineWidth', 2);
title('L-inf'); xlabel('x'); ylabel('y');;
