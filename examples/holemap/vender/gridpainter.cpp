#include "gridpainter.h"
#include "params.h"

namespace HoleMap {

    float DegToRad(const float deg)
    {
        return deg * 0.0174532925199432954743716805978692718781530857086181640625f;
    }

    float ZoomScale(const float kFieldOfView, float focusDistance) 
    {
        // "Constant" scale factor w.r.t. zoom-level.
        return 2.0f * std::tan(static_cast<float>(DegToRad(kFieldOfView)) / 2.0f) *
                                                        std::abs(focusDistance) / 10000;
    }

    GridPainter::GridPainter() {}

    GridPainter::~GridPainter()
    {
        coordinate_grid_painter_.~LinePainter();
        coordinate_axes_painter_.~LinePainter();
    }

    void GridPainter::Setup(GLuint program)
    {
        coordinate_grid_painter_.Setup(program);
        coordinate_axes_painter_.Setup(program);
    }

    void GridPainter::Upload()
    {
        const float scale = HoleMap::ZoomScale(kFieldOfView, 30);

        // View center grid.
        {
            std::vector<HoleMap::LinePainter::Data> grid_data(3);

            grid_data[0].point1 =
                HoleMap::PointPainter::Data(-20 * scale, 0, 0, kGridColor[0], kGridColor[1],
                                    kGridColor[2], kGridColor[3]);
            grid_data[0].point2 =
                HoleMap::PointPainter::Data(20 * scale, 0, 0, kGridColor[0], kGridColor[1],
                                    kGridColor[2], kGridColor[3]);

            grid_data[1].point1 =
                HoleMap::PointPainter::Data(0, -20 * scale, 0, kGridColor[0], kGridColor[1],
                                    kGridColor[2], kGridColor[3]);
            grid_data[1].point2 =
                HoleMap::PointPainter::Data(0, 20 * scale, 0, kGridColor[0], kGridColor[1],
                                    kGridColor[2], kGridColor[3]);

            grid_data[2].point1 =
                HoleMap::PointPainter::Data(0, 0, -20 * scale, kGridColor[0], kGridColor[1],
                                    kGridColor[2], kGridColor[3]);
            grid_data[2].point2 =
                HoleMap::PointPainter::Data(0, 0, 20 * scale, kGridColor[0], kGridColor[1],
                                    kGridColor[2], kGridColor[3]);

            coordinate_grid_painter_.Upload(grid_data);
        }
        
        // Coordinate axes.
        {
            std::vector<HoleMap::LinePainter::Data> axes_data(3);

            axes_data[0].point1 = HoleMap::PointPainter::Data(
                0, 0, 0, kXAxisColor[0], kXAxisColor[1], kXAxisColor[2], kXAxisColor[3]);
            axes_data[0].point2 =
                HoleMap::PointPainter::Data(50 * scale, 0, 0, kXAxisColor[0], kXAxisColor[1],
                                    kXAxisColor[2], kXAxisColor[3]);

            axes_data[1].point1 = HoleMap::PointPainter::Data(
                0, 0, 0, kYAxisColor[0], kYAxisColor[1], kYAxisColor[2], kYAxisColor[3]);
            axes_data[1].point2 =
                HoleMap::PointPainter::Data(0, 50 * scale, 0, kYAxisColor[0], kYAxisColor[1],
                                    kYAxisColor[2], kYAxisColor[3]);

            axes_data[2].point1 = HoleMap::PointPainter::Data(
                0, 0, 0, kZAxisColor[0], kZAxisColor[1], kZAxisColor[2], kZAxisColor[3]);
            axes_data[2].point2 =
                HoleMap::PointPainter::Data(0, 0, 50 * scale, kZAxisColor[0], kZAxisColor[1],
                                    kZAxisColor[2], kZAxisColor[3]);

            coordinate_axes_painter_.Upload(axes_data);
        }  
    }

    void GridPainter::Render(const glm::mat4x4& pmv_matrix,
                            const float line_width)
    {
        coordinate_grid_painter_.Render(pmv_matrix, line_width);
        coordinate_axes_painter_.Render(pmv_matrix, line_width);
    }
}