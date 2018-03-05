function Fast_kurt_test(x,nlevel,Fs)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FAST COMPUTATION OF THE KURTOGRAM (by means of wavelet packets or STFT)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
x = x - mean(x);

% Analytic generating filters
N = 16;			fc = .4;                        % a short filter is just good enough!
h = fir1(N,fc).*exp(2i*pi*(0:N)*.125);
n = 2:N+1;
g = h(1+mod(1-n,N)).*(-1).^(1-n);
%
Kwav = K_wpQ(x,h,g,nlevel,nlevel);		% kurtosis of the complex envelope
Kwav = Kwav.*(Kwav>0);							% keep positive values only!

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GRAPHICAL DISPLAY OF RESULTS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure
Level_w = 0:nlevel;	
% Level_w = [Level_w;Level_w+log2(3)-1];	Level_w = Level_w(:); Level_w = [0 Level_w(1:2*nlevel-1)'];
freq_w = Fs*((0:3*2^nlevel-1)/(3*2^(nlevel+1)) + 1/(3*2^(2+nlevel)));
imagesc(freq_w,1:nlevel+1,Kwav),colorbar,[I,J,M] = max_IJ(Kwav);
xlabel('frequency [Hz]'),set(gca,'ytick',1:nlevel+1,'yticklabel',round(Level_w*10)/10),ylabel('level k')
J = ((J-1)/((2^(nlevel))/(2^(Level_w(I)))))+1;
J = round(J);
fi = ((J+(1/2))*(2^-(Level_w(I)+1))*Fs) - (Fs*2^-(Level_w(I)+1));
title(['fb-kurt.2 - K_{max}=',num2str(round(10*M)/10),' @ level ',num2str(fix(10*Level_w(I))/10),', Bw= ',num2str(Fs*2^-(Level_w(I)+1)),'Hz, f_c=',num2str(fi),'Hz'])

% END OF THE MAIN ROUTINE

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% LIST OF SUBROUTINES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function K = kurt(x)
% Computes the kurtosis of signal x

if all(x == 0), K = 0;return;end
x = x - mean(x);
E = mean(abs(x).^2);
if E < eps, K = 0; return;end
K = mean(abs(x).^4)/E^2;
if all(isreal(x))
   K = K - 3;							% real signal
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
% LIST OF SUBROUTINES FOR THE WAVELET PACKET KURTOGRAM
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function K = K_wpQ(x,h,g,nlevel,level)
% K = K_wpQ(x,h,g,h1,h2,h3,nlevel)
% Computes the kurtosis K of the complete "binary-ternary" wavelet packet transform w of signal x, 
% up to nlevel, using the lowpass and highpass filters h and g, respectively. 
% The values in K are sorted according to the frequency decomposition.

x = x(:);										 % shapes the signal as column vector if necessary

K = K_wpQ_local(x,h,g,nlevel,level);


%--------------------------------------------------------------------------
function [K] = K_wpQ_local(x,h,g,nlevel,level)
% (Subroutine of K_wpQ)

% performs one analysis level into the analysis tree
[a,d] = DBFB(x,h,g);                    

N = length(a);                       
d = d.*(-1).^(1:N)';
Lh = length(h);
Lg = length(g);

K1 = kurt(a(Lh:end));
K2 = kurt(d(Lg:end));

if level == 1
%    K =[K1*ones(1,1),K2*ones(1,1)];
    K =[K1,K2];
end

if level > 1
   [Ka] = K_wpQ_local(a,h,g,nlevel,level-1);
   [Kd] = K_wpQ_local(d,h,g,nlevel,level-1);
   
   K1 = K1*ones(1,length(Ka));
   K2 = K2*ones(1,length(Kd));
   K = [K1 K2; Ka Kd];

end

if level == nlevel    
   K1 = kurt(x);        % kurtosis of the raw signal is computed here
   K = [K1*ones(1,length(K));K];

end

% ------------------------------------------------------------------------
function [a,d] = DBFB(x,h,g)
% Double-band filter-bank (binary decomposition).
%   [a,d] = DBFB(x,h,g) computes the approximation
%   coefficients vector a and detail coefficients vector d,
%   obtained by passing signal x though a two-band analysis filter-bank.
%   h is the decomposition low-pass filter and
%   g is the decomposition high-pass filter.
% (Subroutine of 'K_wpQ_local')

N = length(x);

% lowpass filter
a = filter(h,1,x);
a = a(2:2:N);
a = a(:);

% highpass filter
d = filter(g,1,x);
d = d(2:2:N);
d = d(:);

