//
//  gif.h
//  GIF-T
//
//  Created by Alexander Hill on 12/28/14.
//  Copyright (c) 2014 Alexander Hill. All rights reserved.
//

#ifndef __GIF_T__gif__
#define __GIF_T__gif__

#include <stdio.h>
#include <iostream>
#include <fstream>

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

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
    int gct_size;
    uint8_t bgcolour_index;
    uint8_t pixel_aspect_ratio;
    uint8_t *gct;
};

struct gif_gce
{
    uint8_t block_size;
    uint16_t disposal_method;
    bool user_input_flag;
    bool transparent_colour_flag;
    uint16_t delay_time;
    uint8_t transparent_colour_index;
};

class GIF
{
    static const int EXTENSION_BLOCK = 0x21;
    static const int EXTENSION_BLOCK_GCE = 0xF9;
    static const int EXTENSION_BLOCK_PLAINTEXT = 0x01;
    static const int EXTENSION_BLOCK_COMMENT = 0xFE;
    static const int EXTENSION_BLOCK_APPLICATION = 0xFF;
    static const int IMAGE_DESCRIPTOR = 0x2C;
    static const int GIF_EOF = 0x3D;

    ifstream gif_file;
    struct gif_header header;
    struct gif_gce gce;
public:
    GIF();
    GIF(const string filename);
    void open(const string filename);
    void parse();
    
    virtual ~GIF();
    
protected:
    void parse_header();
    void parse_gce();
    void parse_subblock();
    void skip_extension();
};


class GIFParseError : public runtime_error
{
public:
    GIFParseError(const std::string& msg) : runtime_error(msg) {}
};

#endif /* defined(__GIF_T__gif__) */
