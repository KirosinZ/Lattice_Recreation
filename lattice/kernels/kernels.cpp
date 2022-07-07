#include "kernels.h"

float LatticeOperation::KernelWeightFunctions::Wyvill::weight(const float distance, const float radius)
{
    if (distance >= radius)
        return 0.0f;

    const float n = (distance * distance) / (radius * radius);

    const float m1 = -22.0f / 9.0f;
    const float m2 = 17.0f / 9.0f;
    const float m3 = -4.0f / 9.0f;

    return 1.0f + m1 * n + m2 * n * n + m3 * n * n * n;
}

float LatticeOperation::KernelWeightFunctions::Custom::weight(float distance, float radius)
{
    return glm::smoothstep(0.0f, radius, radius - distance);
}