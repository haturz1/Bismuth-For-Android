#pragma once

#include <common.hpp>
#include <profiler.hpp>

#if defined(__ANDROID__)
static inline void getQueryResult64(GLuint id, GLint64* out) {
    GLuint result = 0;
    glGetQueryObjectuiv(id, GL_QUERY_RESULT, &result);
    *out = static_cast<GLint64>(result);
}
#elif defined(GL_ES_VERSION_3_0)
static inline void getQueryResult64(GLuint id, GLint64* out) {
    GLuint result = 0;
    glGetQueryObjectuiv(id, GL_QUERY_RESULT, &result);
    *out = static_cast<GLint64>(result);
}
#else
static inline void getQueryResult64(GLuint id, GLint64* out) {
    glGetQueryObjecti64v(id, GL_QUERY_RESULT, out);
}
#endif

class GPUTimer {
public:
    inline GPUTimer(const char* name)
        : name(name)
    {
        glGenQueries(1, &queryObject);
        profiler::addTimer(this);
    }

    inline ~GPUTimer() {
        glDeleteQueries(1, &queryObject);
        profiler::removeTimer(this);
    }

    inline void start() {
        if (profiler::isTakingSnapshot)
            glBeginQuery(GL_TIME_ELAPSED, queryObject);
    }

    inline void stop() {
        if (profiler::isTakingSnapshot) {
            glEndQuery(GL_TIME_ELAPSED);
            getQueryResult64(queryObject, &time);
        }
    }

    inline const char* getName() const { return name; }
    inline i64 getTime() const { return time; }

private:
    i64 time = 0;
    const char* name;
    u32 queryObject = 0;
};
