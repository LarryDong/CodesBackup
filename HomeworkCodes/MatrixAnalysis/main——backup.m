



% showCertainPicture(6,4);
clc;clear;
close all;
load myColorMap.mat;
load imageData.mat;

allData = double(dataMatrix(:,1:end-1));
allLabels = dataMatrix(:,end);
[n, ~] = size(allData);

% for debug, only choose one group.
% allData = allData(1:10, :);
% allLabels = allLabels(1:10, :);

trainNumber = 4;
testNumber = 10 - trainNumber;
totalTrainNumber = trainNumber * 40;
totalTestNumber = testNumber * 40;

trainIndex = generateExtractionIndex(allData, allLabels, trainNumber);

trainData = allData(trainIndex, :);
trainLabels = allLabels(trainIndex);

allIndex = 1:400;
allIndex(trainIndex) = [];	% delete trainIndex
testIndex = allIndex;

testData = allData(testIndex, :);
testLabels = allLabels(testIndex);

for dimension = 1:10
% dimension = 2;
	P = trainProjectionMatrix(trainData, trainLabels, dimension, [4,5,1]);
	predictedLabels = predictLabel(trainData, trainLabels, testData, P);
	correctRate = sum(predictedLabels==testLabels) / totalTestNumber
end

figure('Name', 'Rate');
plot(correctRate, '-o', 'LineWidth', 2, 'Color', 'r');
xlabel('dimension');
ylabel('rate %');
axis([0, 30, 0, 1]);
title('train:6, test:4');
print('train6test4', '-dpng');	% save pictures.



