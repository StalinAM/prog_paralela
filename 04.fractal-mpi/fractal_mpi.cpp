#include "fractal_mpi.h"
#include <complex>
#include "palette.h"

// variables externas definidas en main.cpp
extern int max_iterations;     // número máximo de iteraciones antes de declarar "no divergente"
extern std::complex<double> c; // constante 'c' del conjunto de Julia

uint32_t acotado_2(double x, double y)
{
    // calculos manuales
    int iter = 1;
    double zr = x;
    double zi = y;

    while ((zr * zr + zi * zi) < 4.0 && iter < max_iterations)
    {
        double dr = zr * zr - zi * zi + c.real(); // calculamos la parte real
        double di = 2.0 * zr * zi + c.imag();     // calculamos la parte imaginaria

        zr = dr;
        zi = di;

        iter++;
    }

    if (iter < max_iterations)
    {
        // la norma > 2
        int index = iter % PALETTE_SIZE; // índice para la paleta (ciclo a través de los colores)
        return color_ramp[index];        // devolvemos el color correspondiente de la paleta
    }
    return 0xFF000000; // quiero color negro (AABBGGRR)
}

void julia_mpi(double x_min, double y_min, double x_max, double y_max, uint32_t row_start, uint32_t row_end, uint32_t width, uint32_t height, uint32_t *pixel_buffer)
{

    double dx = (x_max - x_min) / (width);
    double dy = (y_max - y_min) / (height);

    for (int j = row_start; j < row_end; j++)
    {
        for (int i = 0; i < width; i++)
        {
            double x = x_min + i * dx;
            double y = y_max - j * dy;
            // no vamos a usar complejos

            auto color = acotado_2(x, y); // auto es igual a var --> inferencia de tipos

            pixel_buffer[j * width + i] = color; // asignamos el color al pixel
        }
    }
}