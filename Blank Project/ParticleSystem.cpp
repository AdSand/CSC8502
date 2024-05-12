#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(Vector3 rngLower, Vector3 rngHigher, int particlesPerBurst, Vector3 startSpeed, float lifeSpan, float particleSize, float timeBetween, GLuint texture, Type type)
{
    lastUsedParticle = 0;
    this->texture = texture;
    this->particlesPerBurst = particlesPerBurst;
    this->startSpeed = startSpeed;
    this->lifespan = lifeSpan;
    this->particleSize = particleSize;
    this->rngLowerBound = rngLower;
    this->rngRange = Vector3(rngHigher.x - rngLower.x, rngHigher.y - rngLower.y, rngHigher.z - rngLower.z);
    this->timeBetween = timeBetween;
    this->type = type;

    glGenVertexArrays(1, &particleVAO);
    glBindVertexArray(particleVAO);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * MAX_PARTICLES * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);

    positionData = new GLfloat[MAX_PARTICLES * 4];
}

ParticleSystem::~ParticleSystem()
{
    delete[] positionData;
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &positionBuffer);
    glDeleteVertexArrays(1, &particleVAO);
}

void ParticleSystem::UpdateParticles(float dt, Vector3 cameraPosition)
{
    particlesCount = 0;
    updatedParticles = 0;

    switch (type)
    {
    case rain:
        UpdateRain(dt, cameraPosition);
        break;
    case snow:
        UpdateSnow(dt, cameraPosition);
        break;
    case sand:
        UpdateSand(dt, cameraPosition);
        break;
    default:
        std::cerr << "No particle type set";
        break;
    }

    SortParticles();
}

void ParticleSystem::UpdateSnow(float dt, Vector3 cameraPosition)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        if (p.life > 0.0f) {
            p.life -= dt;
            p.pos += p.speed * dt;
            p.pos.x += sin(p.pos.y * 0.005) * dt * 250;
            p.pos.z += sin(p.pos.y * 0.005) * dt * 250;

            p.cameraDistance = Vector3(p.pos - cameraPosition).Length();

            positionData[4 * particlesCount] = p.pos.x;
            positionData[4 * particlesCount + 1] = p.pos.y;
            positionData[4 * particlesCount + 2] = p.pos.z;
            positionData[4 * particlesCount + 3] = p.size;

            updatedParticles++;
        }
        else {
            p.cameraDistance = -1.0f;
            positionData[4 * particlesCount] = -1000;
            positionData[4 * particlesCount + 1] = -1000;
            positionData[4 * particlesCount + 2] = -1000;
            positionData[4 * particlesCount + 3] = p.size;
        }
        particlesCount++;
    }
}

void ParticleSystem::UpdateRain(float dt, Vector3 cameraPosition)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        if (p.life > 0.0f) {
            p.life -= dt;
            p.pos += p.speed * dt;
            p.cameraDistance = Vector3(p.pos - cameraPosition).Length();

            positionData[4 * particlesCount] = p.pos.x;
            positionData[4 * particlesCount + 1] = p.pos.y;
            positionData[4 * particlesCount + 2] = p.pos.z;
            positionData[4 * particlesCount + 3] = p.size;

            updatedParticles++;
        }
        else {
            p.cameraDistance = -1.0f;
            positionData[4 * particlesCount] = -1000;
            positionData[4 * particlesCount + 1] = -1000;
            positionData[4 * particlesCount + 2] = -1000;
            positionData[4 * particlesCount + 3] = p.size;
        }
        particlesCount++;
    }
}

void ParticleSystem::UpdateSand(float dt, Vector3 cameraPosition)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        if (p.life > 0.0f) {
            p.life -= dt;
            p.pos += p.speed * dt;
            p.pos.y += sin(p.pos.z * 0.005) * dt * 25;

            p.cameraDistance = Vector3(p.pos - cameraPosition).Length();

            positionData[4 * particlesCount] = p.pos.x;
            positionData[4 * particlesCount + 1] = p.pos.y;
            positionData[4 * particlesCount + 2] = p.pos.z;
            positionData[4 * particlesCount + 3] = p.size;

            updatedParticles++;
        }
        else {
            p.cameraDistance = -1.0f;
            positionData[4 * particlesCount] = -1000;
            positionData[4 * particlesCount + 1] = -1000;
            positionData[4 * particlesCount + 2] = -1000;
            positionData[4 * particlesCount + 3] = p.size;
        }
        particlesCount++;
    }
}

void ParticleSystem::CreateNewParticles(float dt)
{
    particleTimer += dt;
    int newParticles = 0;
    if (particleTimer >= timeBetween)
    {
        particleTimer = 0;
        newParticles = particlesPerBurst;
    }

    for (int i = 0; i < newParticles; i++) {
        int particleIndex = FindUnusedParticle();
        particles[particleIndex].life = 0.01f * (rand() % 20) + lifespan;
        particles[particleIndex].pos = startPos +
            Vector3(rngLowerBound.x + rand() % (int)rngRange.x,
                rngLowerBound.y + rand() % (int)rngRange.y,
                rngLowerBound.z + rand() % (int)rngRange.z);
        particles[particleIndex].speed = startSpeed;
        particles[particleIndex].size = particleSize;
    }
}

void ParticleSystem::DrawParticles()
{
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, positionData);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);
}

int ParticleSystem::FindUnusedParticle()
{
    for (int i = lastUsedParticle; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0) {
            lastUsedParticle = i;
            return i;
        }
    }

    for (int i = 0; i < lastUsedParticle; i++) {
        if (particles[i].life <= 0) {
            lastUsedParticle = i;
            return i;
        }
    }

    return 0;
}
