#pragma once
#include <vector>
#include "abcg.hpp"

namespace HoleMap {

    class PointPainter {
    public:
        PointPainter();
        ~PointPainter();

        struct Data {
            Data() : x(0), y(0), z(0), r(0), g(0), b(0), a(0) {}
            Data(const float x_, const float y_, const float z_, const float r_,
                const float g_, const float b_, const float a_)
                : x(x_), y(y_), z(z_), r(r_), g(g_), b(b_), a(a_) {}

            float x, y, z;
            float r, g, b, a;
        };

        void Setup(GLuint program);
        void Upload(const std::vector<PointPainter::Data>& data);
        void Render(const glm::mat4x4& pmv_matrix, const float point_size);

        inline int getNumGeoms();
    private:
        GLuint shader_program_;
        GLuint vao_;
        GLuint vbo_;

        size_t num_geoms_;
    };

    inline int PointPainter::getNumGeoms() {
        return static_cast<int>(num_geoms_);
    }
}