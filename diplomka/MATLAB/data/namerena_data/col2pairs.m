function B = col2pairs(A)
%Convert column vector to matrix of pairs

i = 4;
AA = [];
while(i<=numel(A))
    if(A(i-1) == 255 && A(i) == 255 && A(i-2) ~= 255 && A(i-3) ~= 255)
        AA = [AA;A(i-3)];
        AA = [AA;A(i-2)];
        i = i+4;
    else
        i = i+1;
    end
end
    


% if(A(1) == 255)
%     A = A(2:end);
% else
%     if(A(2) == 255 && A(3) == 255)
%         A = A(4:end);
%     end
% end
% if(A(1) == 255)
%     A = A(2:end);
% end
% 
% 
% Q = length(A)-5;
% for i=3:4:Q
%     if(i <= length(A)-5)
%         if(A(i)==255 && A(i+1)==255  && A(i+4)==255 && A(i+5)==255) %&& A(i+2)~=255 && A(i+3)~=255
%             continue;
%         end
%     else
%         break;
%     end
%    A(i:i+3) = [];
% end

if mod(size(AA,1),2) == 0
B = reshape(AA,2,size(AA,1)/2).';
else
AA = AA(1:end-1);
B = reshape(AA,2,size(AA,1)/2).';
end

B;
end

