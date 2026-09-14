#pragma once
#include <cstddef>

namespace eng
{
    class GameObject;

    class Component
    {
        public:
            virtual ~Component() = default;
            virtual void Update(float deltaTime) = 0;
            virtual std::size_t GetTypeId() const = 0;

            GameObject* GetOwner();

            template<typename T>
            static std::size_t StaticTypeId()
            {
                static std::size_t typeId = nextId++;
                return typeId;
            }

        protected:
            GameObject* m_owner = nullptr;

            friend class GameObject;

        private:
            static std::size_t nextId;
    };

    #define COMPONENT(ComponentClass) \
        public: \
            static std::size_t TypeId() { return Component::StaticTypeId<ComponentClass>(); } \
            std::size_t GetTypeId() const override { return TypeId(); }
}