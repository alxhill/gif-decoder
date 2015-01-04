//
//  gif_frame.cpp
//  GIF-T
//
//  Created by Alexander Hill on 12/29/14.
//  Copyright (c) 2014 Alexander Hill. All rights reserved.
//

#include <cstdio>
#include "gif_frame.h"
#include "gif.h"

void GIFFrame::parse()
{
    uint8_t next;
    gif_file.read((char*)&next, 1);
    switch(next)
    {
        case GIF::EXTENSION_BLOCK_GCE:
            parse_gce();
            break;
        case GIF::IMAGE_DESCRIPTOR:
            parse_descriptor();
            break;
        default:
            LOG("Unknown expected symbol for frame\n");
    }
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

    return;
}

void GIFFrame::parse_descriptor()
{
    char dims[8];
    gif_file.read(dims, 8);
    dsc.left   = dims[0] | (dims[1]<<8);
    dsc.top    = dims[2] | (dims[3]<<8);
    dsc.width  = dims[4] | (dims[5]<<8);
    dsc.height = dims[6] | (dims[7]<<8);

    char packed;
    gif_file.read((char*)&packed, 1);
    dsc.lct_flag = (packed>>7) & 0x1;
    dsc.interlace_flag = (packed>>6) & 0x1;
    dsc.sort_flag = (packed>>5) & 0x1;
    dsc.lct_size = 2<<(packed&0x07); // size = 2^(N+1) where n is the last three bits of packed
}
