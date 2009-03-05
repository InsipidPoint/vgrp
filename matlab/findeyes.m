function [a1, b1, a2, b2] = findeyes(im)
% im should be cropped to the face.
im = im2double(im);
% blurfilter = fspecial('gaussian',[10 10],5);
% im = imfilter(im,blurfilter,'symmetric','same','conv');
% im = im(size(im,1)/4:2*size(im,1)/4,size(im,2)/6:5*size(im,2)/6);
[a b] = findnosebridge(im);
imtemp = im((b-size(im,1)/20):(b+size(im,1)/20),size(im,2)/8:size(im,2)*7/8);
figure;imshow(imtemp);
% x = reshape(im,size(im,1)*size(im,2),1);
% y = sort(x,1,'ascend');
% y = y(1:1000);
% imout = ones(size(im));
% for i=1:size(im,1)
%     for j=1:size(im,2)
%         if(find(ismember(y,im(i,j))))
%             imout(i,j) = im(i,j);
%         end
%     end
% end

A = ones(10,10)/100;
im1 = imfilter(imtemp,A,'symmetric','same','conv');
% im11 = im1(size(im1,1)/5:size(im1,1)*4/5,size(im1,2)/8:size(im1,2)*4/8);
% im12 = im1(size(im1,1)/5:size(im1,1)*4/5,size(im1,2)*4/8:size(im1,2)*7/8);
im11 = im1(:,1:size(im1,2)/2);
im12 = im1(:,size(im1,2)/2:size(im1,2));
% figure;imshow(im11);
% figure;imshow(im12);
[y1 i1] = min(im11);
[y2 i2] = min(y1);
a1 = i2;
b1 = i1(i2);
a1
b1
a1 = a1 + size(im,2)/8;
b1 = b1 + (b-size(im,1)/20);

eyedotfilter = [0 -1 0; -1 4 -1;0 -1 0]; 

imeye1 = imfilter(im((b1-10):(b1+10),(a1-10):(a1+10)),eyedotfilter,'same','conv');

[y1 i1] = max(imeye1);
[y2 i2] = max(y1);
a3 = i2;
b3 = i1(i2);
a3 = a3 + a1 - 10;
b3 = b3 + b1 - 10;


[y1 i1] = min(im12);
[y2 i2] = min(y1);
a2 = i2;
b2 = i1(i2);
a2
b2
a2 = a2 + size(im1,2)/2 + size(im,2)/8;
b2 = b2 + (b-size(im,1)/20);

imeye2 = imfilter(im,eyedotfilter,'same','conv');
% figure;imshow(imeye2);
imeye2=imeye2((b2-10):(b2+10),(a2-10):(a2+10));
% figure;imshow(imeye2);
[y1 i1] = max(imeye2);
[y2 i2] = max(y1);
a4 = i2;
b4 = i1(i2);
a4
b4
a4 = a4 + a2 - 10;
b4 = b4 + b2 - 10;
a4
b4
y2

t = linspace(0,2*pi,1000);
r = 10;
x1 = r*cos(t) + a1;
y1 = r*sin(t) + b1;
x2 = r*cos(t) + a2;
y2 = r*sin(t) + b2;
x4 = r*cos(t) + a3;
y4 = r*sin(t) + b3;
x5 = r*cos(t) + a4;
y5 = r*sin(t) + b4;
r = 5;
x3 = r*cos(t) + a;
y3 = r*sin(t) + b;

figure; imshow(im);
hold on;
plot(x1,y1,'r');
plot(x2,y2,'r');
% plot(x4,y4,'g');
% plot(x5,y5,'g');
plot(x3,y3,'b');
