#include <cstdio>
#include <map>
#include <vector>
#include <bitset>
#include "gif_frame.hpp"
#include "gif.hpp"

void GIFFrame::decode(uint8_t *gct, uint8_t gct_size)
{
    decode_descriptor();
    if (dsc.lct_flag)
        decode_lct();
    decode_data(gct, gct_size);
}

void GIFFrame::decode_gce()
{
    LOG("Decoding Graphics Control Extension\n");
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

void GIFFrame::decode_descriptor()
{
    LOG("Decoding image descriptor\n");
    char img_separator;
    gif_file.read(&img_separator, 1);
    if (img_separator != GIF::IMAGE_DESCRIPTOR)
        throw GIFDecodeError(std::string("No image descriptor found."));
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

void GIFFrame::decode_lct()
{
    LOG("Decoding local colour table\n");
    lct = new uint8_t[3*dsc.lct_size]; // 3 bytes per colour
    gif_file.read((char*) lct, 3*dsc.lct_size);
    LOG("local colour table:\n");
    for (int i = 0; i < dsc.lct_size; i++) {
        LOG("%#08x\n", (int)(lct[i*3]<<4 | lct[i*3+1]<<2 | lct[i*3+2]));
    }
}

uint8_t get_code(uint16_t byte, uint8_t bits)
{
    LOG("%d bits", bits);
    uint8_t num = 0;
    while (bits > 0) {
        bits--;
        num |= byte & (1<<bits);
    }
    LOG(" of byte %#04x = %d\n", byte, num);
    return num;
}

void GIFFrame::decode_data(uint8_t *gct, uint8_t gct_size)
{
    LOG("Decoding image data\n");
    uint8_t code_size;
    uint8_t block_size;
    gif_file >> code_size;
    gif_file >> block_size;
    code_size++;
    LOG("Code size: %d, Block size: %d\n", code_size, block_size);

    // initialise the code table
    uint8_t *ct = dsc.lct_flag ? lct : gct;
    uint8_t ct_size = dsc.lct_flag ? dsc.lct_size : gct_size;
    LOG("Colour table size: %d\n", ct_size);
    if (ct == nullptr || ct_size == 0)
        throw GIFDecodeError(string("No colour table found."));
    int i;

    map<uint8_t, vector<uint8_t>> code_table;
    vector<uint8_t> code_stream;
    vector<uint8_t> index_stream;

    uint8_t clear_code = ct_size+1;
    uint8_t eoi_code = ct_size+2;
    LOG("Clear code: %d, eoi_code %d\n", clear_code, eoi_code);

    // initialising the code table with
    for (unsigned char i = 0; i < ct_size; i++) {
        code_table[i] = {i};
    }

    uint8_t cur_code = 0;
    uint16_t cur_byte = 0;
    gif_file.read(reinterpret_cast<char*>(&cur_byte), 1);
    block_size--;
    LOG("Starting byte: %#04x\n", cur_byte);
    i = 0;
    uint8_t bits_used = 8;
    while (block_size > 0) {
        while (block_size--) {
            uint8_t shift = 0;
            while (shift < bits_used - code_size) {
                LOG("=C=\n");
                cur_code = get_code(cur_byte >> shift, code_size);
                if (cur_code == clear_code) {
                    LOG("\n====CLEAR====\n\n");
                }
                code_stream.push_back(cur_code);
                shift += code_size;
                if (cur_code == (1<<code_size)-1) {
                    LOG("INCREASING CODE SIZE\n");
                    code_size++;
                }
            }
            LOG("Getting new byte. shift: %d, curr: %#04x\n", shift, cur_byte);
            uint8_t next_byte;
            gif_file.read((char*)&next_byte, 1);
            cur_byte = (cur_byte >> shift) | (next_byte << (bits_used - shift));
            bits_used = bits_used - shift + 8;
            LOG("cur_byte is now %#08x\n", cur_byte);
        }
        gif_file >> block_size;
    }
    LOG("Code list: \n");
    for (auto code : code_stream) {
        printf("%d\n", code);
    }
}
