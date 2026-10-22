#pragma once
// The one body of 0x0045baa0 (PowerupTypeLookup), shared by both targets. See PowerupTypeSearch.cpp.
#include <cstdint>

const std::uint8_t* PowerupTypeSearch(std::uint32_t key, const std::uint8_t* base, int count, int stride);
