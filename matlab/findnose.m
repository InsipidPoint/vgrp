function [a, b] = findnose(im)

[a1, b1, a2, b2] = findnostrils(im);
[a3, b3] = findnosetip(im);

a = (a1 + a2 + a3)/3;
b = (b1 + b2 + b3)/3;

im = im2double(im);

figure; imshow(im);

t = linspace(0,2*pi,1000);
r = 10;
x1 = r*cos(t) + a;
y1 = r*sin(t) + b;

hold on;
plot(x1,y1,'g');
