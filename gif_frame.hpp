#ifndef __gif_h__
#define __gif_h__

#include "debug.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

struct rgb {
    uint8_t r,g,b;
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

struct gif_descriptor
{
    uint16_t left;
    uint16_t top;
    uint16_t width;
    uint16_t height;
    bool lct_flag;
    bool interlace_flag;
    bool sort_flag;
    unsigned int lct_size;
};

class GIFFrame
{
    struct gif_gce gce;

    uint8_t *lct;
    vector<uint8_t> index_stream;

    ifstream &gif_file;
public:
    struct gif_descriptor dsc;
    struct rgb** pixels;

    GIFFrame(ifstream &gfile) : gif_file(gfile) {};
    void decode(uint8_t *gct = nullptr, uint8_t gct_size = 0);
    void decode_gce();
    void decode_descriptor();
    void decode_lct();
    void decode_data(uint8_t gct_size);

    void build_frame(uint8_t *gct, uint8_t gct_size);
};

#endif
