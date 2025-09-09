#pragma once

template <typename T>
T map(T x, T in_min, T in_max, T out_min, T out_max) {
    if (in_max == in_min) return 0;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
