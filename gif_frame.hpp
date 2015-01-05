#ifndef __GIF_T__gif_frame__
#define __GIF_T__gif_frame__

#include "debug.h"

#include <iostream>
#include <fstream>
#include <map>

using namespace std;

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
    struct gif_descriptor dsc;

    uint8_t *lct;

    ifstream &gif_file;
public:
    GIFFrame(ifstream &gfile) : gif_file(gfile) {};
    void parse(uint8_t *gct = nullptr, uint8_t gct_size = 0);
    void parse_gce();
    void parse_descriptor();
    void parse_lct();
    void parse_data(uint8_t *gct, uint8_t gct_size);
};

#endif /* defined(__GIF_T__gif_frame__) */
