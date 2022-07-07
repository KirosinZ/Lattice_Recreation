#ifndef LATTICE_KERNELS_KERNELS_H
#define LATTICE_KERNELS_KERNELS_H

#include <glm/glm.hpp>

namespace LatticeOperation::KernelWeightFunctions {

    class [[maybe_unused]] Custom
    {
    public:
        static float weight(float distance, float radius);
    };

    class [[maybe_unused]] Wyvill
    {
    public:
        static float weight(float distance, float radius);
    };










/*    float Custom::weight(const float distance, const float radius)
    {
        return glm::smoothstep(0.0f, radius, radius - distance);
    }*/

    /*float Wyvill::weight(const float distance, const float radius)
    {
        if (distance >= radius)
            return 0.0f;

        const float n = (distance * distance) / (radius * radius);

        const float m1 = -22.0f / 9.0f;
        const float m2 = 17.0f / 9.0f;
        const float m3 = -4.0f / 9.0f;

        return 1.0f + m1 * n + m2 * n * n + m3 * n * n * n;
    }*/
}

#endif //LATTICE_KERNELS_KERNELS_H
