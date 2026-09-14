#include "scene/Component.h"
#include <cstddef>

namespace eng
{
    std::size_t Component::nextId = 1;

    GameObject* Component::GetOwner()
    {
        return m_owner;
    }
}