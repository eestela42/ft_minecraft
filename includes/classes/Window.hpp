#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assert.h>

#include <interfaces/Interface_Input.hpp>

#define DEFAULT_WINDOW_WIDTH 1920
#define DEFAULT_WINDOW_HEIGHT 1080

enum class DrawMode {
    FILL,
    LINE
};

class Window : I_Input
{
    public:
        Window(const char *name, DrawMode drawMode);
        virtual ~Window();

        GLFWwindow *GetWindow() const;
        int ShouldContinue();
        void Clear();
        void SwapBuffersAndPollEvents();
		void SendKeys(u_char *keyState, double mouseMoveX, double mouseMoveY) override;
    private:
        GLFWwindow *window;
};

#endif