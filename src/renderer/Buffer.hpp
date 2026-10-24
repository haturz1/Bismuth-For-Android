#pragma once

#include "Geode/cocos/platform/win32/CCGL.h"
#include <common.hpp>

#define GL_SHADER_STORAGE_BUFFER 0x90D2

class Buffer {
public:
    ~Buffer();

    inline usize getSize() const { return size; }
    inline u32 getId() const { return id; }

    void read(void* data, usize size, usize offset = 0);

    void write(void* data, usize size, usize offset = 0);

    inline void bindAs(GLenum binding) {
        glBindBuffer(binding, id);
    }

    inline void bindAsUniformBuffer(u32 binding) {
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
    }

    inline void bindAsStorageBuffer(u32 binding) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
    }

    void* map(i32 type);

    inline void* mapWriteOnly() {
        return map(GL_WRITE_ONLY);
    }

#pragma once

#include "platform/GLPlatform.hpp"
#include <common.hpp>

#define GL_SHADER_STORAGE_BUFFER 0x90D2

class Buffer {
public:
    ~Buffer();

    inline usize getSize() const { return size; }
    inline u32 getId() const { return id; }

    void read(void* data, usize size, usize offset = 0);
    void write(void* data, usize size, usize offset = 0);

    inline void bindAs(GLenum binding) {
        glBindBuffer(binding, id);
    }

    inline void bindAsUniformBuffer(u32 binding) {
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
    }

    inline void bindAsStorageBuffer(u32 binding) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
    }

    void* map(GLbitfield access);

    inline void* mapWriteOnly() {
        return map(GL_MAP_WRITE_BIT);
    }

    inline void* mapReadWrite() {
        return map(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    }

    void unmap();

    void flush(usize offset, usize size);

public:
    static UPtr<Buffer> create(const char* name, usize size, GLenum usage);

    inline static UPtr<Buffer> createStaticDraw(const char* name, void* data, usize size) {
        auto ret = create(name, size, GL_STATIC_DRAW);
        if (data)
            ret->write(data, size);
        return ret;
    }

    inline static UPtr<Buffer> createDynamicDraw(const char* name, usize size) {
        return create(name, size, GL_DYNAMIC_DRAW);
    }

    inline static UPtr<Buffer> createDynamicCopy(const char* name, usize size) {
        // GLES does not reliably support GL_DYNAMIC_COPY the same way desktop GL does
        return create(name, size, GL_DYNAMIC_DRAW);
    }

    static UPtr<Buffer> createGPUExclusive(const char* name, usize size) {
        return create(name, size, GL_DYNAMIC_DRAW);
    }

private:
    usize size = 0;
    u32 id = 0;
    bool shouldMap = true;
    u8* mappedData = nullptr;
};
fer to be written to many times repeatedly by the GPU
        and used by the CPU
    */
    inline static UPtr<Buffer> createDynamicCopy(const char* name, usize size) {
        return create(name, size, GL_DYNAMIC_COPY);
    }

    /*
        Buffer to be written to and read from by the GPU exclusively
    */
    static UPtr<Buffer> createGPUExclusive(const char* name, usize size) {
        return create(name, size, GL_DYNAMIC_DRAW); // GL_DYNAMIC_DRAW for now
    }

private:
    usize size = 0;
    u32 id = 0;
    bool shouldMap = true;
    u8* mappedData = nullptr;
};
