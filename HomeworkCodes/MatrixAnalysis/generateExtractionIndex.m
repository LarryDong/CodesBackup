
% generate index to extract data from all.

% index = fun(dataInput, labels, numberInEachGroup)
% @numberInEachGroup, randomly extract n data in each group.
% @index, return a (10*n)-by-dimension data set.

function index = generateExtractionIndex(dataInput, labels, numberInEachGroup)
	
	load 'outputControl.mat';
	if numberInEachGroup>=10 || numberInEachGroup<=0
		error('Invalid selected number...');
	end
	
	% dataInput should be 400-by-dim
	index = [];
	for i=1:40
		selectedNumber = randperm(10, numberInEachGroup);
		index = [index, (i-1)*10 + selectedNumber];
	end
	
	if outputController == 0
		fprintf('[Extract Data] Select %d data from all(%d) \n', length(index), size(dataInput,1));
	end
end