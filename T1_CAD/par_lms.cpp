#include <iostream>
#include <vector>
#include <random>
#include <cmath>

using namespace std;

int main(int argc, char ** argv) {
    if (argc < 2) return 1;

    const int niter = 450;
    const unsigned long int nsamples = atol(argv[1]);
    const int N = 4;
    const double sigmav = 0.05;
    const double beta = 0.01;

    double w_target[N] = {1.0, -0.9, 0.8, -0.7};

    vector<double> mse_avg(niter, 0.0); // private and shared
    
    #pragma omp parallel
    {
        default_random_engine generator( 0x553627 );
        normal_distribution<double> u_dist(0.0, 1.0);
        normal_distribution<double> v_dist(0.0, sqrt(sigmav));
        vector<double> mse_avg_local(niter, 0.0);

        #pragma omp for
        for (unsigned long int s = 0; s < nsamples; s++) {
            vector<double> w(N, 0.0);
            vector<double> x(N, 0.0);

            for (int k = 0; k < niter; k++) {
                for (int i = N - 1; i > 0; i--){
                    x[i] = x[i - 1];
                }

                x[0] = u_dist(generator);

                double d = 0;
                for (int i = 0; i < N; i++){
                    d += w_target[i] * x[i];
                }    
                
                d += v_dist(generator);

                double y = 0;
                for (int i = 0; i < N; i++){
                    y += w[i] * x[i];
                }

                double error = d - y;

                for (int i = 0; i < N; i++) {
                    w[i] = w[i] + beta * error * x[i];
                }

                mse_avg[k] += (error * error);
            }

        }

        #pragma omp critical
        {
            for (long i = 0; i < niter; i++){
				mse_avg[i] += mse_avg_local[i];
			}
        }
    }

    for (int k = 0; k < niter; k++) {
        cout << k << " " << mse_avg[k] / nsamples << endl;
    }

    return 0;
}