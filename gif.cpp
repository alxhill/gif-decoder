//
//  gif.cpp
//  GIF-T
//
//  Created by Alexander Hill on 12/28/14.
//  Copyright (c) 2014 Alexander Hill. All rights reserved.
//

#include <cassert>
#include <cstring>
#include <cmath>
#include "gif.h"

GIF::GIF() {}

GIF::~GIF() {}

GIF::GIF(const string filename)
{
    open(filename);
    parse();
}

void GIF::open(const string filename)
{
    gif_file.open(filename, ifstream::binary | ifstream::in);
    if (!gif_file) {
        throw runtime_error(std::string("file does not exist"));
    }
}

void GIF::parse()
{
    parse_header();

    while(true) {
        uint8_t next;
        gif_file.read((char*)&next, 1);
        if (next == EXTENSION_BLOCK)
        {
            LOG("Reading extension block\n");
            // extension block
            gif_file.read((char*)&next, 1);
            switch(next)
            {
                case EXTENSION_BLOCK_GCE: {
                    gif_file.putback(next);
                    GIFFrame frame(gif_file);
                    frame.parse();
                    frames.push_back(frame);
                    break;
                }
                case EXTENSION_BLOCK_PLAINTEXT:
                    LOG("PLAINTEXT %#04x\n", next);
                case EXTENSION_BLOCK_APPLICATION:
                    if (next == EXTENSION_BLOCK_APPLICATION)
                        LOG("APPLICATION %#04x\n", next);
                case EXTENSION_BLOCK_COMMENT:
                    if (next == EXTENSION_BLOCK_COMMENT)
                        LOG("COMMENT %#04x\n", next);
                    // we don't care about these extensions all that much
                    LOG("Skipping extension\n");
                    skip_extension();
                    LOG("Skipped extension %#04x\n", next);
                    break;
                default:
                    LOG("Unsupported extension type\n");

            }
        }
        else if (next == IMAGE_DESCRIPTOR)
        {
            LOG("Parsing image descriptor\n");
            gif_file.putback(next);
            GIFFrame frame(gif_file);
            frame.parse();
            frames.push_back(frame);
        }
        else if (next == GIF_EOF)
        {
            // EOF! :)
            LOG("Finished parsing gif file\n");
            return;
        }
        else
        {
            LOG("unknown thing found: %#04x\n", next);
            exit(1);

        }
    }
}

void GIF::parse_header()
{
    LOG("Parsing GIF Header\n");

    gif_file.read(header.signature, 3);
    gif_file.read(header.version, 3);
    header.signature[3] = '\0';
    header.version[3] = '\0';
    if (memcmp(header.signature, "GIF", 3) != 0)
        throw GIFParseError(std::string("Invalid signature. Are you sure this file is a GIF?"));

    if (memcmp(header.version, "89a", 3) != 0)
        throw GIFParseError(std::string("Invalid GIF version. Only GIF89a is currently supported."));

    LOG("signature: %s\n", header.signature);
    LOG("version: %s\n", header.version);

    uint8_t lsd[7]; // logical screen descriptor
    gif_file.read((char*)lsd, 7);
    // convert little endian to big endian
    header.width = lsd[0] | (lsd[1]<<8);
    header.height = lsd[2] | (lsd[3]<<8);
    LOG("width: %d\n", (int)header.width);
    LOG("height: %d\n", (int)header.height);

    // 1 if global colour table is present
    header.gct_flag = lsd[4] >> 7;
    LOG("has gct: %d\n", header.gct_flag);

    header.colour_res = ((lsd[4] >> 4) & 0x7) + 1;
    LOG("colour resolution: %d\n", (int)header.colour_res);

    header.sort_flag = (lsd[4] >> 3) & 1;
    LOG("sort flag: %d\n", header.sort_flag);

    header.gct_size = 2<<(lsd[4]&0x7); // size = 2^(N+1) where N is the last 3 bits of lsd[4]
    LOG("gct size: %d\n", header.gct_size);

    header.bgcolour_index = lsd[5];
    header.pixel_aspect_ratio = lsd[6];
    LOG("bg colour index: %d, pixel aspect ratio: %d\n", (int)header.bgcolour_index, (int)header.pixel_aspect_ratio);

    if (header.gct_flag) {
        header.gct = new uint8_t[3*header.gct_size]; // 3 bytes per colour
        gif_file.read((char*)header.gct, 3*header.gct_size);
        LOG("colour table:\n");
        for (int i = 0; i < header.gct_size; i++) {
            LOG("%#08x\n", (int)(header.gct[i*3]<<4 | header.gct[i*3+1]<<2 | header.gct[i*3+2]));
        }
    }
}

void GIF::skip_extension()
{
    int total_size = 0;
    uint8_t block_size;
    while (true) {
        gif_file.read((char*)&block_size, 1);
        total_size += block_size + 1; // include the size bytes in the total size
        LOG("skipping %d bytes\n", (int)block_size);
        if (block_size == 0) {
            LOG("Total number of skipped bytes: %d\n", total_size-1);
            return;
        }
        gif_file.seekg((unsigned int)block_size, gif_file.cur);
    }
}
