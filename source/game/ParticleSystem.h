/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Graphics.h"
#include "Math.h"
#include "Object.h"
#include "curves.h"
#include <vector>
#include <algorithm>
using namespace std;

class ParticleSystem : public Object
{
	bool _getEmit();
	void _setEmit(bool emit);
public:
	struct Particle
	{
		float birth;
		float death;
		vec2f position;
		vec2f velocity;
		float rotation;
		float scale;
		float angularVelocity;
		float alpha;
	};

	ParticleSystem();
	~ParticleSystem();

	void position(const vec2f &setPosition);
	vec2f position() const;

	void SetTexture(shared_ptr<Texture> texture);
	void SetShader(shared_ptr<Shader> shader);

	void SetRadius(float radius);
	void SetMaxParticles(uint32_t maxParticles);
	void SetRate(uint32_t rate);
	
	void SetLife(float life, float variation = 0.0f);
	void SetSpeed(float start, float variation = 0.0f, crv::function_t scale = crv::constant);
	void SetRotation(float start, float variation = 0.0f);
	void SetScale(float start, float variation = 0.0f, crv::function_t scale = crv::constant);
	void SetAngularVelocity(float start, float variation = 0.0f, crv::function_t scale = crv::constant);
	void SetForce(const vec2f &start, float variation = 0.0f, crv::function_t scale = crv::constant);
	void SetAlpha(float start, float variation = 0.0f, crv::function_t scale = crv::constant);

	void emit(bool setEmit);
	bool emit();

	void Spawn(uint32_t count);

	bool Alive();

	virtual void Update() override;
	virtual void Draw() override;

private:
	vec2f _position;
	float particleDelay;
	float lastSpawn;
	float particlesToSpawn;

	float radius;
	uint32_t maxParticles;
	uint32_t rate;
	bool _emit;

	float life;
	float startSpeed;
	float startRotation;
	float startScale;
	float startAngularVelocity;
	vec2f startForce;
	float startAlpha;

	float lifeVariation;
	float speedVariation;
	float rotationVariation;
	float scaleVariation;
	float angularVelocityVariation;
	float forceVariation;
	float alphaVariation;
	
	crv::function_t speedScale;
	crv::function_t scaleScale;
	crv::function_t angularVelocityScale;
	crv::function_t forceScale;
	crv::function_t alphaScale;

	vector<Particle> particles;
	vector<vec2f> vertices;
	vector<vec2f> texcoords;
	vector<Color> colors;
	vector<uint32_t> indices;
	
	shared_ptr<Texture> texture;
	shared_ptr<Shader> shader;

	DrawBuffer vertexBuffer;
	DrawBuffer colorBuffer;
	DrawBuffer texBuffer;
	DrawBuffer indexBuffer;
};
