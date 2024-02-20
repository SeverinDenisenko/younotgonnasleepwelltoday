#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <memory>
#include <utility>

#include "raylib.h"

namespace engine
{
using u8   = uint8_t;
using i8   = int8_t;
using u16  = uint16_t;
using i16  = int16_t;
using u32  = uint32_t;
using i32  = int32_t;
using u64  = uint64_t;
using i64  = int64_t;
using f32  = float;
using f64  = double;
using cstr = const char*;
using byte = u8;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T>
using wptr = std::weak_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

template <typename T>
using rptr = T*;

struct vec2 {
    vec2(float x, float y)
        : x(x)
        , y(y)
    {
    }

    vec2()
        : x(0.0f)
        , y(0.0f)
    {
    }

    float x;
    float y;
};

struct vec3 {
    vec3(float x, float y, float z)
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

    float x;
    float y;
    float z;
};

constexpr f32 pi = 3.14159f;

} // namespace engine
