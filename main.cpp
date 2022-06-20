#include <iostream>
#include <vector>
#include "Picture.hpp"
#include "WOBA.hpp"

using namespace std;
using namespace stackimport;

int main(int argc, const char* argv[]) {
    vector<uint8_t> wobaData;
    Picture picture(512, 342);

    FILE * wobaDataFile = fopen(argv[1], "r");
    if (!wobaDataFile) {
        return 77;
    }
    fseek(wobaDataFile, 0, SEEK_END);
    size_t fileSize = ftell(wobaDataFile);
    wobaData.resize(fileSize);
    fseek(wobaDataFile, 0, SEEK_SET);
    if (fread(wobaData.data(), 1, fileSize, wobaDataFile) != fileSize) {
        return 88;
    }
    fclose(wobaDataFile);

    WOBA decoder(wobaData, picture);
    decoder.Decode();

    return 0;
}
