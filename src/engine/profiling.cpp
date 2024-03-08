#include "profiling.hpp"

#include <iomanip>
#include <iostream>
#include <map>

namespace engine {

uptr<AutomaticProfilerRegister> AutomaticProfilerRegister::instance_ = nullptr;

rptr<AutomaticProfilerRegister> AutomaticProfilerRegister::get()
{
    if (!instance_) {
        instance_ = std::make_unique<AutomaticProfilerRegister>();
    }

    return instance_.get();
}

void AutomaticProfilerRegister::add(cstr name, time_t begin, time_t end)
{
    std::chrono::duration<elapsed_t> diff = end - begin;

    if (measurements_.contains(name)) {
        measurements_[name] += diff.count();
    }
    else {
        measurements_[name] = diff.count();
    }
}

AutomaticProfilerRegister::~AutomaticProfilerRegister()
{
    std::map<elapsed_t, cstr> results;

    for (auto& [name, elapsed] : measurements_) {
        results.emplace(elapsed, name);
    }

    for (auto& [elapsed, name] : results) {
        std::cout << std::fixed << elapsed << "s\t:\t" << std::string(name) << std::endl;
    }
}

AutomaticProfiler::AutomaticProfiler(cstr name)
    : name_(name)
{
    begin_ = std::chrono::high_resolution_clock::now();
}

AutomaticProfiler::~AutomaticProfiler()
{
    end_ = std::chrono::high_resolution_clock::now();

    AutomaticProfilerRegister::get()->add(name_, begin_, end_);
}


} // namespace engine
