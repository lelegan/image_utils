function grayImage = rgb2gray_safe(inputImage)

[M N P] = size(inputImage);

if (P == 1),
	grayImage = inputImage;
elseif (P == 3),
	grayImage = rgb2gray(inputImage);
else
	error('There is some problem with the input file.');
end;
