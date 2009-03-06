function [r,g,b,labelmap,max] = rgbmode1(im)
%im is an rgb image;
nbins = 32;
temp =zeros(256/nbins,3); %bins
max = zeros(1,3);
labelmap = zeros(size(im,1),size(im,2));
for i=1:size(im,1)
    for j=1:size(im,2)
        temp(ceil(double(im(i,j,1)+1)/nbins),1) = temp(ceil(double(im(i,j,1)+1)/nbins),1) + 1;
        if ceil(double(im(i,j,1)+1)/nbins)>1 && max(1)<temp(ceil(double(im(i,j,1)+1)/nbins),1)
            r = ceil(double(im(i,j,1)+1)/nbins)*nbins-nbins/2;
            max(1) = temp(ceil(double(im(i,j,1)+1)/nbins),1);
        end
        temp(ceil(double(im(i,j,2)+1)/nbins),2) = temp(ceil(double(im(i,j,2)+1)/nbins),2) + 1;
        if ceil(double(im(i,j,2)+1)/nbins)>1 && max(2)<temp(ceil(double(im(i,j,2)+1)/nbins),2)
            g = ceil(double(im(i,j,2)+1)/nbins)*nbins-nbins/2;
            max(2) = temp(ceil(double(im(i,j,2)+1)/nbins),2);
        end
        temp(ceil(double(im(i,j,3)+1)/nbins),3) = temp(ceil(double(im(i,j,3)+1)/nbins),3) + 1;
        if ceil(double(im(i,j,3)+1)/nbins)>1 && max(3)<temp(ceil(double(im(i,j,3)+1)/nbins),3)
            b = ceil(double(im(i,j,3)+1)/nbins)*nbins-nbins/2;
            max(3) = temp(ceil(double(im(i,j,3)+1)/nbins),1);
        end
    end
end
for i=1:size(im,1)
    for j=1:size(im,2)
    if(ceil(double(im(i,j,1)+1)/nbins)*nbins-nbins/2 == r && ceil(double(im(i,j,2)+1)/nbins)*nbins-nbins/2 == g && ceil(double(im(i,j,3)+1)/nbins)*nbins-nbins/2 == b)
       labelmap(i,j) = 1;
    else
        labelmap(i,j) = 0;
    end
    end
end