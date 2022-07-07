#ifndef LATTICE_LATTICE_H
#define LATTICE_LATTICE_H

#include "../mesh/Mesh.h"
#include <map>
#include "kernels/kernels.h"

namespace LatticeOperation {

    class Lattice
    {
    public:
        Lattice(
            const PolygonMesh::Mesh &rest,
            const PolygonMesh::Mesh &deformed,
            const PolygonMesh::Mesh &toDeform);

        [[maybe_unused]]
        [[nodiscard]]
        PolygonMesh::Mesh calculateBrute(float radius);

        template<class kernel = LatticeOperation::KernelWeightFunctions::Wyvill>
        [[nodiscard]]
        PolygonMesh::Mesh calculate(float radius);

    private:

        struct VertexShiftCalculator
        {
            float accumulator = 0.0f;
            glm::vec3 shift = glm::vec3(0.0f);
        };

        class CoordinateTree
        {
        public:
            CoordinateTree(glm::vec3 lower, glm::vec3 upper);

            void insert(glm::vec3 point, int index);

            [[nodiscard]]
            std::vector<int> find(glm::vec3 where, float radius) const;
        private:
            glm::vec3 m_lowerBound;
            glm::vec3 m_upperBound;
            glm::ivec3 m_partitionCount;
            glm::ivec3 m_axesOrder;
            std::map<int, std::map<int, std::map<int, std::vector<int>>>> m_tree;

            [[nodiscard]]
            glm::ivec3 keySelector(glm::vec3 point) const;
        };

        const std::vector<glm::vec3> m_rest;
        const std::vector<glm::vec3> m_deformed;
        const std::vector<glm::vec3> m_toDeform;

        glm::vec3 m_restLowerBound;
        glm::vec3 m_restUpperBound;
        glm::vec3 m_toDeformLowerBound;
        glm::vec3 m_toDeformUpperBound;

        PolygonMesh::Mesh m_result;
        std::vector<PolygonMesh::SparsePoint> m_resultCoordinates;
        //std::vector<Utility::MultiPointer<glm::vec3>> m_resultCoordinates;

        template<class kernel>
        void forwardCaseIteration(const std::vector<int> &candidates, int toDeformIndex, float radius);

        template<class kernel>
        void backwardCaseIteration(const std::vector<int> &candidates, int restIndex, float radius, std::vector<VertexShiftCalculator> &shiftCalculators);

        template<class kernel>
        void forwardCase(float radius);

        template<class kernel>
        void backwardCase(float radius);
    };










    template<class kernel>
    [[nodiscard]]
    PolygonMesh::Mesh Lattice::calculate(float radius)
    {
        if (m_toDeform.size() > 1.5f * m_rest.size())
            backwardCase<kernel>(radius);
        else
            forwardCase<kernel>(radius);

        return m_result;
    }

    template<class kernel>
    void Lattice::forwardCaseIteration(const std::vector<int> &candidates, int toDeformIndex, float radius)
    {
        const glm::vec3 pivot = m_toDeform[toDeformIndex];

        float accumulator = 0.0f;
        auto shift = glm::vec3(0.0f);
        for(auto index : candidates) {
            const glm::vec3 vertex = m_rest[index];
            const glm::vec3 difference = m_deformed[index] - vertex;
            const float distance = glm::length(vertex - pivot);

            const float weight = kernel::weight(distance, radius);
            if(weight == 0.0f)
                continue;

            accumulator += weight;
            shift += difference * weight;
        }

        glm::vec3 result = m_toDeform[toDeformIndex];
        if (accumulator != 0.0f)
            result += shift / accumulator;

        m_resultCoordinates[toDeformIndex].set(result);
        //m_resultCoordinates[toDeformIndex] = result;
    }

    template<class kernel>
    void Lattice::backwardCaseIteration(const std::vector<int> &candidates, int restIndex, float radius, std::vector<Lattice::VertexShiftCalculator> &shiftCalculators)
    {
        const glm::vec3 pivot = m_rest[restIndex];
        const glm::vec3 difference = m_deformed[restIndex] - pivot;

        for(auto index : candidates) {
            const glm::vec3 vertex = m_toDeform[index];
            const float distance = glm::length(vertex - pivot);

            const float weight = kernel::weight(distance, radius);
            if(weight == 0.0f)
                continue;

            shiftCalculators[index].accumulator += weight;
            shiftCalculators[index].shift += weight * difference;
        }
    }

    template<class kernel>
    void Lattice::forwardCase(float radius)
    {
        CoordinateTree tree(m_restLowerBound, m_restUpperBound);

        for(int restIndex = 0; restIndex < m_rest.size(); restIndex++)
            tree.insert(m_rest[restIndex], restIndex);

        for(int toDeformIndex = 0; toDeformIndex < m_toDeform.size(); toDeformIndex++) {
            std::vector<int> candidates = tree.find(m_toDeform[toDeformIndex], radius);
            forwardCaseIteration<kernel>(candidates, toDeformIndex, radius);
        }
    }

    template<class kernel>
    void Lattice::backwardCase(float radius)
    {
        CoordinateTree tree(m_toDeformLowerBound, m_toDeformUpperBound);

        std::vector<VertexShiftCalculator> vertexShiftCalculators(m_toDeform.size());

        for(int toDeformIndex = 0; toDeformIndex < m_toDeform.size(); toDeformIndex++)
            tree.insert(m_toDeform[toDeformIndex], toDeformIndex);

        for (int restIndex = 0; restIndex < m_rest.size(); restIndex++) {
            const std::vector<int> candidates = tree.find(m_rest[restIndex], radius);
            backwardCaseIteration<kernel>(candidates, restIndex, radius, vertexShiftCalculators);
        }

        for(int toDeformIndex = 0; toDeformIndex < m_toDeform.size(); toDeformIndex++) {
            glm::vec3 result = m_toDeform[toDeformIndex];
            if (vertexShiftCalculators[toDeformIndex].accumulator != 0.0f)
                result += vertexShiftCalculators[toDeformIndex].shift / vertexShiftCalculators[toDeformIndex].accumulator;

            m_resultCoordinates[toDeformIndex].set(result);
            //m_resultCoordinates[toDeformIndex] = result;
        }
    }
}

#endif //LATTICE_LATTICE_H
