package com.programacion.paralela;

public class FractalParams {
    public static int max_iteraciones = 10;

    public static int WIDTH = 1600;
    public static int HEIGHT = 900;

    public static double xMin = -1.5;
    public static double xMax = 1.5;

    public static double yMin = -1.0;
    public static double yMax = 1.0;

    public static final double cReal = -0.7;
    public static final double cImag = 0.27015;

    public static final int PALETTE_SIZE = 16;

    public static final int[] color_ramp = {
            (0xFFEC2C22),
            (0xFFDC291F),
            (0xFFCC261D),
            (0xFFBC231B),
            (0xFFAD2018),
            (0xFF9D1D16),
            (0xFF8D1A14),
            (0xFF7D1712),
            (0xFF6E140F),
            (0xFF5E110D),
            (0xFF4E0E0B),
            (0xFF3E0B09),
            (0xFF2F0806),
            (0xFF1F0504),
            (0xFF0F0202),
            (0xFF000000)
    };
    public static final int[] color_ramp_thread = {
            (0x0F93FFFF),
            (0x0E89EEFF),
            (0x0D7FDDFF),
            (0x0C75CCFF),
            (0x0B6BBBFF),
            (0x0A62AAFF),
            (0x095899FF),
            (0x084E88FF),
            (0x074477FF),
            (0x063A66FF),
            (0x053155FF),
            (0x042744FF),
            (0x031D33FF),
            (0x021322FF),
            (0x010911FF),
            (0x000000FF)

    };

}
