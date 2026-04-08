#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

#include "engine.h"

void Engine::load_file(const std::string& filename)
{
    std::ifstream infile(filename);

    if (!infile.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    vectors.clear();
    original_vectors.clear();
    faces.clear();

    std::string line;
    while (std::getline(infile, line))
    {
        // Skip empty lines
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v")
        {
            float x, y, z;
            ss >> x >> y >> z;
            vectors.push_back({x, y, z});
            changed_vectors.push_back({x, y, z});
            original_vectors.push_back({x, y, z});
        }
        else if (type == "f")
        {
            std::vector<int> face;
            int val;
            while (ss >> val)
            {
                face.push_back(val);
            }
            faces.push_back(face);
        }
    }

    infile.close();
}

void Engine::load_camera_file(const std::string& filename)
{
    std::ifstream cam_file(filename);

    if (!cam_file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    cameras.clear();
    
    Camera curr;
    bool has_camera = false;
    
    std::string line;
    while (std::getline(cam_file, line))
    {
        // Skip empty lines
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "c")
        {
            if (has_camera) 
                cameras.push_back(curr);
            curr = Camera();
            has_camera = true;
        }
        else if (type == "i")
        {
            std::getline(ss, curr.name);
            if (!curr.name.empty() && curr.name[0] == ' ')
                curr.name.erase(0, 1);
        }
        else if (type == "t")
        {
            ss >> curr.type;
        }
        else if (type == "r")
        {
            ss >> curr.vrp[0] >> curr.vrp[1] >> curr.vrp[2];
        }
        else if (type == "n")
        {
            ss >> curr.vpn[0] >> curr.vpn[1] >> curr.vpn[2];
        }
        else if (type == "u") 
        {
            ss >> curr.vup[0] >> curr.vup[1] >> curr.vup[2];
        }
        else if (type == "p") 
        {
            ss >> curr.prp[0] >> curr.prp[1] >> curr.prp[2];
        }
        else if (type == "w") 
        {
            for (int i = 0; i < 6; i++)
                ss >> curr.view_volume[i];
        }
        else if (type == "s")
        {
            for (int i = 0; i < 4; i++)
                ss >> curr.viewport[i];
        }
    }

    if (has_camera)
        cameras.push_back(curr);
}

std::vector<Line> Engine::get_lines(int width, int height, int index)
{
    Camera& cam = cameras[index];

    vectors = changed_vectors;
    
    if (cam.type == "parallel")
        parallel_projection(cam);
    else if (cam.type == "perspective")
        perspective_projection(cam);
    
    std::vector<Line> lines;
    auto process_edge = [&](Vec3 a, Vec3 b)
    {
        auto p1 = window_to_viewport(a.x, a.y, width, height, cam);
        auto p2 = window_to_viewport(b.x, b.y, width, height, cam);

        Line c = clip(p1, p2, width, height, cam);

        lines.push_back(c);
    };

    for (const auto& face : faces)
    {
        if (face.size() == 3)
        {
            // Face is defined by 3 vertices (k, l, m), referenced by their index
            int v1_index = face[0];
            int v2_index = face[1];
            int v3_index = face[2];

            // Get the corresponding vertices (ignore z-axis)
            Vec3 v1 = vectors[(v1_index - 1)];
            Vec3 v2 = vectors[(v2_index - 1)];
            Vec3 v3 = vectors[(v3_index - 1)];

            process_edge(v1, v2);
            process_edge(v2, v3);
            process_edge(v3, v1);
        }
        else if (face.size() == 4)
        {
            // Face is defined by 3 vertices (k, l, m), referenced by their index
            int v1_index = face[0];
            int v2_index = face[1];
            int v3_index = face[2];
            int v4_index = face[3];

            // Get the corresponding vertices (ignore z-axis)
            Vec3 v1 = vectors[(v1_index - 1)];
            Vec3 v2 = vectors[(v2_index - 1)];
            Vec3 v3 = vectors[(v3_index - 1)];
            Vec3 v4 = vectors[(v4_index - 1)];

            process_edge(v1, v2);
            process_edge(v2, v3);
            process_edge(v3, v4);
            process_edge(v4, v1);
        }
    }

    return lines;
}

void Engine::parallel_projection(Camera& cam)
{
    // Translate VRP to origin (0,0,0)
    Vec3 vrp;
    vrp.x = cam.vrp[0], vrp.y = cam.vrp[1], vrp.z = cam.vrp[2];
    Mat4 translation_mat = trans_m(-vrp.x, -vrp.y, -vrp.z);
    vrp.x = 0, vrp.y = 0, vrp.z = 0;
    
    // Rotate VPN around x until it lies in the xz plane with positive z
    Vec3 vpn;
    vpn.x = cam.vpn[0], vpn.y = cam.vpn[1], vpn.z = cam.vpn[2];
    float denom = sqrt(vpn.y * vpn.y + vpn.z * vpn.z);
    float x_cos;
    float x_sin;
    (denom != 0) ? x_cos = vpn.z / denom : x_cos = 1;
    (denom != 0) ? x_sin = vpn.y / denom : x_sin = 0;
    
    Mat4 R_x;
    R_x.m[1][1] = x_cos;
    R_x.m[1][2] = -x_sin;
    R_x.m[2][1] = x_sin;
    R_x.m[2][2] = x_cos;
    
    vpn.y = 0;
    vpn.z = denom;

    // Rotate VPN around y until it aligns with the positive z axis
    denom = sqrt(vpn.x * vpn.x + vpn.z * vpn.z);
    float y_cos = vpn.z / denom;
    float y_sin = vpn.x / denom;

    Mat4 R_y;
    R_y.m[0][0] = y_cos;
    R_y.m[0][2] = -y_sin;
    R_y.m[2][0] = y_sin;
    R_y.m[2][2] = y_cos;

    vpn.x = 0;
    vpn.z = denom;

    // Rotate VUP around z until it lies in the yz plane with positive y
    Vec3 vup;
    vup.x = cam.vup[0], vup.y = cam.vup[1], vup.z = cam.vup[2];
    vup = mxv(R_x, vup);
    vup = mxv(R_y, vup);
    
    denom = sqrt(vup.x * vup.x + vup.y * vup.y);
    float z_cos = vup.y / denom;
    float z_sin = vup.x / denom;
    
    Mat4 R_z;
    R_z.m[0][0] = z_cos;
    R_z.m[0][1] = -z_sin;
    R_z.m[1][0] = z_sin;
    R_z.m[1][1] = z_cos;

    vup.x = 0, vup.y = denom;

    // Shear DOP such that it aligns with vpn
    Vec3 prp;
    prp.x = cam.prp[0], prp.y = cam.prp[1], prp.z = cam.prp[2];
    float umin = cam.view_volume[0];
    float umax = cam.view_volume[1];
    float vmin = cam.view_volume[2];
    float vmax = cam.view_volume[3];
    float wmin = cam.view_volume[4];
    float wmax = cam.view_volume[5];

    float cw_u = (umax + umin) / 2.0f;
    float cw_v = (vmax + vmin) / 2.0f;

    float shear_x = -(prp.x - cw_u) / prp.z;
    float shear_y = -(prp.y - cw_v) / prp.z;
    Mat4 shear_mat;
    shear_mat.m[0][2] = shear_x;
    shear_mat.m[1][2] = shear_y;

    // Translate the lower corner of the view volume to the origin
    float dx = -(umin + umax) / 2;
    float dy = -(vmin + vmax) / 2;
    float dz;
    (wmax > wmin) ? dz = -wmin : dz = -wmax;

    Mat4 view_matrix = trans_m(dx, dy, dz);

    // Scale such that the view volume becomes a unit cube
    float s_x;
    float s_y;
    float s_z;

    (umax > umin) ? s_x = 2 / (umax - umin) : s_x = 2 / (umin - umax);
    (vmax > vmin) ? s_y = 2 / (vmax - vmin) : s_y = 2 / (vmin - vmax);
    (wmax > wmin) ? s_z = 1 / (wmax - wmin) : s_z = 1 / (wmin - wmax);

    Mat4 scale_mat = scale_m(s_x, s_y, s_z);

    // Combine matrices
    Mat4 mat = mxm(mxm(mxm(scale_mat, view_matrix), shear_mat), mxm(mxm(R_z, R_y), mxm(R_x, translation_mat)));
    for (auto& v : vectors)
    {
        v = mxv(mat, v);
    }
}

void Engine::perspective_projection(Camera& cam)
{
    // Translate VRP to origin (0,0,0)
    Vec3 vrp;
    vrp.x = cam.vrp[0], vrp.y = cam.vrp[1], vrp.z = cam.vrp[2];
    Mat4 translation_matrix = trans_m(-vrp.x, -vrp.y, -vrp.z);
    vrp.x = 0, vrp.y = 0, vrp.z = 0;
    
    // Rotate VPN around x until it lies in the xz plane with positive z
    Vec3 vpn;
    vpn.x = cam.vpn[0], vpn.y = cam.vpn[1], vpn.z = cam.vpn[2];
    float denom = sqrt(vpn.y * vpn.y + vpn.z * vpn.z);
    float x_cos;
    float x_sin;
    (denom != 0) ? x_cos = vpn.z / denom : x_cos = 1;
    (denom != 0) ? x_sin = vpn.y / denom : x_sin = 0;
    
    Mat4 R_x;
    R_x.m[1][1] = x_cos;
    R_x.m[1][2] = -x_sin;
    R_x.m[2][1] = x_sin;
    R_x.m[2][2] = x_cos;
    
    vpn.y = 0;
    vpn.z = denom;

    // Rotate VPN around y until it aligns with the positive z axis
    denom = sqrt(vpn.x * vpn.x + vpn.z * vpn.z);
    float y_cos = vpn.z / denom;
    float y_sin = vpn.x / denom;

    Mat4 R_y;
    R_y.m[0][0] = y_cos;
    R_y.m[0][2] = -y_sin;
    R_y.m[2][0] = y_sin;
    R_y.m[2][2] = y_cos;

    vpn.x = 0;
    vpn.z = denom;

    // Rotate VUP around z until it lies in the yz plane with positive y
    Vec3 vup;
    vup.x = cam.vup[0], vup.y = cam.vup[1], vup.z = cam.vup[2];
    vup = mxv(R_x, vup);
    vup = mxv(R_y, vup);
    
    denom = sqrt(vup.x * vup.x + vup.y * vup.y);
    float z_cos = vup.y / denom;
    float z_sin = vup.x / denom;
    
    Mat4 R_z;
    R_z.m[0][0] = z_cos;
    R_z.m[0][1] = -z_sin;
    R_z.m[1][0] = z_sin;
    R_z.m[1][1] = z_cos;

    vup.x = 0, vup.y = denom;

    // Translate PRP to origin
    Vec3 prp;
    prp.x = cam.prp[0], prp.y = cam.prp[1], prp.z = cam.prp[2];
    Mat4 prp_translation_matrix = trans_m(-prp.x, -prp.y, -prp.z);
    
    vrp = mxv(prp_translation_matrix, vrp);

    // Shear such that the center line of the view volume becomes the z axis
    float umin = cam.view_volume[0];
    float umax = cam.view_volume[1];
    float vmin = cam.view_volume[2];
    float vmax = cam.view_volume[3];
    float wmin = cam.view_volume[4];
    float wmax = cam.view_volume[5];

    float cw_u = (umax + umin) / 2.0f;
    float cw_v = (vmax + vmin) / 2.0f;

    float shear_x = (-(prp.x - cw_u)) / prp.z;
    float shear_y = (-(prp.y - cw_v)) / prp.z;
    
    Mat4 shear_matrix;
    shear_matrix.m[0][2] = shear_x;
    shear_matrix.m[1][2] = shear_y;
    
    vrp = mxv(shear_matrix, vrp);

    // Scale such that the the sides of the view volume become 45 degrees and view volume becomes the canonical perspective volume
    float s_x;
    float s_y;
    float s_z;
    
    denom = ((umax - umin) / 2) * (vrp.z + wmin);
    if (std::abs(vrp.z + wmax) > std::abs(vrp.z + wmin))
    {    
        denom = ((umax - umin) / 2) * (vrp.z + wmax);  
        s_x = std::abs(vrp.z / denom);
    }
    
    denom = ((vmax - vmin) / 2) * (vrp.z + wmin);
    if (std::abs(vrp.z + wmax) > std::abs(vrp.z + wmin))
    {    
        denom = ((vmax - vmin) / 2) * (vrp.z + wmax);
        s_y = std::abs(vrp.z / denom);
    }

    denom = vrp.z + wmin;
    if (std::abs(vrp.z + wmax) > std::abs(vrp.z + wmin))
    {   
        denom = vrp.z + wmax;
        s_z = 1 / denom;
    }

    Mat4 scale_matrix = scale_m(s_x, s_y, s_z);

    // Combine matrices
    Mat4 mat = mxm(mxm(mxm(scale_matrix, shear_matrix), prp_translation_matrix), mxm(mxm(R_z, R_y), mxm(R_x, translation_matrix)));
    for (auto& v : vectors)
    {
        v = mxv(mat, v);

        if (v.z != 0)
        {
            v.x /= v.z;
            v.y /= v.z;
        }
    }
}

void Engine::rotate(float degrees, char axis)
{
    float theta = degrees * (3.14159265f / 180.0f);

    Mat4 R;

    if (axis == 'X')      R = rot_x(theta);
    else if (axis == 'Y') R = rot_y(theta);
    else if (axis == 'Z') R = rot_z(theta);
    else return;

    for (auto& v : changed_vectors)
        v = mxv(R, v);
}

void Engine::rotate_axis(float degrees, float ax, float ay, float az, float bx, float by, float bz)
{
    float theta = degrees * (3.14159265f / 180.0f);

    // Direction (B - A)
    float ux = bx - ax;
    float uy = by - ay;
    float uz = bz - az;

    float length = sqrt(ux*ux + uy*uy + uz*uz);
    if (length == 0) return;

    ux /= length;
    uy /= length;
    uz /= length;

    // Build matrices
    Mat4 T1 = trans_m(-ax, -ay, -az);
    Mat4 R  = rot_axis(theta, ux, uy, uz);
    Mat4 T2 = trans_m(ax, ay, az);

    Mat4 final = mxm(T2, mxm(R, T1));

    for (auto& v : changed_vectors)
        v = mxv(final, v);
    
}

void Engine::scale(float px, float py, float pz, float sx, float sy, float sz)
{
    Mat4 T1 = trans_m(-px, -py, -pz);
    Mat4 S  = scale_m(sx, sy, sz);
    Mat4 T2 = trans_m(px, py, pz);

    Mat4 mat = mxm(T2, mxm(S, T1));
    for (auto& v : changed_vectors)
    {
        v = mxv(mat, v);
    }
}

void Engine::translate(float dx, float dy, float dz)
{
    Mat4 T = trans_m(dx, dy, dz);

    for (auto& v : changed_vectors)
    {
        v = mxv(T, v);
    }
}

std::pair<float, float> Engine::window_to_viewport(float x, float y, int width, int height, const Camera& cam)
{
    float umin = -1;
    float umax = 1;
    float vmin = -1;
    float vmax = 1;

    float xmin = cam.viewport[0];
    float ymin = cam.viewport[1];
    float xmax = cam.viewport[2];
    float ymax = cam.viewport[3];
    
    float s_x = (xmax - xmin) / (umax - umin);
    float s_y = (ymax - ymin) / (vmax - vmin);

    float d_x = x - umin;
    float d_y = vmax - y;

    float canvas_x = ((s_x * d_x) + xmin) * width;
    float canvas_y = ((s_y * d_y) + ymin) * height;

    return {canvas_x, canvas_y};
}

Line Engine::clip(auto& p1, auto& p2, int width, int height, const Camera& cam)
{
    float clip_xmin = cam.viewport[0] * width;
    float clip_xmax = cam.viewport[2] * width;
    float clip_ymin = cam.viewport[1] * height;
    float clip_ymax = cam.viewport[3] * height;

    auto clip_point = [&](float inX, float inY) -> std::pair<float, float> 
    {
       float outX = std::max(clip_xmin, std::min(clip_xmax, inX));
       float outY = std::max(clip_ymin, std::min(clip_ymax, inY));
       return {outX, outY};
    };

    auto [x1, y1] = clip_point(p1.first, p1.second); // Structured binding (C++17)
    auto [x2, y2] = clip_point(p2.first, p2.second);

    // Return a flat vector of 4 floats
    return {x1, y1, x2, y2};
}   

