function C = pairs2int16(B)
%Convert matrix of pairs to vector of uint16 constructed from unit8 pairs
%   Detailed explanation goes here
C = arrayfun(@(n) typecast(uint8(B(n,:)), 'uint16'), 1:size(B,1));
end

