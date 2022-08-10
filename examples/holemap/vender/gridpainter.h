#pragma once
#include "abcg.hpp"

#include "pointpainter.h"
#include "linepainter.h"

namespace HoleMap {

    float DegToRad(const float deg);
    float ZoomScale(const float kFieldOfView, float focusDistance);

    class GridPainter {
    public:
        GridPainter();
        ~GridPainter();

        void Setup(GLuint program);

        void Upload();

        void Render(const glm::mat4x4& pmv_matrix, const float line_width);

        inline int getNumGeoms();

    private:
        LinePainter coordinate_grid_painter_;
        LinePainter coordinate_axes_painter_;
    };

    inline int GridPainter::getNumGeoms() {
        return coordinate_grid_painter_.getNumGeoms();
    }
}