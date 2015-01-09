#ifndef __gif_frame_h__
#define __gif_frame_h__

#include "debug.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "gif_frame.hpp"

using namespace std;

struct gif_header
{
    char signature[4];
    char version[4];
    uint16_t width; // canvas width
    uint16_t height; // canvas height
    bool gct_flag;
    uint8_t colour_res;
    bool sort_flag;
    unsigned int gct_size;
    uint8_t bgcolour_index;
    uint8_t pixel_aspect_ratio;
    uint8_t *gct;
};

class GIF
{
    ifstream gif_file;

    vector<GIFFrame*> frames;
public:
    static const uint8_t EXTENSION_BLOCK = 0x21;
    static const uint8_t EXTENSION_BLOCK_GCE = 0xF9;
    static const uint8_t EXTENSION_BLOCK_COMMENT = 0xFE;
    static const uint8_t EXTENSION_BLOCK_PLAINTEXT = 0x01;
    static const uint8_t EXTENSION_BLOCK_APPLICATION = 0xFF;
    static const uint8_t IMAGE_DESCRIPTOR = 0x2C;
    static const uint8_t GIF_EOF = 0x3D;

    struct gif_header header;

    GIF();
    GIF(string filename);
    void open(string filename);
    void decode();

    virtual ~GIF();

protected:
    void decode_header();
    void skip_extension();
};


class GIFDecodeError : public runtime_error
{
public:
    GIFDecodeError(const std::string& msg) : runtime_error(msg) {}
};

#endif
