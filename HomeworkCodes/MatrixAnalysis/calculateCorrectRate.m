

% calculate correct rate under current parameters.
% rate = fun(X, l, t-num, d, paraOptions)
% @X,l, input data and correspoding labels.
% @t-num, train number in each group. Randomly select.
% @d, reduce to d-dimension
% @paraOptions, a option, [K, t, b] = [4, 5, 1] by default.
%			Created by larrDong, 2019/1/5, 22:14
%			Last changed, 2-19/1/7, 21:21


function correctRate = calculateCorrectRate...
		(allData, allLabels, trainNumber, dimension, paraOptions)
	
	load 'outputControl.mat';
	correctRate = 0;
	
	if nargin == 4
		paraOptions = [4, 5, 1];	% default parameters for K, t, b.
	end
	
	% check input paramters.
	if( size(allData,1)~=400 || length(allLabels)~=400 || trainNumber>10 ...
		|| trainNumber<=0 || trainNumber>=10 || length(paraOptions)~=3)
		error('Invalid input parameters...');
	end
	
	if outputController == 0 || outputController == 1
		fprintf('[Train1] Train once. Train: %d, Test: %d, dimension: %d\n', ...
			trainNumber, 10-trainNumber, dimension);
	end
	
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

	P = trainProjectionMatrix(trainData, trainLabels, dimension, paraOptions);
	predictedLabels = predictLabel(trainData, trainLabels, testData, P);

	correctRate = sum(predictedLabels==testLabels) / totalTestNumber;
	r = sprintf('%2.2f%%', correctRate*100);
	
	if outputController == 0 || outputController == 1
		fprintf('[Train] Correct rate: %s\n', r);
	end
	
% 	fprintf('t1-%d, t2-%d, t3-%d, t4-%d\n', t1, t2, t3, t4);
end





