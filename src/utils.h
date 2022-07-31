#ifndef UTILS_H
#define UTILS_H

class Utils{
    public:
        bool log_messages_;
        char log_buffer[100];

        int searchIndex(byte look_array[], byte value, size_t size);
        int searchIndexPROGMEM(const byte look_array[], byte value);
};

#endif