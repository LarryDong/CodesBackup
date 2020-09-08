
load result.mat

% get the size of each vector.
y1Size = size(y_l1,2);
y2Size = size(y_l2,2);
yinfSize = size(y_l2,2);

figure('Name','gradient vs iteration times.');
%% draw l1 lin
k1 = [];
v1 = [];
for i=1:fix(y1Size/10)
	k1 = [k1, i*10];
	v1 = [v1, value_l1(i*10)];
end
subplot(1, 3, 1);
plot(k1, log(v1), '-bo', 'LineWidth', 2);
title('l1 norm');
xlabel('iteration times'); ylabel('log f(x)');
grid on;


%% l2
k2 = [];
v2 = [];
for i=1:fix(y2Size/10)
	k2 = [k2, i*10];
	v2 = [v2, value_l2(i*10)];
end
subplot(1, 3, 2);
plot(k2, log(v2), '-go', 'LineWidth', 2);
title('l2 norm');
xlabel('iteration times'); ylabel('log f(x)');
grid on;


%% linf
kinf = [];
vinf = [];
for i=1:fix(yinfSize/10)
	kinf = [kinf, i*10];
	vinf = [vinf, value_l2(i*10)];
end
subplot(1, 3, 3);
plot(kinf, log(vinf), '-ro', 'LineWidth', 2);
title('l-inf norm');
xlabel('iteration times'); ylabel('log f(x)');
grid on;
