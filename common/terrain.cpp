#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "terrain.h"
#include <glad/glad.h>

// get vertices
void Terrain::loadHeightmap(const char* path,
    float heightScale,
    float size)
{
    int channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 1);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            int index = z * width + x;
            float h = data[index] / 255.0f * heightScale;

            float xpos = (float)x / (width - 1) * size;
            float zpos = (float)z / (height - 1) * size;

            // position
            vertices.push_back(xpos);
            vertices.push_back(h);
            vertices.push_back(zpos);

            // normal
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            // texture coords
            vertices.push_back((float)x / width);
            vertices.push_back((float)z / height);
        }
    }
    // triangles
    for (int z = 0; z < height - 1; z++) {
        for (int x = 0; x < width - 1; x++) {
            int i = z * width + x;

            indices.push_back(i);
            indices.push_back(i + width);
            indices.push_back(i + 1);

            indices.push_back(i + 1);
            indices.push_back(i + width);
            indices.push_back(i + width + 1);
        }
    }

    // buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    int stride = 8 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    stbi_image_free(data);
}

//terrain constructor and draw
Terrain::Terrain(const char* path, float heightScale, float size)
{
    loadHeightmap(path, heightScale, size);
}

void Terrain::Draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,
        (width - 1) * (height - 1) * 6,
        GL_UNSIGNED_INT,
        0);
}
