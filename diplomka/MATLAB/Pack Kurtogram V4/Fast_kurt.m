function Fast_kurt(x,nlevel,Fs)

% Fast computation of the kurtogram
x = x - mean(x);

% Generating filters
N = 16;			fc = .4;                       
h = fir1(N,fc).*exp(2i*pi*(0:N)*.125);
n = 2:N+1;
g = h(1+mod(1-n,N)).*(-1).^(1-n);

x = x(:);
% Recursive function for computation of the kurtogram
kurtogram = K_local(x,h,g,nlevel,nlevel);

% Only positive values
kurtogram = kurtogram.*(kurtogram>0);							

% Graphical display of results

figure
Level_w = 0:nlevel;	
freq_w = Fs*((0:3*2^nlevel-1)/(3*2^(nlevel+1)) + 1/(3*2^(2+nlevel)));
imagesc(freq_w,1:nlevel+1,kurtogram),colorbar,[I,J,M] = max_IJ(kurtogram);
xlabel('frequency [Hz]'),set(gca,'ytick',1:nlevel+1,'yticklabel',round(Level_w*10)/10),ylabel('level k')
J = ((J-1)/((2^(nlevel))/(2^(Level_w(I)))))+1;
J = round(J);
fi = ((J+(1/2))*(2^-(Level_w(I)+1))*Fs) - (Fs*2^-(Level_w(I)+1));
title(['fb-kurt.2 - K_{max}=',num2str(round(10*M)/10),' @ level ',num2str(fix(10*Level_w(I))/10),', Bw= ',num2str(Fs*2^-(Level_w(I)+1)),'Hz, f_c=',num2str(fi),'Hz'])


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function K = kurt(x)
% Computes the kurtosis of signal x

if all(x == 0), K = 0;return;end
x = x - mean(x);
E = mean(abs(x).^2);
if E < eps, K = 0; return;end
K = mean(abs(x).^4)/E^2;
if all(isreal(x))
   K = K - 3;						
else
   K = K - 2;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [I,J,M] = max_IJ(X)
% Returns the row and column indices of the maximum in matrix X.

[temp,tempI] = max(X);
[M,J] = max(temp);
I = tempI(J);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [K] = K_local(x,h,g,nlevel,level)
% performs one analysis level into the analysis tree

[a,d] = DBFB(x,h,g);                    

N = length(a);                       
d = d.*(-1).^(1:N)';
Lh = length(h);
Lg = length(g);

K1 = kurt(a(Lh:end));
K2 = kurt(d(Lg:end));

if level == 1
    K =[K1,K2];
end

if level > 1
   [Ka] = K_local(a,h,g,nlevel,level-1);
   [Kd] = K_local(d,h,g,nlevel,level-1);
   
   K1 = K1*ones(1,length(Ka));
   K2 = K2*ones(1,length(Kd));
   K = [K1 K2; Ka Kd];

end

% kurtosis of the raw signal
if level == nlevel    
   K1 = kurt(x);        
   K = [K1*ones(1,length(K));K];

end

% ------------------------------------------------------------------------
function [a,d] = DBFB(x,h,g)
% Double-band filter-bank (binary decomposition).
%   [a,d] = DBFB(x,h,g) computes vector a and vector d,
%   obtained by passing signal x through a lowpass and highpass filters.

N = length(x);

% lowpass filter
a = filter(h,1,x);
a = a(2:2:N);
a = a(:);

% highpass filter
d = filter(g,1,x);
d = d(2:2:N);
d = d(:);

