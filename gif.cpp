#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

struct gif_header
{
    char header[6];
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

// parse the graphics control extension
struct gif_gce *parse_gce(ifstream &gif)
{
    cout << "Parsing GCE" << endl;
    struct gif_gce *gce = new struct gif_gce;
    gif.read((char*)&gce->block_size, 1);


    return gce;
}

int main(int argc, char const *argv[])
{
    std::cout << "GIF Parser" << endl;

    ifstream gif(argv[1], ifstream::binary);
    assert(gif);

    struct gif_header h;

    gif.read(h.header, 6);

    cout << "signature: " << h.header[0] << h.header[1] << h.header[2] << endl;
    cout << "version: " << h.header[3] << h.header[4] << h.header[5] << endl;
    assert(memcmp((char *)h.header, "GIF89a", 6)==0);

    uint8_t lsd[7]; // logical screen descriptor
    gif.read((char*)lsd, 7);
    // convert little endian to big endian
    h.width = (lsd[0]<<0) | (lsd[1]<<8);
    h.height = (lsd[2]<<0) | (lsd[3]<<8);
    cout << "width: " << (int)h.width << endl;
    cout << "height: " << (int)h.height << endl;

    // 1 if global colour table is present
    h.gct_flag = lsd[4] >> 7;
    cout << "has gct: " << h.gct_flag << endl;

    h.colour_res = ((lsd[4] >> 4) & 0x7) + 1;
    cout << "colour resolution: " << (int)h.colour_res << endl;

    h.sort_flag = (lsd[4] >> 3) & 1;
    cout << "sort flag: " << h.sort_flag << endl;

    h.gct_size = pow(2, ((lsd[4]) & 0x7)+1);
    cout << "gct size: " << h.gct_size << endl;

    h.bgcolour_index = lsd[5];
    h.pixel_aspect_ratio = lsd[6];
    cout << "bg colour index: " << (int)h.bgcolour_index << ", pixel_aspect_ratio: " << (int)h.pixel_aspect_ratio << endl;

    if (h.gct_flag) {
        h.gct = new uint8_t[3*h.gct_size]; // 3 bytes per
        gif.read((char*)h.gct, 3*h.gct_size);
        cout << "colour table:" << endl;
        for (int i = 0; i < h.gct_size; i++)
            cout << hex << (int)h.gct[i*3] << (int)h.gct[i*3+1] << (int)h.gct[i*3+2] << endl;
    }

    // header has now been parsed

    uint8_t next;
    gif.read((char*)&next, 1);
    if (next == 0x21)
    {
        // extension block
        gif.read((char*)&next, 1);
        switch(next)
        {
            case 0xF9:
                parse_gce(gif);
                break;
            case 0x01:
                // plain text extension
            case 0xFF:
                // application extension
            case 0xFE:
                // comment extension
            default:
                cout << "Unsupported extension type" << endl;
                return 1;

        }
    }
    else if (next == 0x2C)
    {
        // image descriptor
    }
    else if (next == 0x3D)
    {
        // EOF! :)
        cout << "Finished parsing gif file";
        return 0;
    }

    // we should never get here
    assert(false);

    return 0;
}
