/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "utils.h"

int AttribComponentCount(GLenum type)
{
	switch(type)
	{
	case GL_BOOL:
	case GL_FLOAT:
	case GL_INT:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_UNSIGNED_INT:
		return 1;
	case GL_BOOL_VEC2:
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
	case GL_UNSIGNED_INT_VEC2:
		return 2;
	case GL_INT_VEC3:
	case GL_FLOAT_VEC3:
	case GL_BOOL_VEC3:
	case GL_UNSIGNED_INT_VEC3:
		return 3;
	case GL_BOOL_VEC4:
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
	case GL_UNSIGNED_INT_VEC4:
	case GL_FLOAT_MAT2:
		return 4;
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT3x2:
		return 6;
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT4x2:
		return 8;
	case GL_FLOAT_MAT3:
		return 9;
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x3:
		return 12;
	case GL_FLOAT_MAT4:
		return 16;
	}

	return 0;
}

GLenum AttribComponentType(GLenum type)
{
	switch(type)
	{
	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
		return GL_BOOL;
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x3:
		return GL_FLOAT;
	case GL_INT:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		return GL_INT;
	case GL_UNSIGNED_INT:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
		return GL_UNSIGNED_INT;
	}

    return GL_NONE;
}

/////////////////////

weak_ptr<Shader> Shader::_activeShader;

Shader::Shader()
{
	hProgram = -1;
}

Shader::Shader(const string &vert, const string &frag)
{
	hProgram = -1;
	Load(vert, frag);
}

Shader::~Shader()
{
	Unload();
}

bool Shader::Load(const string &vert, const string &frag)
{
	Unload();

	bytestream vertexFile = bytestream_from_file(vert);

	if(vertexFile.empty())
	{
		Trace("Couldn't open vertex shader: " + vert);
		return false;
	}

	bytestream fragmentFile = bytestream_from_file(frag);

	if(fragmentFile.empty())
	{
		Trace("Couldn't open vertex shader: " + frag);
		return false;
	}

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	char *vsData = vertexFile.data();
	char *fsData = fragmentFile.data();
	int vsLength[] = { (int)vertexFile.size() };
	int fsLength[] = { (int)fragmentFile.size() };

	glShaderSource(vertShader, 1, (const char **)&vsData, vsLength); 
	glShaderSource(fragShader, 1, (const char **)&fsData, fsLength); 

	glCompileShader(vertShader);
	glCompileShader(fragShader);

	if(!CheckShader(vertShader) || !CheckShader(fragShader))
	{
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return false;
	}

	hProgram = glCreateProgram();
    glAttachShader(hProgram, vertShader);
	glAttachShader(hProgram, fragShader);
    glLinkProgram(hProgram);

	GLint activeAttribCount = 0;
	GLint activeUniformCount = 0;
	glGetProgramiv(hProgram, GL_ACTIVE_ATTRIBUTES, &activeAttribCount);
	glGetProgramiv(hProgram, GL_ACTIVE_UNIFORMS, &activeUniformCount);

	GLint maxAttribNameLength = 0;
	GLint maxUniformNameLength = 0;
	glGetProgramiv(hProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
	glGetProgramiv(hProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

	unique_ptr<char[]> attribNameData(new char[maxAttribNameLength]);
	unique_ptr<char[]> uniformNameData(new char[maxUniformNameLength]);
	
	attribs.reserve(activeAttribCount);
	uniforms.reserve(activeUniformCount);

	for(int i = 0; i < activeAttribCount; ++i)
	{
		GLsizei nameSize;
		GLint arraySize;
		GLenum type;
		
		glGetActiveAttrib(hProgram,
						  i,
						  maxAttribNameLength,
						  &nameSize,
						  &arraySize,
						  &type,
						  attribNameData.get());
		
		VertexAttrib attrib;

		string name(attribNameData.get(), nameSize);
		attrib.location = glGetAttribLocation(hProgram, name.c_str());
		attrib.ctype = AttribComponentType(type);
		attrib.ccount = AttribComponentCount(type);
		
		int id = attribs.size();
		attribs.push_back(attrib);
		attribIDs.emplace(move(name), id);
	}

	for(int i = 0; i < activeUniformCount; ++i)
	{
		GLsizei nameSize;
		GLint arraySize;
		GLenum type;
		
		glGetActiveUniform(hProgram,
						   i,
						   maxUniformNameLength,
						   &nameSize,
						   &arraySize,
						   &type,
						   uniformNameData.get());

		ShaderUniform uniform;

		string name(uniformNameData.get(), nameSize);
		uniform.location = glGetUniformLocation(hProgram, name.c_str());
		uniform.ctype = AttribComponentType(type);
		uniform.ccount = AttribComponentCount(type);

		int id = uniforms.size();
		uniforms.push_back(uniform);
		uniformIDs.emplace(move(name), id);
	}

	return true;
}

void Shader::Unload()
{
	Deactivate();

	if(glIsProgram(hProgram))
		glDeleteProgram(hProgram);
	
	hProgram = -1;

	attribs.clear();
	uniforms.clear();
	attribIDs.clear();
	uniformIDs.clear();
}

bool Shader::CheckShader(GLuint shader_id)
{
	GLint Status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &Status);

	if(Status == GL_FALSE)
	{
		GLint errLength;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH , &errLength);

		if(errLength > 1)
		{
			unique_ptr<char[]> error(new char[errLength]);

			GLsizei nChars;
			glGetShaderInfoLog(shader_id, errLength, &nChars, error.get());
			Trace("Could not compile shader:", error.get());
		}
	}

	return (Status == GL_TRUE);
}

bool Shader::ValidateProgram(GLuint hProgram)
{
	GLint Status;

	glGetProgramiv(hProgram, GL_LINK_STATUS, &Status);
	if(Status == GL_FALSE)
		return false;

	glValidateProgram(hProgram);
	glGetProgramiv(hProgram, GL_VALIDATE_STATUS, &Status);
	if(Status == GL_FALSE)
		return false;

	return true;
}

void Shader::_enableShader()
{
	assert(glIsProgram(hProgram));

	glUseProgram(hProgram);

	for(auto& att : this->attribs)
		glEnableVertexAttribArray(att.location);
}

void Shader::_disableShader()
{
	assert(glIsProgram(hProgram));

	for(auto& att : this->attribs)
		glDisableVertexAttribArray(att.location);

	glUseProgram(0);
}

void Shader::SetActive()
{
	if(auto p = _activeShader.lock())
		p->_disableShader();

	_enableShader();

	_activeShader = static_pointer_cast<Shader>(shared_from_this());
}

void Shader::Deactivate()
{
	if(_activeShader.lock().get() == this)
	{
		_disableShader();
		_activeShader.reset();
	}
}

shared_ptr<Shader> Shader::activeShader()
{
	return _activeShader.lock();
}

void Shader::activeShader(const shared_ptr<Shader> &shader)
{
	if(auto p = _activeShader.lock())
		p->_disableShader();

	if(shader)
		shader->_enableShader();
	
	_activeShader = shader;
}

int Shader::GetAttribID(const string &name)
{
	auto it = attribIDs.find(name);

	if(it == attribIDs.end())
	{
		Trace("Shader attribute " + name + " not found.");
		return -1;
	}

	return it->second;
}

int Shader::GetUniformID(const string &name)
{
	auto it = uniformIDs.find(name);

	if(it == uniformIDs.end())
	{
		Trace("Shader uniform " + name + " not found.");
		return -1;
	}

	return it->second;
}

bool Shader::HasAttrib(const string &name)
{
	return attribIDs.find(name) != attribIDs.end();
}

bool Shader::HasUniform(const string &name)
{
	return uniformIDs.find(name) != uniformIDs.end();
}

void Shader::SetUniform(const string &name, float value)
{
	SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const string &name, const vec2f &value)
{
	SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const string &name, const vec3f &value)
{
	SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const string &name, const vec4f &value)
{
	SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const string &name, const Color &value)
{
	SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const string &name, const mat3f &value)
{
	SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const string &name, const mat4f &value)
{
	SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const string &name, const Texture *texture)
{
	SetUniform(GetUniformID(name), texture);
}

void Shader::SetVertexBuffer(const string &name, DrawBuffer *buffer)
{
	SetVertexBuffer(GetAttribID(name), buffer);
}

void Shader::SetUniform(int id, float value)
{
	if(id == -1) return;
	auto uni = uniforms.begin() + id;
	glUniform1f(uni->location, value);
}

void Shader::SetUniform(int id, const vec2f &value)
{
	if(id == -1) return;
	auto uni = uniforms.begin() + id;
	glUniform2fv(uni->location, 1, (float*)&value);
}

void Shader::SetUniform(int id, const vec3f &value)
{
	if(id == -1) return;
	auto uni = uniforms.begin() + id;
	glUniform3fv(uni->location, 1, (float*)&value);
}

void Shader::SetUniform(int id, const vec4f &value)
{
	if(id == -1) return;
	auto uni = uniforms.begin() + id;
	glUniform4fv(uni->location, 1, (float*)&value);
}

void Shader::SetUniform(int id, const Color &value)
{
	if(id == -1) return;
	auto uni = uniforms.begin() + id;
	glUniform4fv(uni->location, 1, (float*)&value);
}

void Shader::SetUniform(int id, const mat3f &value)
{
	if(id == -1) return;
	auto uni = uniforms.begin() + id;
	glUniformMatrix3fv(uni->location, 1, GL_FALSE, (float*)&value);
}

void Shader::SetUniform(int id, const mat4f &value)
{
	if(id == -1) return;
	auto uni = uniforms.begin() + id;
	glUniformMatrix4fv(uni->location, 1, GL_FALSE, (float*)&value);
}

void Shader::SetUniform(int id, const Texture *texture)
{
	if(id == -1) return;

	auto uni = uniforms.begin() + id;

	int textureUnit = id;
	int samplerSlot = uni->location;
	
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture? texture->textureID() : 0);
	glUniform1i(samplerSlot, textureUnit);
}

void Shader::SetVertexBuffer(int id, DrawBuffer *buffer)
{
	if(id == -1)
		return;
	
	auto att = attribs.begin() + id;

	int location = att->location;

	glBindBuffer(GL_ARRAY_BUFFER, buffer->bufferID());
	glVertexAttribPointer(location, att->ccount, att->ctype, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Shader::SetIndexBuffer(DrawBuffer *buffer)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer ? buffer->bufferID() : 0);
}

