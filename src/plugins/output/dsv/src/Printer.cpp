//
// Created by ekocbiyik on 3/6/20.
//

#include "Printer.hpp"
#include <iostream>

Printer::Printer(const struct cfg_print &cfg, ipx_ctx_t *ctx) : Output(cfg.name, ctx) {
    // Nothing to do
}

int Printer::process(const char *str, size_t len) {
    (void) len;
    std::cout << str;
    return IPX_OK;
}
