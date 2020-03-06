//
// Created by ekocbiyik on 3/6/20.
//

#ifndef DSV_PRINTER_H
#define DSV_PRINTER_H

#include "Config.hpp"
#include "Storage.hpp"

/** Printer to standard output */
class Printer : public Output {
public:
    /**
     * \brief Constructor
     * \param[in] cfg Output configuration
     * \param[in] ctx Instance context
     */
    explicit Printer(const struct cfg_print &cfg, ipx_ctx_t *ctx);

    /**
     * \brief Print a record on standard output
     * \param[in] str DSV string to print
     * \param[in] len Length of the string
     * \return #IPX_OK on success
     * \return #IPX_ERR_DENIED in case of fatal failure
     */
    int process(const char *str, size_t len);
};

#endif //DSV_PRINTER_H
