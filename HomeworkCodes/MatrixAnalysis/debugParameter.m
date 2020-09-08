
clc;clear;
close all;
load myColorMap.mat;
load imageData.mat;

% use outputController to control the information output from other files.
outputController = 2;		
save ('outputControl.mat', 'outputController');

allData = double(dataMatrix(:,1:end-1));
allLabels = dataMatrix(:,end);
[n, ~] = size(allData);

% KMap = 1:15;		% K is from 1 to 15
% tMap = [0.1, 0.2, 0.5, 1, 2, 4, 8, 16];
% bMap = [0.1, 0.2, 0.5, 1, 2, 4, 8, 16];

% KMap = [1,2,4,6,8,10];		% K is from 1 to 15
KMap = 6:12;
tMap = [7,8,9,10,11,12,13];
bMap = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9];


parameterMatrix = [];
averageTimes = 10;
for i = 1:length(KMap)
	for j = 1:length(tMap)
		for k = 1:length(bMap)
			K = KMap(i);	
			t = tMap(j);
			b = bMap(k);
			options = [K, t, b];
			
			correctRateSum = 0;
			for ave = 1:averageTimes
				correctRate = calculateCorrectRate(allData, allLabels, 4, 10, options);	
				correctRateSum = correctRateSum + correctRate;
			end
			correctRateAverate = correctRateSum/averageTimes;
			parameterMatrix(i,j,k) = correctRateAverate;
			fprintf('For K=%d, t=%d, b=%d, rate=%d\n', K, t, b, correctRateAverate);

		end
	end
end

save('parameterMatrix.mat', 'parameterMatrix');
h=msgbox('Parameter saved','Finised');

% 
% rate = 0;
% for i =1:10
% % 	rate = rate + calculateCorrectRate(allData, allLabels, 4, 10, [4,5,2)
% end
% rate = rate/10

% 
% plotRate = [];
% averageTimes = 10;
% for dimension = 1:10
% 	correctRateSum = 0;
% 	for ave = 1:averageTimes
% 		correctRate = calculateCorrectRate(allData, allLabels, 7, dimension)
% 		correctRateSum = correctRateSum + correctRate;
% 	end
% 	correctRateAverate = correctRateSum/averageTimes;
% 	s = sprintf('%2.2f%%', correctRateAverate*100);
% 	fprintf('Average correct rate for dimension %d is %s\n', dimension, s);
% 	plotRate = [plotRate, correctRateAverate];
% end
% 
% plot(plotRate,'-ro');
% 
% rate64 = plotRate;
% save('rate64.mat','rate73');
