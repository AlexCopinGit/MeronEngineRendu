#include "A4Game/PlayerController.hpp"
#include <A4Engine/Sprite.hpp>
#include <A4Engine/ResourceManager.hpp>
#include <A4Engine/InputManager.hpp>
#include <chipmunk/chipmunk.h>
#include <A4Engine/RigidBodyComponent.h>
#include <A4Engine/SpritesheetComponent.hpp>
#include <A4Engine/GraphicsComponent.hpp>
#include <A4Engine/Transform.hpp>
#include <A4Engine/Sound.hpp>
#include "A4Engine/SoundSystem.h"
#include <A4Engine/PhysicsSystem.h>
#include <A4Engine/BoxShape.hpp>
#include <A4Game/Game.h>


Player::Player(entt::registry& regist, std::shared_ptr<Spritesheet> spritesheet) :
	registry(regist)
{
	player = CreatePlayer(spritesheet);
}

entt::entity& Player::GetEntity()
{
	return player;
}

entt::entity Player::CreatePlayer(std::shared_ptr<Spritesheet> spritesheet)
{
	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(ResourceManager::Instance().GetTexture("assets/sprites/spritesheets/BirdPlayer.png"));
	sprite->SetOrigin({ 0.5f, 0.5f });
	sprite->Resize(205, 205);
	sprite->SetRect(SDL_Rect{ 0, 0, 205, 205 });

	player = registry.create();
	registry.emplace<SpritesheetComponent>(player, spritesheet, sprite);
	registry.emplace<GraphicsComponent>(player, std::move(sprite));
	registry.emplace<Transform>(player);
	registry.emplace<RigidBodyComponent>(player, 30.f);


	//Create Shape for BIRD
	std::shared_ptr<Shape> shapeBird = std::make_shared<BoxShape>(128, 128);
	registry.get<RigidBodyComponent>(player).AddShape(PhysicsSystem::Instance().GetSpace(), shapeBird.get());
	shapeBird->SetCollisionType(CollisionTypes::PLAYER);
	registry.get<RigidBodyComponent>(player).SetPosition({ 600,200 });
	entityPhysics = &registry.get<RigidBodyComponent>(player);

	std::shared_ptr<Sound> jumpSound = ResourceManager::Instance().GetSound("assets/sounds/jump.wav");
	SoundSystem::Instance().AddSoundEffect(jumpSound);

	return player;
}
void Player::Update(float deltaTime)
{
	registry.get<SpritesheetComponent>(player).PlayAnimation(0);
	Vector2f velocity = Vector2f(0.f, 0.f);

	movementPlayer.jumpinterval -= deltaTime;

	if (movementPlayer.jumpinterval <= 0 && InputManager::Instance().IsActive("Jump"))
	{
		ResourceManager::Instance().GetSound("assets/sounds/jump.wav")->Play();
		entityPhysics->Impulse({ 0.f, movementPlayer.JUMPFORCE });
		movementPlayer.jumpinterval = movementPlayer.JUMPINTERVALVALUE;
	}
	float velY = entityPhysics->GetVelocity().y;
	entityPhysics->SetVelocity({ velocity.x, velY });
}
