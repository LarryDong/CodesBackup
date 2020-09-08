

load result.mat

pStar = min(save_value);


% draw error lines.
error = log(save_value - pStar);

x = 1:size(save_x,2);

% figure('Name','Newton method');

subplot(1, 2, 1);
plot(x, error, '-b', 'LineWidth', 2);
title('log(f-p*)');
xlabel('iteration times'); ylabel('log(f-p*)');
hold on;

subplot(1, 2, 2);
plot(x, save_stepLength, '-r', 'LineWidth', 2);
title('step length');
xlabel('iteration times'); ylabel('step size(t)');
