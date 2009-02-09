function  eyebrows(im)

[m n] = size(im);
response = zeros(m);
for i=1:m
    for j=floor(3*n/10):floor(7*n/10)
        response(i) = response(i) + im(i,j);
    end
end

[out idx] = sort(response,1,'descend');
out(1:10)
idx(1:10)
x1 = idx(1);
for j=floor(3*n/10):floor(5*n/10)
    if(j>1 && im(x1,j)==1 && im(x1,j-1)==0) 
        y1 = j;
    end
    if(im(x1,j)==1)
        y2=j;
    end
end