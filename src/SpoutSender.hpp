#pragma once

#include <string>
#include <cstdint>

// Thin RAII wrapper around the bundled Spout2 SDK.
// Sends an OpenGL texture (the fully decorated GD frame) to OBS.
namespace trakines {

class SpoutSender {
public:
    static SpoutSender& get();

    // Create/refresh the sender with the given name and dimensions.
    void ensure(const std::string& name, unsigned int width, unsigned int height);

    // Send an OpenGL texture id (GL_TEXTURE_2D) to the Spout receiver.
    // invert = flip vertically (GD/OpenGL textures are bottom-up).
    void sendTexture(unsigned int glTextureId, unsigned int width, unsigned int height, bool invert = true);

    void release();

    bool active() const { return m_initialized; }

    ~SpoutSender();

private:
    SpoutSender() = default;
    SpoutSender(const SpoutSender&) = delete;
    SpoutSender& operator=(const SpoutSender&) = delete;

    void* m_spout = nullptr; // opaque pointer to spout::SPOUT object
    std::string m_name;
    unsigned int m_width = 0;
    unsigned int m_height = 0;
    bool m_initialized = false;
};

} // namespace trakines
