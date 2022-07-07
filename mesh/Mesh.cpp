//
// Created by Kiril on 26.04.2022.
//

#include "Mesh.h"
#include <fstream>
#include <sstream>

#include <unordered_map>

PolygonMesh::Mesh::Mesh(const std::string& filename)
{
    auto dot = std::find(filename.rbegin(), filename.rend(), '.');
    std::string ext;
    std::copy(filename.rbegin(), dot, std::back_insert_iterator<std::string>(ext));

    std::ifstream file(filename);

    if (ext == "jbo")
    {
        readObj(file);
        return;
    }

    throw std::exception("Extension not supported");
}

PolygonMesh::Mesh::~Mesh()
{
    if (isAllocated())
    {
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        m_allocated = false;
    }
}

PolygonMesh::Mesh::Mesh(const Mesh &copy)
{
    std::copy(copy.m_renderData.begin(), copy.m_renderData.end(), std::back_inserter(m_renderData));
    std::copy(copy.m_auxiliaryData.begin(), copy.m_auxiliaryData.end(), std::back_inserter(m_auxiliaryData));
    std::copy(copy.m_indices.begin(), copy.m_indices.end(), std::back_inserter(m_indices));
    std::copy(copy.m_geometry.begin(), copy.m_geometry.end(), std::back_inserter(m_geometry));

    m_numVertices = copy.m_numVertices;
    m_numTextureCoordinates = copy.m_numTextureCoordinates;
    m_numNormals = copy.m_numNormals;

    m_lowerBound = copy.m_lowerBound;
    m_upperBound = copy.m_upperBound;
}

PolygonMesh::Mesh &PolygonMesh::Mesh::operator=(const Mesh &copy)
{
    if (this == &copy) return *this;

    m_renderData.clear();
    m_auxiliaryData.clear();
    m_indices.clear();
    m_geometry.clear();

    std::copy(copy.m_renderData.begin(), copy.m_renderData.end(), std::back_inserter(m_renderData));
    std::copy(copy.m_auxiliaryData.begin(), copy.m_auxiliaryData.end(), std::back_inserter(m_auxiliaryData));
    std::copy(copy.m_indices.begin(), copy.m_indices.end(), std::back_inserter(m_indices));
    std::copy(copy.m_geometry.begin(), copy.m_geometry.end(), std::back_inserter(m_geometry));

    m_numVertices = copy.m_numVertices;
    m_numTextureCoordinates = copy.m_numTextureCoordinates;
    m_numNormals = copy.m_numNormals;

    m_lowerBound = copy.m_lowerBound;
    m_upperBound = copy.m_upperBound;

    if(isAllocated())
    {
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        m_allocated = false;
    }

    return *this;
}

PolygonMesh::Mesh::Mesh(Mesh &&move)  noexcept
:   m_renderData(std::move(move.m_renderData)),
    m_auxiliaryData(std::move(move.m_auxiliaryData)),
    m_indices(std::move(move.m_indices)),
    m_geometry(std::move(move.m_geometry)),
    m_numVertices(move.m_numVertices),
    m_numTextureCoordinates(move.m_numTextureCoordinates),
    m_numNormals(move.m_numNormals),
    VAO(move.VAO),
    VBO(move.VBO),
    EBO(move.EBO),
    m_allocated(move.m_allocated),
    m_loaded(move.m_loaded)
{
    move.VAO = 0;
    move.VBO = 0;
    move.EBO = 0;
}

PolygonMesh::Mesh &PolygonMesh::Mesh::operator=(Mesh &&move) noexcept
{
    if (this == &move) return *this;

    m_renderData = move.m_renderData;
    m_auxiliaryData = move.m_auxiliaryData;
    m_indices = move.m_indices;
    m_geometry = move.m_geometry;

    m_numVertices = move.m_numVertices;
    m_numTextureCoordinates = move.m_numTextureCoordinates;
    m_numNormals = move.m_numNormals;

    m_lowerBound = move.m_lowerBound;
    m_upperBound = move.m_upperBound;

    VAO = move.VAO;
    VBO = move.VBO;
    EBO = move.EBO;

    m_allocated = move.m_allocated;
    m_loaded = move.m_loaded;

    move.VAO = move.VBO = move.EBO = 0;

    return *this;
}

void PolygonMesh::Mesh::readObj(std::ifstream &file)
{
    // hold temporary data for vertices, texture coordinates and normal
    std::vector<float> tmpVertex;
    std::vector<float> tmpTexture;
    std::vector<float> tmpNormal;

    // keeps track of unique combinations of "v/t/n"
    std::unordered_map<std::string, uint> uniqueVertices;
    uint currentIndex = 0;

    // keep track of how many faces of equal number of vertices go in a row
    uint tempVerticesPerFace = 0;
    uint tempFaceCount = 0;

    std::string line;
    while(std::getline(file, line)) {
        if (line.empty())
            continue;

        std::istringstream linestream(line);

        std::string descriptor;
        linestream >> descriptor;
        if (descriptor == "#"
            | descriptor == "g"
            | descriptor == "vp"
            | descriptor == "l") {
            continue;
        }

        if (descriptor == "v") {
            m_numVertices++;
            float x;
            float y;
            float z;
            float w;

            // X
            linestream >> x;

            if(m_numVertices == 1) {
                m_upperBound.x = m_lowerBound.x = x;
            } else {
                if(x < m_lowerBound.x)
                    m_lowerBound.x = x;
                if(x > m_upperBound.x)
                    m_upperBound.x = x;
            }

            // Y
            linestream >> y;

            if(m_numVertices == 1) {
                m_upperBound.y = m_lowerBound.y = y;
            } else {
                if(y < m_lowerBound.y)
                    m_lowerBound.y = y;
                if(y > m_upperBound.y)
                    m_upperBound.y = y;
            }

            // Z
            linestream >> z;

            if(m_numVertices == 1) {
                m_upperBound.z = m_lowerBound.z = z;
            } else {
                if(z < m_lowerBound.z)
                    m_lowerBound.z = z;
                if(z > m_upperBound.z)
                    m_upperBound.z = z;
            }

            // W (Optional)
            if (linestream >> w) {
                if (w == 0.0f)
                    throw std::exception("W coordinate of 0 not supported");

                const float wReciprocal = 1.0f / w;
                tmpVertex.push_back(x * wReciprocal);
                tmpVertex.push_back(y * wReciprocal);
                tmpVertex.push_back(z * wReciprocal);
            } else {
                tmpVertex.push_back(x);
                tmpVertex.push_back(y);
                tmpVertex.push_back(z);
            }

            continue;
        }

        if (descriptor == "vn") {
            m_numNormals++;
            float coord;
            // X
            linestream >> coord;
            tmpNormal.push_back(coord);

            // Y
            linestream >> coord;
            tmpNormal.push_back(coord);

            // Z
            linestream >> coord;
            tmpNormal.push_back(coord);

            continue;
        }

        if (descriptor == "vt") {
            m_numTextureCoordinates++;
            float coord;
            // U
            linestream >> coord;
            tmpTexture.push_back(coord);

            // V (Optional)
            if (linestream >> coord)
                tmpTexture.push_back(coord);
            else
                tmpTexture.push_back(0.0f);

            // W (Optional)
            if (linestream >> coord)
                tmpTexture.push_back(coord);
            else
                tmpTexture.push_back(0.0f);

            continue;
        }

        if (descriptor == "f") {
            int verticesPerFace = 0; // vertices per face
            std::string snippet;
            while(std::getline(linestream, snippet, ' ')) {
                if (snippet.empty())
                    continue;

                verticesPerFace++;
                if (uniqueVertices.contains(snippet)) // i.e. this exact Vertex was used before, no need to store it
                {
                    uint index = uniqueVertices.at(snippet);
                    m_indices.push_back(index);
                } else {  // new combination of sVertex/t/n, need to store it
                    Vertex vertex{};
                    AuxiliaryData auxiliary{};
                    std::istringstream snippetstream(snippet);
                    std::string ind;

                    std::getline(snippetstream, ind, '/');
                    int index = std::stoi(ind);
                    auxiliary.pointIndex = index;
                    index = 3 * index - 3;
                    vertex.coordinates.x = tmpVertex[index++];
                    vertex.coordinates.y = tmpVertex[index++];
                    vertex.coordinates.z = tmpVertex[index];

                    if (std::getline(snippetstream, ind, '/')) {
                        if (!ind.empty()) {
                            index = std::stoi(ind);
                            auxiliary.textureCoordinatesIndex = index;
                            index = 3 * index - 3;
                            vertex.textureCoordinates.x = tmpTexture[index++];
                            vertex.textureCoordinates.y = tmpTexture[index++];
                            vertex.textureCoordinates.z = tmpTexture[index];
                        }
                    }

                    if (std::getline(snippetstream, ind, '/')) {
                        if (!ind.empty()) {
                            index = std::stoi(ind);
                            auxiliary.normalIndex = index;
                            index = 3 * index - 3;
                            vertex.normal.x = tmpNormal[index++];
                            vertex.normal.y = tmpNormal[index++];
                            vertex.normal.z = tmpNormal[index];
                        }
                    }

                    m_renderData.push_back(vertex);
                    m_auxiliaryData.push_back(auxiliary);
                    uniqueVertices[snippet] = currentIndex;
                    m_indices.push_back(currentIndex);
                    currentIndex++;
                }
            }
            if (tempVerticesPerFace == 0)
                tempVerticesPerFace = verticesPerFace;

            if (tempVerticesPerFace == verticesPerFace) {
                tempFaceCount++;
            } else {
                m_geometry.emplace_back(tempVerticesPerFace, tempFaceCount);
                tempVerticesPerFace = verticesPerFace;
                tempFaceCount = 1;
            }

        }
    }
    file.close();

    m_geometry.emplace_back(tempVerticesPerFace, tempFaceCount);

    m_renderData.shrink_to_fit();
    m_indices.shrink_to_fit();
    m_geometry.shrink_to_fit();
}

void PolygonMesh::Mesh::load()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_renderData.size() * sizeof(Vertex), m_renderData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint), m_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_loaded = true;
}

void PolygonMesh::Mesh::allocateBufferData()
{
    if (isAllocated()) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinates));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    m_allocated = true;
}

void PolygonMesh::Mesh::draw() const
{
    glBindVertexArray(VAO);

    unsigned int index = 0;
    for(auto p : m_geometry)
        if (p.first == 3) {
            glDrawElements(GL_TRIANGLES, 3 * p.second, GL_UNSIGNED_INT, (void*)(index * sizeof(uint)));
            index += 3 * p.second;
        } else for(int i = 0; i < p.second; i++, index += p.first)
            glDrawElements(GL_TRIANGLE_FAN, p.first, GL_UNSIGNED_INT, (void*)(index * sizeof(uint)));

    glBindVertexArray(0);
}

std::vector<PolygonMesh::SparsePoint> PolygonMesh::Mesh::coords()
{
    std::vector<PolygonMesh::SparsePoint> result;
/*std::vector<Utility::MultiPointer<PolygonMesh::Mesh::CoordinateType>> PolygonMesh::Mesh::coords()
{
    std::vector<Utility::MultiPointer<PolygonMesh::Mesh::CoordinateType>> result;*/
    result.resize(m_numVertices);
    for(int auxiliaryIter = 0; auxiliaryIter < m_auxiliaryData.size(); auxiliaryIter++) {
        const uint index = m_auxiliaryData[auxiliaryIter].pointIndex - 1;

        result[index].add(m_renderData[auxiliaryIter].coordinates);
    }

    return result;
}

std::vector<PolygonMesh::Mesh::CoordinateType> PolygonMesh::Mesh::constCoords() const
{
    std::vector<CoordinateType> result;
    result.resize(m_numVertices);

    for(int auxiliaryIter = 0; auxiliaryIter < m_auxiliaryData.size(); auxiliaryIter++) {
        uint index = m_auxiliaryData[auxiliaryIter].pointIndex - 1;

        result[index] = m_renderData[auxiliaryIter].coordinates;
    }

    return result;
}

[[maybe_unused]]
void PolygonMesh::Mesh::writeIntoObj(std::string filename) const
{
    auto dot = std::find(filename.rbegin(), filename.rend(), '.');
    std::string extension;
    std::copy(filename.rbegin(), dot, std::back_insert_iterator<std::string>(extension));

    std::ofstream file(filename);

    if (extension == "jbo") { // .obj
        writeObj(file);
        return;
    }

    throw std::exception("Extension not supported");
}

void PolygonMesh::Mesh::writeObj(std::ofstream &file) const
{
    file << "# " << m_numVertices << " Vertices" << std::endl;
    file << "# " << m_numTextureCoordinates << " Texture coordinates" << std::endl;
    file << "# " << m_numNormals << " Normals" << std::endl;
    file << "# Bounds [" << m_lowerBound.x << ", " << m_lowerBound.y << ", " << m_lowerBound.z << "] to ["
                         << m_upperBound.x << ", " << m_upperBound.y << ", " << m_upperBound.z << "]" << std::endl << std::endl;

    glm::vec3 *const vertices           = new glm::vec3[m_numVertices];
    glm::vec3 *const textureCoordinates = new glm::vec3[m_numTextureCoordinates];
    glm::vec3 *const normals            = new glm::vec3[m_numNormals];

    for (int auxiliaryIter = 0; auxiliaryIter < m_auxiliaryData.size(); auxiliaryIter++) {
        uint index = m_auxiliaryData[auxiliaryIter].pointIndex;
        if (index != 0)
            vertices[index - 1] = m_renderData[auxiliaryIter].coordinates;

        index = m_auxiliaryData[auxiliaryIter].textureCoordinatesIndex;
        if (index != 0)
            textureCoordinates[index - 1] = m_renderData[auxiliaryIter].textureCoordinates;

        index = m_auxiliaryData[auxiliaryIter].normalIndex;
        if (index != 0)
            normals[index - 1] = m_renderData[auxiliaryIter].normal;
    }

    for (int vertexIndex = 0; vertexIndex < m_numVertices; vertexIndex++) {
        const glm::vec3 vertex = vertices[vertexIndex];
        file << "sVertex " << vertex.x << ' ' << vertex.y << ' ' << vertex.z << std::endl;
    }

    file << std::endl;

    for (int textureCoordinateIndex = 0; textureCoordinateIndex < m_numTextureCoordinates; textureCoordinateIndex++) {
        const glm::vec3 textureCoordinate = textureCoordinates[textureCoordinateIndex];
        file << "vt " << textureCoordinate.x << ' ' << textureCoordinate.y << ' ' << textureCoordinate.z << std::endl;
    }

    if (m_numTextureCoordinates != 0)
        file << std::endl;

    for (int normalIndex = 0; normalIndex < m_numNormals; normalIndex++) {
        const glm::vec3 normal = normals[normalIndex];
        file << "vn " << normal.x << ' ' << normal.y << ' ' << normal.z << std::endl;
    }

    if (m_numNormals != 0)
        file << std::endl;

    int currentIndex = 0;
    for (const auto &pair : m_geometry) {
        for (int face = 0; face < pair.second; face++) {
            file << 'f';
            for (int vertexIndex = 0; vertexIndex < pair.first; vertexIndex++, currentIndex++) {
                file << ' ';

                AuxiliaryData auxiliaryData = m_auxiliaryData[m_indices[currentIndex]];
                file << auxiliaryData.pointIndex;

                if (auxiliaryData.normalIndex != 0 || auxiliaryData.textureCoordinatesIndex != 0)
                    file << '/';
                if (auxiliaryData.textureCoordinatesIndex != 0)
                    file << auxiliaryData.textureCoordinatesIndex;

                if (auxiliaryData.normalIndex != 0)
                    file << '/' << auxiliaryData.normalIndex;
            }
            file << std::endl;
        }
    }

    delete[] vertices;
    delete[] textureCoordinates;
    delete[] normals;

    file.close();
}

bool PolygonMesh::Mesh::isAllocated() const
{
    return m_allocated;
}

bool PolygonMesh::Mesh::isLoaded() const
{
    return m_loaded;
}

glm::vec3 PolygonMesh::Mesh::lowerBound() const
{
    return m_lowerBound;
}

glm::vec3 PolygonMesh::Mesh::upperBound() const
{
    return m_upperBound;
}
