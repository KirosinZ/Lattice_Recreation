//
// Created by Kiril on 05.07.2022.
//

#include "SparsePoint.h"

void PolygonMesh::SparsePoint::add(glm::vec3 &ptr)
{
    if (!m_points.empty())
    {
        ptr = *m_points[0];
    }
    m_points.push_back(&ptr);
}

void PolygonMesh::SparsePoint::remove(glm::vec3 &ptr)
{
    m_points.erase(std::remove(m_points.begin(), m_points.end(), &ptr), m_points.end());
}

glm::vec3 PolygonMesh::SparsePoint::get() const
{
    return *m_points[0];
}

void PolygonMesh::SparsePoint::set(const glm::vec3 &vec)
{
    for(glm::vec3 *iter : m_points) {
        *iter = vec;
    }
}