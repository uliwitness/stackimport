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
        uint8_t get_byte(size_t offset) const { return mPixels.at(offset); }
        void fill_rect(size_t top, size_t left, size_t bottom, size_t right);

        // Or the pixels from picture into this object's pixels:
        void add_picture_or(const Picture &picture);

        void write_to_pbm_with_mask(const Picture& mask, const string& filePath);

    protected:
        size_t mWidth;
        size_t mHeight;
        size_t mRowBytes;
        vector<uint8_t> mPixels;
    };

} // stackimport
