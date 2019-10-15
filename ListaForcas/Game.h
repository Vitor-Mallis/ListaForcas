#pragma once

#include "Renderer.h"
#include "PhysicsWorld.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

class Game {

public:
	Game();
	~Game();

	void UpdateDeltaTime();
	void UpdateSFMLEvents();
	void Update();
	void Render();

	void DrawBody(b2Body *body);
	
	float32 WorldToBox(float32 value);
	b2Vec2 WorldToBox(b2Vec2 vector);
	b2Vec2 *WorldToBox(b2Vec2 *vectors, float32 size);

	void Run();

private:
	//Scale for converting from world coordinates to Box2D coordinates
	const float32 SCALE = 30.0f;
	
	void createWindow();

	//DeltaTime
	float dt;

	sf::Clock dtClock;

	PhysicsWorld *world;

	Renderer *renderer;

	sf::RenderWindow *window;
	sf::Event sfEvent;
};