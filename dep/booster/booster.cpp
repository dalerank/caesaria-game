
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_types.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

#include "../../android/libs/SDL/src/render/SDL_sysrender.h"

#define RENDERER_CONTEXT_MAJOR 2
#define RENDERER_CONTEXT_MINOR 1

typedef struct GL_FBOList GL_FBOList;
typedef struct GL_ShaderContext GL_ShaderContext;

typedef enum {
    SHADER_NONE,
    SHADER_SOLID,
    SHADER_RGB,
    SHADER_YUV,
    SHADER_NV12,
    SHADER_NV21,
    NUM_SHADERS
} GL_Shader;

struct GL_FBOList
{
    Uint32 w, h;
    GLuint FBO;
    GL_FBOList *next;
};

typedef struct
{
    SDL_GLContext context;

    SDL_bool debug_enabled;
    SDL_bool GL_ARB_debug_output_supported;
    int errors;
    char **error_messages;
    GLDEBUGPROCARB next_error_callback;
    GLvoid *next_error_userparam;

    SDL_bool GL_ARB_texture_non_power_of_two_supported;
    SDL_bool GL_ARB_texture_rectangle_supported;
    struct {
        GL_Shader shader;
        Uint32 color;
        int blendMode;
    } current;

    SDL_bool GL_EXT_framebuffer_object_supported;
    GL_FBOList *framebuffers;

    /* OpenGL functions */
#define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;
#include "glfuncs.hpp"
#undef SDL_PROC

    /* Multitexture support */
    SDL_bool GL_ARB_multitexture_supported;
    PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
    GLint num_texture_units;

    PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
    PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
    PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
    PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
    PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;

    /* Shader support */
    GL_ShaderContext *shaders;

} GL_RenderData;

typedef struct
{
    GLuint texture;
    GLenum type;
    GLfloat texw;
    GLfloat texh;
    GLenum format;
    GLenum formattype;
    void *pixels;
    int pitch;
    SDL_Rect locked_rect;

    /* YUV texture support */
    SDL_bool yuv;
    SDL_bool nv12;
    GLuint utexture;
    GLuint vtexture;

    GL_FBOList *fbo;
} GL_TextureData;

typedef struct
{
    GLhandleARB program;
    GLhandleARB vert_shader;
    GLhandleARB frag_shader;
} GL_ShaderData;

struct GL_ShaderContext
{
    GLenum (*glGetError)(void);

    PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
    PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
    PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
    PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
    PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
    PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
    PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
    PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
    PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
    PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
    PFNGLUNIFORM1IARBPROC glUniform1iARB;
    PFNGLUNIFORM1FARBPROC glUniform1fARB;
    PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;

    SDL_bool GL_ARB_texture_rectangle_supported;

    GL_ShaderData shaders[NUM_SHADERS];
};

static const float inv255f = 1.0f / 255.0f;

static void
GL_SetColor(GL_RenderData * data, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    Uint32 color = ((a << 24) | (r << 16) | (g << 8) | b);

    if (color != data->current.color) {
        data->glColor4f((GLfloat) r * inv255f,
                        (GLfloat) g * inv255f,
                        (GLfloat) b * inv255f,
                        (GLfloat) a * inv255f);
        data->current.color = color;
    }
}

void
GL_SelectShader(GL_ShaderContext *ctx, GL_Shader shader)
{
    ctx->glUseProgramObjectARB(ctx->shaders[shader].program);
}

static void
GL_SetShader(GL_RenderData * data, GL_Shader shader)
{
    if (data->shaders && shader != data->current.shader) {
        GL_SelectShader(data->shaders, shader);
        data->current.shader = shader;
    }
}

static void
GL_SetBlendMode(GL_RenderData * data, int blendMode)
{
    if (blendMode != data->current.blendMode) {
        switch (blendMode) {
        case SDL_BLENDMODE_NONE:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            data->glDisable(GL_BLEND);
            break;
        case SDL_BLENDMODE_BLEND:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            data->glEnable(GL_BLEND);
            data->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case SDL_BLENDMODE_ADD:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            data->glEnable(GL_BLEND);
            data->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
            break;
        case SDL_BLENDMODE_MOD:
            data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            data->glEnable(GL_BLEND);
            data->glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
            break;
        }
        data->current.blendMode = blendMode;
    }
}

static int
GL_SetupCopy(SDL_Renderer * renderer, SDL_Texture * texture)
{
    GL_RenderData *data = (GL_RenderData *) renderer->driverdata;
    GL_TextureData *texturedata = (GL_TextureData *) texture->driverdata;

    data->glEnable(texturedata->type);
    if (texturedata->yuv) {
        data->glActiveTextureARB(GL_TEXTURE2_ARB);
        data->glBindTexture(texturedata->type, texturedata->vtexture);

        data->glActiveTextureARB(GL_TEXTURE1_ARB);
        data->glBindTexture(texturedata->type, texturedata->utexture);

        data->glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    if (texturedata->nv12) {
        data->glActiveTextureARB(GL_TEXTURE1_ARB);
        data->glBindTexture(texturedata->type, texturedata->utexture);

        data->glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    data->glBindTexture(texturedata->type, texturedata->texture);

    if (texture->modMode) {
        GL_SetColor(data, texture->r, texture->g, texture->b, texture->a);
    } else {
        GL_SetColor(data, 255, 255, 255, 255);
    }

    GL_SetBlendMode(data, texture->blendMode);

    if (texturedata->yuv) {
        GL_SetShader(data, SHADER_YUV);
    } else if (texturedata->nv12) {
        if (texture->format == SDL_PIXELFORMAT_NV12) {
            GL_SetShader(data, SHADER_NV12);
        } else {
            GL_SetShader(data, SHADER_NV21);
        }
    } else {
        GL_SetShader(data, SHADER_RGB);
    }
    return 0;
}

static SDL_GLContext SDL_CurrentContext = NULL;

static int
GL_ActivateRenderer(SDL_Renderer * renderer)
{
    GL_RenderData *data = (GL_RenderData *) renderer->driverdata;

    if (SDL_CurrentContext != data->context ||
        SDL_GL_GetCurrentContext() != data->context) {
        if (SDL_GL_MakeCurrent(renderer->window, data->context) < 0) {
            return -1;
        }
        SDL_CurrentContext = data->context;

        GL_UpdateViewport(renderer);
    }

    GL_ClearErrors(renderer);

    return 0;
}

static int
GL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
              const SDL_Rect * srcrect, const SDL_FRect * dstrect)
{
    GL_RenderData *data = (GL_RenderData *) renderer->driverdata;
    GL_TextureData *texturedata = (GL_TextureData *) texture->driverdata;
    GLfloat minx, miny, maxx, maxy;
    GLfloat minu, maxu, minv, maxv;

    GL_ActivateRenderer(renderer);

    if (GL_SetupCopy(renderer, texture) < 0) {
        return -1;
    }

    minx = dstrect->x;
    miny = dstrect->y;
    maxx = dstrect->x + dstrect->w;
    maxy = dstrect->y + dstrect->h;

    minu = (GLfloat) srcrect->x / texture->w;
    minu *= texturedata->texw;
    maxu = (GLfloat) (srcrect->x + srcrect->w) / texture->w;
    maxu *= texturedata->texw;
    minv = (GLfloat) srcrect->y / texture->h;
    minv *= texturedata->texh;
    maxv = (GLfloat) (srcrect->y + srcrect->h) / texture->h;
    maxv *= texturedata->texh;

    data->glBegin(GL_TRIANGLE_STRIP);
    data->glTexCoord2f(minu, minv);
    data->glVertex2f(minx, miny);
    data->glTexCoord2f(maxu, minv);
    data->glVertex2f(maxx, miny);
    data->glTexCoord2f(minu, maxv);
    data->glVertex2f(minx, maxy);
    data->glTexCoord2f(maxu, maxv);
    data->glVertex2f(maxx, maxy);
    data->glEnd();

    data->glDisable(texturedata->type);

    return GL_CheckError("", renderer);
}

/* vi: set ts=4 sw=4 expandtab: */
