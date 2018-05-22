

% read data from file
% x = round(d,6);
x = importdata('file.txt');

% sampling frequency
Fs = 21739;         

% data visualization
figure()
x=double(x);
data=x-mean(x);
subplot(1,2,1),plot(data, '*-');
xlabel('n', 'fontsize', 18)
ylabel('v(n)', 'fontsize', 18)
xt = get(gca, 'XTick'); 
set(gca, 'FontSize', 18)

% visualization of frequency spectrum
n = length(data);                      
y = fft(data);                         
f = (0:n-1)*(Fs/n);                    
power = abs(y).^2/n;                   
subplot(1,2,2),plot(f(1:floor(n/2)),power(1:floor(n/2)))
xlabel('Frequency', 'fontsize', 18)
ylabel('Power', 'fontsize', 18)
xt = get(gca, 'XTick'); 
set(gca, 'FontSize', 18)


% Fast Kurtogram

% number of decomposition levels
nlevel = 4;     

% Function for fast computation of the kurtogram
Fast_kurt(x,nlevel,Fs);



% RMS
x = x - mean(x);
r = rms(x)

% Crest factor
maxim = max(abs(x));
crest = maxim/r



