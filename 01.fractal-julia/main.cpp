#include <fmt/core.h>
#include <SFML/Graphics.hpp>

#include <complex>
#include "fractal_serial.h"
#include "fractal_simd.h"

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

enum class runtime_type
{
    SERIAL_1 = 0,
    SERIAL_2,
    SIMD
};

int main()
{

    runtime_type r_type = runtime_type::SERIAL_1;

    pixel_buffer = new uint32_t[WIDTH * HEIGHT];
    // texture_buffer = new uint16_t[WIDTH * HEIGHT];

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

    std::string options = "Options: [1] Serial 1 [2] Serial 2 [3] SIMD | Up/Down: Change Iterations";
    sf::Text textOptions(font, options, 20);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setPosition({10, window.getView().getSize().y - 40});
    textOptions.setStyle(sf::Text::Bold);

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
                    // TECLAS PARA AUMENTAR O DISMINUIR LAS ITERACIONES
                case sf::Keyboard::Scan::Up:
                    max_iterations += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iterations -= 10;
                    if (max_iterations < 10)
                        max_iterations = 10;
                    break;

                    // TECLAS PARA CAMBIAR EL TIPO DE RUNTIME
                case sf::Keyboard::Scan::Num1:
                    r_type = runtime_type::SERIAL_1;
                    break;
                case sf::Keyboard::Scan::Num2:
                    r_type = runtime_type::SERIAL_2;
                    break;

                case sf::Keyboard::Scan::Num3:
                    r_type = runtime_type::SIMD;
                    break;

                default:
                    break;
                }
            }
        }

        std::string mode = "";

        if (r_type == runtime_type::SERIAL_1)
        {
            mode = "Serial 1";
            julia_serial_1(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::SERIAL_2)
        {
            mode = "Serial 2";
            julia_serial_2(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::SIMD)
        {
            mode = "SIMD";
            julia_simd(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }

        // dibujamos

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
        auto msg = fmt::format("Julia set - SMFL - iteraciones: {} - FPS: {} - Mode:{}", max_iterations, fps, mode);
        text.setString(msg);

        // pintamos

        // Clear screen
        window.clear();
        {
            window.draw(sprite);
            window.draw(text);
            window.draw(textOptions);
        }

        // Update the window
        window.display();
    }

    delete[] pixel_buffer;
    // delete[] texture_buffer;

    return 0;
}