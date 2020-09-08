

load result.mat
load('A_m=30_n=100.mat');		% load matrix A



pStar = save_value(end);

% draw error lines.
error = log(save_value - pStar);

x = 1:size(save_x,2);

figure('Name','Newton method');
plot(x, error, '-ob', 'LineWidth', 2);
title('log(f-p*)');
xlabel('iteration times'); ylabel('log(f-p*)');
hold on;
