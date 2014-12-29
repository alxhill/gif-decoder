//
//  gif.h
//  GIF-T
//
//  Created by Alexander Hill on 12/28/14.
//  Copyright (c) 2014 Alexander Hill. All rights reserved.
//

#ifndef __GIF_T__gif__
#define __GIF_T__gif__

#include "debug.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "gif_frame.cpp"

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

    vector<GIFFrame> frames;
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
    void parse();

    virtual ~GIF();

protected:
    void parse_header();
    void skip_extension();
};


class GIFParseError : public runtime_error
{
public:
    GIFParseError(const std::string& msg) : runtime_error(msg) {}
};

#endif /* defined(__GIF_T__gif__) */
