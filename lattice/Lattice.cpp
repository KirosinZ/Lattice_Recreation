#include "Lattice.h"

LatticeOperation::Lattice::Lattice(const PolygonMesh::Mesh &rest, const PolygonMesh::Mesh &deformed, const PolygonMesh::Mesh &to_deform)
:   m_rest(rest.constCoords()),
    m_deformed(deformed.constCoords()),
    m_toDeform(to_deform.constCoords()),
    m_result(to_deform),
    m_restLowerBound(rest.lowerBound()),
    m_restUpperBound(rest.upperBound()),
    m_toDeformLowerBound(to_deform.lowerBound()),
    m_toDeformUpperBound(to_deform.upperBound())
{
    m_resultCoordinates = m_result.coords();
}

[[maybe_unused]]
PolygonMesh::Mesh LatticeOperation::Lattice::calculateBrute(float radius)
{
    for(int toDeformIndex = 0; toDeformIndex < m_toDeform.size(); toDeformIndex++) {
        float accumulator = 0.0f;
        auto shift = glm::vec3(0.0f);
        for (int restIndex = 0; restIndex < m_rest.size(); restIndex++) {
            const glm::vec3 difference = m_toDeform[toDeformIndex] - m_rest[restIndex];
            const float weight = glm::smoothstep(0.0f, radius, radius - glm::length(difference));
            if (weight == 0.0f)
                continue;

            accumulator += weight;
            shift += weight * (m_deformed[restIndex] - m_rest[restIndex]);
        }

        glm::vec3 result = m_toDeform[toDeformIndex];
        if (accumulator != 0.0f)
            result += shift / accumulator;

        m_resultCoordinates[toDeformIndex].set(result);
    }

    return m_result;
}

LatticeOperation::Lattice::CoordinateTree::CoordinateTree(const glm::vec3 lower, const glm::vec3 upper)
:   m_lowerBound(lower),
    m_upperBound(upper)
{
    const glm::vec3 size = m_upperBound - m_lowerBound;

    m_partitionCount = glm::trunc(size);

    if (size.x >= size.y && size.x >= size.z) {
        m_axesOrder[0] = 0;
        if(size.y >= size.z) {
            m_axesOrder[1] = 1;
            m_axesOrder[2] = 2;
        } else {
            m_axesOrder[1] = 2;
            m_axesOrder[2] = 1;
        }
    } else if (size.y >= size.x && size.y >= size.z) {
        m_axesOrder[0] = 1;
        if(size.x >= size.z) {
            m_axesOrder[1] = 0;
            m_axesOrder[2] = 2;
        } else {
            m_axesOrder[1] = 2;
            m_axesOrder[2] = 0;
        }
    } else {
        m_axesOrder[0] = 2;
        if(size.x >= size.y) {
            m_axesOrder[1] = 0;
            m_axesOrder[2] = 1;
        } else {
            m_axesOrder[1] = 1;
            m_axesOrder[2] = 0;
        }
    }
}

glm::ivec3 LatticeOperation::Lattice::CoordinateTree::keySelector(glm::vec3 point) const
{
    glm::ivec3 result;
    const glm::vec3 clampedPoint = (point - m_lowerBound) / (m_upperBound - m_lowerBound);

    for(int i = 0; i < 3; i++)
        result[i] = (int)truncf(m_partitionCount[m_axesOrder[i]] * clampedPoint[m_axesOrder[i]]);

    return result;
}

void LatticeOperation::Lattice::CoordinateTree::insert(glm::vec3 point, int index)
{
    const glm::ivec3 key = keySelector(point);
    m_tree[key[0]][key[1]][key[2]].push_back(index);
}

std::vector<int> LatticeOperation::Lattice::CoordinateTree::find(glm::vec3 where, float radius) const
{
    std::vector<int> result;
    const glm::ivec3 keyLower = keySelector(where - radius);
    const glm::ivec3 keyUpper = keySelector(where + radius);

    for(auto outerAxisIterator = m_tree.lower_bound(keyLower[0]); outerAxisIterator != m_tree.upper_bound(keyUpper[0]); outerAxisIterator++) {
        const auto& interTree = outerAxisIterator->second;
        for(auto interAxisIterator = interTree.lower_bound(keyLower[1]); interAxisIterator != interTree.upper_bound(keyUpper[1]); interAxisIterator++) {
            const auto& innerTree = interAxisIterator->second;
            for(auto innerAxisIterator = innerTree.lower_bound(keyLower[2]); innerAxisIterator != innerTree.upper_bound(keyUpper[2]); innerAxisIterator++) {
                const std::vector<int> val = innerAxisIterator->second;
                result.insert(result.end(), val.begin(), val.end());
            }
        }
    }

    result.shrink_to_fit();
    return result;
}
