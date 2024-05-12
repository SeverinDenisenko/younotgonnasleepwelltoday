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
        measurements_[name].elapsed += diff.count();
        measurements_[name].count += 1;
    }
    else {
        measurements_[name].elapsed = diff.count();
        measurements_[name].count   = 1;
    }
}

AutomaticProfilerRegister::~AutomaticProfilerRegister()
{
    std::map<elapsed_t, cstr> results;

    for (auto& [name, entry] : measurements_) {
        results.emplace(entry.elapsed / entry.count, name);
    }

    std::cout << "Brief stats per frame:" << std::endl;
    for (auto& [elapsed, name] : results) {
        std::cout << std::fixed << std::setw(13) << elapsed * 1000.0 << " ms :\t" << std::string(name) << std::endl;
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
