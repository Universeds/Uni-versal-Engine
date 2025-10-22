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
                
                if (std::abs(rigidbody.angularVelocity) > 0.01f) {
                    float angularDragTorque = -rigidbody.angularVelocity * rigidbody.angularDrag * std::abs(rigidbody.angularVelocity);
                    rigidbody.angularVelocity += angularDragTorque * deltaTime;
                }
                
                transform.position += rigidbody.velocity * deltaTime;
                transform.rotation += glm::degrees(rigidbody.angularVelocity * deltaTime);
                
                if (m_World->HasComponent<BoxCollider2D>(entity)) {
                    auto& collider = m_World->GetComponent<BoxCollider2D>(entity);
                    CheckCollisions(entity, transform, rigidbody, collider);
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
        void CheckCollisions(Entity entity, Transform2D& transform, Rigidbody2D& rigidbody, BoxCollider2D& collider) {
            if (!m_World) return;
            
            for (auto otherEntity : m_Entities) {
                if (entity.GetID() == otherEntity.GetID()) continue;
                
                if (!m_World->HasComponent<Transform2D>(otherEntity) ||
                    !m_World->HasComponent<BoxCollider2D>(otherEntity)) {
                    continue;
                }
                
                auto& otherTransform = m_World->GetComponent<Transform2D>(otherEntity);
                auto& otherCollider = m_World->GetComponent<BoxCollider2D>(otherEntity);
                
                if (IntersectsOBB(transform.position, transform.rotation, collider, otherTransform.position, otherTransform.rotation, otherCollider)) {
                    bool otherHasRigidbody = m_World->HasComponent<Rigidbody2D>(otherEntity);
                    
                    if (otherCollider.isStatic || !otherHasRigidbody) {
                        ResolveStaticCollision(transform, rigidbody, collider, otherTransform, otherCollider);
                    } else {
                        auto& otherRigidbody = m_World->GetComponent<Rigidbody2D>(otherEntity);
                        ResolveDynamicCollision(transform, rigidbody, collider, otherTransform, otherRigidbody, otherCollider);
                    }
                }
            }
        }

        void ResolveStaticCollision(Transform2D& transform, Rigidbody2D& rigidbody, BoxCollider2D& collider,
                                   const Transform2D& otherTransform, const BoxCollider2D& otherCollider) {
            detail::OBB A = detail::makeOBB(transform.position, transform.rotation, collider);
            detail::OBB B = detail::makeOBB(otherTransform.position, otherTransform.rotation, otherCollider);
            glm::vec2 n;
            float pen;
            if (!detail::satMTV(A, B, n, pen)) return;

            glm::vec2 mtv = -n * pen;
            transform.position += mtv;

            auto Ac = A.corners();
            auto Bc = B.corners();
            glm::vec2 pA = detail::support(Ac, -n);
            glm::vec2 pB = detail::support(Bc, n);
            glm::vec2 contactPoint = 0.5f * (pA + pB);
            glm::vec2 contactNormal = n;

            float vn = glm::dot(rigidbody.velocity, contactNormal);
            glm::vec2 vN = vn * contactNormal;
            glm::vec2 vT = rigidbody.velocity - vN;
            glm::vec2 newV = -rigidbody.restitution * vN + (1.f - rigidbody.friction) * vT;
            rigidbody.velocity = newV;

            glm::vec2 r = contactPoint - A.c;
            float torqueImpulse = (r.x * contactNormal.y - r.y * contactNormal.x) * (-vn);
            float inertia = rigidbody.GetInertia(collider.size);
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

            auto Ac = A.corners();
            auto Bc = B.corners();
            glm::vec2 pA = detail::support(Ac, -n);
            glm::vec2 pB = detail::support(Bc, n);
            glm::vec2 contactPoint = 0.5f * (pA + pB);
            glm::vec2 contactNormal = n;

            glm::vec2 relativeVel = rb1.velocity - rb2.velocity;
            float vn = glm::dot(relativeVel, contactNormal);
            
            if (vn > 0.f) return;

            float restitution = std::min(rb1.restitution, rb2.restitution);
            float j = -(1.f + restitution) * vn;
            j /= (1.f / rb1.mass + 1.f / rb2.mass);

            glm::vec2 impulse = j * contactNormal;
            rb1.velocity += impulse / rb1.mass;
            rb2.velocity -= impulse / rb2.mass;

            glm::vec2 tangent = relativeVel - vn * contactNormal;
            if (glm::length2(tangent) > 0.0001f) {
                tangent = glm::normalize(tangent);
                float friction = std::max(rb1.friction, rb2.friction);
                glm::vec2 frictionImpulse = friction * glm::length(impulse) * tangent;
                rb1.velocity -= frictionImpulse / rb1.mass;
                rb2.velocity += frictionImpulse / rb2.mass;
            }

            glm::vec2 r1 = contactPoint - A.c;
            glm::vec2 r2 = contactPoint - B.c;
            float inertia1 = rb1.GetInertia(collider1.size);
            float inertia2 = rb2.GetInertia(collider2.size);
            
            float angularImpulse = (r1.x * contactNormal.y - r1.y * contactNormal.x) * j;
            if (inertia1 > 0.f) rb1.angularVelocity += angularImpulse / inertia1;
            if (inertia2 > 0.f) rb2.angularVelocity -= angularImpulse / inertia2;
        }
        
    private:
        glm::vec2 m_Gravity{0.0f, -9.81f};
        World* m_World = nullptr;
    };
    
}
