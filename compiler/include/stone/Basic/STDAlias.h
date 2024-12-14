#ifndef STONE_BASIC_STDTYPEALIAS_H
#define STONE_BASIC_STDTYPEALIAS_H

#include <stdint.h>
#include <vector>

namespace stone {
using UInt8 = uint8_t;
using UInt16 = uint16_t;
using UInt32 = uint32_t;
using UInt64 = uint64_t;
using Int32 = int32_t;
using Int64 = int64_t;
using UIntPtr = uintptr_t;
using String = std::string;

template <typename T> using Safe = std::unique_ptr<T>;
template <typename T> using Vector = std::vector<T>;
template <typename F, typename S> using Pair = std::pair<F, S>;

} // namespace stone
#endif
