/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include <fmt/core.h>
#include "kmap.h"

int main(int argc, char *argv[]) {
    nnplib::kmap kmap;
    kmap.from_truthtable("100x1010x11x1x000010x0x0xx01xx10x0");
    auto terms = kmap.optimize();
    for (auto &t: terms) fmt::print("{}\n", t);
    return 0;
}
