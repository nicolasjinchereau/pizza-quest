/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "PQCredits.h"
#include "PQMenu.h"

PQCredits::PQCredits()
{

}

void PQCredits::Start()
{
	camera = AddChild(make_shared<Camera>());
	camera->SetSize(480);
	Camera::activeCamera(camera);

	background = AddChild(make_shared<Sprite>());
	background->Open("assets\\Images\\Menus\\BackgroundEmpty.png");
	background->layer = DrawLayer::Background;
	background->SetPos(camera->NormalizedToWorldX(0.5f),
					   camera->NormalizedToWorldY(0.5f));

	shared_ptr<Texture> headers[17]={
		make_shared<Texture>("assets/Images/credits/headers/PizzaQuest.png"),
		make_shared<Texture>("assets/Images/credits/headers/ProducedByGourmetGames.png"),
		make_shared<Texture>("assets/Images/credits/headers/CreatedAndDirectedBy.png"),
		make_shared<Texture>("assets/Images/credits/headers/Producers.png"),
		make_shared<Texture>("assets/Images/credits/headers/MarketingDirector.png"),
		make_shared<Texture>("assets/Images/credits/headers/OfficeManager.png"),
		make_shared<Texture>("assets/Images/credits/headers/ConceptArt.png"),
		make_shared<Texture>("assets/Images/credits/headers/GraphicArt.png"),
		make_shared<Texture>("assets/Images/credits/headers/EnvironmentModelling.png"),
		make_shared<Texture>("assets/Images/credits/headers/CharacterModellingAndAnimation.png"),
		make_shared<Texture>("assets/Images/credits/headers/MusicAndSoundEffects.png"),
		make_shared<Texture>("assets/Images/credits/headers/VoiceActors.png"),
		make_shared<Texture>("assets/Images/credits/headers/LevelDesign.png"),
		make_shared<Texture>("assets/Images/credits/headers/LeadProgrammer.png"),
		make_shared<Texture>("assets/Images/credits/headers/AIAndObjectivesProgramming.png"),
		make_shared<Texture>("assets/Images/credits/headers/QATesting.png"),
		make_shared<Texture>("assets/Images/credits/headers/SpecialThanksTo.png"),
	};

	shared_ptr<Texture> names[24]={
		make_shared<Texture>("assets/Images/credits/names/01.png"),
		make_shared<Texture>("assets/Images/credits/names/02.png"),
		make_shared<Texture>("assets/Images/credits/names/03.png"),
		make_shared<Texture>("assets/Images/credits/names/04.png"),
		make_shared<Texture>("assets/Images/credits/names/05.png"),
		make_shared<Texture>("assets/Images/credits/names/06.png"),
		make_shared<Texture>("assets/Images/credits/names/07.png"),
		make_shared<Texture>("assets/Images/credits/names/08.png"),
		make_shared<Texture>("assets/Images/credits/names/09.png"),
		make_shared<Texture>("assets/Images/credits/names/10.png"),
		make_shared<Texture>("assets/Images/credits/names/11.png"),
		make_shared<Texture>("assets/Images/credits/names/12.png"),
		make_shared<Texture>("assets/Images/credits/names/13.png"),
		make_shared<Texture>("assets/Images/credits/names/14.png"),
		make_shared<Texture>("assets/Images/credits/names/15.png"),
        make_shared<Texture>("assets/Images/credits/names/16.png"),
		make_shared<Texture>("assets/Images/credits/names/17.png"),
		make_shared<Texture>("assets/Images/credits/names/18.png"),
		make_shared<Texture>("assets/Images/credits/names/19.png"),
		make_shared<Texture>("assets/Images/credits/names/20.png"),
		make_shared<Texture>("assets/Images/credits/names/21.png"),
		make_shared<Texture>("assets/Images/credits/names/22.png"),
		make_shared<Texture>("assets/Images/credits/names/23.png"),
		make_shared<Texture>("assets/Images/credits/names/24.png")
	};

	const float titleInset = 0;
	const float headerInset = 10;
	const float nameInset = 30;
	const float spacing = 20;

	AddCredit(headers[0], titleInset, spacing);
	AddCredit(headers[1], headerInset, spacing);
	AddCredit(headers[2], headerInset, 0);
	AddCredit(names[14], nameInset, 0);
	AddCredit(names[10], nameInset, spacing);

	AddCredit(headers[3], headerInset, 0);
	AddCredit(names[6], nameInset, 0);
	AddCredit(names[18], nameInset, spacing);

	AddCredit(headers[4], headerInset, 0);
	AddCredit(names[18], nameInset, spacing);

	AddCredit(headers[6], headerInset, 0);
	AddCredit(names[1], nameInset, 0);
	AddCredit(names[8], nameInset, 0);
	AddCredit(names[16], nameInset, 0);
	AddCredit(names[21], nameInset, 0);
	AddCredit(names[11], nameInset, spacing);

	AddCredit(headers[7], headerInset, 0);
	AddCredit(names[10], nameInset, 0);
	AddCredit(names[16], nameInset, 0);
	AddCredit(names[1], nameInset, 0);
	AddCredit(names[4], nameInset, 0);
	AddCredit(names[15], nameInset, spacing);

	AddCredit(headers[8], headerInset, 0);
	AddCredit(names[21], nameInset, 0);
	AddCredit(names[11], nameInset, 0);
	AddCredit(names[8], nameInset, 0);
	AddCredit(names[4], nameInset, 0);
	AddCredit(names[16], nameInset, 0);
	AddCredit(names[1], nameInset, 0);
	AddCredit(names[22], nameInset, 0);
	AddCredit(names[15], nameInset, 0);
	AddCredit(names[6], nameInset, spacing);

	AddCredit(headers[9], headerInset, 0);
	AddCredit(names[21], nameInset, 0);
	AddCredit(names[16], nameInset, 0);
	AddCredit(names[11], nameInset, spacing);

	AddCredit(headers[10], headerInset, 0);
	AddCredit(names[5], nameInset, 0);
	AddCredit(names[10], nameInset, spacing);

	AddCredit(headers[11], headerInset, 0);
	AddCredit(names[7], nameInset, 0);
	AddCredit(names[6], nameInset, 0);
	AddCredit(names[18], nameInset, 0);
	AddCredit(names[3], nameInset, 0);
	AddCredit(names[23], nameInset, 0);
	AddCredit(names[4], nameInset, spacing);

	AddCredit(headers[12], headerInset, 0);
	AddCredit(names[10], nameInset, 0);
	AddCredit(names[18], nameInset, 0);
	AddCredit(names[6], nameInset, 0);
	AddCredit(names[8], nameInset, 0);
	AddCredit(names[18], nameInset, 0);
	AddCredit(names[11], nameInset, 0);
	AddCredit(names[16], nameInset, spacing);

	AddCredit(headers[13], headerInset, 0);
	AddCredit(names[14], nameInset, spacing);

	AddCredit(headers[14], headerInset, 0);
	AddCredit(names[2], nameInset, 0);
	AddCredit(names[13], nameInset, 0);
	AddCredit(names[17], nameInset, 0);
	AddCredit(names[0], nameInset, spacing);

	AddCredit(headers[15], headerInset, 0);
	AddCredit(names[0], nameInset, 0);
	AddCredit(names[17], nameInset, 0);
	AddCredit(names[13], nameInset, 0);
	AddCredit(names[2], nameInset, 0);
	AddCredit(names[12], nameInset, spacing);

	AddCredit(headers[16], headerInset, 0);
	AddCredit(names[9], nameInset, 0);
	AddCredit(names[19], nameInset, spacing);

	float Y = (float)credits[0].entry->GetHeight() / 2.0f;

	for(auto &cr : credits)
	{
		float indent = cr.indent + cr.entry->GetWidth() * 0.5f;
		cr.pos.set(20 + indent, Y);
		Y += cr.entry->GetHeight() + 10 + cr.trailingSpace;
	}

	currentY = camera->viewRect.h + (float)credits[0].entry->GetHeight();
	lastY = -(credits[credits.size() - 1].pos.y + (float)credits[credits.size() - 1].entry->GetHeight() / 2.0f);
}

PQCredits::~PQCredits()
{
	
}

void PQCredits::AddCredit(shared_ptr<Texture> entry, float indent, float trailingSpace)
{
	auto sp = AddChild(make_shared<Sprite>(entry));
	sp->layer = DrawLayer::UserInterface;
	credits.push_back(CreditEntry(sp, indent, trailingSpace)); 
}

void PQCredits::Update()
{
	currentY -= 50.0f * Time::deltaTime();
	
	if(currentY < lastY)
		Engine::SetState(make_shared<PQMenu>());
	
	for(auto &cr : credits)
		cr.entry->SetPos(cr.pos.x, cr.pos.y + currentY);
}

void PQCredits::OnTouchUp(float x, float y, int id)
{
	Engine::SetState(make_shared<PQMenu>());
}

void PQCredits::OnBackPressed()
{
	Engine::SetState(make_shared<PQMenu>());
}
