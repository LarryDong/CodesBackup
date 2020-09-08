
load result.mat

% get the size of each vector.
times = size(save_x1,2);

figure('Name','gradient vs iteration times.');
%% draw l1 lin
k1 = [];
v1 = [];
for i=1:times
	k1 = [k1, i];
end

plot(k1, log(save_value), '-bo', 'LineWidth', 2);
title('Newton method');
xlabel('iteration times'); ylabel('log f(x)');
grid on;

