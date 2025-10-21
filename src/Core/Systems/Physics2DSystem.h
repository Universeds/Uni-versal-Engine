#pragma once
#include "../ECS/System.h"
#include "../ECS/World.h"
#include "../Components/Transform2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/BoxCollider2D.h"
#include <glm/glm.hpp>

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
                
                if (collider.Intersects(otherCollider, transform.position, otherTransform.position)) {
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
                                   Transform2D& otherTransform, BoxCollider2D& otherCollider) {
            glm::vec2 thisMin = transform.position + collider.offset - collider.size * 0.5f;
            glm::vec2 thisMax = transform.position + collider.offset + collider.size * 0.5f;
            
            glm::vec2 otherMin = otherTransform.position + otherCollider.offset - otherCollider.size * 0.5f;
            glm::vec2 otherMax = otherTransform.position + otherCollider.offset + otherCollider.size * 0.5f;
            
            glm::vec2 overlap;
            overlap.x = std::min(thisMax.x - otherMin.x, otherMax.x - thisMin.x);
            overlap.y = std::min(thisMax.y - otherMin.y, otherMax.y - thisMin.y);
            
            glm::vec2 contactPoint;
            glm::vec2 contactNormal;
            
            if (overlap.x < overlap.y) {
                if (transform.position.x < otherTransform.position.x) {
                    transform.position.x -= overlap.x;
                    contactNormal = glm::vec2(-1.0f, 0.0f);
                    contactPoint = glm::vec2(thisMax.x - overlap.x * 0.5f, (thisMin.y + thisMax.y) * 0.5f);
                } else {
                    transform.position.x += overlap.x;
                    contactNormal = glm::vec2(1.0f, 0.0f);
                    contactPoint = glm::vec2(thisMin.x + overlap.x * 0.5f, (thisMin.y + thisMax.y) * 0.5f);
                }
                
                float relativeVelocity = rigidbody.velocity.x;
                rigidbody.velocity.x = -relativeVelocity * rigidbody.restitution;
                rigidbody.velocity.y *= (1.0f - rigidbody.friction);
                
                glm::vec2 r = contactPoint - transform.position;
                float crossProduct = r.x * contactNormal.y - r.y * contactNormal.x;
                float angularImpulse = crossProduct * relativeVelocity * 5.0f;
                float inertia = rigidbody.GetInertia(collider.size);
                if (inertia > 0.0f) {
                    rigidbody.angularVelocity += angularImpulse / inertia;
                }
            } else {
                if (transform.position.y < otherTransform.position.y) {
                    transform.position.y -= overlap.y;
                    contactNormal = glm::vec2(0.0f, -1.0f);
                    contactPoint = glm::vec2((thisMin.x + thisMax.x) * 0.5f, thisMax.y - overlap.y * 0.5f);
                } else {
                    transform.position.y += overlap.y;
                    contactNormal = glm::vec2(0.0f, 1.0f);
                    contactPoint = glm::vec2((thisMin.x + thisMax.x) * 0.5f, thisMin.y + overlap.y * 0.5f);
                }
                
                float relativeVelocity = rigidbody.velocity.y;
                rigidbody.velocity.y = -relativeVelocity * rigidbody.restitution;
                rigidbody.velocity.x *= (1.0f - rigidbody.friction);
                
                glm::vec2 r = contactPoint - transform.position;
                float crossProduct = r.x * contactNormal.y - r.y * contactNormal.x;
                float angularImpulse = crossProduct * relativeVelocity * 5.0f;
                float inertia = rigidbody.GetInertia(collider.size);
                if (inertia > 0.0f) {
                    rigidbody.angularVelocity += angularImpulse / inertia;
                }
            }
        }
        
        void ResolveDynamicCollision(Transform2D& transform1, Rigidbody2D& rb1, BoxCollider2D& collider1,
                                    Transform2D& transform2, Rigidbody2D& rb2, BoxCollider2D& collider2) {
            glm::vec2 thisMin = transform1.position + collider1.offset - collider1.size * 0.5f;
            glm::vec2 thisMax = transform1.position + collider1.offset + collider1.size * 0.5f;
            
            glm::vec2 otherMin = transform2.position + collider2.offset - collider2.size * 0.5f;
            glm::vec2 otherMax = transform2.position + collider2.offset + collider2.size * 0.5f;
            
            glm::vec2 overlap;
            overlap.x = std::min(thisMax.x - otherMin.x, otherMax.x - thisMin.x);
            overlap.y = std::min(thisMax.y - otherMin.y, otherMax.y - thisMin.y);
            
            float totalMass = rb1.mass + rb2.mass;
            float mass1Ratio = rb1.mass / totalMass;
            float mass2Ratio = rb2.mass / totalMass;
            
            glm::vec2 contactPoint;
            glm::vec2 contactNormal;
            
            if (overlap.x < overlap.y) {
                float separation = overlap.x;
                if (transform1.position.x < transform2.position.x) {
                    transform1.position.x -= separation * mass2Ratio;
                    transform2.position.x += separation * mass1Ratio;
                    contactNormal = glm::vec2(-1.0f, 0.0f);
                    contactPoint = glm::vec2(thisMax.x - separation * 0.5f, (thisMin.y + thisMax.y) * 0.5f);
                } else {
                    transform1.position.x += separation * mass2Ratio;
                    transform2.position.x -= separation * mass1Ratio;
                    contactNormal = glm::vec2(1.0f, 0.0f);
                    contactPoint = glm::vec2(thisMin.x + separation * 0.5f, (thisMin.y + thisMax.y) * 0.5f);
                }
                
                float relativeVelocity = rb1.velocity.x - rb2.velocity.x;
                float restitution = std::min(rb1.restitution, rb2.restitution);
                float impulse = -(1.0f + restitution) * relativeVelocity / totalMass;
                
                rb1.velocity.x += impulse * rb2.mass;
                rb2.velocity.x -= impulse * rb1.mass;
                
                float friction = std::max(rb1.friction, rb2.friction);
                rb1.velocity.y *= (1.0f - friction);
                rb2.velocity.y *= (1.0f - friction);
                
                glm::vec2 r1 = contactPoint - transform1.position;
                glm::vec2 r2 = contactPoint - transform2.position;
                float inertia1 = rb1.GetInertia(collider1.size);
                float inertia2 = rb2.GetInertia(collider2.size);
                float angularImpulse = (r1.x * contactNormal.y - r1.y * contactNormal.x) * impulse;
                rb1.angularVelocity += angularImpulse / inertia1;
                rb2.angularVelocity -= angularImpulse / inertia2;
            } else {
                float separation = overlap.y;
                if (transform1.position.y < transform2.position.y) {
                    transform1.position.y -= separation * mass2Ratio;
                    transform2.position.y += separation * mass1Ratio;
                    contactNormal = glm::vec2(0.0f, -1.0f);
                    contactPoint = glm::vec2((thisMin.x + thisMax.x) * 0.5f, thisMax.y - separation * 0.5f);
                } else {
                    transform1.position.y += separation * mass2Ratio;
                    transform2.position.y -= separation * mass1Ratio;
                    contactNormal = glm::vec2(0.0f, 1.0f);
                    contactPoint = glm::vec2((thisMin.x + thisMax.x) * 0.5f, thisMin.y + separation * 0.5f);
                }
                
                float relativeVelocity = rb1.velocity.y - rb2.velocity.y;
                float restitution = std::min(rb1.restitution, rb2.restitution);
                float impulse = -(1.0f + restitution) * relativeVelocity / totalMass;
                
                rb1.velocity.y += impulse * rb2.mass;
                rb2.velocity.y -= impulse * rb1.mass;
                
                float friction = std::max(rb1.friction, rb2.friction);
                rb1.velocity.x *= (1.0f - friction);
                rb2.velocity.x *= (1.0f - friction);
                
                glm::vec2 r1 = contactPoint - transform1.position;
                glm::vec2 r2 = contactPoint - transform2.position;
                float inertia1 = rb1.GetInertia(collider1.size);
                float inertia2 = rb2.GetInertia(collider2.size);
                float angularImpulse = (r1.x * contactNormal.y - r1.y * contactNormal.x) * impulse;
                rb1.angularVelocity += angularImpulse / inertia1;
                rb2.angularVelocity -= angularImpulse / inertia2;
            }
        }
        
    private:
        glm::vec2 m_Gravity{0.0f, -9.81f};
        World* m_World = nullptr;
    };
    
}
