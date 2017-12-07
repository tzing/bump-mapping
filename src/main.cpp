#include <GL/glew.h>
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define MATERIAL_BY_FACE //HIGHLIGHT DO NOT REMOVE
#include <glm.h>

#include <cmath>
#include <cassert>
#include <string>
#include <sys/stat.h>
#include <array>
#include <vector>

#include "shader.hpp"
#include "glm_helper.hpp"
#include "interaction.hpp"

using namespace std;


/*
 *  types
 *
 */
typedef unsigned short ushort;

typedef array<float, 2> vec2;
typedef array<float, 3> vec3;


/*
 *  constants
 *
 */
constexpr char filename_object_descriptor[] = "resources/Ball.object";
constexpr char filename_texture[] = "resources/Tile.ppm";
constexpr char filename_normal_map[] = "resources/NormalMap.ppm";

constexpr char filename_shader_vert[] = "shaders/ball.vert";
constexpr char filename_shader_frag[] = "shaders/ball.frag";


/*
 *  prototypes
 *
 */
void init(void);
void display(void);


/*
 *  global variables
 *
 */
float eyet = 0.0;   //theta in degree
float eyep = 90.0;  //phi in degree
float eyex = 0.0;
float eyey = 0.0;
float eyez = 3.0;

bool mleft = false;
bool mright = false;
bool mmiddle = false;

bool forward = false;
bool backward = false;
bool left = false;
bool right = false;
bool up = false;
bool down = false;
bool lforward = false;
bool lbackward = false;
bool lleft = false;
bool lright = false;
bool lup = false;
bool ldown = false;

bool bforward = false;
bool bbackward = false;
bool bleft = false;
bool bright = false;
bool bup = false;
bool bdown = false;
bool bx = false;
bool by = false;
bool bz = false;
bool brx = false;
bool bry = false;
bool brz = false;

int mousex = 0;
int mousey = 0;

GLfloat light_pos[] = { 1, 1, 1 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };

GLuint use_bump = 0;

static GLuint _shader_idx;
static GLuint _vao;
static GLuint _vbo_elements;
static size_t _num_indices;


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Bump Mapping");
    glutReshapeWindow(512, 512);

    glewInit();

    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardup);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();

    return 0;
}


inline bool file_exist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}


template<size_t _Nm>
void assign_buffer(GLuint idx, vector<array<float, _Nm>>& src) {
    GLuint buffer;
    glEnableVertexAttribArray(idx);
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, src.size() *sizeof(src[0]), src.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(idx, _Nm, GL_FLOAT, GL_FALSE, 0, (void*)NULL);
}

void init(void) {
    // anti-foolish
    assert(file_exist(filename_object_descriptor));
    assert(file_exist(filename_texture));
    assert(file_exist(filename_normal_map));
    assert(file_exist(filename_shader_vert));
    assert(file_exist(filename_shader_frag));

    // gl setup
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // load shader
    GLuint vert = createShader(filename_shader_vert, "vertex");
    GLuint frag = createShader(filename_shader_frag, "fragment");
    _shader_idx = createProgram(vert, frag);

    // read model
    GLMmodel *model = glmReadOBJ(filename_object_descriptor);

    glmUnitize(model);
    glmVertexNormals(model, 90.0, GL_FALSE);

    print_model_info(model);

    // get texture
    GLMgroup *group = model->groups;

    const auto idx_material = group->material;
    const auto material = model->materials[idx_material];

    const auto map_diffuse = material.map_diffuse;
    const auto texture = model->textures[map_diffuse];

    // indexing
    vector<ushort> v_indices;
    vector<vec3> v_positions;
    vector<vec3> v_normals;
    vector<vec2> v_texcoords;

    for (int i = 0; i < group->numtriangles; i++) {
        // get triangle
        const auto triangle = model->triangles[group->triangles[i]];

        // iter through 3 vertices
        for (int j=0; j<3; j++) {
            // get indices
            const int vindex = triangle.vindices[j];
            const int nindex = triangle.nindices[j];
            const int tindex = triangle.tindices[j];

            assert(vindex <= model->numvertices);
            assert(nindex <= model->numnormals);
            assert(tindex <= model->numtexcoords);

            // access data
            const float *position = &model->vertices[3 *vindex];
            const float *normal = &model->normals[3 *nindex];
            const float *texcoord = &model->texcoords[2 *tindex];

            // check vertex existence
            ushort idx = v_positions.size();

            #define _VEC3(x) { x[0], x[1], x[2] }
            v_indices.push_back(idx);
            v_positions.push_back(_VEC3(position));
            v_normals.push_back(_VEC3(normal));
            v_texcoords.push_back({
                texcoord[0] *texture.width,
                texcoord[1] *texture.height
            });
            #undef _VEC3
        }
    }

    // release model
    glmDelete(model);

    // bind VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    assign_buffer(0, v_positions);
    assign_buffer(1, v_normals);
    assign_buffer(2, v_texcoords);

    glBindVertexArray(0);

    // element array
    glGenBuffers(1, &_vbo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, v_indices.size() * sizeof(ushort), v_indices.data() , GL_STATIC_DRAW);

    _num_indices = v_indices.size();

    // set material parameters
    glUseProgram(_shader_idx);

    glUniform4fv(glGetUniformLocation(_shader_idx, "ambient"), 1, material.ambient);
    glUniform4fv(glGetUniformLocation(_shader_idx, "diffuse"), 1, material.diffuse);
    glUniform4fv(glGetUniformLocation(_shader_idx, "specular"), 1, material.specular);
    glUniform1fv(glGetUniformLocation(_shader_idx, "shininess"), 1, &material.shininess);

    // set textures
    glUniform1i(glGetUniformLocation(_shader_idx, "textureSampler"), 0);
    glUniform1i(glGetUniformLocation(_shader_idx, "normalSampler"), 1);

    glUseProgram(0);

    GLfloat temp;
    glActiveTexture(GL_TEXTURE0);
    glmLoadTexture(filename_texture, false, true, true, true, &temp, &temp); // work around

    glActiveTexture(GL_TEXTURE1);
    glmLoadTexture(filename_normal_map, false, true, true, true, &temp, &temp);
}


inline double cosd(double deg) {
    return cos(deg *M_PI /180);
}

inline double sind(double deg) {
    return sin(deg *M_PI /180);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //please try not to modify the following block of code(you can but you are not supposed to)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        eyex, eyey, eyez,
        eyex+cosd(eyet)*cosd(eyep), eyey+sind(eyet), eyez-cosd(eyet)*sind(eyep),
        0.0, 1.0, 0.0
    );

    // get matrix
    GLfloat mat_projection[16];
    GLfloat mat_view[16];
    glGetFloatv(GL_PROJECTION_MATRIX, mat_projection);
    glGetFloatv(GL_MODELVIEW_MATRIX, mat_view);

    draw_light_bulb();

    glPushMatrix();
        glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
        glRotatef(ball_rot[0], 1, 0, 0);
        glRotatef(ball_rot[1], 0, 1, 0);
        glRotatef(ball_rot[2], 0, 0, 1);
        //please try not to modify the previous block of code

        GLfloat mat_model[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mat_model);

        // setup conifg
        glUseProgram(_shader_idx);
        glBindVertexArray(_vao);

        glUniformMatrix4fv(glGetUniformLocation(_shader_idx, "mat_projection"), 1, GL_FALSE, mat_projection);
        glUniformMatrix4fv(glGetUniformLocation(_shader_idx, "mat_view"), 1, GL_FALSE, mat_view);
        glUniformMatrix4fv(glGetUniformLocation(_shader_idx, "mat_modelview"), 1, GL_FALSE, mat_model);
        glUniform3fv(glGetUniformLocation(_shader_idx, "wLightPosition"), 1, light_pos);
        glUniform1ui(glGetUniformLocation(_shader_idx, "use_bump"), use_bump);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_elements);
        glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_SHORT, (void*)NULL);

        // restore config
        glBindVertexArray(0);
        glUseProgram(0);
    glPopMatrix();

    glutSwapBuffers();
    camera_light_ball_move();
}
