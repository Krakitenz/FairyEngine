#pragma once
#include <vector>
#include <string>
#include <memory>
#include <cstddef>
#include "core/Texture.h"

namespace fairy::core {

    class Animation {
    public:
        // frames — список текстур, fps — кадров в секунду
        Animation(std::vector<const Texture*> frames, float fps);
        Animation(std::vector<std::string> framePaths, float fps);

        void update(float dt);           // dt в секундах
        const Texture* currentFrame() const;
        void reset();

    private:
        void loadStreamedFrame();

        std::vector<const Texture*> m_frames;
        std::vector<std::string> m_framePaths;
        std::unique_ptr<Texture> m_streamedFrame;
        float m_fps;
        float m_timer = 0.0f;
        std::size_t m_index = 0;
    };

}