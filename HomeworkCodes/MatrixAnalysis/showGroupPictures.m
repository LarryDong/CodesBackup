
% showGroupPictures(groupNumber, specialOnes)
% show a group of pictures.
% @groupNumber, 1,2,3...
% @specialOnes, mark the special pictures by title '-- x --'
% @hander, maybe not necessary, but still pass it out.
function handler = showGroupPictures(groupNumber, specialOnes)
% 	fprintf('==> plot pictures of sample %d\n', groupNumber);
	handler = figure('Name', strcat('Sample ',num2str(groupNumber)));
	databasePath = '..\database\orl_faces\';
	filePath = strcat(databasePath,'s', num2str(groupNumber));
	
	for i=1:10
		subplot(2, 5, i);
		img = imread(strcat(filePath,'\',num2str(i),'.pgm'));
		imshow(img);
		if ismember(i,specialOnes)	% if i is a sepcail index, show special marks.
			title(strcat('-- ', num2str(i), ' --'));
		else
			title(num2str(i));
		end
	end
% 	fprintf('==> Show picture: s%d, %d\n', sampleNumber, pictureNumber)
end
