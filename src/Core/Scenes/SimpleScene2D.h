#pragma once
#include "../ECS/World.h"
#include "../ECS/Entity.h"
#include "../Components/Transform2D.h"
#include "../Components/MeshRenderer2D.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/Rigidbody2D.h"
#include <glm/glm.hpp>

namespace UniversalEngine {
    
    class SimpleScene2D {
    public:
        static void CreateScene(World& world) {
            world.RegisterComponent<Transform2D>();
            world.RegisterComponent<MeshRenderer2D>();
            world.RegisterComponent<BoxCollider2D>();
            world.RegisterComponent<Rigidbody2D>();
            
            CreateBox(world);
            CreatePlatform(world);
        }
        
        static Entity CreateBox(World& world) {
            Entity box = world.CreateEntity();
            
            Transform2D transform;
            transform.position = glm::vec2(0.0f, 3.0f);
            transform.scale = glm::vec2(1.0f, 1.0f);
            transform.rotation = 45.0f;
            world.AddComponent(box, std::move(transform));
            
            MeshRenderer2D meshRenderer(glm::vec2(1.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
            world.AddComponent(box, std::move(meshRenderer));
            
            BoxCollider2D collider(glm::vec2(1.0f, 1.0f), false, false);
            world.AddComponent(box, std::move(collider));
            
            Rigidbody2D rigidbody(1.0f, 1.0f, true);
            world.AddComponent(box, std::move(rigidbody));
            
            return box;
        }
        
        static Entity CreatePlatform(World& world) {
            Entity platform = world.CreateEntity();
            
            Transform2D transform;
            transform.position = glm::vec2(0.0f, -2.0f);
            transform.scale = glm::vec2(1.0f, 1.0f);
            transform.rotation = 0.0f;
            world.AddComponent(platform, std::move(transform));
            
            MeshRenderer2D meshRenderer(glm::vec2(5.0f, 0.5f), glm::vec4(0.3f, 0.8f, 0.3f, 1.0f));
            world.AddComponent(platform, std::move(meshRenderer));
            
            BoxCollider2D collider(glm::vec2(5.0f, 0.5f), false, true);
            world.AddComponent(platform, std::move(collider));
            
            return platform;
        }
    };
    
}
