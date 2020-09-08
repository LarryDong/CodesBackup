% draw slice of each parameters.
% for debug parameter use.

clc;clear;close all;

KMap = 6:12;
tMap = [7,8,9,10,11,12,13];
bMap = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9];

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
caxis([0.9 max(max(max(parameterMatrix)))]);

% colormap(colorMap_80To100);
xlabel('K'); ylabel('t'); zlabel('b');
xticks(1:length(KMap));
xticklabels({'6','7','8','9','10','11','12','13'})
yticks(1:length(tMap));
yticklabels({'7','8','9','10','11','12','13'});
zticks(1:length(bMap));
zticklabels({'0.1','0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9'});

%% find max-i value and index.
topK = 100;
A = parameterMatrix;
[value,index] = sort(A(:),'descend');
% max10Index = find(A(:)==max10Value);
maxKValue = value(1:topK);
maxKIndex = index(1:topK);
[x, y, z] = ind2sub(size(A), maxKIndex);

fprintf('Largest %d values and parameters are:\n', topK);
for i=1:topK
	fprintf('K=%d, t=%d, b=%d, rate:%d\n', KMap(x(i)),tMap(y(i)),bMap(z(i)),maxKValue(i));
end
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