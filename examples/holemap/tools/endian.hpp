#include <istream>
#include <iostream>
#include <vector>
#include <algorithm>

// 引用于colmap
// Reverse the order of each byte.
template <typename T>
T ReverseBytes(const T& data);

// Check the order in which bytes are stored in computer memory.
bool IsLittleEndian();

// Convert data between endianness and the native format. Note that, for float
// and double types, these functions are only valid if the format is IEEE-754.
// This is the case for pretty much most processors.
template <typename T>
T LittleEndianToNative(const T x);

// Read data in little endian format for cross-platform support.
template <typename T>
T ReadBinaryLittleEndian(std::istream* stream);
template <typename T>
void ReadBinaryLittleEndian(std::istream* stream, std::vector<T>* data);

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

template <typename T>
T ReverseBytes(const T& data) {
    T data_reversed = data;
    std::reverse(reinterpret_cast<char*>(&data_reversed),
                reinterpret_cast<char*>(&data_reversed) + sizeof(T));
    return data_reversed;
}

inline bool IsLittleEndian() {
#ifdef BOOST_BIG_ENDIAN
    return false;
#else
    return true;
#endif
}

template <typename T>
T LittleEndianToNative(const T x) {
    if (IsLittleEndian()) {
        return x;
    } else {
        return ReverseBytes(x);
    }
}

template <typename T>
T ReadBinaryLittleEndian(std::istream* stream) {
    T data_little_endian;
    stream->read(reinterpret_cast<char*>(&data_little_endian), sizeof(T));
    return LittleEndianToNative(data_little_endian);
}

template <typename T>
void ReadBinaryLittleEndian(std::istream* stream, std::vector<T>* data) {
    for (size_t i = 0; i < data->size(); ++i) {
        (*data)[i] = ReadBinaryLittleEndian<T>(stream);
    }
}