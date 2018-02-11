#pragma once
#include <iostream>
#include <cstdint>

uint32_t dax_encode_op(uint32_t x, uint32_t mul_even, uint32_t add_even, uint32_t mul_odd, uint32_t add_odd);
int call_encryption(int a1, int a2);