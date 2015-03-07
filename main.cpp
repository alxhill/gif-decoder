#include <iostream>
// uses SFML to display the final image
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "gif.hpp"

int main(int argc, const char * argv[]) {
    std::cout << "GIF Parser" << std::endl;

    std::string filename;
    if (argc < 2)
        filename = "test.gif";
    else
        filename = std::string(argv[1]);

    GIF gif(filename);
    GIFFrame *frame = gif[0];
    struct rgb** pixels = frame->pixels;
    sf::Image sf_gif;
    sf_gif.create(frame->dsc.width, frame->dsc.height);

    for (int i = 0; i < frame->dsc.width; i++)
    {
        for (int j = 0; j < frame->dsc.height; j++)
        {
            auto &c = pixels[j][i];
            sf::Color colour(c.r, c.g, c.b);
            sf_gif.setPixel(i, j, colour);
        }
    }

    sf::Texture gif_texture;
    gif_texture.loadFromImage(sf_gif);
    sf::Sprite gif_sprite;
    gif_sprite.setTexture(gif_texture, true);
    gif_sprite.scale(3, 3);

    sf::RenderWindow window(sf::VideoMode(3*frame->dsc.width, 3*frame->dsc.height), "First gif frame!");

    while(window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(gif_sprite);
        window.display();
    }

    return 0;
}
