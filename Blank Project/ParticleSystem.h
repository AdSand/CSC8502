#pragma once

#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Matrix4.h"
#include "../nclgl/OGLRenderer.h"
#include <cstdlib>
#include <algorithm>
#define MAX_PARTICLES 1000

// This particle system code is based on the implementation found at:
// https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/

enum Type { rain, snow, sand };

struct Particle {
    Vector3 pos;
    Vector3 speed;
    float size;
    float life;
    float cameraDistance;
    Type type;

    bool operator<(const Particle& other)const {
        return this->cameraDistance > other.cameraDistance;
    }
};


class ParticleSystem
{
public:
    ParticleSystem(Vector3 startPos, Vector3 rngLower, Vector3 rngHigher, int particlesPerBurst,
        float particleSpeed, float lifeSpan, float particleSize, float timeBetween, GLuint texture, Type type);
    ~ParticleSystem();

    void UpdateParticles(float dt, Vector3 cameraPosition);
    void UpdateSnow(float dt, Vector3 cameraPosition);
    void UpdateRain(float dt, Vector3 cameraPosition);
    void UpdateSand(float dt, Vector3 cameraPosition);
    void CreateNewParticles(float dt);
    void DrawParticles();
    GLuint GetTexture() { return texture; }
    int GetParticleCount() { return particlesCount; }
    Particle GetParticle() { return particles[0]; }

protected:
    GLuint texture;

    GLuint positionBuffer;
    GLuint vertexBuffer;
    GLuint particleVAO;
    GLfloat* positionData;
    GLfloat vertices[12] = { -0.5f, -0.5f, 0.0f,0.5f, -0.5f, 0.0f,-0.5f, 0.5f, 0.0f,0.5f, 0.5f, 0.0f };

    Particle particles[MAX_PARTICLES] = {};
    int lastUsedParticle = 0;
    int particlesPerBurst;

    Vector3 startPos;
    Vector3 rngLowerBound;
    Vector3 rngRange; // range to be placed from the lower bound

    int particlesCount;
    float lifespan;
    float particleSpeed;
    float particleSize;

    float timeBetween = 1.0f;

    Type type;

    float particleTimer = 0;

    int FindUnusedParticle();
    void SortParticles() { std::sort(&particles[0], &particles[MAX_PARTICLES]); }
};

