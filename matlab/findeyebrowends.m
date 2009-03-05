function imout = findeyebrowends(im)
% im should be cropped to the face.
im = im2double(im);
blurfilter = fspecial('gaussian',[10 10],5);
im = imfilter(im,blurfilter,'symmetric','same','conv');
%im = im(size(im,1)/4:2*size(im,1)/4,size(im,2)/8:7*size(im,2)/8);
im = im(size(im,1)/4:2*size(im,1)/4,:);
imedge = edge(im,'log',0.001);
imout = imedge;