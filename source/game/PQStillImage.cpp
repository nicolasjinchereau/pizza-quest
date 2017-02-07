/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQStillImage.h"

PQStillImage::PQStillImage(const StillImageDesc &desc)
	: desc(desc)
{

}

PQStillImage::~PQStillImage()
{
	
}

void PQStillImage::Start()
{
	done = false;
	
	camera = AddChild(make_shared<Camera>());
	camera->SetSize(480);
	Camera::activeCamera(camera);

	shader = AddChild(make_shared<Shader>());
	shader->Load("assets\\Shaders\\default.vert",
				 "assets\\Shaders\\tinted.frag");

	image = AddChild(make_shared<Sprite>(shader));
	image->Open(desc.fnImage.c_str());
	image->SetShader(shader);
	image->category = 0;
	category = 1;

	sound = AddChild(make_shared<Sound>());
	if(desc.fnSound != "")
		sound->Open( desc.fnSound.c_str() );
	
	image->SetPos(camera->NormalizedToWorldX(0.5f),
				  camera->NormalizedToWorldY(0.5f) );

	if( sound->IsOpen() )
		sound->Play();

	RunCoroutine([this](yield_token<float> yield)
	{
		float length = desc.fadeInLength;
		float start = Time::exactTime();
		float finish = start + length;

		while(Time::exactTime() < finish)
		{
			float t = (Time::exactTime() - start) / length;

			shader->SetActive();
			shader->SetUniform("uTint", Color(t, t, t));

			yield(0);
		}

		shader->SetActive();
		shader->SetUniform("uTint", Color::white);

		yield(desc.sustainLength);

		length = desc.fadeOutLength;
		start = Time::exactTime();
		finish = start + length;

		while(Time::exactTime() < finish)
		{
			float t = (Time::exactTime() - start) / length;
			float c = 1.0f - t;

			shader->SetActive();
			shader->SetUniform("uTint", Color(c, c, c));

			yield(0);
		}

		shader->SetActive();
		shader->SetUniform("uTint", Color::black);

		if(desc.callback)
			desc.callback();
	});
}

void PQStillImage::Update()
{
	
}

void PQStillImage::Draw()
{
	if(image)
		image->Draw();
}

void PQStillImage::OnTouchUp(float x, float y, int id)
{
	if(desc.tapOut && desc.callback)
		desc.callback();
}

void PQStillImage::OnBackPressed()
{
	if(desc.tapOut && desc.callback)
		desc.callback();
}
