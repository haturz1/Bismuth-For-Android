#include "Buffer.hpp"

using namespace geode::prelude;

Buffer::~Buffer() {
    if (mappedData) {
        if (shouldMap) {
            bindAs(GL_ARRAY_BUFFER);
            glUnmapBuffer(GL_ARRAY_BUFFER);
        } else {
            delete[] mappedData;
        }
        mappedData = nullptr;
    }

    if (id) {
        glDeleteBuffers(1, &id);
        id = 0;
    }
}

void Buffer::read(void* data, usize size, usize offset) {
    assert(data != nullptr);
    assert((offset + size) <= this->size);

    GLint previouslyBoundBuffer = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previouslyBoundBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, id);
#if defined(GL_ES_VERSION_3_0) || defined(__ANDROID__)
    void* mapped = glMapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_READ_BIT);
    if (mapped) {
        std::memcpy(data, mapped, size);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
#else
    glGetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
#endif

    glBindBuffer(GL_ARRAY_BUFFER, previouslyBoundBuffer);
}

void Buffer::write(void* data, usize size, usize offset) {
    assert(data != nullptr);
    assert((offset + size) <= this->size);

    GLint previouslyBoundBuffer = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previouslyBoundBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

    glBindBuffer(GL_ARRAY_BUFFER, previouslyBoundBuffer);
}

void* Buffer::map(GLbitfield access) {
    if (mappedData) return nullptr;

    bindAs(GL_ARRAY_BUFFER);
    if (shouldMap) {
        mappedData = static_cast<u8*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, size, access));
    } else {
        mappedData = new u8[size];
    }

    return mappedData;
}

void Buffer::unmap() {
    if (!mappedData) return;

    if (shouldMap) {
        bindAs(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    } else {
        delete[] mappedData;
    }

    mappedData = nullptr;
}

void Buffer::flush(usize offset, usize size) {
    if (!mappedData) return;

    if (shouldMap) {
        bindAs(GL_ARRAY_BUFFER);
        glFlushMappedBufferRange(GL_ARRAY_BUFFER, offset, size);
    } else {
        write(mappedData + offset, size, offset);
    }
}

UPtr<Buffer> Buffer::create(const char* name, usize size, GLenum usage) {
    u32 buffer = 0;
    glGenBuffers(1, &buffer);

    glObjectLabel(GL_BUFFER, buffer, strlen(name), name);

    GLint previouslyBoundBuffer = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previouslyBoundBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, usage);

    glBindBuffer(GL_ARRAY_BUFFER, previouslyBoundBuffer);

    auto ret = std::make_unique<Buffer>();
    ret->id = buffer;
    ret->size = size;
    ret->shouldMap = Mod::get()->getSettingValue<bool>("use_mapped_buffers");

    return ret;
}
