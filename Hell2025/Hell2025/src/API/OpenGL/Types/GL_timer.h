// GL_timer.h
#pragma once

#include <glad/glad.h>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <deque>
#include <chrono>

#if !defined(OPENGL_PROFILING)
#define OPENGL_PROFILING 1
#endif

#if OPENGL_PROFILING

struct OpenGLFrameTimer {
    static constexpr uint32_t kSkipZoneFrames = 1;
    static constexpr size_t   kAverageFrameCount = 60;
    static constexpr size_t   kOutputPrecision = 2;
    static constexpr bool     kStrictZones = false;

    using Clock = std::chrono::steady_clock;

    struct RollingAverage {
        std::deque<double> mSamples;
        size_t mCapacity = kAverageFrameCount;
        double mSum = 0.0;

        void Clear();
        void Push(double value);
        double GetValue() const;
    };

    struct Zone {
        std::string mName;

        RollingAverage mCpuRollingAverage;
        RollingAverage mGpuRollingAverage;

        Clock::time_point mCpuStartTime{};
        bool mCpuRunning = false;

        GLuint   mStartTimestampQueryId = 0;
        GLuint   mEndTimestampQueryId = 0;
        uint64_t mStartFrameId = 0;
        uint64_t mEndFrameId = 0;
        uint64_t mTouchedFrameId = 0;
        int      mNestingDepth = 0;
        bool     mInitialized = false;

        Zone() = default;
        void Init(const std::string& name);
        void Destroy();
    };

    struct ZoneScope {
        OpenGLFrameTimer* mTimer = nullptr;
        std::string mName;
        bool mActive = false;

        ZoneScope(OpenGLFrameTimer& timer, const char* name);
        ZoneScope(OpenGLFrameTimer& timer, const std::string& name);
        ZoneScope(const ZoneScope&) = delete;
        ZoneScope& operator=(const ZoneScope&) = delete;
        ZoneScope(ZoneScope&&) = delete;
        ZoneScope& operator=(ZoneScope&&) = delete;
        ~ZoneScope();

        void Release();
    };

    struct FrameScope {
        OpenGLFrameTimer* mTimerPtr = nullptr;
        bool mActive = false;

        FrameScope(OpenGLFrameTimer& timer);
        FrameScope(const FrameScope&) = delete;
        FrameScope& operator=(const FrameScope&) = delete;
        FrameScope(FrameScope&&) = delete;
        FrameScope& operator=(FrameScope&&) = delete;
        ~FrameScope();

        void Release();
    };

    OpenGLFrameTimer() = default;

    void BeginFrame();
    void EndFrame();
    void BeginZone(const std::string& name);
    void EndZone(const std::string& name);
    void Reset();

    const std::string& GetZoneList() const;
    const std::string& GetCPUTimingList() const;
    const std::string& GetGPUTimingList() const;

    const std::string& GetTotalCPUFrameTime() const;
    const std::string& GetTotalGPUFrameTime() const;

    std::unordered_map<std::string, Zone> mZones;

    RollingAverage mCpuFrameRollingAverage;
    RollingAverage mGpuFrameRollingAverage;

    GLuint   mFrameStartQueryId = 0;
    GLuint   mFrameEndQueryId = 0;
    uint64_t mFrameId = 0;
    bool     mInitialized = false;

private:
    void ClearStrings();
    void BuildStringsFromCurrentFrame();
    static std::string FormatMs(double ms, size_t precision);

    Clock::time_point mCpuFrameStartTime{};
    bool mCpuFrameRunning = false;

    std::string m_zoneList;
    std::string m_cpuTimingList;
    std::string m_gpuTimingList;

    std::string m_totalCpuFrameTime;
    std::string m_totalGpuFrameTime;
};

OpenGLFrameTimer& GetTimer();

#define ConcatImpl(a,b) a##b
#define Concat(a,b) ConcatImpl(a,b)

#define ProfilerOpenGLFrame() \
    OpenGLFrameTimer::FrameScope Concat(_gpu_frame_, __COUNTER__){ GetTimer() }

#define ProfilerOpenGLZone(label) \
    OpenGLFrameTimer::ZoneScope Concat(_gpu_zone_, __COUNTER__){ GetTimer(), (label) }

#define ProfilerOpenGLZoneNames()   (GetTimer().GetZoneList())
#define ProfilerOpenGLCpuTimings()  (GetTimer().GetCPUTimingList())
#define ProfilerOpenGLGpuTimings()  (GetTimer().GetGPUTimingList())
#define ProfilerOpenGLTotalCPU()    (GetTimer().GetTotalCPUFrameTime())
#define ProfilerOpenGLTotalGPU()    (GetTimer().GetTotalGPUFrameTime())

static inline std::string ProfilerOpenGLStripToFinalFunctionName(const char* functionString) {
    std::string_view v = functionString ? functionString : "";

    size_t paren = v.find('(');
    if (paren != std::string_view::npos) v = v.substr(0, paren);

    size_t scope = v.rfind("::");
    if (scope != std::string_view::npos) v = v.substr(scope + 2);

    size_t space = v.rfind(' ');
    if (space != std::string_view::npos) v = v.substr(space + 1);

    return std::string(v);
}

#if defined(_MSC_VER)
#define ProfilerOpenGLZoneFunction() \
        ProfilerOpenGLZone(ProfilerOpenGLStripToFinalFunctionName(__FUNCSIG__))
#else
#define ProfilerOpenGLZoneFunction() \
        ProfilerOpenGLZone(ProfilerOpenGLStripToFinalFunctionName(__PRETTY_FUNCTION__))
#endif

#else

struct OpenGLFrameTimer {
    OpenGLFrameTimer() = default;
    void BeginFrame() {}
    void EndFrame() {}
    void BeginZone(const std::string&) {}
    void EndZone(const std::string&) {}
    void Reset() {}

    const std::string& GetZoneList() const { static std::string s; return s; }
    const std::string& GetCPUTimingList() const { static std::string s; return s; }
    const std::string& GetGPUTimingList() const { static std::string s; return s; }
    const std::string& GetTotalCPUFrameTime() const { static std::string s; return s; }
    const std::string& GetTotalGPUFrameTime() const { static std::string s; return s; }

    struct ZoneScope {
        ZoneScope(OpenGLFrameTimer&, const char*) {}
        ZoneScope(OpenGLFrameTimer&, const std::string&) {}
        void Release() {}
    };
    struct FrameScope {
        FrameScope(OpenGLFrameTimer&) {}
        void Release() {}
    };
};

inline OpenGLFrameTimer& GetTimer() {
    static OpenGLFrameTimer t;
    return t;
}

#define ConcatImpl(a,b) a##b
#define Concat(a,b) ConcatImpl(a,b)

#define ProfilerOpenGLFrame()
#define ProfilerOpenGLZone(label)

#define ProfilerOpenGLZoneNames()   (GetTimer().GetZoneList())
#define ProfilerOpenGLCpuTimings()  (GetTimer().GetCPUTimingList())
#define ProfilerOpenGLGpuTimings()  (GetTimer().GetGPUTimingList())
#define ProfilerOpenGLTotalCPU()    (GetTimer().GetTotalCPUFrameTime())
#define ProfilerOpenGLTotalGPU()    (GetTimer().GetTotalGPUFrameTime())

#define ProfilerOpenGLZoneFunction()

#endif
