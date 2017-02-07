/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "ParticleSystem.h"
#include "Camera.h"
#include "Graphics.h"

ParticleSystem::ParticleSystem()
{
	category = 0xFFFFFFFF;

	_position = vec2f::zero;
	particleDelay = 0.1f;
	lastSpawn = 0;
	particlesToSpawn = 0;

	rate = 10;
	_emit = false;
	radius = 10;
	maxParticles = 0;

	life = 5.0f;
	startSpeed = 0;
	startRotation = 0;
	startScale = 1.0f;
	startAngularVelocity = 0;
	startForce = vec2f::zero;
	startAlpha = 1.0f;
	
	lifeVariation = 0.0f;
	speedVariation = 0.0f;
	rotationVariation = 0.0f;
	scaleVariation = 0.0f;
	angularVelocityVariation = 0.0f;
	forceVariation = 0.0f;
	alphaVariation = 0.0f;

	speedScale = crv::constant;
	scaleScale = crv::constant;
	angularVelocityScale = crv::constant;
	forceScale = crv::constant;
	alphaScale = crv::constant;

	shader = Graphics::defaultShader();
}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::position(const vec2f &setPosition)
{
	_position = setPosition;
}

vec2f ParticleSystem::position() const
{
	return _position;
}

void ParticleSystem::SetTexture(shared_ptr<Texture> texture)
{
	this->texture = texture;
}

void ParticleSystem::SetShader(shared_ptr<Shader> shader)
{
	this->shader = shader;
}

void ParticleSystem::SetRadius(float radius)
{
	this->radius = radius;
}

void ParticleSystem::SetMaxParticles(uint32_t maxParticles)
{
	this->maxParticles = maxParticles;
	particles.reserve(maxParticles);

	uint32_t numVerts = maxParticles * 4;
	uint32_t numIndices = maxParticles * 6;

	vertices.resize(numVerts);
	texcoords.resize(numVerts);
	colors.resize(numVerts);
	indices.resize(numIndices);

	for(uint32_t v = 0, i = 0; v < numVerts; v += 4, i += 6)
	{
		uint32_t a = v;
		uint32_t b = v + 1;
		uint32_t c = v + 2;
		uint32_t d = v + 3;

		texcoords[a].set(0.0f, 0.0f);
		texcoords[b].set(0.0f, 1.0f);
		texcoords[c].set(1.0f, 1.0f);
		texcoords[d].set(1.0f, 0.0f);

		indices[i + 0] = a;
		indices[i + 1] = b;
		indices[i + 2] = c;
		indices[i + 3] = a;
		indices[i + 4] = c;
		indices[i + 5] = d;
	}

	vertexBuffer.SetData(nullptr, numVerts * sizeof(vec2f), DrawBuffer::Type::VertexData, true);
	colorBuffer.SetData(nullptr, numVerts * sizeof(Color), DrawBuffer::Type::VertexData, true);
	texBuffer.SetData(texcoords.data(), numVerts * sizeof(vec2f), DrawBuffer::Type::VertexData);
	indexBuffer.SetData(indices.data(), numIndices * sizeof(uint32_t), DrawBuffer::Type::IndexData);
}

void ParticleSystem::SetRate(uint32_t rate)
{
	this->rate = rate;
	particleDelay = rate > 0 ? (1.0f / (float)rate) : 0.0f;
}

void ParticleSystem::SetLife(float life, float variation)
{
	this->life = life;
	this->lifeVariation = variation;
}

void ParticleSystem::SetSpeed(float start, float variation, crv::function_t scale)
{
	this->startSpeed = start;
	this->speedVariation = variation;
	this->speedScale = scale;
}

void ParticleSystem::SetRotation(float start, float variation)
{
	this->startRotation = start;
	this->rotationVariation = variation;
}

void ParticleSystem::SetScale(float start, float variation, crv::function_t scale)
{
	this->startScale = start;
	this->scaleVariation = variation;
	this->scaleScale = scale;
}

void ParticleSystem::SetAngularVelocity(float start, float variation, crv::function_t scale)
{
	this->startAngularVelocity = start;
	this->angularVelocityVariation = variation;
	this->angularVelocityScale = scale;
}

void ParticleSystem::SetForce(const vec2f &start, float variation, crv::function_t scale)
{
	this->startForce = start;
	this->forceVariation = variation;
	this->forceScale = scale;
}

void ParticleSystem::SetAlpha(float start, float variation, crv::function_t scale)
{
	this->startAlpha = start;
	this->alphaVariation = variation;
	this->alphaScale = scale;
}

void ParticleSystem::emit(bool setEmit)
{
	if(setEmit && !_emit)
	{
		lastSpawn = Time::time();
		particlesToSpawn = 0;
	}

	_emit = setEmit;
}

bool ParticleSystem::emit()
{
	return _emit;
}

void ParticleSystem::Spawn(uint32_t count)
{
	count = min(maxParticles - particles.size(), count);
	
	for(uint32_t i = 0; i < count; ++i)
	{
		particles.emplace_back();
		Particle &p = particles.back();

		p.birth = Time::time();
		p.death = p.birth + life + Random::signedValue() * lifeVariation;
		p.position = _position + Random::vector2() * radius;
		p.velocity = Random::vector2().Normalized() * (startSpeed + Random::signedValue() * speedVariation);
		p.rotation = startRotation + Random::signedValue() * rotationVariation;
		p.scale = startScale + Random::signedValue() * scaleVariation;
		p.angularVelocity = startAngularVelocity + Random::signedValue() * angularVelocityVariation;
		p.alpha = startAlpha + Random::signedValue() * alphaVariation;
	}
}

bool ParticleSystem::Alive()
{
	return !particles.empty();
}

void ParticleSystem::Update()
{
	if(!Camera::activeCamera()
	|| !texture
	|| !shader)
		return;

	float hw = texture->width() * 0.5f;
	float hh = texture->height() * 0.5f;

	uint32_t i = 0;
	for(auto it = particles.begin(); it != particles.end(); )
	{
		if(Time::time() < it->death)
		{
			float t = (Time::time() - it->birth) / (it->death - it->birth);
			
			it->velocity += startForce * (forceScale(t) * Time::deltaTime());
			it->position += it->velocity * (speedScale(t) * Time::deltaTime());
			it->rotation += it->angularVelocity * (angularVelocityScale(t) * Time::deltaTime());
			
			float scale = it->scale * scaleScale(t);
			float alpha = it->alpha * alphaScale(t);

			uint32_t a = 4 * i;
			uint32_t b = 4 * i + 1;
			uint32_t c = 4 * i + 2;
			uint32_t d = 4 * i + 3;

			float cosAng = cos(math::rad(it->rotation));
			float sinAng = sin(math::rad(it->rotation));
			
			mat3f xf(cosAng, -sinAng, it->position.x,
					 sinAng, cosAng,  it->position.y,
					 0,      0,       1);

			float hws = hw * scale;
			float hhs = hh * scale;

			vertices[a] = vec2f(xf, -hws, -hhs);
			vertices[b] = vec2f(xf, -hws,  hhs);
			vertices[c] = vec2f(xf,  hws,  hhs);
			vertices[d] = vec2f(xf,  hws, -hhs);

			Color color(1, 1, 1, alpha);
			colors[a] = color;
			colors[b] = color;
			colors[c] = color;
			colors[d] = color;

			++it;
			++i;
		}
		else
		{
			it = particles.erase(it);
		}
	}

	float timeSinceSpawn = Time::time() - lastSpawn;

	if(_emit && timeSinceSpawn >= particleDelay)
	{
		particlesToSpawn += timeSinceSpawn / particleDelay;
		int pcount = (int)particlesToSpawn;
		particlesToSpawn -= (float)pcount;

		Spawn(pcount);

		lastSpawn = Time::time();
	}

	uint32_t vertsToUpdate = particles.size() * 4;

	vertexBuffer.UpdateData(vertices.data(), 0, vertsToUpdate * sizeof(vec2f));
	colorBuffer.UpdateData(colors.data(), 0, vertsToUpdate * sizeof(Color));
}

void ParticleSystem::Draw()
{
	if(!Camera::activeCamera()
	|| !texture
	|| !shader
	|| particles.empty())
		return;

	shader->SetActive();
	shader->SetUniform("uMainTex", texture.get());
	shader->SetUniform("uMtxMVP", Camera::activeCamera()->matrix());
	shader->SetVertexBuffer("aPosition", &vertexBuffer);
	shader->SetVertexBuffer("aTexCoord", &texBuffer);
	shader->SetVertexBuffer("aColor", &colorBuffer);
	shader->SetIndexBuffer(&indexBuffer);
	
	Graphics::DrawIndexed(0, particles.size() * 6, DrawMode::Triangles);
}
