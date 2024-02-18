#include "core.hpp"

#include "raylib.h"

namespace engine
{

Game::Game(i32 width, i32 height, cstr title) noexcept
    : width_(width)
    , height_(height)
    , title_(title)
{
}

void Game::setup() noexcept
{
    InitWindow(width_, height_, title_);
    SetTargetFPS(60);
}

bool Game::running() noexcept
{
    return !WindowShouldClose();
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
