#pragma once

#include <common/vector.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    namespace vertex
    {
        enum CullMode
        {
            CULL_NONE,
            CULL_BACK,
            CULL_FRONT,
            CULL_MAX_ENUM
        };

        enum Winding
        {
            WINDING_CW,
            WINDING_CCW,
            WINDING_MAX_ENUM
        };

        enum TriangleIndexMode
        {
            TRIANGLE_NONE,
            TRIANGLE_STRIP,
            TRIANGLE_FAN,
            TRIANGLE_QUADS
        };

        enum PrimitiveType
        {
            PRIMITIVE_TRIANGLES,
            PRIMITIVE_TRIANGLE_STRIP,
            PRIMITIVE_TRIANGLE_FAN,
            PRIMITIVE_QUADS,
            PRIMITIVE_POINTS,
            PRIMITIVE_MAX_ENUM
        };

        /*
        ** Position, Color
        ** Position, TexCoords, Color
        */
        enum class CommonFormat
        {
            NONE,
            PRIMITIVE,
            TEXTURE
        };

        static inline uint16_t normto16t(float in)
        {
            return uint16_t(in * 0xFFFF);
        }

        // clang-format off
        static constexpr BidirectionalMap cullModes = {
            "none",  CULL_NONE,
            "back",  CULL_BACK,
            "front", CULL_FRONT
        };

        static constexpr BidirectionalMap windingModes = {
            "cw",  WINDING_CW,
            "ccw", WINDING_CCW
        };

        static constexpr BidirectionalMap triangleModes = {
            "none",  TRIANGLE_NONE,
            "strip", TRIANGLE_STRIP,
            "fan",   TRIANGLE_FAN,
            "quads", TRIANGLE_QUADS
        };
        // clang-format on
    } // namespace vertex
} // namespace love