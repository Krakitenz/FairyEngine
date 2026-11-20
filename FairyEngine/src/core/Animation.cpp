#include "core/Animation.h"
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <utility>

namespace fairy::core {

    Animation::Animation(std::vector<const Texture*> frames, float fps)
        : m_frames(std::move(frames)), m_fps(fps) {
        if (m_frames.empty()) {
            throw std::invalid_argument("Animation must contain at least one frame");
        }
        if (!std::isfinite(m_fps) || m_fps < 0.0f
            || (m_frames.size() > 1 && m_fps == 0.0f)) {
            throw std::invalid_argument("Animation frame rate must be positive for multiple frames");
        }
        for (const Texture* frame : m_frames) {
            if (!frame) {
                throw std::invalid_argument("Animation frame cannot be null");
            }
        }
    }

    Animation::Animation(std::vector<std::string> framePaths, float fps)
        : m_framePaths(std::move(framePaths)), m_fps(fps) {
        if (m_framePaths.empty()) {
            throw std::invalid_argument("Animation must contain at least one frame path");
        }
        if (!std::isfinite(m_fps) || m_fps < 0.0f
            || (m_framePaths.size() > 1 && m_fps == 0.0f)) {
            throw std::invalid_argument("Animation frame rate must be positive for multiple frames");
        }
        for (const std::string& path : m_framePaths) {
            if (path.empty()) {
                throw std::invalid_argument("Animation frame path cannot be empty");
            }
        }

        loadStreamedFrame();
    }

    void Animation::update(float dt) {
        const std::size_t frameCount = m_framePaths.empty()
            ? m_frames.size() : m_framePaths.size();
        if (frameCount <= 1 || m_fps <= 0.0f || !std::isfinite(dt) || dt <= 0.0f) {
            return;
        }

        const double frameTime = 1.0 / static_cast<double>(m_fps);
        const double elapsed = static_cast<double>(m_timer) + dt;
        if (elapsed < frameTime) {
            m_timer = static_cast<float>(elapsed);
            return;
        }

        const double elapsedFrames = std::floor(elapsed / frameTime);
        const std::size_t advances = static_cast<std::size_t>(
            std::fmod(elapsedFrames, static_cast<double>(frameCount)));
        m_timer = static_cast<float>(std::fmod(elapsed, frameTime));
        m_index = (m_index + advances) % frameCount;

        if (!m_framePaths.empty()) {
            try {
                loadStreamedFrame();
            }
            catch (const std::exception& error) {
                std::fprintf(stderr, "[Resource] Could not load animation frame '%s': %s\n",
                    m_framePaths[m_index].c_str(), error.what());
            }
        }
    }

    const Texture* Animation::currentFrame() const {
        if (!m_framePaths.empty()) {
            return m_streamedFrame.get();
        }
        return m_frames.empty() ? nullptr : m_frames[m_index];
    }

    void Animation::reset() {
        m_timer = 0.0f;
        m_index = 0;
        if (!m_framePaths.empty()) {
            loadStreamedFrame();
        }
    }

    void Animation::loadStreamedFrame() {
        m_streamedFrame = std::make_unique<Texture>(m_framePaths[m_index]);
    }

}