clc;clear;close all;

KMap = 1:15;
tMap = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
bMap = [0, 0.1, 0.2, 0.5, 1];

load parameterMatrix.mat;
k = 1:length(KMap);
t = 1:length(tMap);
b = 1:length(bMap);

% [X, Y, Z] = meshgrid(y,x,z);
[K, T, B] = meshgrid(k, t, b);

V = zeros(size(K));
counter = 0;
for i = 1:length(t)
	for j = 1:length(b)
		for p = 1:length(k)
			V(i,p,j) = parameterMatrix(p,i,j);
			counter = counter + 1;
		end
	end
end

% slice(K, T, B, V, k, [], []);
slice(K, T, B, V, [], [], b);
% slice(K, T, B, V, [], t, []);

load colorMap_80To100.mat;

colorbar;
colormap(colorMap_80To100);
caxis([0.8 max(max(max(parameterMatrix)))]);

% colormap(colorMap_80To100);
xlabel('K'); ylabel('t'); zlabel('b');
xticks(1:15);
xticklabels({'1','2','3','4','5','6','7','8','9','10','11','12','13','14','15'})
yticks(1:length(tMap));
yticklabels({'1','2','3','4','5','6','7','8','9','10'});
zticks(1:length(bMap));
zticklabels({'0','0.1','0.2','0.5','1'});


% hc = colorbar;
% colormap([1 0 0;0 1 0])
% set(hc,'YTick',[0 0.5 1])
% set(hc,'YTickLabel',[0 0 0 ])



% h=msgbox('Parameter saved','Finised');

	
% [K, T, B] = 


% 
% for i=1:10
% 	for j=1:10;
% V = parameterMatrix(X,Y,Z);

% % 
% 
% plot3(parameterMatrix(