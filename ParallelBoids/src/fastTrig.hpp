#ifndef FAST_TRIG_HPP
#define FAST_TRIG_HPP

#include <cmath>
#include <iostream>

#define QT_SINE_TABLE_SIZE 256

//https://stackoverflow.com/a/66868438
inline float sine_fast(float x)
{
    const float B = 4/M_PI;
    const float C = -4/(M_PI*M_PI);

    float y = B * x + C * x * std::abs(x);

    #ifdef EXTRA_PRECISION
    //  const float Q = 0.775;
        const float P = 0.225;

        y = P * (y * abs(y) - y) + y;   // Q * y + P * y * abs(y)
    #endif

	return y;
}

// https://stackoverflow.com/a/28050328
template<typename T>
inline T cosine_fast(T x) noexcept
{
    constexpr T tp = 1./(2.*M_PI);
    x *= tp;
    x -= T(.25) + std::floor(x + T(.25));
    x *= T(16.) * (std::abs(x) - T(.5));
    #if EXTRA_PRECISION
    x += T(.225) * x * (std::abs(x) - T(1.));
    #endif
    return x;
}

//https://gist.github.com/volkansalma/2972237?permalink_comment_id=3872525#gistcomment-3872525
inline float atan2_approx(float y, float x) {
	float abs_y = std::fabs(y) + 1e-10f;      // kludge to prevent 0/0 condition
	float r = (x - std::copysign(abs_y, x)) / (abs_y + std::fabs(x));
	float angle = M_PI/2.f - std::copysign(M_PI/4.f, x);

	angle += (0.1963f * r * r - 0.9817f) * r;
	return std::copysign(angle, y);
}

inline double FastArcTan(double x) {
	return M_PI_4 * x - x * (fabs(x) - 1) * (0.2447 + 0.0663 * fabs(x));
}
inline double FastArcTan2(double y, double x) {
	if (x >= 0) { // -pi/2 .. pi/2
		if (y >= 0) { // 0 .. pi/2
			if (y < x) { // 0 .. pi/4
				return FastArcTan(y / x);
			} else { // pi/4 .. pi/2
				return M_PI_2 - FastArcTan(x / y);
			}
		} else {
			if (-y < x) { // -pi/4 .. 0
				return FastArcTan(y / x);
			} else { // -pi/2 .. -pi/4
				return -M_PI_2 - FastArcTan(x / y);
			}
		}
	} else { // -pi..-pi/2, pi/2..pi
		if (y >= 0) { // pi/2 .. pi
			if (y < -x) { // pi*3/4 .. pi
				return FastArcTan(y / x) + M_PI;
			} else { // pi/2 .. pi*3/4
				return M_PI_2 - FastArcTan(x / y);
			}
		} else { // -pi .. -pi/2
			if (-y < -x) { // -pi .. -pi*3/4
				return FastArcTan(y / x) - M_PI;
			} else { // -pi*3/4 .. -pi/2
				return -M_PI_2 - FastArcTan(x / y);
			}
		}
	}
}

#endif
