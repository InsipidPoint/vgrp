function imout = readimage(name,a,b,s)

im = imread(name);
im = im2double(im);
im = im(b:(b+s),a:(a+s),:);
imout = rgb2gray(im);
imout = imout/sqrt(var(reshape(imout,size(imout,1)*size(imout,2),1)));
imout = imout/max(max(imout));