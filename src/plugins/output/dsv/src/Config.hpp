//
// Created by ekocbiyik on 3/6/20.
//

#ifndef DSV_CONFIG_H
#define DSV_CONFIG_H

#include <string>
#include <vector>
#include <ipfixcol2.h>

/** Configuration of output format                                                               */
struct cfg_format {
    /** TCP flags format - true (formatted), false (raw)                                         */
    bool tcp_flags;
    /** Timestamp format - true (formatted), false (UNIX)                                        */
    bool timestamp;
    /** Protocol format  - true (formatted), false (raw)                                         */
    bool proto;
    /** Skip unknown elements                                                                    */
    bool ignore_unknown;
    /** Converter octetArray type as unsigned integer (only if field size <= 8)                  */
    bool octets_as_uint;
    /** Convert white spaces in string (do not skip)                                             */
    bool white_spaces;
    /** Add detailed information about each record                                               */
    bool detailed_info;
    /** Ignore Options Template records                                                          */
    bool ignore_options;
    /** Use only numeric identifiers of Information Elements                                     */
    bool numeric_names;
    /** Split biflow records                                                                     */
    bool split_biflow;
    /** Add template records                                                                     */
    bool template_info;
};

/** Output configuration base structure                                                          */
struct cfg_output {
    /** Plugin identification                                                                    */
    std::string name;
};

/** Configuration of printer to standard output                                                  */
struct cfg_print : cfg_output {
    // Nothing more
};

/** Configuration of sender                                                                      */
struct cfg_send : cfg_output {
    /** Remote IPv4/IPv6 address                                                                 */
    std::string addr;
    /** Destination port                                                                         */
    uint16_t port;
    /** Blocking communication                                                                   */
    bool blocking;
    /** Transport Protocol                                                                       */
    enum {
        SEND_PROTO_UDP, SEND_PROTO_TCP
    } proto; /**< Communication protocol                                                         */
};

/** Configuration of TCP server                                                                  */
struct cfg_server : cfg_output {
    /** Destination port                                                                         */
    uint16_t port;
    /** Blocking communication                                                                   */
    bool blocking;
};

enum class calg {
    NONE, ///< Do not use compression
    GZIP  ///< GZIP compression
};

/** Configuration of file writer                                                                 */
struct cfg_file : cfg_output {
    /** Path pattern                                                                             */
    std::string path_pattern;
    /** File prefix                                                                              */
    std::string prefix;
    /** Window size (0 == disabled)                                                              */
    uint32_t window_size;
    /** Enable/disable window alignment                                                          */
    bool window_align;
    /** Compression algorithm                                                                    */
    calg m_calg;
};

/** Parsed configuration of an instance                                                          */
class Config {
private:
    bool check_ip(const std::string &ip_addr);

    bool check_or(const std::string &elem, const char *value, const std::string &val_true, const std::string &val_false);

    void check_validity();

    void default_set();

    void parse_print(fds_xml_ctx_t *print);

    void parse_server(fds_xml_ctx_t *server);

    void parse_send(fds_xml_ctx_t *send);

    void parse_file(fds_xml_ctx_t *file);

    void parse_outputs(fds_xml_ctx_t *outputs);

    void parse_params(fds_xml_ctx_t *params);

public:
    /** Transformation format                                                                    */
    struct cfg_format format;

    struct {
        /** Printers                                                                             */
        std::vector<struct cfg_print> prints;
        /** Senders                                                                              */
        std::vector<struct cfg_send> sends;
        /** File writers                                                                         */
        std::vector<struct cfg_file> files;
        /** Servers                                                                              */
        std::vector<struct cfg_server> servers;
    } outputs; /**< Outputs                                                                      */

    /**
     * \brief Create a new configuration
     * \param[in] params XML configuration of JSON plugin
     * \throw runtime_error in case of invalid configuration
     */
    Config(const char *params);

    /**
     * \brief Configuration destructor
     */
    ~Config();
};

#endif //DSV_CONFIG_H
