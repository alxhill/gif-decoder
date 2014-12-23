#include <cassert>
#include <cstring>
#include <cmath>
#include "gif.hpp"

GIF::GIF() {}

GIF::~GIF() {}

GIF::GIF(const string filename)
{
    open(filename);
    parse();
}

void GIF::open(const string filename)
{
    gif_file.open(filename, ifstream::binary);
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
        if (next == 0x21)
        {
            LOG("Reading extension block\n");
            // extension block
            gif_file.read((char*)&next, 1);
            switch(next)
            {
                case 0xF9:
                    parse_gce();
                    break;
                case 0x01:
                    // plain text extension
                case 0xFF:
                    // application extension
                case 0xFE:
                    // comment extension
                default:
                    LOG("Unsupported extension type\n");

            }
        }
        else if (next == 0x2C)
        {
            // image descriptor
        }
        else if (next == 0x3D)
        {
            // EOF! :)
            LOG("Finished parsing gif file\n");
            return;
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
        throw GIFParseException(); // "Invalid signature. Are you sure this file is a GIF?"

    if (memcmp(header.version, "89a", 3) != 0)
        throw GIFParseException(); // "Invalid GIF version. Only GIF89a is currently supported."

    LOG("signature: %s\n", header.signature);
    LOG("version: %s\n", header.version);


    uint8_t lsd[7]; // logical screen descriptor
    gif_file.read((char*)lsd, 7);
    // convert little endian to big endian
    header.width = (lsd[0]<<0) | (lsd[1]<<8);
    header.height = (lsd[2]<<0) | (lsd[3]<<8);
    LOG("width: %d\n", (int)header.width);
    LOG("height: %d", (int)header.height);

    // 1 if global colour table is present
    header.gct_flag = lsd[4] >> 7;
    LOG("has gct: %d\n", header.gct_flag);

    header.colour_res = ((lsd[4] >> 4) & 0x7) + 1;
    LOG("colour resolution: %d\n", (int)header.colour_res);

    header.sort_flag = (lsd[4] >> 3) & 1;
    LOG("sort flag: %d\n", header.sort_flag);

    header.gct_size = 1<<((lsd[4]&0x7)+1); // size = 2^(N+1) where N is the last 3 bits of lsd[4]
    LOG("gct size: %d\n", header.gct_size);

    header.bgcolour_index = lsd[5];
    header.pixel_aspect_ratio = lsd[6];
    LOG("bg colour index: %d, pixel aspect ratio: %d\n", (int)header.bgcolour_index, (int)header.pixel_aspect_ratio);

    if (header.gct_flag) {
        header.gct = new uint8_t[3*header.gct_size]; // 3 bytes per colour
        gif_file.read((char*)header.gct, 3*header.gct_size);
        cout << "colour table:" << endl;
        for (int i = 0; i < header.gct_size; i++)
            cout << hex << (int)header.gct[i*3] << (int)header.gct[i*3+1] << (int)header.gct[i*3+2] << endl;
    }
}

void GIF::parse_gce()
{
    LOG("Parsing Graphics Control Extension\n");
    gif_file.read((char*)&gce.block_size, 1);
    LOG("gce block size: %d\n", (int)gce.block_size);

    exit(0);
}

void GIF::skip_extension()
{
    char next;
    while (true) {
        gif_file.read(&next, 1);
    }
}

int main(int argc, char const *argv[])
{
    std::cout << "GIF Parser" << endl;

    GIF gif(argv[1]);


    return 0;
}
