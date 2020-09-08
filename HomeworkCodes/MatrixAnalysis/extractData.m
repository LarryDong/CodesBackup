% extract part of data X.
% [subX, newLabels] = extractData(X, labels, index)
% @index, a vector, [i,...] to extract data at index i.

% abondoned by 2019/1/5 21:08

function [subX, newLabels] = extractData(X, labels, index, showInfoFlag)
	
	error('Please not use this function any more...');

	if showInfoFlag
		fprintf('==> extracting from %d-th to %d-th.\n', indexBegin, indexEnd);
	end
	
	if length(index)-length(unique(index)) ~= 0
		fprintf('
	
	subX = [];
	newLabels = [];
	for i = 1:40
		i1 = (i-1)*10 + indexBegin;
		i2 = (i-1)*10 + indexEnd;
		subX = [subX; X(i1:i2, :)];
		newLabels = [newLabels; labels(i1:i2)];
	end
	fprintf('<== extracted. return a %d-by-%d matrix\n', size(subX,1), size(subX,2));
end

