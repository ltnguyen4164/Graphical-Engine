#include "engine.h"

static Engine engine;

extern "C" {

    __declspec(dllexport)
    void load_file(const char* filename)
    {
        engine.load_file(filename);
    }

    __declspec(dllexport)
    int get_line_count(int width, int height, int index)
    {
        return engine.get_lines(width, height, index).size();
    }

    __declspec(dllexport)
    void get_lines(int width, int height, int index, Line* out_lines)
    {
        auto lines = engine.get_lines(width, height, index);
        for (size_t i = 0; i < lines.size(); i++)
        {
            out_lines[i] = lines[i];
        }
    }

    __declspec(dllexport)
    void rotate(float degrees, char axis)
    {
        engine.rotate(degrees, axis);
    }

    __declspec(dllexport)
    void rotate_axis(float degrees, float ax, float ay, float az, float bx, float by, float bz)
    {
        engine.rotate_axis(degrees, ax, ay, az, bx, by, bz);
    }

    __declspec(dllexport)
    void scale(float x, float y, float z, float sx, float sy, float sz)
    {
        engine.scale(x, y, z, sx, sy, sz);
    }

    __declspec(dllexport)
    void translate(float dx, float dy, float dz)
    {
        engine.translate(dx, dy, dz);
    }

    __declspec(dllexport)
    void load_camera_file(const char* filename)
    {
        engine.load_camera_file(std::string(filename));
    }

    __declspec(dllexport)
    int get_camera_count()
    {
        return engine.get_camera_count();
    }

    __declspec(dllexport)
    void get_camera_viewport(int index, float* out)
    {
        const Camera& cam = engine.get_camera(index);
        for (int i = 0; i < 4; i++)
            out[i] = cam.viewport[i];
    }

    __declspec(dllexport)
    void get_camera_name(int index, char* buffer, int max_len)
    {
        const Camera& cam = engine.get_camera(index);
        std::strncpy(buffer, cam.name.c_str(), max_len);
    }
}


/*Compile with g++ -shared -o engine.dll src/renderer.cpp src/engine_api.cpp src/math.cpp -Iinclude -std=c++20*/