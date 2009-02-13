function min_array = findlips(ima)
figure;
w = size(ima,1);
im = ima(uint16(w-w/4):w,uint16(w/4):uint16(w-w/4));
l = size(im,2);

min_array = [];
for i=1:l
  [a,b] = min(im(:,i));
  min_array(1,i) = b+uint16(w-w/4);
end

midval = mode(min_array);
for i=1:l
  if(abs(min_array(1,i)-midval) > 5)
    min_array(1,i) = 0;
  end
end

left = 0;
right = 0;
for i=1:l
  if(min_array(i) > 0)
    if((min_array(i)+min_array(i+1)+min_array(i+2)+min_array(i+3)+min_array(i+4)) > 3*midval)
      left = i;
      break;
    end
  end
end
for j=1:l
  i = l-j+1;
  if(min_array(i) > 0)
    if((min_array(i)+min_array(i-1)+min_array(i-2)+min_array(i-3)+min_array(i-4)) > 3*midval)
      right = i;
      break;
    end
  end
end

imshow(ima);
hold on;
plot((1+uint16(w/4)):(l+uint16(w/4)),min_array,'.');
plot(left+uint16(w/4),min_array(left),'ro');
plot(right+uint16(w/4),min_array(right),'ro');