#include "pointpainter.h"

namespace HoleMap {

    PointPainter::PointPainter() : num_geoms_(0) {}

    PointPainter::~PointPainter() {
        glDeleteBuffers(1, &vbo_);
        glDeleteVertexArrays(1, &vao_);
    }

    void PointPainter::Setup(GLuint program) {
        shader_program_ = program;
        glDeleteBuffers(1, &vbo_);
        glDeleteVertexArrays(1, &vao_);
        // Create VAO and VBO
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
    }

    void PointPainter::Upload(const std::vector<PointPainter::Data>& data) {
        num_geoms_ = data.size();
        if (num_geoms_ == 0) {
            return;
        }

        // Bind VBO in order to use it
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        // Upload data to VBO
        glBufferData(GL_ARRAY_BUFFER,
                    static_cast<int>(data.size() * sizeof(PointPainter::Data)),
                    data.data(), GL_STATIC_DRAW);
        // Unbinding the VBO is allowed (data can be released now)
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Get location of attributes in the program
        GLint positionAttribute{glGetAttribLocation(shader_program_, "inPosition")};
        GLint colorAttribute{glGetAttribLocation(shader_program_, "inColor")};

        // Bind vertex attributes to current VAO
        glBindVertexArray(vao_);

        glEnableVertexAttribArray(positionAttribute);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(PointPainter::Data), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(colorAttribute);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE,
                                sizeof(PointPainter::Data),
                                reinterpret_cast<void*>(sizeof(glm::vec3)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // End of binding to current VAO
        glBindVertexArray(0);
    }

    void PointPainter::Render(const glm::mat4x4& pmv_matrix,
                            const float point_size) {
        if (num_geoms_ == 0) {
            return;
        }

        glBindVertexArray(vao_);
        glUseProgram(shader_program_);

        // Get location of uniform variables
        GLint pmvMatrixLoc{glGetUniformLocation(shader_program_, "pmvMatrix")};
        // Set uniform variables used by every scene object
        glUniformMatrix4fv(pmvMatrixLoc, 1, GL_FALSE, &pmv_matrix[0][0]);
        glPointSize(point_size);
        glDrawArrays(GL_POINTS, 0, (GLsizei)num_geoms_);

        // Make sure the VAO is not changed from the outside
        glBindVertexArray(0);
    }
}