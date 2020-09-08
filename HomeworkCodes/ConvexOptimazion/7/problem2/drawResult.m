

load result.mat

m1 = min(f1);
m2 = min(f2);
mInf = min(finf);
pStar = min([m1, m2, mInf]);

if sum([find(f1-pStar<0), find(f2-pStar<0), find(finf-pStar<0)]) > 0
	fprintf('p star is not the smallest...\n');
end

%% draw error lines.
error1 = log(f1 - pStar);
error2 = log(f2 - pStar);
errorinf = log(finf - pStar);

x1 = 1:size(f1,2);
x2 = 1:size(f2,2);
xinf = 1:size(finf,2);

% figure('Name','log(f-p*)');

subplot(1, 3, 1);
plot(x1, error1, '-b', 'LineWidth', 2);
title('l1 norm');
xlabel('iteration times'); ylabel('log(f-p*)');
hold on;

subplot(1, 3, 2);
plot(x2, error2, '-g', 'LineWidth', 2);
title('l2 norm');
xlabel('iteration times'); ylabel('log(f-p*)');
hold on;

subplot(1, 3, 3);
plot(xinf, errorinf, '-r', 'LineWidth', 2);
title('l-inf norm');
xlabel('iteration times'); ylabel('log(f-p*)');
hold on;
% 
% %% draw step lines.
% figure('Name','step size');
% subplot(1, 3, 1);
% plot(x1, t1, '-b', 'LineWidth', 0.5);
% title('l1 norm');
% xlabel('iteration times'); ylabel('step size(t)');
% hold on;
% 
% subplot(1, 3, 2);
% plot(x2, t2, '-g', 'LineWidth', 0.5);
% title('l2 norm');
% xlabel('iteration times'); ylabel('step size(t)');
% hold on;
% 
% subplot(1, 3, 3);
% plot(xinf, tinf, '-r', 'LineWidth', 0.5);
% title('l-inf norm');
% xlabel('iteration times'); ylabel('step size(t)');
% hold on;

