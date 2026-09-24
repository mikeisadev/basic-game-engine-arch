#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace eng 
{
    class Texture;
    
    struct CameraData
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 orthoMatrix;
        glm::mat4 view2D = glm::mat4(1.0f);
        glm::vec3 position;
    };

    struct LightData
    {
        glm::vec3 color;
        glm::vec3 position;
    };

    struct UIBatch
    {
        Texture* texture = nullptr;
        uint32_t indexCount = 0;
    };
}