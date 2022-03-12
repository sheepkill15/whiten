#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {
public:
    Texture() = default;

    void create(int tex_width, int tex_height, int tex_channels, int tex_slot = 0) {
        glGenTextures(1, &texture);
        slot = tex_slot;
        width = tex_width;
        height = tex_height;
        channels = tex_channels;

        if(tex_channels == 3) {
            format = GL_RGB;
            channel_format = GL_RGB;
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        else {
            format = GL_RGBA32F;
            channel_format = GL_RGBA;
        }

        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0, channel_format, GL_FLOAT, nullptr);
    }

    void bind() const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void load(void* data) const {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, channel_format, GL_UNSIGNED_BYTE, data);
    }

    void bind_as_image() const {
        bind();
        glBindImageTexture(slot, texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    void resize(int new_width, int new_height) const {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, format, new_width, new_height, 0, channel_format, GL_FLOAT, nullptr);
    }

    [[nodiscard]] GLuint get() const { return texture; }

private:
    GLuint texture;
    int slot;
    int channels;
    GLint format;
    GLint channel_format;
public:
    int width, height;
};

#endif