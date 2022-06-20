#pragma once

#include <vector>

namespace stackimport {

    using namespace std;

    class Picture {
    public:
        Picture(size_t width, size_t height) : mWidth(width), mHeight(height) {
            assert((width % 8) == 0);
            set_size(mWidth, mHeight);
        }

        void set_size(size_t width, size_t height);
        size_t width() const { return mWidth; }
        size_t height() const { return mHeight; }
        size_t row_bytes() const { return mRowBytes; }
        void set_byte(size_t offset, uint8_t val) { mPixels.at(offset) = val; }
        void set_mask_byte(size_t offset, uint8_t val) { mMask.at(offset) = val; }

        void fill_rect(size_t top, size_t left, size_t bottom, size_t right);
        void fill_mask_rect(size_t top, size_t left, size_t bottom, size_t right);

    protected:
        size_t mWidth;
        size_t mHeight;
        size_t mRowBytes;
        vector<uint8_t> mPixels;
        vector<uint8_t> mMask;
    };

} // stackimport
