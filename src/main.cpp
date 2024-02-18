#include "box2d/box2d.h"
#include "raylib.h"

#include "engine/core.hpp"
#include "engine/defines.hpp"
#include "engine/ecs.hpp"


namespace impl
{
class Game : public engine::Game
{
public:
    Game()
        : engine::Game(800, 450, "You Not Gonna Dleep Well Today")
    {
    }

    void setup() noexcept override
    {
        engine::Game::setup();

        ecs.registerComponent<engine::vec3>();
        ecs.finishRegistration();

        engine::ecs::EntityBuilder builder = ecs.getEntityBuilder();
        engine::ecs::EntityStorage storage = ecs.getEntityStorage();

        engine::ecs::entity_id_t id = storage.createEntity(builder.clear().withComponent<engine::vec3>().build());

        engine::vec3& vec = storage.getComponent<engine::vec3>(id);
        vec.x             = 0;
        vec.y             = 0;
        vec.z             = 0;
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
        engine::Game::shutdown();
    }

private:
    engine::ecs::ECS ecs;
};
} // namespace impl


int main(void)
{
    engine::uptr<engine::IGame> game    = std::make_unique<impl::Game>();
    engine::uptr<engine::Runner> runner = std::make_unique<engine::Runner>(std::move(game));
    runner->run();

    return 0;
}
