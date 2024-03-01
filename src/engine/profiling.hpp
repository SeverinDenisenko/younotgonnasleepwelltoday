#pragma once

#include <chrono>
#include <source_location>
#include <unordered_map>

#include "defines.hpp"

namespace engine
{

using time_t    = std::chrono::time_point<std::chrono::high_resolution_clock>;
using elapsed_t = f64;

class AutomaticProfilerRegister
{
public:
    void add(cstr name, time_t begin, time_t end);

    static rptr<AutomaticProfilerRegister> get();

    ~AutomaticProfilerRegister();
    AutomaticProfilerRegister() = default;

    std::unordered_map<cstr, elapsed_t> measurements_;

    static uptr<AutomaticProfilerRegister> instance_;
};

class AutomaticProfiler
{
public:
    AutomaticProfiler(cstr name);

    ~AutomaticProfiler();

private:
    time_t begin_;
    time_t end_;
    cstr name_;
};

#define GET_NAME() __FILE__##__FUNCTION__##__LINE__

#if PROFILING == 1
#define PROFILE(name) engine::AutomaticProfiler GET_NAME()(#name)
#define PROFILE_FUNCTION() engine::AutomaticProfiler GET_NAME()(std::source_location::current().function_name())
#else
#define PROFILE(name)
#define PROFILE_FUNCTION()
#endif

} // namespace engine
