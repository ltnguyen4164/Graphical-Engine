#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <cstring>
#include <vector>

#include "math.h"

struct Line {
    float x1, y1, x2, y2;
};

struct Camera {
    std::string name = "";
    std::string type = "parallel";
    float vrp[3] = {0, 0, 0};
    float vpn[3] = {0, 0, 1};
    float vup[3] = {0, 1, 0};
    float prp[3] = {0, 0, 1};
    float view_volume[6] = {-1, 1, -1, 1, -1, 1};
    float viewport[4] = {0.1, 0.1, 0.4, 0.4};
};

class Engine {
    public:
        void load_file(const std::string& filename);
        void load_camera_file(const std::string& filename);
        std::vector<Line> get_lines(int width, int height, int index);
        
        // Transformation functions
        void rotate(float degrees, char axis);
        void rotate_axis(float degrees, float ax, float ay, float az, float bx, float by, float bz);
        void scale(float x, float y, float z, float sx, float sy, float sz);
        void translate(float dx, float dy, float dz);

        // Helper functions
        int get_camera_count() const
        {
            return cameras.size();
        }

        const Camera& get_camera(int index) const
        {
            return cameras[index];
        }

        void reset()
        {
            vectors = original_vectors;
        }
    
    private:
        std::vector<Vec3> vectors;
        std::vector<Vec3> changed_vectors;
        std::vector<Vec3> original_vectors;
        std::vector<std::vector<int>> faces;
        std::vector<float> window;
        std::vector<float> viewpoint;

        std::vector<Camera> cameras;
        
        // Projection functions
        void parallel_projection(Camera& cam);  
        void perspective_projection(Camera& cam);
        
        std::pair<float, float> window_to_viewport(float x, float y, int width, int height, const Camera& cam);
        Line clip(auto& p1, auto& p2, int width, int height, const Camera& cam);
};

#endif