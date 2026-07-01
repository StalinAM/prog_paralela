#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include "fractal_mpi.h"
#include "palette.h"
#include <complex>
#include <cstdint>
#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

// dimesion de la imagen
#define WIDTH 1600
#define HEIGHT 900

double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

int max_iterations = 10;

std::complex<double> c(-0.7, 0.27015);

uint32_t *pixel_buffer = nullptr;
uint32_t *texture_buffer = nullptr;

int nprocs;
int rank;
int delta;
int padding;
int row_start;
int row_end;

int32_t running = 1;

void setup_ui()
{
    std::cout.flush();
    texture_buffer = new uint32_t[WIDTH * HEIGHT];
    std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));

    // inicializar la Ui

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Fractal MPI",
                            sf::Style::Default);

#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif

    sf::Texture texture({WIDTH, HEIGHT});
    texture.update((const uint8_t *)texture_buffer);
    sf::Sprite sprite(texture);

    //-- textos
    // const sf::Font font(arial_ttf2::data2, arial_ttf2::data_len2);

    // sf::Text text(font, "Fractal MPI", 24);
    // text.setFillColor(sf::Color::White);
    // text.setPosition({10, 10});
    // text.setStyle(sf::Text::Bold);

    // std::string options = "OPTIONS: [1] Serial 1 UP/DOWN: Change Iterations";
    // sf::Text textOptions(font, options, 24);
    // textOptions.setFillColor(sf::Color::White);
    // textOptions.setStyle(sf::Text::Bold);
    // textOptions.setPosition({10, window.getView().getSize().y - 40});

    // FPS
    int frame = 0;
    int fps = 0; // cuantos mas fps aumenta signifa que dibuja mucho mas rapido
    sf::Clock clockFrames;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // notificar a los otros RANKS
            if (event->is<sf::Event::Closed>())
            {
                running = false;
                window.close();
            }
            else if (event->is<sf::Event::KeyReleased>())
            {
                auto evt = event->getIf<sf::Event::KeyReleased>();
                switch (evt->scancode)
                {
                case sf::Keyboard::Scan::Up:
                    max_iterations += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iterations -= 10;
                    if (max_iterations < 10)
                        max_iterations = 10;
                    break;
                }
                std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
            }
        }
        // notificar

        // julia_mpi(x_min, y_min, x_max, y_max, row_start, row_end, pixel_buffer);
        // dibuja_text_rank();
        std::memcpy(texture_buffer, pixel_buffer, WIDTH * delta * sizeof(uint32_t));

        texture.update((const uint8_t *)texture_buffer);

        // contar FPS
        frame++;
        if (clockFrames.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frame;
            frame = 0;
            clockFrames.restart();
        }

        auto msg = fmt::format("Fractal: Iteraciones: {}, FPS: {}", max_iterations, fps);
        // text.setString(msg);
        // escribir imagen
        // stbi_write_png("fractal.png", WIDTH, HEIGHT, STBI_rgb_alpha, texture_buffer, WIDTH * 4);
        window.clear();
        {
            window.draw(sprite);
            // window.draw(text);
            // window.draw(textOptions);
        }
        window.display();
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    delta = std::ceil(HEIGHT * 1.0 / nprocs);
    padding = delta * nprocs - HEIGHT;

    /**
     * r0: start = 0*400=0, end=0+400=400
     * r1: start = 1*400=400, end400+400=800
     * r2: start = 2*400=800, end=800+400=1200
     * r3: start = 3*400=1200, end=1200+400=1600
     */
    row_start = rank * delta;
    row_end = row_start + delta;

    if (row_end > HEIGHT)
    {
        row_end = HEIGHT;
    }

    // icicializar buffers
    pixel_buffer = new uint32_t[WIDTH * delta];
    std::memset(pixel_buffer, 0, WIDTH * delta * sizeof(uint32_t));

    fmt::print("Rank {}, nprocs {}, delta {}, row_start {}, row_end {}\n",
               rank, nprocs, delta, row_start, row_end);

    std::cout.flush();

    if (rank == 0)
    {
        setup_ui();

        // Notificar a todos los procesos que deben terminar
        std::vector<int32_t> dummy{running, max_iterations};

        MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        std::vector<int32_t> dummy(2);
        while (true)
        {

            MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
            running = dummy[0];
            max_iterations = dummy[1];
            fmt::print("rank: {}: received iterations={}, running={}\n", rank, max_iterations, running);

            if (running == 0)
            {
                fmt::print("rank: {}: exiting...\n", rank);
                break;
            }

            // julia_mpi(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, row_start, row_end, pixel_buffer);
        }
    }

    MPI_Finalize();
    return 0;
}