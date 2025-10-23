#pragma once
#include "../ECS/System.h"
#include "../ECS/World.h"
#include "../Components/Transform2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/BoxCollider2D.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace UniversalEngine {
    
    class Physics2DSystem : public System {
    public:
        Physics2DSystem() = default;
        ~Physics2DSystem() = default;
        
        void Init() override {
            m_Gravity = glm::vec2(0.0f, -9.81f);
        }
        
        
        void Update(float deltaTime) override {
            for (auto entity : m_Entities) {
                if (!m_World) continue;
                
                if (!m_World->HasComponent<Transform2D>(entity) || 
                    !m_World->HasComponent<Rigidbody2D>(entity)) {
                    continue;
                }
                
                auto& transform = m_World->GetComponent<Transform2D>(entity);
                auto& rigidbody = m_World->GetComponent<Rigidbody2D>(entity);
                
                if (rigidbody.useGravity) {
                    rigidbody.velocity += m_Gravity * rigidbody.gravityScale * deltaTime;
                }
                
                float speed = glm::length(rigidbody.velocity);
                if (speed > 0.01f) {
                    glm::vec2 dragForce = -rigidbody.velocity * rigidbody.drag * speed;
                    rigidbody.velocity += dragForce * deltaTime;
                }
                
                transform.position += rigidbody.velocity * deltaTime;
            }
            
            std::vector<Entity> entityList(m_Entities.begin(), m_Entities.end());
            for (size_t i = 0; i < entityList.size(); ++i) {
                auto entity = entityList[i];
                if (!m_World->HasComponent<Transform2D>(entity) || 
                    !m_World->HasComponent<BoxCollider2D>(entity)) {
                    continue;
                }
                
                auto& transform = m_World->GetComponent<Transform2D>(entity);
                auto& collider = m_World->GetComponent<BoxCollider2D>(entity);
                bool hasRigidbody = m_World->HasComponent<Rigidbody2D>(entity);
                
                for (size_t j = i + 1; j < entityList.size(); ++j) {
                    auto otherEntity = entityList[j];
                    
                    if (!m_World->HasComponent<Transform2D>(otherEntity) ||
                        !m_World->HasComponent<BoxCollider2D>(otherEntity)) {
                        continue;
                    }
                    
                    auto& otherTransform = m_World->GetComponent<Transform2D>(otherEntity);
                    auto& otherCollider = m_World->GetComponent<BoxCollider2D>(otherEntity);
                    
                    if (IntersectsOBB(transform.position, transform.rotation, collider, 
                                     otherTransform.position, otherTransform.rotation, otherCollider)) {
                        bool otherHasRigidbody = m_World->HasComponent<Rigidbody2D>(otherEntity);
                        
                        if (hasRigidbody && !otherHasRigidbody && otherCollider.isStatic) {
                            auto& rigidbody = m_World->GetComponent<Rigidbody2D>(entity);
                            ResolveStaticCollision(transform, rigidbody, collider, otherTransform, otherCollider);
                        }
                        else if (!hasRigidbody && otherHasRigidbody && collider.isStatic) {
                            auto& otherRigidbody = m_World->GetComponent<Rigidbody2D>(otherEntity);
                            ResolveStaticCollision(otherTransform, otherRigidbody, otherCollider, transform, collider);
                        }
                        else if (hasRigidbody && otherHasRigidbody) {
                            auto& rigidbody = m_World->GetComponent<Rigidbody2D>(entity);
                            auto& otherRigidbody = m_World->GetComponent<Rigidbody2D>(otherEntity);
                            ResolveDynamicCollision(transform, rigidbody, collider, 
                                                   otherTransform, otherRigidbody, otherCollider);
                        }
                    }
                }
            }
        }
        
        void SetWorld(World* world) {
            m_World = world;
        }
        
        void SetGravity(const glm::vec2& gravity) {
            m_Gravity = gravity;
        }
        
        glm::vec2 GetGravity() const {
            return m_Gravity;
        }
        
    private:
        void ResolveStaticCollision(Transform2D& transform, Rigidbody2D& rigidbody, BoxCollider2D& collider,
                                    const Transform2D& otherTransform, const BoxCollider2D& otherCollider) {
            detail::OBB A = detail::makeOBB(transform.position, transform.rotation, collider);
            detail::OBB B = detail::makeOBB(otherTransform.position, otherTransform.rotation, otherCollider);
            glm::vec2 n;
            float pen;
            if (!detail::satMTV(A, B, n, pen)) return;

            glm::vec2 mtv = -n * pen;
            transform.position += mtv;

            float vnRel = glm::dot(rigidbody.velocity, n);

            float Jn = -(1.0f + rigidbody.restitution) * vnRel;
            rigidbody.velocity += Jn * n;

            glm::vec2 tangent = rigidbody.velocity - glm::dot(rigidbody.velocity, n) * n;
            float tLen = glm::length(tangent);
            if (tLen > 1e-6f) {
                tangent /= tLen;
                float vtRel = glm::dot(rigidbody.velocity, tangent);
                float Jt = -vtRel;
                float maxF = rigidbody.friction * std::abs(Jn);
                Jt = glm::clamp(Jt, -maxF, maxF);
                rigidbody.velocity += Jt * tangent;
            }
        }

        void ResolveDynamicCollision(Transform2D& transform1, Rigidbody2D& rb1, BoxCollider2D& collider1,
                                    Transform2D& transform2, Rigidbody2D& rb2, BoxCollider2D& collider2) {
            detail::OBB A = detail::makeOBB(transform1.position, transform1.rotation, collider1);
            detail::OBB B = detail::makeOBB(transform2.position, transform2.rotation, collider2);
            glm::vec2 n;
            float pen;

            
            if (!detail::satMTV(A, B, n, pen)) return;

            float totalMass = rb1.mass + rb2.mass;
            float mass1Ratio = rb2.mass / totalMass;
            float mass2Ratio = rb1.mass / totalMass;

            glm::vec2 mtv = -n * pen;
            transform1.position += mtv * mass1Ratio;
            transform2.position -= mtv * mass2Ratio;

            glm::vec2 relativeVel = rb1.velocity - rb2.velocity;
            float vn = glm::dot(relativeVel, n);

            float invMass1 = 1.f / rb1.mass;
            float invMass2 = 1.f / rb2.mass;

            float restitution = std::min(rb1.restitution, rb2.restitution);
            float invMassSum = invMass1 + invMass2;

            float j = -(1.f + restitution) * vn / invMassSum;

            glm::vec2 impulse = j * n;
            rb1.velocity += impulse * invMass1;
            rb2.velocity -= impulse * invMass2;

            glm::vec2 tangent = relativeVel - vn * n;
            float tangentLen = glm::length(tangent);
            tangent /= tangentLen;
            float friction = std::max(rb1.friction, rb2.friction);
            float vt = glm::dot(relativeVel, tangent);
            float jt = -vt / invMassSum;
            jt = glm::clamp(jt, -friction * j, friction * j);
            
            glm::vec2 frictionImpulse = jt * tangent;
            rb1.velocity += frictionImpulse / rb1.mass;
            rb2.velocity -= frictionImpulse / rb2.mass;
        }
        
    private:
        glm::vec2 m_Gravity{0.0f, -9.81f};
        World* m_World = nullptr;
    };
    
}
