#include <fmt/core.h>
#include <SFML/Graphics.hpp>

#include <complex>
#include "fractal_serial.h"

#ifdef _WIN32
#include <windows.h>
#endif

// dimesion de la imagen
#define WIDTH 1600
#define HEIGHT 900

// parametros de la imagen
int max_iterations = 10;

double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

std::complex<double> c(-0.7, 0.27015);

// textura
uint32_t *pixel_buffer = nullptr;
uint16_t *texture_buffer = nullptr;

int main()
{

    pixel_buffer = new uint32_t[WIDTH * HEIGHT];
    texture_buffer = new uint16_t[WIDTH * HEIGHT];

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia set - SMFL");

#ifdef _WIN32
    HWND hWnd = window.getNativeHandle();
    ShowWindow(hWnd, SW_MAXIMIZE);
#endif

    sf::Texture texture({WIDTH, HEIGHT});
    sf::Sprite sprite(texture);

    sf::Font font("arial.ttf");
    sf::Text text(font, "Julia Set", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    // FPS
    int frames = 0;
    int fps = 0;
    sf::Clock clock;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto *evt = event->getIf<sf::Event::KeyReleased>())
            {
                switch (evt->scancode)
                {
                case sf::Keyboard::Scancode::Up:
                    max_iterations += 10;
                    break;
                case sf::Keyboard::Scancode::Down:
                    max_iterations -= 10;
                    if (max_iterations < 10)
                        max_iterations = 10;
                    break;
                default:
                    break;
                }
            }
        }

        // dibujamos
        julia_serial_1(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);

        texture.update((const uint8_t *)pixel_buffer);

        // contar FPS
        frames++;

        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }

        // Actualizar el titulod de la ventana con el FPS
        auto msg = fmt::format("Julia set - SMFL - iteraciones: {} - FPS: {}", max_iterations, fps);
        text.setString(msg);

        // pintamos

        // Clear screen
        window.clear();
        {
            window.draw(sprite);
            window.draw(text);
        }

        // Update the window
        window.display();
    }

    delete[] pixel_buffer;
    delete[] texture_buffer;

    return 0;
}