#include "Picture.hpp"
#include <fstream>

namespace stackimport {

    using namespace std;

    void Picture::set_size(size_t width, size_t height) {
        mWidth = width;
        mHeight = height;
        size_t partialPixels = (width % 8);
        mRowBytes = (width / 8) + ((partialPixels != 0) ? 1 : 0);
        if (width != 0 && height != 0) {
            mPixels.resize(mRowBytes * mHeight, 0);
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

    void Picture::add_picture_or(const Picture &picture) {
        assert(picture.mWidth == mWidth && picture.mHeight == mHeight && picture.mRowBytes == mRowBytes);

        for (size_t x = 0; x < mPixels.size(); ++x) {
            mPixels.at(x) |= picture.mPixels.at(x);
        }
    }

    void Picture::write_to_pbm_with_mask(const Picture& mask, const string& filePath) {
        fstream fp(filePath, ios::out|ios::binary|ios::trunc);
        char str[256] = {};

        // Write PBM Header:
        snprintf(str, sizeof(str) - 1, "P4\n%zu %zu\n", mWidth, mHeight);
        fp.write(str, strlen(str));

        fp.write((char*) mPixels.data(), mPixels.size());

        // Write second PBM Header:
        snprintf(str, sizeof(str) - 1, "\nP4\n%zu %zu\n", mWidth, mHeight);
        fp.write(str, strlen(str));

        fp.write((char*) mask.mPixels.data(), mask.mPixels.size());
        fp.close();
    }

} // stackimport