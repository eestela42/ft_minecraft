// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <iostream>

// // Placeholder for your shader handler class
// class ShaderHandler {
// public:
//     GLuint computeProgram;

//     void Use(const std::string& name) {
//         if (name == "compute") {
//             glUseProgram(computeProgram);
//         }
//     }

//     // Assume you have methods to compile and link the compute shader
//     void LoadComputeShader(const char* source) {
//         computeProgram = glCreateProgram();
//         GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
//         glShaderSource(shader, 1, &source, nullptr);
//         glCompileShader(shader);
//         // Check compilation errors (simplified)
//         GLint success;
//         glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//         if (!success) {
//             char infoLog[512];
//             glGetShaderInfoLog(shader, 512, nullptr, infoLog);
//             std::cerr << "Compute shader compilation failed: " << infoLog << std::endl;
//         }
//         glAttachShader(computeProgram, shader);
//         glLinkProgram(computeProgram);
//         glDeleteShader(shader);
//         // Check linking errors (simplified)
//         glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
//         if (!success) {
//             char infoLog[512];
//             glGetProgramInfoLog(computeProgram, 512, nullptr, infoLog);
//             std::cerr << "Compute program linking failed: " << infoLog << std::endl;
//         }
//     }
// };

// // Placeholder for your window class
// class Window {
// public:
//     GLFWwindow* glfwWindow;
//     int width = 800; // DEFAULT_WINDOW_WIDTH
//     int height = 600; // DEFAULT_WINDOW_HEIGHT

//     void SwapBuffersAndPollEvents() {
//         glfwSwapBuffers(glfwWindow);
//         glfwPollEvents();
//     }
// };

// // Placeholder for drawEntity (assumed to render to the FBO)
// void drawEntity(GLuint fbo) {
//     glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//     // Your rendering code here (e.g., draw geometry, set shaders, etc.)
//     // Example: Clear to green to verify it’s overwritten by compute shader
//     glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
// }

// int main() {
//     // Initialize GLFW and create window
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     Window window;
//     window.glfwWindow = glfwCreateWindow(window.width, window.height, "Compute Shader", nullptr, nullptr);
//     glfwMakeContextCurrent(window.glfwWindow);

//     // Initialize GLAD
//     gladLoadGL();

//     // Load compute shader
//     ShaderHandler shaderHandler;
//     const char* computeSource = R"(
//         #version 460 core
//         layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
//         layout(rgba32f, binding = 0) uniform image2D outputImage;
//         void main() {
//             ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
//             vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
//             imageStore(outputImage, pixelCoord, color);
//         }
//     )";
//     shaderHandler.LoadComputeShader(computeSource);

//     // Create FBO with texture attachment
//     GLuint fbo, colorTexture;
//     glGenFramebuffers(1, &fbo);
//     glBindFramebuffer(GL_FRAMEBUFFER, fbo);

//     glGenTextures(1, &colorTexture);
//     glBindTexture(GL_TEXTURE_2D, colorTexture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window.width, window.height, 0, GL_RGBA, GL_FLOAT, nullptr);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

//     // Check FBO completeness
//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//         std::cerr << "Framebuffer is not complete!" << std::endl;
//         return -1;
//     }
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     // Main loop
//     while (!glfwWindowShouldClose(window.glfwWindow)) {
//         // Step 1: Render to FBO with drawEntity
//         drawEntity(fbo);

//         // Step 2: Run compute shader to overwrite FBO's color texture
//         shaderHandler.Use("compute");
//         glBindImageTexture(0, colorTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
//         glDispatchCompute((window.width + 15) / 16, (window.height + 15) / 16, 1);
//         glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

//         // Step 3: Copy FBO's texture to default framebuffer for display
//         glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
//         glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//         glBlitFramebuffer(0, 0, window.width, window.height, 0, 0, window.width, window.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//         glBindFramebuffer(GL_FRAMEBUFFER, 0);

//         // Step 4: Swap buffers to display
//         window.SwapBuffersAndPollEvents();

//         // Note: You can continue rendering to `fbo` in subsequent frames
//     }

//     // Cleanup
//     glDeleteTextures(1, &colorTexture);
//     glDeleteFramebuffers(1, &fbo);
//     glDeleteProgram(shaderHandler.computeProgram);
//     glfwDestroyWindow(window.glfwWindow);
//     glfwTerminate();
//     return 0;
// }