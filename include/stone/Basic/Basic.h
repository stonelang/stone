#ifndef STONE_BASIC_BASIC_H
#define STONE_BASIC_BASIC_H

#include <stdint.h>
#include <vector>

namespace stone {

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using int32 = int32_t;
using int64 = int64_t;

template <typename T> using SafePtr = std::unique_ptr<T>;
template <typename F, typename S> using Pair = std::pair<F, S>;

} // namespace stone
#endif
