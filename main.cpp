#include <iostream>
#include <vector>
#include "Picture.hpp"
#include "WOBA.hpp"

using namespace std;
using namespace stackimport;

int main(int argc, const char* argv[]) {
    vector<uint8_t> wobaData;
    Picture picture(512, 342);
    Picture mask(512, 342);

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

    WOBA decoder(wobaData, picture, mask);
    try {
        decoder.Decode();
    } catch(exception& err) {
        cerr << "Error: " << err.what() << endl;
        cerr.flush();
    }

    string destPath(argv[1]);
    destPath.append(".pbm");
    picture.write_to_pbm_with_mask(mask, destPath);

    return 0;
}
