//
// Created by ekocbiyik on 3/6/20.
//

#ifndef DSV_SERVER_H
#define DSV_SERVER_H


#include <atomic>
#include <string>
#include <vector>
#include <pthread.h>
#include <sys/socket.h>

#include "Storage.hpp"

/**
 * \brief The class for server output interface
 */
class Server : public Output {
public:
    Server(const struct cfg_server &cfg, ipx_ctx_t *ctx);

    ~Server();

    // Send a record to connected clients
    int process(const char *str, size_t len);

private:
    /** Transmission status */
    enum Send_status {
        SEND_OK,               /**< Successfully sent                             */
        SEND_WOULDBLOCK,       /**< Message skipped or partly sent                */
        SEND_FAILED            /**< Failed                                        */
    };

    /** Structure for connected client */
    typedef struct client_s {
        struct sockaddr_storage info; /**< Info about client (IP, port)           */
        int socket;                   /**< Client's socket                        */
        std::string msg_rest;         /**< Rest of a message in non-blocking mode */
    } client_t;

    /** Configuration of acceptor thread */
    typedef struct acceptor_s {
        ipx_ctx_t *ctx;                     /**< Instance context (for log only ) */
        pthread_t thread;                    /**< Thread                           */
        pthread_mutex_t mutex;              /**< Mutex for the array              */
        std::atomic<bool> stop;             /**< Stop flag for terminating        */

        int socket_fd;                      /**< Server socket                    */
        std::atomic<bool> new_clients_ready; /**< New clients flag                 */
        std::vector<client_t> new_clients;  /**< Array of new clients             */
    } acceptor_t;

    /** Connected client */
    std::vector<client_t> _clients;
    /** Socket state */
    bool _non_blocking;
    /** Acceptor of new clients */
    acceptor_t *_acceptor;

    // Brief description of a client
    static std::string get_client_desc(const struct sockaddr_storage &client);

    // Send data to the client
    enum Send_status msg_send(const char *data, ssize_t len, client_t &client);

    // Acceptor's thread function
    static void *thread_accept(void *context);
};

#endif //DSV_SERVER_H
