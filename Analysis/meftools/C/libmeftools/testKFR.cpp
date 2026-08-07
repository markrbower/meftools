/* testKFR.cpp
Mark Bower
Yale University

Compilation:
make testKFR

Usage:

Result:

*/
#include <kfr/base.hpp>
#include <kfr/dft.hpp>

using namespace kfr;

int main()
{
    // 256 complex samples of a complex exponential
    univector<complex<double>, 256> data =
        cexp(linspace(0, c_pi<double, 2>, 256) * make_complex(0, 1));

    // Forward FFT — returns a univector holding the spectrum
    univector<complex<double>, 256> freq = dft(data);

    // Inverse FFT back to the time domain
    data = idft(freq) / data.size(); // KFR does not scale automatically
}
/*
	// Design a 4th-order Butterworth low-pass filter
	zpk filt = iir_lowpass(butterworth(4), 500, 48000);

	// Convert to second-order sections
	iir_params<float> params = to_sos<float>(filt);

	// Apply zero-phase filtering to a signal
	univector<float> data(1024);
	data[512] = 1; // Unit impulse
	filtfilt(data, params); // Apply forward-backward filtering in-place
*/
