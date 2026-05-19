#include "fft.hh"
#include <bit>

//Basic O(N^2) DFT
std::vector<cd> dft(std::vector<double>& x)
{
    int N = (int)x.size();
    std::vector<cd> freq(N);
    for(int i = 0; i<N; i++)
    {
        cd z(0.0, 0.0);
        for(int j = 0; j<N; j++)
        {
            double angle = (-2)*M_PI*i*j/N;
            z += x[j]*exp(cd(0, angle));
            
        }
        freq[i] = z;
    }
    return freq;
}


// Recursive Cooley–Tukey FFT, O(NLogN)
void fft(std::vector<std::complex<double>>& x) {
    size_t N = x.size();
    if(std::popcount(N)!=1)
    {
        std::cout << "N is snot a power of 2\n";
        return;
    }
    if(N<=1)
    {
        return;
    }
    
    std::vector<cd> even(N/2);
    std::vector<cd> odd(N/2);
    
    for(int i = 0; i<(N/2); i++)
    {
        even[i] = x[2*i];
        odd[i] = x[2*i+1];
    }
    
    fft(even);
    fft(odd);
    
    for(int i = 0; i<N/2; i++)
    {
        double angle = 2*M_PI*i/N;
        cd W = exp(cd(0, angle));
        x[i] = even[i] + W*odd[i];
        x[i+N/2] = even[i] - W*odd[i];
    }
    
}

