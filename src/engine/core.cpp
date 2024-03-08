#include "core.hpp"

#include "raylib.h"

namespace engine {

Game::Game(i32 width, i32 height, cstr title) noexcept
    : width_(width)
    , height_(height)
    , title_(title)
{
}

Game::Game(cstr title) noexcept
    : width_(GetMonitorWidth(GetCurrentMonitor()))
    , height_(GetMonitorHeight(GetCurrentMonitor()))
    , title_(title)
    , fullscreen_(true)
{
}

void Game::setup() noexcept
{
    InitWindow(width_, height_, title_);
    if (fullscreen_) {
        ToggleFullscreen();
    }
    width_  = GetRenderWidth();
    height_ = GetRenderHeight();
    SetTargetFPS(60);
    SetExitKey(0);
}

bool Game::running() noexcept
{
    return !WindowShouldClose() && !exit_;
}

void Game::exit() noexcept
{
    exit_ = true;
}

i32 Game::width() const noexcept
{
    return width_;
}

i32 Game::height() const noexcept
{
    return height_;
}


void Game::shutdown() noexcept
{
    CloseWindow();
}


Runner::Runner(uptr<IGame> game) noexcept
    : game_(std::move(game))
{
}

void Runner::run() noexcept
{
    game_->setup();

    while (game_->running()) {
        game_->update();
    }

    game_->shutdown();
}

} // namespace engine
