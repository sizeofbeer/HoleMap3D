#pragma once
#include "abcg.hpp"
#include "pointpainter.h"

namespace HoleMap {

class TrianglePainter {
    public:
        TrianglePainter();
        ~TrianglePainter();

        struct Data {
            Data() {}
            Data(const PointPainter::Data& p1, const PointPainter::Data& p2,
                const PointPainter::Data& p3)
                : point1(p1), point2(p2), point3(p3) {}

            PointPainter::Data point1;
            PointPainter::Data point2;
            PointPainter::Data point3;
        };

        void Setup(GLuint program);
        void Upload(const std::vector<TrianglePainter::Data>& data);
        void Render(const glm::mat4x4& pmv_matrix);

    private:
        GLuint shader_program_;
        GLuint vao_;
        GLuint vbo_;

        size_t num_geoms_;
    };
}