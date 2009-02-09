function [r,g,b,labelmap] = rgbmode(im)
%im is an rgb image;
nbins = [64 32 64];
temp =zeros(ceil(double(256)/nbins(1)),256/nbins(2),256/nbins(3)); %bins
max = 0;
labelmap = zeros(size(im,1),size(im,2));
for i=1:size(im,1)
    for j=1:size(im,2)
        temp(ceil(double(im(i,j,1)+1)/nbins(1)),ceil(double(im(i,j,2)+1)/nbins(2)),ceil(double(im(i,j,3)+1)/nbins(3))) = temp(ceil(double(im(i,j,1)+1)/nbins(1)),ceil(double(im(i,j,2)+1)/nbins(2)),ceil(double(im(i,j,3)+1)/nbins(3))) + 1;
        if ceil(double(im(i,j,1)+1)/nbins(1))>1 && ceil(double(im(i,j,2)+1)/nbins(2))>1 && ceil(double(im(i,j,3)+1)/nbins(3))>0 && max<temp(ceil(double(im(i,j,1)+1)/nbins(1)),ceil(double(im(i,j,2)+1)/nbins(2)),ceil(double(im(i,j,3)+1)/nbins(3)))
            r = ceil(double(im(i,j,1)+1)/nbins(1))*nbins(1)-nbins(1)/2;
            g = ceil(double(im(i,j,2)+1)/nbins(2))*nbins(2)-nbins(2)/2;
            b = ceil(double(im(i,j,3)+1)/nbins(3))*nbins(3)-nbins(3)/2;
            max = temp(ceil(double(im(i,j,1)+1)/nbins(1)),ceil(double(im(i,j,2)+1)/nbins(2)),ceil(double(im(i,j,3)+1)/nbins(3)));
        end
    end
end

for i=1:size(im,1)
    for j=1:size(im,2)
    if(ceil(double(im(i,j,1)+1)/nbins(1))*nbins(1)-nbins(1)/2 == r && ceil(double(im(i,j,2)+1)/nbins(2))*nbins(2)-nbins(2)/2 == g && ceil(double(im(i,j,3)+1)/nbins(3))*nbins(3)-nbins(3)/2 == b)
       labelmap(i,j) = 1;
    else
        labelmap(i,j) = 0;
    end
    end
end