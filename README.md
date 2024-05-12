# Game framework for 2d.

## Plan

* Basic setup ✔️
* * Show window with staff ✔️
* * Correct view for mutliple aspect ratio ✔️
* ECS ✔️
* Rendering
* * Sprite rendering ✔️
* * Sprite animation
* * UI rendering
* * * UI components
* * Rendering demo
* Audio ✔️
* * Audio demo
* Virtual filesystem ✔️
* Physics
* * Task-based parallelism
* * Physics demo
* Event system overlay
* Setup CI

## Tooling

### Profiling

When "-DPROFILING=1" is enabled, on shutdown in console will appear:

```
Brief stats per frame:
     0.013486 ms :	void impl::RenderSystem::texures(Storage &, bool)
     0.014534 ms :	void impl::RenderSystem::text(Storage &, bool)
     0.029762 ms :	virtual void impl::AudioSystem::update(Storage &)
     8.752665 ms :	virtual void impl::RenderSystem::update(Storage &)
     8.795858 ms :	virtual void impl::Game::update()
    31.585375 ms :	virtual void impl::Game::shutdown()
   312.231541 ms :	virtual void impl::Game::setup()
```

To profile function, use `PROFILE_FUNCTION();` macro in the begginig of the function.
