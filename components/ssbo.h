#ifndef SSBO_H
#define SSBO_H

class Ssbo
{
public:
    Ssbo(int size, void* data, int own_slot)
    {
        slot = own_slot;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ~Ssbo() {
        bind();
        glDeleteBuffers(1, &ssbo);
    }

    void bind() const {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, ssbo);
    }

    void resize(int size) const {
        bind();
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_STATIC_READ);
    }

    void get_data(int size, void* out_data) const {
        bind();
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, out_data);
    }

private:
    GLuint ssbo{};
    int slot;
};

#endif