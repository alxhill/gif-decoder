//
//  gif_frame.cpp
//  GIF-T
//
//  Created by Alexander Hill on 12/29/14.
//  Copyright (c) 2014 Alexander Hill. All rights reserved.
//

#include <cstdio>
#include <unordered_map>
#include "gif_frame.h"
#include "gif.h"

void GIFFrame::parse(uint8_t *gct, uint8_t gct_size)
{
    parse_descriptor();
    if (dsc.lct_flag)
        parse_lct();
    parse_data(gct, gct_size);
}

void GIFFrame::parse_gce()
{
    LOG("Parsing Graphics Control Extension\n");
    gif_file.read((char*)&gce.block_size, 1);
    LOG("gce block size: %d\n", (int)gce.block_size);

    char packed;
    gif_file.read((char*)&packed, 1);
    gce.disposal_method = (packed>>2) & 0x7;
    LOG("Disposal method: %d\n", gce.disposal_method);

    gce.user_input_flag = (packed>>1) & 0x1;
    gce.transparent_colour_flag = packed & 0x1;
    LOG("UI Flag: %d, transparent colour flag: %d\n", gce.user_input_flag, gce.transparent_colour_flag);

    char delay_time[2];
    gif_file.read(delay_time, 2);
    gce.delay_time = (delay_time[0]<<0) | (delay_time[1]<<8);
    LOG("Delay time: %d\n", gce.delay_time);

    gif_file.read((char*)&gce.transparent_colour_index, 1);
    LOG("Transparent colour index: %d\n", gce.transparent_colour_index);
    gif_file.seekg(1, gif_file.cur); // skip the block terminator
}

void GIFFrame::parse_descriptor()
{
    LOG("Parsing image descriptor\n");
    char img_separator;
    gif_file.read(&img_separator, 1);
    if (img_separator != GIF::IMAGE_DESCRIPTOR)
        throw GIFParseError(std::string("No image descriptor found."));
    char dims[8];
    gif_file.read(dims, 8);
    dsc.left   = dims[0] | (dims[1]<<8);
    dsc.top    = dims[2] | (dims[3]<<8);
    dsc.width  = dims[4] | (dims[5]<<8);
    dsc.height = dims[6] | (dims[7]<<8);
    LOG("width: %d, height: %d, offset: (%d, %d)\n", dsc.width, dsc.height, dsc.top, dsc.left);
    char packed;
    gif_file.read((char*)&packed, 1);
    dsc.lct_flag = (packed>>7) & 0x1;
    dsc.interlace_flag = (packed>>6) & 0x1;
    dsc.sort_flag = (packed>>5) & 0x1;
    dsc.lct_size = 2<<(packed&0x07); // size = 2^(N+1) where n is the last three bits of packed
}

void GIFFrame::parse_lct()
{
    LOG("Parsing local colour table\n");
    lct = new uint8_t[3*dsc.lct_size]; // 3 bytes per colour
    gif_file.read((char*) lct, 3*dsc.lct_size);
    LOG("local colour table:\n");
    for (int i = 0; i < dsc.lct_size; i++) {
        LOG("%#08x\n", (int)(lct[i*3]<<4 | lct[i*3+1]<<2 | lct[i*3+2]));
    }
}

void GIFFrame::parse_data(uint8_t *gct, uint8_t gct_size)
{
    LOG("Parsing image data\n");
    uint8_t code_size;
    gif_file.read((char *) &code_size, 1);

    unordered_map<uint8_t, uint8_t> code_table;
    vector<uint8_t> index_stream;

    // initialise the code table
    uint8_t *ct = dsc.lct_flag ? lct : gct;
    uint8_t ct_size = dsc.lct_flag ? dsc.lct_size : gct_size;
    if (ct == nullptr || ct_size == 0)
        throw GIFParseError(string("No colour table found."));
    int i;
    for (i = 0; i < ct_size+2; i++)
        code_table[i] = i;

    uint8_t clear_code = i-1;
    uint8_t eoi_code = i-2;


}
