//
// Created by ekocbiyik on 3/6/20.
//

#ifndef DSV_SENDER_H
#define DSV_SENDER_H


#include "Storage.hpp"

/** DSV sender (over TCP or UDP)                                                 */
class Sender : public Output {
public:
    // Constructor
    Sender(const struct cfg_send &cfg, ipx_ctx_t *ctx);

    // Destructor
    ~Sender();

    // Processing records
    int process(const char *str, size_t len);

private:
    /** Transmission status */
    enum Send_status {
        SEND_OK,               /**< Successfully sent                             */
        SEND_WOULDBLOCK,       /**< Message skipped or partly sent                */
        SEND_FAILED            /**< Failed                                        */
    };

    /** Rest of a message to send in non-blocking mode                            */
    std::string msg_rest;
    /** File descriptor of the connection                                         */
    int sd;
    /** Configuration parameters of the output                                    */
    struct cfg_send params;
    /** Time of the last connection attempt                                       */
    struct timespec connection_time;

    int connect();

    enum Send_status send(const char *str, size_t len);
};

#endif //DSV_SENDER_H
