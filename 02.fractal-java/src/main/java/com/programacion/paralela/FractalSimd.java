package com.programacion.paralela;

import java.nio.ByteBuffer;

public class FractalSimd {
    public ByteBuffer pixelBuffer;

    public FractalSimd() {
        pixelBuffer = ByteBuffer.allocateDirect(FractalParams.WIDTH * FractalParams.HEIGHT * 4);
    }

    public void juliaSimd() {
        FractalDll.INSTANCE.julia_simd(
                FractalParams.xMin,
                FractalParams.yMin,
                FractalParams.xMax,
                FractalParams.yMax,
                FractalParams.WIDTH,
                FractalParams.HEIGHT,
                FractalParams.max_iteraciones,
                pixelBuffer
        );
    }
}
