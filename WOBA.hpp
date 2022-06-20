#pragma once

#include "Picture.hpp"
#include <vector>

namespace stackimport {

    class WOBA {
    public:
        WOBA(const vector<uint8_t> &wobaData, Picture &picture) : mWOBAData(wobaData), mPicture(picture) {}

        void Decode();

    protected:
        void ApplyNextOpcode(size_t &currOffset, size_t endByte);
        void PrintBitstring(const uint8_t* buf, size_t currOffset, size_t endOffset);
        void PrintByte(uint8_t byte);

        const vector<uint8_t> &mWOBAData;
        Picture &mPicture;
        size_t mCurrPictureByte{0};
        // Pre-fill repeat buffer with the two alternating bytes that make up a checkerboard pattern:
        uint8_t mRepeatBuffer[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
    };

}
