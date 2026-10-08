#include "core/Player.h"
#include "core/Sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fairy::core {

    Player::Player(const Texture* tex, float x, float y)
        : m_texture(tex),
        m_x(x), m_y(y),
        m_vx(0), m_vy(0),
        m_onGround(false)
    {
    }

    void Player::update(float dt, bool left, bool right, bool jump) {
        if (left && !right)       m_vx = -MOVE_SPEED;
        else if (right && !left)  m_vx = MOVE_SPEED;
        else                      m_vx = 0.0f;

        if (jump && m_onGround) {
            m_vy = JUMP_SPEED;
            m_onGround = false;
        }

        m_vy -= GRAVITY * dt;
        m_x += m_vx * dt;
        m_y += m_vy * dt;

        if (m_y <= GROUND_Y) {
            m_y = GROUND_Y;
            m_vy = 0.0f;
            m_onGround = true;
        }
    }

    void Player::draw(Shader& shader, float camX, float camY,
        float screenW, float screenH) const {
        float aspect = (float)m_texture->width() / (float)m_texture->height();
        float h = 300.0f;
        float w = h * aspect;

        float screenX = m_x - camX + screenW * 0.5f;
        float screenY = m_y - camY + screenH * 0.5f;

        Sprite tmp(m_texture, screenX, screenY + h * 0.5f, w, h);
        tmp.draw(shader);
    }

} // namespace fairy::core