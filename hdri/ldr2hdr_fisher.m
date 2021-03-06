function hdr = ldr2hdr_fisher(ldrIms, exposures, minLimit, maxLimit, sigmasq, maxVal)
%	Combine different exposures using Fisher scoring (Carl N. Morris), to
%	obtain the minimum variance estimate. Note that this assumes that images
%	are taken under the same aperture and sensitivity (ISO) setting.
%
%	The default parameters assume 8-bit LDR images and require no
%	radiometric calibration for the sensor noise.
%
%	Input:
%	ldrIms		-	M x N x S exposure stack of S LDR images, where M x N the
%						size of each image, and S the number of exposures.
%	exposures	-	S x 1 vector of exposure times.
%	minLimit		-	Underexposure value, in gray tones (default 5).
%	maxLimit		-	Overexposure value, in gray tones (default 250).
%	sigmasq		-	Noise variance from radiometric calibration (default 0).
%	maxVal		-	Maximum gray tone of original LDR images (default 255).

[M N numIms] = size(ldrIms);
if (length(exposures) ~= numIms),
	error('Number of exposure values must be equal to number of LDR images.');
end;

if ((nargin < 3) || (isempty(minLimit))),
	minLimit = 5;
end;

if ((nargin < 4) || (isempty(maxLimit))),
	maxLimit = 250;
end;

if ((nargin < 5) || (isempty(sigmasq))),
	sigmasq = 0;
end;

if ((nargin < 6) || (isempty(maxVal))),
	maxVal = 255;
end;

numPixels = M * N;
ldrIms = reshape(ldrIms, [numPixels, numIms]);
numerator = zeros(numPixels, 1);
denominator = zeros(numPixels, 1); 

someUnderExposed = false(numPixels, 1);
someOverExposed = false(numPixels, 1);
someProperlyExposed = false(numPixels, 1);

% Construct the HDR image by iterating over the LDR images.
for iterIm = 1:numIms,

	ldr = ldrIms(:, iterIm);
	t = exposures(iterIm);

	underExposed = ldr < minLimit;
	someUnderExposed = someUnderExposed | underExposed;

	overExposed = ldr > maxLimit;
	someOverExposed = someOverExposed | overExposed;

	properlyExposed = ~(underExposed | overExposed);
	someProperlyExposed = someProperlyExposed | properlyExposed;

	if (sigmasq == 0),
		numerator(properlyExposed) = numerator(properlyExposed) ...
											+ t;
		denominator(properlyExposed) = denominator(properlyExposed) ...
											+ t ^ 2 ...
											./ double(ldr(properlyExposed));
	else
		numerator(properlyExposed) = numerator(properlyExposed) ...
											+ t .* double(ldr(properlyExposed)) ...
											./ (double(ldr(properlyExposed)) + sigmasq);
		denominator(properlyExposed) = denominator(properlyExposed) ...
											+ t .^ 2 ...
											./ (double(ldr(properlyExposed)) + sigmasq);
	end;
end;

% Althought the latter makes more sense, it destroys specularities...
hdr = numerator ./ denominator;
% hdr(someProperlyExposed) = numerator(someProperlyExposed) ...
% 								./ max(denominator(someProperlyExposed), 1);

hdr = fillNeverProperlyExposedPixels(hdr, ...
						someUnderExposed, someOverExposed, someProperlyExposed);

hdr = hdr / maxVal;
hdr = single(hdr);
hdr = reshape(hdr, [M N]);
