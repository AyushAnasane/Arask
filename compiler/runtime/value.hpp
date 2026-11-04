#pragma once

#include <cstdint>
#include <variant>

namespace newlang {

using RuntimeValue = std::variant<
    std::int32_t,
    std::int64_t
>;

}