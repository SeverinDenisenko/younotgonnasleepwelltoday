#include "raylib.h"

#include "defines.hpp"

namespace en
{
class IGame
{
public:
    virtual void setup() noexcept    = 0;
    virtual void update() noexcept   = 0;
    virtual void shutdown() noexcept = 0;

    virtual ~IGame() = default;
};

class IRunner
{
public:
    virtual void run() noexcept = 0;

    virtual ~IRunner() = default;
};

class Runner : public IRunner
{
public:
    Runner(uptr<IGame> game)
        : game_(std::move(game))
    {
    }

    void run() noexcept override
    {
        game_->setup();

        while (!WindowShouldClose()) {
            game_->update();
        }

        game_->shutdown();
    }

private:
    uptr<IGame> game_;
};
} // namespace en

namespace ge
{
class Game : public en::IGame
{
public:
    void setup() noexcept override
    {
        InitWindow(800, 450, "raylib [core] example - basic window");
    }
    void update() noexcept override
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }
    void shutdown() noexcept override
    {
        CloseWindow();
    }
};
} // namespace ge


int main(void)
{
    en::uptr<en::IGame> game    = std::make_unique<ge::Game>();
    en::uptr<en::Runner> runner = std::make_unique<en::Runner>(std::move(game));
    runner->run();

    return 0;
}
