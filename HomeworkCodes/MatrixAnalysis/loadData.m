
% fprintf('---------------Matlab2018b-----------------\n');
fprintf('-------------Matlab2018b---------------\n');
fprintf('loading data from orl_faces...\n');

dataMatrix = [];
databasePath = '..\database\orl_faces\';
sampleFileList = dir(strcat(databasePath, 's*'));
for s = 1:length(sampleFileList)		% outer loop for s01-s40
% 	fprintf('--> Change to sample %d\n', s);
	firstDir = strcat(databasePath, sampleFileList(s).name);
	secondDir = dir(strcat(firstDir, '\', '*.pgm'));
	for i=1:length(secondDir)			% inner loop for 1.pgm-10.pgm\
		% for matlab2018b
		% img = imread(strcat(secondDir(i).folder, '\', secondDir(i).name));
		
		% for matlab2014a
		img = imread(strcat(firstDir, '\', secondDir(i).name));
		img = imresize(img, [32,32]);	% resize to make 32x32
% 		imshow(img);
% 		pause(0.01);
		dataMatrix = [dataMatrix; [img(:)', s]];
	end
end
% whos dataMatrix;
save('imageData.mat', 'dataMatrix');
fprintf('data saved in ''imageData.mat'' as ''dataMatrix''\n');

fprintf('--> data size: %d x %d\n', size(dataMatrix, 1), size(dataMatrix, 2));
fprintf('----------------------------------------\n');



