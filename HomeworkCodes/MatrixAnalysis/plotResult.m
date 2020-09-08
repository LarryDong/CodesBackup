% plot (2D) data to see the results.
% handler = fun(inputData, labels, dontDrawIndex, figureHandler, colorMap)

% @inputData, n-by-2 data.
% @labels
% @dontDrawIndex, for some certain index, do not draw. Because these may far-away groups.
% @figureHandler, draw all figure in input handler.
% @colorMap, load colormap to make color the same.

function handler = plotResult(inputData, labels, dontDrawIndex, figureHandler, colorMap)
	
	if size(inputData, 2) ~= 2
		error('Input data should be in 2-dimensions');
	end

	X = inputData;
% 	fprintf('--> ploting data in 2d\n');
	handler = figure(figureHandler);
	tbl = tabulate(labels);
	labelNumber = size(tbl,1);
	X3d = inputData;
	for i = 1:labelNumber
		if ismember(i,dontDrawIndex)
			continue;
		end
		numberInGroup = tbl(i,2);	% number in each label can be different(extracted 6, for example)
		i1 = (i-1)*numberInGroup + 1;
		i2 = i*numberInGroup;
% 		scatter3(X3d(i1:i2, 1), X3d(i1:i2, 2), X3d(i1:i2,3), 'SizeData', 50);
		scatter(X(i1:i2, 1), X(i1:i2, 2), 'SizeData', 50, 'MarkerEdgeColor', colorMap(i,:));
		hold on;
		plot(X(i1:i2, 1), X(i1:i2, 2), 'LineWidth', 1, 'Color', colorMap(i,:));
		hold on;
% 		findFarawayPoints(X(i1:i2,:));
	end

	pictureName = strcat('K= ',num2str(4), ', t=',num2str(5), ', b=', num2str(1));
	title([pictureName]);
end

