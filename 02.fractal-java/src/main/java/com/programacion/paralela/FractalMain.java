package com.programacion.paralela;

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

import org.lwjgl.*;
import org.lwjgl.glfw.*;
import org.lwjgl.opengl.*;
import org.lwjgl.system.*;

import static org.lwjgl.glfw.Callbacks.*;
import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.system.MemoryUtil.*;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class FractalMain {
    // windos handle
    private long window;
    private int textureID;
    private int overlayTextureID;

    private IntBuffer pixelBuffer;
    private ByteBuffer overlayBuffer;
    private BufferedImage overlayImage;
    private Graphics2D overlayGraphics;
    private int[] overlayPixels;

    private static final int OVERLAY_WIDTH = 300;
    private static final int OVERLAY_HEIGHT = 90;
    private static final int OVERLAY_MARGIN = 10;

    FractalCpu fractalCpu;
    FractalSimd fractalSimd;
    FPSCounter fpsCounter;

    int modo = 1;

    public FractalMain() {
        fractalCpu = new FractalCpu();
        fractalSimd = new FractalSimd();

        fpsCounter = new FPSCounter();

        pixelBuffer = BufferUtils.createIntBuffer(FractalParams.WIDTH * FractalParams.HEIGHT);
    }

    public void run() {
        System.out.println("Fractal Julia " + Version.getVersion());

        init();
        loop();

        glfwFreeCallbacks(window);
        glfwDestroyWindow(window);

        glfwTerminate();
        glfwSetErrorCallback(null).free();
    }

    private void init() {

        GLFWErrorCallback.createPrint(System.err).set();

        if (!glfwInit())
            throw new IllegalStateException("Unable to initialize GLFW");

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // Create the window
        window = glfwCreateWindow(FractalParams.WIDTH, FractalParams.HEIGHT, "Julia Set", NULL, NULL);

        if (window == NULL)
            throw new RuntimeException("Failed to create the GLFW window");

        // manejo de eventos
        glfwSetKeyCallback(window, (window, key, scancode, action, mods) -> {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
                glfwSetWindowShouldClose(window, true);
            if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
                FractalParams.max_iteraciones += 10;
            }
            if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
                FractalParams.max_iteraciones -= 10;
                if (FractalParams.max_iteraciones < 10)
                    FractalParams.max_iteraciones = 10;
            }
            if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
                System.out.println("Modo Java CPU");
                modo = 1;
            } else if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
                System.out.println("Modo C/C++ SIMD");
                modo = 2;
                fractalSimd = new FractalSimd();
            }

        });
        GLFWVidMode vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(window,
                (vidmode.width() - FractalParams.WIDTH) / 2,
                (vidmode.height() - FractalParams.HEIGHT) / 2);

        // Make the OpenGL context current
        glfwMakeContextCurrent(window);

        GL.createCapabilities();
        GL.createCapabilitiesWGL(); // usado para windows

        // ----version de OpenGL
        String version = GL11.glGetString(GL11.GL_VERSION);
        String vendor = GL11.glGetString(GL11.GL_VENDOR);
        String renderer = GL11.glGetString(GL11.GL_RENDERER);

        System.out.println("OpenGL version: " + version);
        System.out.println("OpenGL vendor: " + vendor);
        System.out.println("OpenGL renderer: " + renderer);

        // --conf
        GL11.glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_TEXTURE_2D);
        glLoadIdentity();

        // Enable v-sync
        glfwSwapInterval(1);

        // Make the window visible
        glfwShowWindow(window);

        setupTexture();
        setupOverlay();

    }

    private void setupTexture() {
        textureID = glGenTextures();
        glBindTexture(GL_TEXTURE_2D, textureID);

        // RESERVA LA MEMORIA UNICAMENTE
        glTexImage2D(
                GL_TEXTURE_2D, 0,
                GL_RGBA8,
                FractalParams.WIDTH,
                FractalParams.HEIGHT,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    }

    private void setupOverlay() {
        overlayImage = new BufferedImage(OVERLAY_WIDTH, OVERLAY_HEIGHT, BufferedImage.TYPE_INT_ARGB);
        overlayGraphics = overlayImage.createGraphics();
        overlayGraphics.setFont(new Font("SansSerif", Font.PLAIN, 14));
        overlayGraphics.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        overlayPixels = new int[OVERLAY_WIDTH * OVERLAY_HEIGHT];
        overlayBuffer = BufferUtils.createByteBuffer(OVERLAY_WIDTH * OVERLAY_HEIGHT * 4);

        overlayTextureID = glGenTextures();
        glBindTexture(GL_TEXTURE_2D, overlayTextureID);
        glTexImage2D(
                GL_TEXTURE_2D, 0,
                GL_RGBA8,
                OVERLAY_WIDTH,
                OVERLAY_HEIGHT,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    private void loop() {

        GL.createCapabilities();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            paint();

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }

    private void paint() {

        int fps = fpsCounter.update();
        System.out.println("FPS: " + fps);

        pixelBuffer.clear();

        if (modo == 1) {
            fractalCpu.julia_serial_2(FractalParams.xMin, FractalParams.yMin, FractalParams.xMax, FractalParams.yMax,
                    FractalParams.WIDTH, FractalParams.HEIGHT);
            pixelBuffer.put(fractalCpu.pixel_buffer);
        } else if (modo == 2) {
            fractalSimd.juliaSimd();
            pixelBuffer.put(fractalSimd.pixelBuffer.asIntBuffer());
        }

        pixelBuffer.flip();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0,
                GL_RGBA8,
                FractalParams.WIDTH,
                FractalParams.HEIGHT,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                pixelBuffer);

        glBegin(GL_QUADS);
        {
            glTexCoord2d(0.0f, 0.0f);
            glVertex2d(-1, -1);

            glTexCoord2d(0.0f, 1f);
            glVertex2d(-1, 1);

            glTexCoord2d(1f, 1f);
            glVertex2d(1, 1);

            glTexCoord2d(1f, 0f);
            glVertex2d(1, -1);
        }
        glEnd();

        updateOverlay(fps);
        drawOverlay();

    }

    private void updateOverlay(int fps) {
        overlayGraphics.setComposite(AlphaComposite.Src);
        overlayGraphics.setColor(new Color(0, 0, 0, 160));
        overlayGraphics.fillRect(0, 0, OVERLAY_WIDTH, OVERLAY_HEIGHT);

        overlayGraphics.setColor(Color.WHITE);
        overlayGraphics.drawString("FPS: " + fps, 10, 20);
        overlayGraphics.drawString("[1] CPU  [2] SIMD", 10, 40);
        overlayGraphics.drawString("UP/DOWN: iteraciones", 10, 60);
        overlayGraphics.drawString("ESC: salir", 10, 80);

        overlayImage.getRGB(0, 0, OVERLAY_WIDTH, OVERLAY_HEIGHT, overlayPixels, 0, OVERLAY_WIDTH);
        overlayBuffer.clear();
        for (int i = 0; i < overlayPixels.length; i++) {
            int pixel = overlayPixels[i];
            overlayBuffer.put((byte) ((pixel >> 16) & 0xFF));
            overlayBuffer.put((byte) ((pixel >> 8) & 0xFF));
            overlayBuffer.put((byte) (pixel & 0xFF));
            overlayBuffer.put((byte) ((pixel >> 24) & 0xFF));
        }
        overlayBuffer.flip();

        glBindTexture(GL_TEXTURE_2D, overlayTextureID);
        glTexSubImage2D(
                GL_TEXTURE_2D, 0,
                0, 0,
                OVERLAY_WIDTH,
                OVERLAY_HEIGHT,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                overlayBuffer);
    }

    private void drawOverlay() {
        float left = -1.0f + 2.0f * OVERLAY_MARGIN / FractalParams.WIDTH;
        float right = -1.0f + 2.0f * (OVERLAY_MARGIN + OVERLAY_WIDTH) / FractalParams.WIDTH;
        float top = 1.0f - 2.0f * OVERLAY_MARGIN / FractalParams.HEIGHT;
        float bottom = 1.0f - 2.0f * (OVERLAY_MARGIN + OVERLAY_HEIGHT) / FractalParams.HEIGHT;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, overlayTextureID);
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(left, top);

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(left, bottom);

            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(right, bottom);

            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(right, top);
        }
        glEnd();

        glDisable(GL_BLEND);
    }

    public static void main(String[] args) {
        new FractalMain().run();
    }
}
