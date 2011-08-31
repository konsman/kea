// Copyright (C) 2011  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#ifndef OPTION_H_
#define OPTION_H_

#include <string>
#include <map>
#include <boost/shared_array.hpp>

namespace isc {
namespace dhcp {

class Option {
public:
    enum Universe { V4, V6 };
    typedef std::map<unsigned int, boost::shared_ptr<Option> > Option4Lst;
    typedef std::multimap<unsigned int, boost::shared_ptr<Option> > Option6Lst;
    typedef boost::shared_ptr<Option> Factory(Option::Universe u,
                                              unsigned short type,
                                              boost::shared_array<char> buf,
                                              unsigned int offset,
                                              unsigned int len);

    // ctor, used for options constructed, usually during transmission
    Option(Universe u, unsigned short type);

    // ctor, used for received options
    // boost::shared_array allows sharing a buffer, but it requires that
    // different instances share pointer to the whole array, not point
    // to different elements in shared array. Therefore we need to share
    // pointer to the whole array and remember offset where data for
    // this option begins
    Option(Universe u, unsigned short type, boost::shared_array<char> buf,
           unsigned int offset,
           unsigned int len);

    // writes option in wire-format to buf, returns pointer to first unused
    // byte after stored option
    virtual unsigned int
    pack(boost::shared_array<char> buf,
         unsigned int buf_len,
         unsigned int offset);

    // parses received buffer, returns pointer to first unused byte
    // after parsed option
    // TODO: Do we need this overload? Commented out for now
    // virtual const char* unpack(const char* buf, unsigned int len);

    // parses received buffer, returns offset to the first unused byte after
    // parsed option

    ///
    /// Parses buffer and creates collection of Option objects.
    ///
    /// @param buf pointer to buffer
    /// @param buf_len length of buf
    /// @param offset offset, where start parsing option
    /// @param parse_len how many bytes should be parsed
    ///
    /// @return offset after last parsed option
    ///
    virtual unsigned int
    unpack(boost::shared_array<char> buf,
           unsigned int buf_len,
           unsigned int offset,
           unsigned int parse_len);

    ///
    /// Returns string representation of the option.
    ///
    /// @return string with text representation.
    ///
    virtual std::string
    toText();

    ///
    /// Returns option type (0-255 for DHCPv4, 0-65535 for DHCPv6)
    ///
    /// @return option type
    ///
    unsigned short
    getType();

    /// Returns length of the complete option (data length + DHCPv4/DHCPv6
    /// option header)
    ///
    /// @return length of the option
    ///
    virtual unsigned short
    len();

    // returns if option is valid (e.g. option may be truncated)
    virtual bool
    valid();

    /// Adds a sub-option.
    ///
    /// @param opt shared pointer to a suboption that is going to be added.
    ///
    void
    addOption(boost::shared_ptr<Option> opt);

    // just to force that every option has virtual dtor
    virtual
    ~Option();

protected:

    ///
    /// Builds raw (over-wire) buffer of this option, including all
    /// defined suboptions. Version for building DHCPv4 options.
    ///
    /// @param buf output buffer (built options will be stored here)
    /// @param buf_len buffer length (used for buffer overflow checks)
    /// @param offset offset from start of the buf buffer
    ///
    /// @return offset to the next byte after last used byte
    ///
    virtual unsigned int
    pack4(boost::shared_array<char> buf,
          unsigned int buf_len,
          unsigned int offset);

    ///
    /// Builds raw (over-wire) buffer of this option, including all
    /// defined suboptions. Version for building DHCPv4 options.
    ///
    /// @param buf output buffer (built options will be stored here)
    /// @param buf_len buffer length (used for buffer overflow checks)
    /// @param offset offset from start of the buf buffer
    ///
    /// @return offset to the next byte after last used byte
    ///
    virtual unsigned int
    pack6(boost::shared_array<char> buf,
          unsigned int buf_len,
          unsigned int offset);


    ///
    /// Parses provided buffer and creates DHCPv4 options.
    ///
    /// @param buf buffer that contains raw buffer to parse (on-wire format)
    /// @param buf_len buffer length (used for buffer overflow checks)
    /// @param offset offset from start of the buf buffer
    ///
    /// @return offset to the next byte after last parsed byte
    ///
    virtual unsigned int
    unpack4(boost::shared_array<char> buf,
            unsigned int buf_len,
            unsigned int offset,
            unsigned int parse_len);

    ///
    /// Parses provided buffer and creates DHCPv6 options.
    ///
    /// @param buf buffer that contains raw buffer to parse (on-wire format)
    /// @param buf_len buffer length (used for buffer overflow checks)
    /// @param offset offset from start of the buf buffer
    ///
    /// @return offset to the next byte after last parsed byte
    ///
    virtual unsigned int
    unpack6(boost::shared_array<char> buf,
            unsigned int buf_len,
            unsigned int offset,
            unsigned int parse_len);

    Universe universe_; // option universe (V4 or V6)
    unsigned short type_; // option type (0-255 for DHCPv4, 0-65535 for DHCPv6)

    boost::shared_array<char> data_;
    unsigned int data_len_; // length of data only. Use len() if you want to
                            // know proper length with option header overhead
    unsigned int offset_; // data is a shared_pointer that points out to the
                          // whole packet. offset_ specifies where data for
                          // this option begins.

    // TODO: probably 2 different containers have to be used for v4 (unique
    // options) and v6 (options with the same type can repeat)
    Option6Lst optionLst_;
};

} // namespace isc::dhcp
} // namespace isc

#endif
