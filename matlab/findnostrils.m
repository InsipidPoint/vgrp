function [a1, b1, a2, b2] = findnostrils(im)

im = im2double(im);
%blurfilter = fspecial('gaussian',[10 10],5);
%im = imfilter(im,blurfilter,'symmetric','same','conv');
imtemp = im(3*size(im,1)/7:4*size(im,1)/7,size(im,2)/3:2*size(im,2)/3);

%A = ones(10,10)/100;
%im1 = imfilter(imtemp,A,'symmetric','same','conv');
%im13 = im1(size(im1,1)/3:size(im1,1)*2/3,size(im1,2)/3:size(im1,2)*2/3);
%figure; imshow(im13);

imL = imtemp(:,1:size(imtemp,2)/2);
imR = imtemp(:,size(imtemp,2)/2:size(imtemp,2));
figure;imshow(imL);
figure;imshow(imR);

[y1 i1] = min(imL);
[y2 i2] = min(y1);
a1 = i2;
b1 = i1(i2);

a1 = a1 + size(im,2)/3;
b1 = b1 + 3*size(im,1)/7;
a1
b1

[y1 i1] = min(imR);
[y2 i2] = min(y1);
a2 = i2;
b2 = i1(i2);

a2 = a2 + size(im,2)/3 + size(imL,2);
b2 = b2 + 3*size(im,1)/7;
a2
b2

figure; imshow(im);

t = linspace(0,2*pi,1000);
r = 5;
x1 = r*cos(t) + a1;
y1 = r*sin(t) + b1;
x2 = r*cos(t) + a2;
y2 = r*sin(t) + b2;

hold on;
plot(x1,y1,'g');
plot(x2,y2,'g');