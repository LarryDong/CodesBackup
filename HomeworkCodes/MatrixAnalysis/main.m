
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

% KMap = 6:12;
% tMap = [7,8,9,10,11,12,13];
% bMap = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9];


K = 9;
t = 10;
b = 0.3;
options = [K, t, b];

averageTimes = 10;		% to calculate average value. 10 times.
maxDimension = 30;
testNumberMark = zeros(10,1);		% to record which test number has used.
correctRateMatrix = zeros(10, maxDimension);	% a 10-by-d matrix. row is which test number, col is correct rate at dimension-d.

for testNumber = [4,5,6]	% check rate in different test number.
	testNumberMark(testNumber) = 1;	% record corresponding number.
	
	for dimension = 1:maxDimension
		correctRateSum = 0;
		for ave = 1:averageTimes
			r = calculateCorrectRate(allData, allLabels, testNumber, dimension, options);	
			correctRateSum = correctRateSum + r;
		end

		correctRateAverate = correctRateSum/averageTimes;
		correctRateMatrix(testNumber, dimension) = correctRateAverate;
		fprintf('At test number: %d, dimension %d, rate %d\n', testNumber, dimension, correctRateAverate);
	end
end

save ('dimensionCurve.mat', 'testNumberMark','correctRateMatrix');
h=msgbox('Parameter saved','Finised');


