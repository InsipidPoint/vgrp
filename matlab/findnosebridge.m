function [a1, b1] = findnosebridge(im)
% im should be cropped to the face.
im = im2double(im);
blurfilter = fspecial('gaussian',[10 10],5);
im = imfilter(im,blurfilter,'symmetric','same','conv');
imtemp = im(size(im,1)/4:2*size(im,1)/4,size(im,2)/6:5*size(im,2)/6);
% figure; imshow(imtemp);
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
im13 = im1(size(im1,1)/3:size(im1,1)*2/3,size(im1,2)/3:size(im1,2)*2/3);
% figure; imshow(im13);
[y1 i1] = max(im13);
[y2 i2] = max(y1);
a1 = i2;
b1 = i1(i2);
a1
b1

a1 = a1 + size(im1,2)/3;
b1 = b1 + size(im1,1)/3;
a1
b1
a1 = a1 + size(im,2)/6;
b1 = b1 + size(im,1)/4;

% im11 = im1(size(im1,1)/5:size(im1,1)*4/5,size(im1,2)/8:size(im1,2)*4/8);
% im12 = im1(size(im1,1)/5:size(im1,1)*4/5,size(im1,2)*4/8:size(im1,2)*7/8);
% figure;imshow(im1(size(im1,1)/4:size(im1,1)*3/4,size(im1,2)/8:size(im1,2)*7/8));
% figure;imshow(im11);
% figure;imshow(im12);
% [y1 i1] = min(im11);
% [y2 i2] = min(y1);
% a1 = i2;
% b1 = i1(i2);
% a1
% b1
% 
% a1 = a1 + size(im1,2)/8;
% b1 = b1 + size(im1,1)/5;
% [y1 i1] = min(im12);
% [y2 i2] = min(y1);
% a2 = i2;
% b2 = i1(i2);
% a2
% b2
% a2 = a2 + size(im1,2)*4/8;
% b2 = b2 + size(im1,1)/5;