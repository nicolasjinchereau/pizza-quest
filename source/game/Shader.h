/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "includes.h"
#include "Trace.h"
#include "Math.h"
#include "Object.h"
#include "Engine.h"
#include "DrawBuffer.h"
#include <stdlib.h>
#include <unordered_map>
#include <string>
using namespace std;

class Texture;

class Shader : public Object
{
public:
	struct VertexAttrib
	{
		GLint location;
		GLenum ctype;
		GLint ccount;
	};

	struct ShaderUniform
	{
		GLint location;
		GLenum ctype;
		GLint ccount;
	};

	Shader();
	Shader(const string &vert, const string &frag);
	virtual ~Shader();

	bool Load(const string &vert, const string &frag);
	void Unload();

	void SetActive();
	void Deactivate();

	int GetAttribID(const string &name);
	int GetUniformID(const string &name);
	bool HasAttrib(const string &name);
	bool HasUniform(const string &name);

	void SetUniform(const string &name, float value);
	void SetUniform(const string &name, const vec2f &value);
	void SetUniform(const string &name, const vec3f &value);
	void SetUniform(const string &name, const vec4f &value);
	void SetUniform(const string &name, const Color &value);
	void SetUniform(const string &name, const mat3f &value);
	void SetUniform(const string &name, const mat4f &value);
	void SetUniform(const string &name, const Texture *texture);
	void SetVertexBuffer(const string &name, DrawBuffer *buffer);

	void SetUniform(int id, float value);
	void SetUniform(int id, const vec2f &value);
	void SetUniform(int id, const vec3f &value);
	void SetUniform(int id, const vec4f &value);
	void SetUniform(int id, const Color &value);
	void SetUniform(int id, const mat3f &value);
	void SetUniform(int id, const mat4f &value);
	void SetUniform(int id, const Texture *texture);
	void SetVertexBuffer(int id, DrawBuffer *buffer);
	void SetIndexBuffer(DrawBuffer *buffer);
	
	static shared_ptr<Shader> activeShader();
	static void activeShader(const shared_ptr<Shader> &shader);

protected:
	static weak_ptr<Shader> _activeShader;

	GLuint hProgram;

	vector<VertexAttrib> attribs;
	vector<ShaderUniform> uniforms;
	unordered_map<string, int> attribIDs;
	unordered_map<string, int> uniformIDs;
	
	void _enableShader();
	void _disableShader();

	bool CheckShader(GLuint shader_id);
	bool ValidateProgram(GLuint hProgram);

	friend class Camera;
};
