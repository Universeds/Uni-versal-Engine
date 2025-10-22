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

            glm::vec2 r = contactPoint - A.c;
            float inertia = rigidbody.GetInertia(collider.size);
            if (inertia <= 0.0f) return;

            glm::vec2 rPerp(-r.y, r.x);
            float rn = r.x * contactNormal.y - r.y * contactNormal.x;

            glm::vec2 vRel = rigidbody.velocity + rigidbody.angularVelocity * rPerp;
            float vnRel = glm::dot(vRel, contactNormal);

            float Jn = -(1.0f + rigidbody.restitution) * vnRel / (1.0f + (rn * rn) / inertia);
            rigidbody.velocity += Jn * contactNormal;
            rigidbody.angularVelocity += (rn * Jn) / inertia;

            vRel = rigidbody.velocity + rigidbody.angularVelocity * rPerp;
            glm::vec2 tangent = vRel - glm::dot(vRel, contactNormal) * contactNormal;
            float tLen = glm::length(tangent);
            if (tLen > 1e-6f) tangent /= tLen;

            float vtRel = glm::dot(vRel, tangent);
            float rt = r.x * tangent.y - r.y * tangent.x;
            float Jt = -vtRel / (1.0f + (rt * rt) / inertia);

            float maxF = rigidbody.friction * std::abs(Jn);
            if (Jt >  maxF) Jt =  maxF;
            if (Jt < -maxF) Jt = -maxF;

            rigidbody.velocity += Jt * tangent;
            rigidbody.angularVelocity += (rt * Jt) / inertia;
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

            glm::vec2 r1 = contactPoint - A.c;
            glm::vec2 r2 = contactPoint - B.c;
            float inertia1 = rb1.GetInertia(collider1.size);
            float inertia2 = rb2.GetInertia(collider2.size);

            glm::vec2 v1 = rb1.velocity + glm::vec2(-r1.y, r1.x) * rb1.angularVelocity;
            glm::vec2 v2 = rb2.velocity + glm::vec2(-r2.y, r2.x) * rb2.angularVelocity;
            glm::vec2 relativeVel = v1 - v2;
            float vn = glm::dot(relativeVel, contactNormal);
            
            if (vn > 0.f) return;

            float restitution = std::min(rb1.restitution, rb2.restitution);
            
            float r1CrossN = r1.x * contactNormal.y - r1.y * contactNormal.x;
            float r2CrossN = r2.x * contactNormal.y - r2.y * contactNormal.x;
            float invMassSum = 1.f / rb1.mass + 1.f / rb2.mass;
            if (inertia1 > 0.f) invMassSum += (r1CrossN * r1CrossN) / inertia1;
            if (inertia2 > 0.f) invMassSum += (r2CrossN * r2CrossN) / inertia2;
            
            float j = -(1.f + restitution) * vn / invMassSum;

            glm::vec2 impulse = j * contactNormal;
            rb1.velocity += impulse / rb1.mass;
            rb2.velocity -= impulse / rb2.mass;
            
            if (inertia1 > 0.f) rb1.angularVelocity += r1CrossN * j / inertia1;
            if (inertia2 > 0.f) rb2.angularVelocity -= r2CrossN * j / inertia2;

            glm::vec2 tangent = relativeVel - vn * contactNormal;
            if (glm::length2(tangent) > 0.0001f) {
                tangent = glm::normalize(tangent);
                float friction = std::max(rb1.friction, rb2.friction);
                
                float vt = glm::dot(relativeVel, tangent);
                float r1CrossT = r1.x * tangent.y - r1.y * tangent.x;
                float r2CrossT = r2.x * tangent.y - r2.y * tangent.x;
                float invMassSumT = 1.f / rb1.mass + 1.f / rb2.mass;
                if (inertia1 > 0.f) invMassSumT += (r1CrossT * r1CrossT) / inertia1;
                if (inertia2 > 0.f) invMassSumT += (r2CrossT * r2CrossT) / inertia2;
                
                float jt = -vt / invMassSumT;
                jt = glm::clamp(jt, -friction * j, friction * j);
                
                glm::vec2 frictionImpulse = jt * tangent;
                rb1.velocity += frictionImpulse / rb1.mass;
                rb2.velocity -= frictionImpulse / rb2.mass;
                
                if (inertia1 > 0.f) rb1.angularVelocity += r1CrossT * jt / inertia1;
                if (inertia2 > 0.f) rb2.angularVelocity -= r2CrossT * jt / inertia2;
            }
        }
        
    private:
        glm::vec2 m_Gravity{0.0f, -9.81f};
        World* m_World = nullptr;
    };
    
}
