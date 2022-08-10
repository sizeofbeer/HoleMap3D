#pragma once
#include "abcg.hpp"
#include "pointpainter.h"

namespace HoleMap {

    class LinePainter {
    public:
        LinePainter();
        ~LinePainter();

        struct Data {
            Data() {}
            Data(const PointPainter::Data& p1, const PointPainter::Data& p2)
                : point1(p1), point2(p2) {}

            PointPainter::Data point1;
            PointPainter::Data point2;
        };

        void Setup(GLuint program);
        void Upload(const std::vector<LinePainter::Data>& data);

        void Render(const glm::mat4x4& pmv_matrix, const float line_width);
        inline int getNumGeoms();
    private:
        GLuint shader_program_;
        GLuint vao_;
        GLuint vbo_;

        size_t num_geoms_;
    };

    inline int LinePainter::getNumGeoms() {
        return static_cast<int>(num_geoms_);
    }
}