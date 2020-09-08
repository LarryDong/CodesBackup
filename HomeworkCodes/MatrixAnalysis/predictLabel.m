
% predict label for test data.
% l = predictLabel(trainData, trainLabels, testData, P)

function predictedLabels = predictLabel(trainX, trainLabels, testData, P)
	predictedLabels = [];
	trainY = trainX*P;		% ___Y means the data in low dimensional space.

	for i = 1:size(testData, 1)
		y = testData(i,:) * P;		% check a new data.
		
		nearestDistance = Inf;
		nearestIndex = [];
		% find class
		for j = 1:size(trainY, 1)	% traverse all data in m-dimensions
			currentDistance = norm(y-trainY(j,:), 2);		% find Euclidean distance.
			if currentDistance<nearestDistance
				nearestDistance = currentDistance;
				nearestIndex = j;
			end
		end
		l = trainLabels(nearestIndex);
		predictedLabels = [predictedLabels; l];	% add new predicted labels.
	end

	load 'outputControl.mat';	% control the output.
	if outputController == 0
		fprintf('[Predict] Return a %d-by-1 predicted labels.\n', length(predictedLabels));
	end
end
