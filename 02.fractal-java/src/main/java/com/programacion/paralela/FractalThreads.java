package com.programacion.paralela;

public class FractalThreads {

    public int[] pixel_buffer;

    public FractalThreads() {
        pixel_buffer = new int[FractalParams.WIDTH * FractalParams.HEIGHT];
    }

    int acotado_2(double x, double y) {
        int iter = 1;
        double zr = x;
        double zi = y;

        while (iter < FractalParams.max_iteraciones && (zr * zr + zi * zi) < 4.0) {
            double dr = zr * zr - zi * zi + FractalParams.cReal;
            double di = 2.0 * zr * zi + FractalParams.cImag;
            zr = dr;
            zi = di;
            iter++;
        }

        if (iter < FractalParams.max_iteraciones) {
            int index = iter % FractalParams.PALETTE_SIZE;
            return FractalParams.color_ramp_thread[index];
        }

        return 0xFF000000;
    }

    void julia_threads(double x_min, double y_min, double x_max, double y_max, int width, int height) {
        int availableProcessors = Runtime.getRuntime().availableProcessors();
        int threadCount = Math.clamp(availableProcessors, 1, height);
        Thread[] workers = new Thread[threadCount];

        double dx = (x_max - x_min) / width;
        double dy = (y_max - y_min) / height;
        int rowsPerThread = (height + threadCount - 1) / threadCount;

        for (int t = 0; t < threadCount; t++) {
            final int startY = t * rowsPerThread;
            final int endY = Math.min(height, startY + rowsPerThread);

            if (startY >= endY) {
                continue;
            }

            workers[t] = new Thread(() -> {
                for (int j = startY; j < endY; j++) {
                    for (int i = 0; i < width; i++) {
                        double x = x_min + i * dx;
                        double y = y_max - j * dy;
                        pixel_buffer[j * width + i] = acotado_2(x, y);
                    }
                }
            });
            workers[t].start();
        }

        for (Thread worker : workers) {
            if (worker == null) {
                continue;
            }

            try {
                worker.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                throw new RuntimeException("Thread execution interrupted", e);
            }
        }
    }
}

