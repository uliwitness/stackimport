#include "Picture.hpp"

namespace stackimport {

    using namespace std;

    void Picture::set_size(size_t width, size_t height) {
        assert((width % 8) == 0);
        mRowBytes = (width / 8);
        if (width != 0 && height != 0) {
            mPixels.resize(mRowBytes * mHeight, 0);
            mMask.resize(mRowBytes * mHeight, 0);
        }
    }

    void Picture::fill_rect(size_t top, size_t left, size_t bottom, size_t right) {
        size_t leftBitIndex = left % 8;
        size_t leftByte = left / 8;
        size_t rightBitIndex = right % 8;
        size_t rightByte = right / 8;
        size_t startByte = top * mRowBytes + leftByte;
        size_t endByte = bottom * mRowBytes + rightByte;
        size_t width = right - left;

        size_t currByte = startByte;
        size_t byteWidth = (width - leftBitIndex - rightBitIndex) / 8;
        while(currByte < endByte) {
            for(size_t bitIndex = leftBitIndex; bitIndex < 8; ++bitIndex) {
                mPixels.at(currByte) = mPixels.at(currByte) | (1 << bitIndex);
            }

            size_t endByteOnRow = currByte + byteWidth;

            for(; currByte < endByteOnRow; ++currByte) {
                mPixels.at(currByte) = 0xff;
            }

            for(size_t bitIndex = rightBitIndex; bitIndex < 8; ++bitIndex) {
                mPixels.at(currByte) = mPixels.at(currByte) | (1 << bitIndex);
            }
        }
    }

    void Picture::fill_mask_rect(size_t top, size_t left, size_t bottom, size_t right) {
        size_t leftBitIndex = left % 8;
        size_t leftByte = left / 8;
        size_t rightBitIndex = right % 8;
        size_t rightByte = right / 8;
        size_t startByte = top * mRowBytes + leftByte;
        size_t endByte = bottom * mRowBytes + rightByte;
        size_t width = right - left;

        size_t currByte = startByte;
        size_t byteWidth = (width - leftBitIndex - rightBitIndex) / 8;
        while(currByte < endByte) {
            for(size_t bitIndex = leftBitIndex; bitIndex < 8; ++bitIndex) {
                mMask.at(currByte) = mMask.at(currByte) | (1 << bitIndex);
            }

            size_t endByteOnRow = currByte + byteWidth;

            for(; currByte < endByteOnRow; ++currByte) {
                mMask.at(currByte) = 0xff;
            }

            for(size_t bitIndex = rightBitIndex; bitIndex < 8; ++bitIndex) {
                mMask.at(currByte) = mMask.at(currByte) | (1 << bitIndex);
            }
        }
    }

} // stackimport