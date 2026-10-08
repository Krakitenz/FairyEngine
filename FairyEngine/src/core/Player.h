#pragma once
#include "core/Texture.h"
#include "core/Shader.h"

namespace fairy::core {

    class Player {
    public:
        Player(const Texture* tex, float x, float y);

        void update(float dt, bool left, bool right, bool jump);
        void draw(Shader& shader, float camX, float camY, float screenW, float screenH) const;

        float x()        const { return m_x; }
        float y()        const { return m_y; }
        bool  onGround() const { return m_onGround; }
        void setPosition(float x, float y) { m_x = x; m_y = y; }

    private:
        static constexpr float GRAVITY = 1500.0f;
        static constexpr float MOVE_SPEED = 300.0f;
        static constexpr float JUMP_SPEED = 600.0f;
        static constexpr float GROUND_Y = 120.0f;

        const Texture* m_texture;
        float m_x, m_y;
        float m_vx, m_vy;
        bool  m_onGround = false;
    };

}