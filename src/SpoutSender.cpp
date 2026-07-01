#include "SpoutSender.hpp"

// Bundled Spout2 SDK. See libs/Spout2. This is the high-level SDK entry point
// which internally handles GL/DX interop and shared texture creation.
//
// IMPORTANT: this TU must not include any Geode/cocos headers, because Spout's
// bundled SpoutGLextensions.h conflicts with cocos' glew.h.
#include <SpoutGL/Spout.h>

namespace trakines {

static Spout* asSpout(void* p) {
    return static_cast<Spout*>(p);
}

SpoutSender& SpoutSender::get() {
    static SpoutSender inst;
    return inst;
}

SpoutSender::~SpoutSender() {
    release();
}

void SpoutSender::ensure(const std::string& name, unsigned int width, unsigned int height) {
    if (width == 0 || height == 0) return;

    if (!m_spout) {
        m_spout = new Spout();
    }

    auto spout = asSpout(m_spout);

    // Recreate the sender if the name or the resolution changed.
    if (!m_initialized || m_name != name) {
        if (m_initialized) {
            spout->ReleaseSender();
            m_initialized = false;
        }
        spout->SetSenderName(name.c_str());
        m_name = name;
    }

    if (!m_initialized || m_width != width || m_height != height) {
        // CreateSender is idempotent-ish; UpdateSender handles resize once created.
        if (!m_initialized) {
            spout->CreateSender(name.c_str(), width, height);
        } else {
            spout->UpdateSender(name.c_str(), width, height);
        }
        m_width = width;
        m_height = height;
        m_initialized = true;
    }
}

void SpoutSender::sendTexture(unsigned int glTextureId, unsigned int width, unsigned int height, bool invert) {
    if (!m_initialized || !m_spout) return;
    if (width == 0 || height == 0) return;

    auto spout = asSpout(m_spout);

    if (m_width != width || m_height != height) {
        spout->UpdateSender(m_name.c_str(), width, height);
        m_width = width;
        m_height = height;
    }

    // 0x0DE1 == GL_TEXTURE_2D. Kept as a literal to avoid pulling GL headers here.
    spout->SendTexture(glTextureId, 0x0DE1, width, height, invert, 0);
}

void SpoutSender::release() {
    if (m_spout) {
        auto spout = asSpout(m_spout);
        if (m_initialized) {
            spout->ReleaseSender();
        }
        delete spout;
        m_spout = nullptr;
    }
    m_initialized = false;
    m_width = 0;
    m_height = 0;
}

} // namespace trakines
