#pragma once
#include "../ECS/Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <array>
#include <algorithm>
#include <limits>

namespace UniversalEngine {

class BoxCollider2D : public Component {
public:
    glm::vec2 size{1.0f, 1.0f};
    glm::vec2 offset{0.0f, 0.0f};
    bool isTrigger{false};
    bool isStatic{false};

    BoxCollider2D() = default;
    BoxCollider2D(const glm::vec2& colliderSize, bool trigger = false, bool staticCollider = false)
        : size(colliderSize), isTrigger(trigger), isStatic(staticCollider) {}
    BoxCollider2D(const glm::vec2& colliderSize, const glm::vec2& colliderOffset, bool trigger = false, bool staticCollider = false)
        : size(colliderSize), offset(colliderOffset), isTrigger(trigger), isStatic(staticCollider) {}

    bool IntersectsAABB(const BoxCollider2D& other, const glm::vec2& thisPos, const glm::vec2& otherPos) const {
        glm::vec2 thisMin = thisPos + offset - size * 0.5f;
        glm::vec2 thisMax = thisPos + offset + size * 0.5f;
        glm::vec2 otherMin = otherPos + other.offset - other.size * 0.5f;
        glm::vec2 otherMax = otherPos + other.offset + other.size * 0.5f;
        return (thisMin.x < otherMax.x && thisMax.x > otherMin.x && thisMin.y < otherMax.y && thisMax.y > otherMin.y);
    }
};

namespace detail {
struct OBB {
    glm::vec2 c;
    std::array<glm::vec2, 2> axis;
    glm::vec2 he;
    
    std::array<glm::vec2, 4> corners() const {
        glm::vec2 ax = axis[0] * he.x;
        glm::vec2 ay = axis[1] * he.y;
        return { c - ax - ay, c + ax - ay, c + ax + ay, c - ax + ay };
    }
};

inline OBB makeOBB(const glm::vec2& position, float rotation, const BoxCollider2D& collider) {
    float rotRad = glm::radians(rotation);
    float s = std::sin(rotRad);
    float c = std::cos(rotRad);
    glm::vec2 ax = { c, s };
    glm::vec2 ay = { -s, c };
    return { position + collider.offset, {ax, ay}, collider.size * 0.5f };
}

inline std::pair<float, float> project(const OBB& b, const glm::vec2& n) {
    float center = glm::dot(b.c, n);
    float radius = std::abs(glm::dot(b.axis[0] * b.he.x, n)) + std::abs(glm::dot(b.axis[1] * b.he.y, n));
    return { center - radius, center + radius };
}

inline bool satMTV(const OBB& a, const OBB& b, glm::vec2& nOut, float& dOut) {
    glm::vec2 axes[4] = { a.axis[0], a.axis[1], b.axis[0], b.axis[1] };
    glm::vec2 d = b.c - a.c;
    float minPen = std::numeric_limits<float>::infinity();
    glm::vec2 bestN{0};
    
    for (glm::vec2 ax : axes) {
        glm::vec2 n = glm::normalize(ax);
        auto pa = project(a, n);
        auto pb = project(b, n);
        float overlap = std::min(pa.second, pb.second) - std::max(pa.first, pb.first);
        if (overlap <= 0.f) return false;
        if (overlap < minPen) {
            minPen = overlap;
            bestN = (glm::dot(d, n) < 0.f) ? -n : n;
        }
    }
    nOut = bestN;
    dOut = minPen;
    return true;
}

inline glm::vec2 support(const std::array<glm::vec2, 4>& pts, const glm::vec2& dir) {
    int idx = 0;
    float m = glm::dot(pts[0], dir);
    for (int i = 1; i < 4; ++i) {
        float s = glm::dot(pts[i], dir);
        if (s > m) {
            m = s;
            idx = i;
        }
    }
    return pts[idx];
}
}

inline bool IntersectsOBB(const glm::vec2& aPos, float aRot, const BoxCollider2D& aCollider,
                          const glm::vec2& bPos, float bRot, const BoxCollider2D& bCollider) {
    detail::OBB A = detail::makeOBB(aPos, aRot, aCollider);
    detail::OBB B = detail::makeOBB(bPos, bRot, bCollider);
    glm::vec2 n;
    float d;
    return detail::satMTV(A, B, n, d);
}

}
