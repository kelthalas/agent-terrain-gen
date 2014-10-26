#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

class GameWidget;
class QOpenGLVertexArrayObject;
class QOpenGLShaderProgram;
class QOpenGLTexture;

/**
 * @brief La classe permettant de charger et afficher les heightmap.
 */
class HeightMap
{
public:
    /**
     * @brief Constructeur qui crée une heightmap aléatoire.
     */
    HeightMap(int size);
    /**
     * @brief Constructeur qui charge une heightmap à partir du disque.
     * @param image L'image à charger.
     */
    HeightMap(QImage image);
    ~HeightMap();

    /**
     * @brief Initialise la heightmap.
     */
    void initialize(GameWidget* gl);
    /**
     * @brief Dessine la heightmap
     * @param viewProj La matrice view-Proj.
     */
    void render(GameWidget* gl, const QMatrix4x4& viewProj);
    void destroy(GameWidget* gl);

    /**
     * @brief Renvoie la hauteur du sommet (x, z) de la heightmap.
     */
    float get(int x, int z);
    /**
     * @brief Modifie la heuteur du sommet (x, z) de la heightmap.
     * @param height La nouvelle hauteur du sommet.
     */
    void set(int x, int z, float height);
    /**
     * @brief Renvoie la hauteur de la heightmap pour un certain point dans l'espace.
     * @param pos Le point pour lequel on veut la hauteur.
     */
    float getHeight(QVector3D pos);
    /**
     * @brief Renvoie la taille de la Heightmap.
     */
    int getSize() const;
    /**
     * @brief Renvoie la rotation de la heightmap (autour de l'axe Y).
     */
    float getRotation() const;
    /**
     * @brief Modifie la rotation de la heightmap (autour de l'axe Y).
     * @param rotation La nouvelle rotation.
     */
    void setRotation(float rotation);

    QMatrix4x4 getTranform();

private:
    // Le shader pour render la heightmap
    QOpenGLShaderProgram* m_program;
    // Le vertex array object pour rendre la heightmap
    QOpenGLVertexArrayObject* m_vao;

    QOpenGLTexture* m_waterTexture;
    QOpenGLTexture* m_sandTexture;
    QOpenGLTexture* m_grassTexture;
    QOpenGLTexture* m_rockTexture;
    QOpenGLTexture* m_snowTexture;

    GLuint m_posAttr;
    // L'identifiant de l'"uniform" de la matrice dans le shader
    GLuint m_matrixUniform;
    // L'identifiant du buffer de sommets
    GLuint m_vertexBuffer;
    // L'identifiant du buffer d'indices
    GLuint m_indexBuffer;

    // Le tableau de sommets
    GLfloat* m_vertices;
    // Le tableau d'indices
    GLuint* m_indices;

    // La taille de la heightmap
    int m_nbPoints;
    // La rotation sur l'axe y
    float m_rotation;
};

#endif // HEIGHTMAP_H