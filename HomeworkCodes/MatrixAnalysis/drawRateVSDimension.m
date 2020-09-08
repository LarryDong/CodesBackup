
% draw recognition rate VS dimension change.


close all;

load dimensionCurve.mat;

figure('Name', 'rate-vs-dimension');

plot(correctRateMatrix(4,:),'-ro','LineWidth',2); hold on;
plot(correctRateMatrix(5,:),'-g*','LineWidth',2); hold on;
plot(correctRateMatrix(6,:),'-b^','LineWidth',2); hold on;
grid on;

legend('4 tests', '5 tests', '6 tests' , 'Location','southeast');
title('correct rates VS dimensions');
xlabel('dimensions');
ylabel('correct rate');
