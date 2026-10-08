#include "core/Animation.h"

namespace fairy::core {

    Animation::Animation(std::vector<const Texture*> frames, float fps)
        : m_frames(std::move(frames)), m_fps(fps) {
    }

    void Animation::update(float dt) {
        if (m_frames.size() <= 1) return;
        m_timer += dt;
        float frameTime = 1.0f / m_fps;
        while (m_timer >= frameTime) {
            m_timer -= frameTime;
            m_index = (m_index + 1) % (int)m_frames.size();
        }
    }

    const Texture* Animation::currentFrame() const {
        return m_frames[m_index];
    }

    void Animation::reset() {
        m_timer = 0.0f;
        m_index = 0;
    }

}