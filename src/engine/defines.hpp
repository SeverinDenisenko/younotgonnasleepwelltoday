#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <memory>
#include <string>
#include <utility>

#include "raylib.h"

namespace engine {
using u8     = uint8_t;
using i8     = int8_t;
using u16    = uint16_t;
using i16    = int16_t;
using u32    = uint32_t;
using i32    = int32_t;
using u64    = uint64_t;
using i64    = int64_t;
using f32    = float;
using f64    = double;
using cstr   = const char*;
using byte   = u8;
using string = std::string;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T>
using wptr = std::weak_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

template <typename T>
using rptr = T*;

struct vec2 {
    vec2(f32 x, f32 y)
        : x(x)
        , y(y)
    {
    }

    vec2()
        : x(0.0f)
        , y(0.0f)
    {
    }

    f32 x;
    f32 y;
};

struct vec3 {
    vec3(f32 x, f32 y, f32 z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    vec3()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {
    }

    f32 x;
    f32 y;
    f32 z;
};

constexpr f32 pi = 3.14159f;

} // namespace engine
