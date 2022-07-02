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
        char str[9] = {};
        for (size_t y = 0; y < 8; ++y) {
            str[y] = (byte & (1 << y)) ? '*' : ' ';
        }

        cout << str;
    }

    * * * *  | ******** |  * *****
      * * *  | * ****** | ** * ***

    void bitshift_bytes(size_t amount, uint8_t * buf, size_t len) {
        size_t bytesToMove = amount / 8;
        size_t bitsToMove = amount % 8;
        uint8_t lastByte = 0;
        if (bitsToMove != 0) {
            ;
        }
        if (bytesToMove != 0) {
            memmove(buf + bytesToMove, buf, len - bytesToMove);
        }
    }

    void WOBA::ApplyNextOpcode(size_t &currOffset, size_t endByte, Picture &picture) {
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
                picture.set_byte(mCurrPictureByte++, 0);
            }
            for (size_t x = 0; x < numData; ++x) {
                cout << " ";
                PrintByte(mWOBAData[currOffset]);
                picture.set_byte(mCurrPictureByte++, mWOBAData[currOffset++]);
            }
        } else if ((type & 0b11100000) == 0b10100000) {
            // Repeat the following command several times:
            size_t numRepetitions = type & 0b00011111;
            size_t wobaStartOffs = currOffset;
            for (size_t x = 0; x < numRepetitions; ++x) {
                currOffset = wobaStartOffs;
                ApplyNextOpcode(currOffset, endByte, picture);
            }
        } else if ((type & 0b11100000) == 0b11000000) {
            // data in blocks of 8:
            size_t numData = (type & 0b00011111) * 8;
            cout << "{-," << numData << "}";
            for (size_t x = 0; x < numData; ++x) {
                cout << " ";
                PrintByte(mWOBAData[currOffset]);
                picture.set_byte(mCurrPictureByte++, mWOBAData[currOffset++]);
            }
        } else if ((type & 0b11100000) == 0b11100000) {
            // zeroes in blocks of 16:
            size_t numZeroes = (type & 0b00011111) * 16;
            cout << "{" << numZeroes << ",-}";
            for (size_t x = 0; x < numZeroes; ++x) {
                cout << " ";
                PrintByte(0);
                picture.set_byte(mCurrPictureByte++, 0);
            }
        } else {
            switch (type) {
                case 0x80:
                    // One row of uncompressed data:
                    cout << "{DATAROW}";
                    for (size_t x = (currOffset % picture.width()); x < picture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(mWOBAData[currOffset]);
                        picture.set_byte(mCurrPictureByte++, mWOBAData[currOffset++]);
                    }
                    break;
                case 0x81:
                    // One white row:
                    cout << "{WHITEROW}";
                    for (size_t x = (currOffset % picture.width()); x < picture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(0);
                        picture.set_byte(mCurrPictureByte++, 0);
                    }
                    break;
                case 0x82:
                    // One black row:
                    cout << "{BLACKROW}";
                    for (size_t x = (currOffset % picture.width()); x < picture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(0xff);
                        picture.set_byte(mCurrPictureByte++, 0xff);
                    }
                    break;

                case 0x83: {
                    // Set repeat buffer byte:
                    size_t saveIndex = (mCurrPictureByte / picture.row_bytes()) % sizeof(mRepeatBuffer);
                    cout << "{SETREP," << saveIndex << "} ";
                    PrintByte(mWOBAData[currOffset]);
                    mRepeatBuffer[saveIndex] = mWOBAData[currOffset++];
                    break;
                }

                case 0x84: {
                    // Fill row with repeat buffer byte:
                    // TODO: Stop at end of row instead of adding a full row?
                    size_t saveIndex = (mCurrPictureByte / picture.row_bytes()) % sizeof(mRepeatBuffer);
                    cout << "{REPROW," << saveIndex << "}";
                    for (size_t x = (currOffset % picture.width()); x < picture.row_bytes(); ++x) {
                        cout << " ";
                        PrintByte(mRepeatBuffer[saveIndex]);
                        picture.set_byte(mCurrPictureByte++, mRepeatBuffer[saveIndex]);
                    }
                    break;
                }

                case 0x85: {
                    cout << "{COPYROW}";
                    // Copy previous row:
                    size_t endByte = mCurrPictureByte;
                    size_t startByte = endByte - picture.row_bytes();
                    for (size_t x = startByte; x < endByte; ++x) {
                        cout << " ";
                        PrintByte(picture.get_byte(x));
                        picture.set_byte(mCurrPictureByte++, picture.get_byte(x));
                    }
                    break;
                }
                case 0x86: {
                    cout << "{COPYPREVROW}";
                    // Copy row before previous row:
                    size_t endByte = mCurrPictureByte - picture.row_bytes();
                    size_t startByte = endByte - picture.row_bytes();
                    for (size_t x = startByte; x < endByte; ++x) {
                        cout << " ";
                        PrintByte(picture.get_byte(x));
                        picture.set_byte(mCurrPictureByte++, picture.get_byte(x));
                    }
                    break;
                }

                case 0x87:
                    assert(false); // Unused opcode.
                    break;

                case 0x88:
                    cout << "{dh = 16}";
                    mDH = 16;
                    mDV = 0;
                    break;

                case 0x89:
                    cout << "{dh = 0}";
                    mDH = 0;
                    mDV = 0;
                    break;

                case 0x8A:
                    cout << "{dv = 1}";
                    mDH = 0;
                    mDV = 1;
                    break;

                case 0x8B:
                    cout << "{dv = 2}";
                    mDH = 0;
                    mDV = 2;
                    break;

                case 0x8C:
                    cout << "{dh = 1}";
                    mDH = 1;
                    mDV = 0;
                    break;

                case 0x8D:
                    cout << "{dh = 1, dv = 1}";
                    mDH = 1;
                    mDV = 1;
                    break;

                case 0x8E:
                    cout << "{dh = 2, dv = 2}";
                    mDH = 2;
                    mDV = 2;
                    break;

                case 0x8F:
                    cout << "{dh = 8}";
                    mDH = 8;
                    mDV = 0;
                    break;
            }
        }

        if ((currOffset % picture.row_bytes()) == 0) {
            cout << endl;
        }

        if (type < 0x80 || type > 0x87) {
            memcpy(mRowBuffer.data(), picture.data_at(currOffset - picture.row_bytes()), picture.row_bytes());
            if (mDH != 0) {

            }

            if (mDV != 0) {

            }
        }

        cout << endl;
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

        // Mask data
        mMask.set_size(right, bottom);
        mCurrPictureByte = 0;
        mDH = mDV = 0;

        if (maskDataSize == 0 && (maskLeft != 0 || maskTop != 0 || maskRight != 0 || maskBottom != 0)) {
            mMask.fill_rect(maskTop, maskLeft, maskBottom, maskRight);
        } else if (maskDataSize > 0) {
            size_t maskEnd = currOffset + maskDataSize;
            while (currOffset < maskEnd) {
                ApplyNextOpcode(currOffset, maskEnd, mMask);
            }
        }

        // Image data
        mCurrPictureByte = 0;
        mDH = mDV = 0;

        if (imageDataSize == 0 && (imageLeft != 0 || imageTop != 0 || imageRight != 0 || imageBottom != 0)) {
            mPicture.set_size(right - left, bottom - top);
            mPicture.fill_rect(imageTop, imageLeft, imageBottom, imageRight);
        } else if (imageDataSize > 0) {
            mPicture.set_size(imageRight - imageLeft, imageBottom - imageTop);
            size_t imageEnd = currOffset + imageDataSize;
            while (currOffset < imageEnd) {
                ApplyNextOpcode(currOffset, imageEnd, mPicture);
            }
        }
    }

}
