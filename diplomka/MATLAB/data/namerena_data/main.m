
fileID0 = fopen('3505_otacek.txt');

%nacti data
sin = fread(fileID0);

%paruj byty
sin_pairs = col2pairs(sin);

%preved na int16
d = pairs2int16(sin_pairs);

%zmena frekvence dle mereni
fs = 6060; %6060/22793

%  plot(d)

d=double(d);
data=d-mean(d);
subplot(1,2,1),plot(data, '*-');

n = length(data);                      % number of samples
y = fft(data);                         % DFT of signal
f = (0:n-1)*(fs/n);                    % frequency range
power = abs(y).^2/n;                   % power of the DFT
subplot(1,2,2),plot(f(1:floor(n/2)),power(1:floor(n/2)))
xlabel('Frequency')
ylabel('Power')

