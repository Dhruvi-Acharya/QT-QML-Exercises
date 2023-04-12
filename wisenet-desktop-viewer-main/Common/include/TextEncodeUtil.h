#pragma once


#include <cctype>
#include <limits>
#include <sstream>

namespace Wisenet
{
namespace Common
{

inline bool is_unreserved(int c)
{
    return (48 <= c && c <= 57) ||//0-9
            (65 <= c && c <= 90) ||//abc...xyz
            (97 <= c && c <= 122) || //ABC...XYZ
            (c == '-' || c == '_' || c == '.' || c == '~' || c == '/') ||
            (c == ':' || c == '?' || c == '[' || c == ']' || c == '@' ||
             c == '!' || c == '$' || c == '&' || c == '\\' || c == ';' || c == '=') ||
            (c==',');
}

// http://www.zedwood.com/article/cpp-urlencode-function
inline std::string UrlEncode(const std::string &s)
{
    static const char lookup[] = "0123456789abcdef";
    std::stringstream e;

    for (size_t i = 0, ix = s.length(); i<ix; i++)
    {
        const char& c = s[i];
        if (is_unreserved(c) )
        {
            e << c;
        }
        else
        {
            e << '%';
            e << lookup[(c & 0xF0) >> 4];
            e << lookup[(c & 0x0F)];
        }
    }
    return e.str();
}


// http://stackoverflow.com/questions/5288076/doing-base64-encoding-and-decoding-in-openssl-c
inline std::string Base64Encode(const std::string &bindata)
{
    static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    using ::std::string;
    using ::std::numeric_limits;

    if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
        throw ::std::length_error("Converting too large a string to base64.");
    }

    const ::std::size_t binlen = bindata.size();
    // Use = signs so the end is properly padded.
    string retval((((binlen + 2) / 3) * 4), '=');
    ::std::size_t outpos = 0;
    int bits_collected = 0;
    unsigned int accumulator = 0;
    const string::const_iterator binend = bindata.end();

    for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
        accumulator = (accumulator << 8) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }
    if (bits_collected > 0) { // Any trailing bits that are missing.
        //assert(bits_collected < 6);
        accumulator <<= 6 - bits_collected;
        retval[outpos++] = b64_table[accumulator & 0x3fu];
    }
    //assert(outpos >= (retval.size() - 2));
    //assert(outpos <= retval.size());
    return retval;
}


}
}

