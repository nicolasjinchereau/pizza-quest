/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQProgressBar.h"
#include "Engine.h"

PQProgressBar::PQProgressBar()
{
	
}

PQProgressBar::~PQProgressBar()
{

}

void PQProgressBar::Start()
{

}

bool PQProgressBar::OpenImages(const string &fnBackground, const string &fnBorder, const string &fnFiller)
{
	background = AddChild(make_shared<Sprite>());
	border = AddChild(make_shared<Sprite>());
	filler = AddChild(make_shared<Sprite>());

	background->layer = DrawLayer::UserInterface;
	filler->layer = DrawLayer::UserInterface + 1;
	border->layer = DrawLayer::UserInterface + 2;

	if(!background->Open(fnBackground.c_str()))
		Trace("failed to initialize progress bar");

	if(!border->Open(fnBorder.c_str()))
		Trace("failed to initialize progress bar");
	
	if(!filler->Open(fnFiller.c_str()))
		Trace("failed to initialize progress bar");

	return true;
}

void PQProgressBar::SetPosition(float wx, float wy)
{
	position.set(wx, wy);
	background->SetPos(wx, wy);
	border->SetPos(wx, wy);
	filler->SetPos(wx, wy);
}

void PQProgressBar::SetProgress(float prg)
{
	progress = min(max(prg, 0.0f), 1.0f);
	filler->clipBorder(Rect(0, 0, (1.0f - progress) * filler->GetWidth(), 0));
}

void PQProgressBar::AddProgress(float prg)
{
	progress = min(max(progress + prg, 0.0f), 1.0f);
	filler->clipBorder(Rect(0, 0, (1.0f - progress) * filler->GetWidth(), 0));
}

float PQProgressBar::GetProgress()
{
	return progress;
}
