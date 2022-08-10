#pragma once
#include "abcg.hpp"

struct Vertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texCoord{};
    glm::vec4 tangent{};

    bool operator==(const Vertex& other) const noexcept {
        static const auto epsilon{std::numeric_limits<float>::epsilon()};
        return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
            glm::all(glm::epsilonEqual(normal, other.normal, epsilon)) &&
            glm::all(glm::epsilonEqual(texCoord, other.texCoord, epsilon));
    }
};

namespace HoleMap {

    class MeshPainter {
    public:
        MeshPainter();
        ~MeshPainter();

        void Setup(GLuint program);

        void Upload(std::filesystem::path path);

        void Render(glm::mat4x4 model_matrix,
                            glm::mat4x4 view_matrix, glm::mat4x4 proj_matrix);

        void UploadDiffuseTexture(std::string_view path);
        void UploadNormalTexture(std::string_view path);
        void UploadFromFile(std::string_view path, bool standardize = true);

        [[nodiscard]] int getNumTriangles() const {
            return static_cast<int>(m_indices.size()) / 3;
        }

        [[nodiscard]] glm::vec4 getKa() const { return m_Ka; }
        [[nodiscard]] glm::vec4 getKd() const { return m_Kd; }
        [[nodiscard]] glm::vec4 getKs() const { return m_Ks; }
        [[nodiscard]] float getShininess() const { return m_shininess; }
        [[nodiscard]] bool isUVMapped() const { return m_hasTexCoords; }

    private:
        GLuint shader_program_;

        GLuint m_VAO{};
        GLuint m_VBO{};
        GLuint m_EBO{};

        glm::vec4 m_Ka;
        glm::vec4 m_Kd;
        glm::vec4 m_Ks;
        float m_shininess;

        glm::vec4 m_Ia{1.0f};
        glm::vec4 m_Id{1.0f};
        glm::vec4 m_Is{0.25f};

        GLuint m_diffuseTexture{};
        GLuint m_normalTexture{};

        std::vector<Vertex> m_vertices;
        std::vector<GLuint> m_indices;

        bool m_hasNormals{false};
        bool m_hasTexCoords{false};

        void ComputeNormals();
        void ComputeTangents();
        void standardize();
    };
}