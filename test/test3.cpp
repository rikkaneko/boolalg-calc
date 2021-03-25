/* This file is part of boolalg_calc.
 * Copyright (c) 2021 rikkaneko. */
#include <fmt/core.h>
#include "kmap.h"

int main(int argc, char *argv[]) {
    nnplib::kmap kmap;
    kmap.from_minterm({ 0, 2, 4 }, {}, 3);
    auto terms = kmap.optimize();
    for (auto &t: terms) fmt::print("{}\n", t);
    return 0;
}
