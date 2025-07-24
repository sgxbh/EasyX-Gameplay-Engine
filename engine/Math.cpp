#include "Math.h"


int Math::RandomInt(int lower, int upper)
{
    if (lower > upper) {
        int temp = lower;
        lower = upper;
        upper = temp;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(lower, upper);
    return dis(gen);
}

double Math::RandomReal(double lower, double upper)
{
    if (lower > upper) {
        double temp = lower;
        lower = upper;
        upper = temp;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(lower, upper);
    return dis(gen);
}

double Math::RandomPerc()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    return std::generate_canonical<double, 10>(gen);
}

double Math::Clamp(double value, double lower, double upper)
{
#define min(a,b) ((a)<(b)? (a):(b))
#define max(a,b) ((a)>(b)? (a):(b))
    return min(max(value, min(lower, upper)), max(lower, upper));
}
