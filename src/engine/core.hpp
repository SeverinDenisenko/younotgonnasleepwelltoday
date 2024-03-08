#pragma once

#include "defines.hpp"

namespace engine {
class IGame {
public:
    virtual void setup() noexcept    = 0;
    virtual void update() noexcept   = 0;
    virtual void shutdown() noexcept = 0;
    virtual bool running() noexcept  = 0;

    virtual ~IGame() = default;
};

class Game : public IGame {
public:
    Game(i32 width, i32 height, cstr title) noexcept;
    Game(cstr title) noexcept;

    void setup() noexcept override;

    bool running() noexcept override;

    void shutdown() noexcept override;

    void exit() noexcept;

    i32 width() const noexcept;

    i32 height() const noexcept;

private:
    i32 width_{800};
    i32 height_{450};
    cstr title_{nullptr};
    bool exit_{false};
    bool fullscreen_{false};
};

class IRunner {
public:
    virtual void run() noexcept = 0;

    virtual ~IRunner() = default;
};

class Runner : public IRunner {
public:
    Runner(uptr<IGame> game) noexcept;

    void run() noexcept override;

private:
    uptr<IGame> game_;
};
} // namespace engine
