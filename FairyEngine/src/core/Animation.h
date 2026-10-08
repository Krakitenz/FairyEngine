#pragma once
#include <vector>
#include "core/Texture.h"

namespace fairy::core {

    class Animation {
    public:
        // frames — список текстур, fps — кадров в секунду
        Animation(std::vector<const Texture*> frames, float fps);

        void update(float dt);           // dt в секундах
        const Texture* currentFrame() const;
        void reset();

    private:
        std::vector<const Texture*> m_frames;
        float m_fps;
        float m_timer = 0.0f;
        int   m_index = 0;
    };

}