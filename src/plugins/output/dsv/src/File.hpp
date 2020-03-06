//
// Created by ekocbiyik on 3/6/20.
//

#ifndef DSV_FILE_H
#define DSV_FILE_H


#include <atomic>
#include <string>
#include <ctime>

#include <pthread.h>
#include "Storage.hpp"
#include "Config.hpp"

/**
 * \brief The class for file output interface
 */
class File : public Output {
public:
    // Constructor & destructor
    File(const struct cfg_file &cfg, ipx_ctx_t *ctx);

    ~File();

    // Store a record to the file
    int process(const char *str, size_t len);

    void flush();

private:
    /** Minimal window size */
    const unsigned int _WINDOW_MIN_SIZE = 60; // seconds

    /** Configuration of a thread */
    typedef struct thread_ctx_s {
        ipx_ctx_t *ctx;              /**< Plugin instance context    */
        pthread_t thread;            /**< Thread                     */
        pthread_rwlock_t rwlock;     /**< Data rwlock                */
        std::atomic<bool> stop;      /**< Stop flag for termination  */

        unsigned int window_size;    /**< Size of a time window      */
        time_t window_time;          /**< Current time window        */
        std::string storage_path;    /**< Storage path (template)    */
        std::string file_prefix;     /**< File prefix                */
        calg m_calg;                 /**< Compression                */

        void *file;                  /**< File descriptor            */
    } thread_ctx_t;

    /** Thread for changing time windows */
    thread_ctx_t *_thread;

    // Get a directory path for a time window
    static int dir_name(const time_t &tm, const std::string &tmplt, std::string &dir);

    // Create a directory for a time window
    static int dir_create(ipx_ctx_t *ctx, const std::string &path);

    // Create a file for a time window
    static void *file_create(ipx_ctx_t *ctx, const std::string &tmplt, const std::string &prefix, const time_t &tm, calg m_calg);

    // Window changer
    static void *thread_window(void *context);
};

#endif //DSV_FILE_H
