function [a1, b1] = findnosetip(im)

im = im2double(im);
%blurfilter = fspecial('gaussian',[10 10],5);
%im = imfilter(im,blurfilter,'symmetric','same','conv');
imtemp = im(3*size(im,1)/7:5*size(im,1)/7,size(im,2)/3:2*size(im,2)/3);

%A = ones(10,10)/100;
%im1 = imfilter(imtemp,A,'symmetric','same','conv');
%im13 = im1(size(im1,1)/3:size(im1,1)*2/3,size(im1,2)/3:size(im1,2)*2/3);
%figure; imshow(im13);

[y1 i1] = max(imtemp);
[y2 i2] = max(y1);
a1 = i2;
b1 = i1(i2);

a1 = a1 + size(im,2)/3;
b1 = b1 + 3*size(im,1)/7;
a1
b1


figure; imshow(im);

t = linspace(0,2*pi,1000);
r = 5;
x1 = r*cos(t) + a1;
y1 = r*sin(t) + b1;

hold on;
plot(x1,y1,'r');