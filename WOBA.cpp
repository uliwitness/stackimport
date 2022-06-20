#include "WOBA.hpp"
#include <iostream>
#include <arpa/inet.h>

namespace stackimport {

    using namespace std;

    void WOBA::PrintBitstring(const uint8_t* buf, size_t currOffset, size_t endOffset) {
        for (size_t x = currOffset; currOffset < endOffset; ++x) {
            if (x != currOffset) {
                cout << " ";
            }
            PrintByte(buf[x]);
        }
    }

    void WOBA::PrintByte(uint8_t byte) {
        for (size_t y = 0; y < 8; ++y) {
            cout << (byte & (1 << y)) ? " " : "*";
        }
    }

    void WOBA::ApplyNextOpcode(size_t &currOffset, size_t endByte) {
        uint8_t type = *(uint8_t*)(mWOBAData.data() + currOffset);
        ++currOffset;
        if (type <= 0x7f) {
            // Zeroes followed by data:
            size_t numZeroes = (type & 0xf0) >> 4;
            size_t numData = type & 0x0f;
            cout << "{" << numZeroes << "," << numData << "}";
            for (size_t x = 0; x < numZeroes; ++x) {
                cout << " ";
                PrintByte(0);
                mPicture.set_byte(mCurrPictureByte++, 0);
            }
            for (size_t x = 0; x < numData; ++x) {
                cout << " ";
                PrintByte(mWOBAData[currOffset]);
                mPicture.set_byte(mCurrPictureByte++, mWOBAData[currOffset++]);
            }
            cout << endl;
        } else if ((type & 0b11100000) == 0b10100000) {
            // Repeat the following command several times:
            size_t numRepetitions = type & 0b00011111;
            size_t wobaStartOffs = currOffset;
            for (size_t x = 0; x < numRepetitions; ++x) {
                currOffset = wobaStartOffs;
                ApplyNextOpcode(currOffset, endByte);
            }
        } else if ((type & 0b11100000) == 0b11000000) {
            // data in blocks of 8:
            size_t numData = (type & 0b00011111) * 8;
            cout << "{-," << numData << "}";
            for (size_t x = 0; x < numData; ++x) {
                cout << " ";
                PrintByte(mWOBAData[currOffset]);
                mPicture.set_byte(mCurrPictureByte++, mWOBAData[currOffset++]);
            }
            cout << endl;
        } else if ((type & 0b11100000) == 0b11100000) {
            // zeroes in blocks of 16:
            size_t numZeroes = (type & 0b00011111) * 16;
            cout << "{" << numZeroes << ",-}";
            for (size_t x = 0; x < numZeroes; ++x) {
                cout << " ";
                PrintByte(0);
                mPicture.set_byte(mCurrPictureByte++, 0);
            }
            cout << endl;
        } else {
            switch (type) {
                case 0x80:
                    // One row of uncompressed data:
                    cout << "{DATAROW}";
                    for (size_t x = 0; x < mPicture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(mWOBAData[currOffset]);
                        mPicture.set_byte(mCurrPictureByte++, mWOBAData[currOffset++]);
                    }
                    cout << endl;
                    break;
                case 0x81:
                    // One white row:
                    cout << "{WHITEROW}";
                    for (size_t x = 0; x < mPicture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(0);
                        mPicture.set_byte(mCurrPictureByte++, 0);
                    }
                    cout << endl;
                    break;
                case 0x82:
                    // One black row:
                    cout << "{BLACKROW}";
                    for (size_t x = 0; x < mPicture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(0xff);
                        mPicture.set_byte(mCurrPictureByte++, 0xff);
                    }
                    cout << endl;
                    break;

                case 0x83: {
                    // Set repeat buffer byte:
                    size_t saveIndex = (mCurrPictureByte / mPicture.row_bytes()) % sizeof(mRepeatBuffer);
                    cout << "{SETREP," << saveIndex << "} ";
                    PrintByte(mWOBAData[currOffset]);
                    cout << endl;
                    mRepeatBuffer[saveIndex] = mWOBAData[currOffset++];
                    break;
                }

                case 0x84: {
                    // Fill row with repeat buffer byte:
                    // TODO: Stop at end of row instead of filling one row?
                    size_t saveIndex = (mCurrPictureByte / mPicture.row_bytes()) % sizeof(mRepeatBuffer);
                    cout << "{REPROW," << saveIndex << "}";
                    for (size_t x = 0; x < mPicture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(mRepeatBuffer[saveIndex]);
                        mPicture.set_byte(mCurrPictureByte++, mRepeatBuffer[saveIndex]);
                    }
                    cout << endl;
                    break;
                }
            }
        }

    }

    void WOBA::Decode() {
        size_t currOffset = 4 + 2 + 2 + 2 + 2;
        int16_t top = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t left = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t bottom = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t right = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;

        int16_t maskTop = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t maskLeft = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t maskBottom = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t maskRight = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;

        int16_t imageTop = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t imageLeft = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t imageBottom = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;
        int16_t imageRight = ntohs(*(int16_t*)(mWOBAData.data() + currOffset));
        currOffset += 2;

        currOffset += 4 + 4;

        uint32_t maskDataSize = ntohl(*(uint32_t*)(mWOBAData.data() + currOffset));
        currOffset += 4;
        uint32_t imageDataSize = ntohl(*(uint32_t*)(mWOBAData.data() + currOffset));
        currOffset += 4;

        cout << "Card rect: " << left << "," << top << "," << right << "," << bottom << "\n";
        cout << "Mask rect: " << maskLeft << "," << maskTop << "," << maskRight << "," << maskBottom << "\n";
        cout << "Image rect: " << imageLeft << "," << imageTop << "," << imageRight << "," << imageBottom << endl;

        cout << "Mask: " << maskDataSize << " bytes | Image: " << imageDataSize << " bytes" << endl;

        mPicture.set_size(right, bottom);
        mCurrPictureByte = 0;

        // Mask data
        if (maskDataSize == 0 && (maskLeft != 0 || maskTop != 0 || maskRight != 0 || maskBottom != 0)) {
            mPicture.fill_mask_rect(maskTop, maskLeft, maskBottom, maskRight);
        } else if (maskDataSize > 0) {
            size_t maskEnd = currOffset + maskDataSize;
            while (currOffset < maskEnd) {
                ApplyNextOpcode(currOffset, maskEnd);
            }
        }

        // Image data
        if (imageDataSize == 0 && (imageLeft != 0 || imageTop != 0 || imageRight != 0 || imageBottom != 0)) {
            mPicture.fill_rect(imageTop, imageLeft, imageBottom, imageRight);
        } else if (imageDataSize > 0) {
            size_t imageEnd = currOffset + imageDataSize;
            while (currOffset < imageEnd) {
                ApplyNextOpcode(mWOBAData, currOffset, imageEnd, mPicture);
            }
        }
    }

}