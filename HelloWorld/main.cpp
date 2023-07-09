#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello OpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    // Successfully loaded OpenGL
    std::cout << "Loaded OpenGL: " << GLVersion.major << "." << GLVersion.minor << std::endl;


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glViewport(0, 0, 800, 600);

    // Query vertex attribute
    int attributeNumber = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attributeNumber);

    std::cout << "Maximum number of vertex attributes supported: " << attributeNumber << std::endl;

    // vertex shader
    const char *K_VERTEX_SHADER_SOURCE = "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "\n"
        "//out vec4 vertexColor;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "    //vertexColor = vec4(0.5, 0, 0, 1.0);\n"
        "}\n";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &K_VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertexShader);

    int success = 0;
    char infoLog[512] = { 0 };
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "glCompileShader() vertex failed: " << infoLog << std::endl;
        return -1;
    }

    // fragment shader
    const char* K_FRAGMENT_SHADER_SOURCE = "#version 460 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "//in vec4 vertexColor;\n"
        "uniform vec4 ourColor;"
        "void main() {\n"
        "    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "    //FragColor = vertexColor;\n"
        "    FragColor = ourColor;\n"
        "}\n";

    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &K_FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "glCompileShader() frag failed: " << infoLog << std::endl;
        return -1;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "glLinkProgram() failed: " << infoLog << std::endl;
        return -1;
    }

    // shaders can be delete now
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    // Vertices
    float vertices[] = {
         0.5f,  0.5f, 0.0f, // right-top
         0.5f, -0.5f, 0.0f, // right-bottom
        -0.5f, -0.5f, 0.0f, // left-bottom
        -0.5f,  0.5f, 0.0f  // left-top
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangel
    };

    // Create vertex buffer object
    unsigned int vbo;
    glGenBuffers(1, &vbo);

    // Create vertex array object
    unsigned int vao;
    glGenVertexArrays(1, &vao);

    // Create element array buffer
    unsigned int ebo;
    glGenBuffers(1, &ebo);

    // 1. Bind VAO
    glBindVertexArray(vao);

    // 2. Copy vertices to VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. Copy indices to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 4. Set vertext attibutes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // 5. vbo has been registered as vertex attribute's bound vertex buffer object, so it can be unbound here
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 6. vao can be unbound in case modify accidentally
    glBindVertexArray(0);

    // Line or Fille
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Get uniform ourColor position
    int ourColorPosition = glGetUniformLocation(shaderProgram, "ourColor");

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw triangle
        glUseProgram(shaderProgram);

        // Set uniform ourColor
        if (ourColorPosition >= 0) {
            float timeValue = glfwGetTime();
            float greenValue = sin(timeValue) / 2.0f + 0.5f;
            glUniform4f(ourColorPosition, 0.0f, greenValue, 0.0f, 1.0f);
        }

        glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Free objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}