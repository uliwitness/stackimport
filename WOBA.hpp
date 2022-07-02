#pragma once

#include "Picture.hpp"
#include <vector>

namespace stackimport {

    class WOBA {
    public:
        WOBA(const vector<uint8_t> &wobaData, Picture &picture, Picture &mask) : mWOBAData(wobaData), mPicture(picture), mMask(mask) {}

        void Decode();

    protected:
        void ApplyNextOpcode(size_t &currOffset, size_t endByte, Picture &picture);
        void PrintBitstring(const uint8_t* buf, size_t currOffset, size_t endOffset);
        void PrintByte(uint8_t byte);

        const vector<uint8_t> &mWOBAData;
        vector<uint8_t> mRowBuffer;
        Picture &mPicture;
        Picture &mMask;
        size_t mCurrPictureByte{0};
        size_t mDH{0};
        size_t mDV{0};
        // Pre-fill repeat buffer with the two alternating bytes that make up a checkerboard pattern:
        uint8_t mRepeatBuffer[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
    };

}
