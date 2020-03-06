//
// Created by ekocbiyik on 3/6/20.
//

#include <libfds.h>
#include <ipfixcol2.h>
#include <memory>

#include "Config.hpp"
#include "Storage.hpp"
#include "Printer.hpp"
#include "File.hpp"
#include "Server.hpp"
#include "Sender.hpp"

/** Plugin description */
IPX_API struct ipx_plugin_info ipx_plugin_info = {
        // Plugin identification name
        "dsv",
        // Brief description of plugin
        "Conversion of IPFIX data into DSV format",
        // Plugin type
        IPX_PT_OUTPUT,
        // Configuration flags (reserved for future use)
        0,
        // Plugin version string (like "1.2.3")
        "2.1.0",
        // Minimal IPFIXcol version string (like "1.2.3")
        "2.1.0"};

/** DSV instance data                                                                           */
struct Instance {
    /** Parser configuration                                                                     */
    Config *config;
    /** Storage (output manager)                                                                 */
    Storage *storage;
};

/**
 * \brief Initialize outputs
 *
 * For each definition of an output in a plugin configuration, call its constructor and add it to
 * an output manager.
 * \param[in] ctx     Instance context
 * \param[in] storage Manager of output
 * \param[in] cfg     Parsed configuration of the instance
 */
static void outputs_initialize(ipx_ctx_t *ctx, Storage *storage, Config *cfg) {
    for (const auto &print : cfg->outputs.prints) {
        storage->output_add(new Printer(print, ctx));
    }

    for (const auto &file : cfg->outputs.files) {
        storage->output_add(new File(file, ctx));
    }

    for (const auto &server : cfg->outputs.servers) {
        storage->output_add(new Server(server, ctx));
    }

    for (const auto &send : cfg->outputs.sends) {
        storage->output_add(new Sender(send, ctx));
    }
}

int ipx_plugin_init(ipx_ctx_t *ctx, const char *params) {
    struct Instance *data = nullptr;
    try {
        // Create and parse the configuration
        std::unique_ptr<Instance> ptr(new Instance);
        std::unique_ptr<Config> cfg(new Config(params));
        std::unique_ptr<Storage> storage(new Storage(ctx, cfg.get()->format));

        // Initialize outputs
        outputs_initialize(ctx, storage.get(), cfg.get());

        // Success
        data = ptr.release();
        data->config = cfg.release();
        data->storage = storage.release();

    } catch (std::exception &ex) {
        IPX_CTX_ERROR(ctx, "%s", ex.what());
        return IPX_ERR_DENIED;
    } catch (...) {
        IPX_CTX_ERROR(ctx, "Unexpected exception has occurred!", '\0');
        return IPX_ERR_DENIED;
    }

    ipx_ctx_private_set(ctx, data);
    return IPX_OK;
}

void ipx_plugin_destroy(ipx_ctx_t *ctx, void *cfg) {
    (void) ctx;

    struct Instance *data = reinterpret_cast<struct Instance *>(cfg);
    delete data->storage;
    delete data->config;
    delete data;
}

int ipx_plugin_process(ipx_ctx_t *ctx, void *cfg, ipx_msg_t *msg) {
    (void) ctx;

    int ret_code = IPX_OK;
    const fds_iemgr_t *iemgr = ipx_ctx_iemgr_get(ctx);

    try {
        struct Instance *data = reinterpret_cast<struct Instance *>(cfg);
        ret_code = data->storage->records_store(ipx_msg_base2ipfix(msg), iemgr);
    } catch (std::exception &ex) {
        IPX_CTX_ERROR(ctx, "%s", ex.what());
        ret_code = FDS_ERR_DENIED;
    } catch (...) {
        IPX_CTX_ERROR(ctx, "Unexpected exception has occurred!", '\0');
        ret_code = FDS_ERR_DENIED;
    }

    return ret_code;
}