//
// Created by ekocbiyik on 3/6/20.
//

#ifndef DSV_STORAGE_H
#define DSV_STORAGE_H


#include <string>
#include <vector>
#include <arpa/inet.h>
#include <ipfixcol2.h>
#include "Config.hpp"

/** Base class                                                                                   */
class Output {
protected:
    /** Identification name of the output                                                        */
    std::string _name;
    /** Instance context (only for messages)                                                     */
    ipx_ctx_t *_ctx;
public:
    /**
     * \brief Base class constructor
     * \param[in] id  Identification of the output
     * \param[in] ctx Instance context
     */
    Output(const std::string &id, ipx_ctx_t *ctx) : _name(id), _ctx(ctx) {};

    /** \brief Base class destructor                                                             */
    virtual
    ~Output() {};

    /**
     * \brief Process a converted DSV
     * \param[in] str DSV Record
     * \param[in] len Length of the record (excluding the terminating null byte '\0')
     * \return #IPX_OK on success
     * \return #IPX_ERR_DENIED in case of a fatal error (the output cannot continue)
     */
    virtual int process(const char *str, size_t len) = 0;

    /**
     * \brief Flush buffered records
     */
    virtual void flush() {};
};

/** DSV converter and output manager                                                            */
class Storage {
private:
    /** Plugin context (only for log!)                                                           */
    const ipx_ctx_t *m_ctx;
    /** Registered outputs                                                                       */
    std::vector<Output *> m_outputs;
    /** Formatting options                                                                       */
    struct cfg_format m_format;
    /** Conversion flags for libfds converter                                                    */
    uint32_t m_flags;
    /** IPv4/IPv6 exporter address of the current message (can be nullptr)                       */
    const char *m_src_addr = nullptr;

    struct {
        char *buffer;
        size_t size_alloc;
        size_t size_used;
    } m_record; /**< Converted DSV record                                                       */

    // Convert an IPFIX record to a DSV string
    void convert(struct fds_drec &rec, const fds_iemgr_t *iemgr, struct fds_ipfix_msg_hdr *hdr, bool reverse = false);

    // Remaining buffer size
    size_t buffer_remain() const { return m_record.size_alloc - m_record.size_used; };

    // Total size of allocated buffer
    size_t buffer_alloc() const { return m_record.size_alloc; };

    // Used buffer size
    size_t buffer_used() const { return m_record.size_used; };

    // Append append a string
    void buffer_append(const char *str);

    // Reserve memory for a DSV string
    void buffer_reserve(size_t n);

    // Convert set to DSV string
    int convert_tset(struct ipx_ipfix_set *set, const struct fds_ipfix_msg_hdr *hdr);

    // Convert template record to a DSV string
    void convert_tmplt_rec(struct fds_tset_iter *tset_iter, uint16_t set_id, const struct fds_ipfix_msg_hdr *hdr);

    // Add detailed info (templateId, ODID, seqNum, exportTime) to DSV string
    void addDetailedInfo(const struct fds_ipfix_msg_hdr *hdr);

    // Get src_addr from IPFIX session
    static const char *session_src_addr(const struct ipx_session *ipx_desc, char *src_addr, socklen_t size);

public:
    /**
     * \brief Constructor
     * \param[in] ctx Plugin context (only for log!)
     * \param[in] fmt Conversion specifier
     */
    explicit Storage(const ipx_ctx_t *ctx, const struct cfg_format &fmt);

    /** Destructor */
    ~Storage();

    /**
     * \brief Add a new output instance
     *
     * Every time a new record is converted, the output instance will receive a reference
     * to the record and store it.
     * \note The storage will destroy the output instance when during destruction of this storage
     * \param[in] output Instance to add
     */
    void output_add(Output *output);

    /**
     * \brief Process IPFIX Message records
     *
     * For each record perform conversion to DSV and pass it to all output instances.
     * \param[in] msg   IPFIX Message to convert
     * \param[in] iemgr Information Element manager (can be NULL)
     * \return #IPX_OK on success
     * \return #IPX_ERR_DENIED if a fatal error has occurred and the storage cannot continue to
     *   work properly!
     */
    int records_store(ipx_msg_ipfix_t *msg, const fds_iemgr_t *iemgr);
};


#endif //DSV_STORAGE_H
