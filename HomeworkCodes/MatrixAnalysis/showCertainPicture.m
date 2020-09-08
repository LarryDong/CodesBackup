
function showCertainPicture(sampleNumber, pictureNumber)
	databasePath = '..\database\orl_faces\';
	filePath = strcat(databasePath,'s', num2str(sampleNumber));
	img = imread(strcat(filePath,'\',num2str(pictureNumber),'.pgm'));
	fprintf('==> Show picture: s%d, %d\n', sampleNumber, pictureNumber)
	imshow(img);
end

