#include "Game.h"
#include <iostream>

Game::Game()
{
	world = NULL;
	window = NULL;
	renderer = NULL;
	srand(time(NULL));
}

Game::~Game()
{
	delete world;
	delete window;
	delete renderer;
}

void Game::UpdateDeltaTime()
{
	dt = dtClock.restart().asSeconds();
}

void Game::UpdateSFMLEvents()
{
	//Checking if the user closes the window
	while (window->pollEvent(sfEvent)) {
		if (sfEvent.type == sf::Event::Closed) window->close();
	}
}

void Game::Update()
{
	UpdateDeltaTime();
	//UpdateSFMLEvents();
}

void Game::Render()
{
	//Physics engine update tick
	b2Body *bodies = world->Step(dt);

	window->clear(sf::Color::White);

	//Going through the bodies list and drawing each one at its updated position and rotation
	for (b2Body *bodyIterator = bodies; bodyIterator != 0; bodyIterator = bodyIterator->GetNext()) {
		DrawBody(bodyIterator);
	}

	window->display();
}

void Game::DrawBody(b2Body * body)
{
	for (b2Fixture *fixtureIterator = body->GetFixtureList(); fixtureIterator != 0; fixtureIterator = fixtureIterator->GetNext()) {

		const b2Transform& xf = fixtureIterator->GetBody()->GetTransform();

		switch (fixtureIterator->GetType())
		{
		case b2Shape::e_circle:
		{
			b2CircleShape* circle = (b2CircleShape*)fixtureIterator->GetShape();

			b2Vec2 center = b2Mul(xf, circle->m_p);
			float radius = SCALE * circle->m_radius;
			float centerX = SCALE * center.x;
			float centerY = SCALE * center.y;
			
			//Calling the SFML renderer
			renderer->DrawCircle(radius, centerX - radius, centerY - radius, window, sf::Color::Black);
		}
		break;

		case b2Shape::e_polygon:
		{
			b2PolygonShape* poly = (b2PolygonShape*)fixtureIterator->GetShape();
			int32 vertexCount = poly->m_count;
			b2Assert(vertexCount <= b2_maxPolygonVertices);
			
			float **vertices;
			vertices = new float*[vertexCount];
			float rotation = fixtureIterator->GetBody()->GetAngle() * 180 / b2_pi;

			for (int i = 0; i < vertexCount; i++) {
				b2Vec2 multipliedVertex = b2Mul(xf, poly->m_vertices[i]);

				vertices[i] = new float[2];
				vertices[i][0] = SCALE * multipliedVertex.x;
				vertices[i][1] = SCALE * multipliedVertex.y;
			}

			renderer->DrawPolygon(vertexCount, vertices, window, sf::Color::Black);
		}

		break;
		case b2Shape::e_edge:
		{
			b2EdgeShape* edge = (b2EdgeShape*)fixtureIterator->GetShape();
			int32 vertexCount;

			b2Vec2 vertices[b2_maxPolygonVertices];
			int i = 0;

			if (edge->m_hasVertex0) {
				vertices[i] = b2Mul(xf, edge->m_vertex0);
				i++;
			}

			vertices[i] = b2Mul(xf, edge->m_vertex1); i++;
			vertices[i] = b2Mul(xf, edge->m_vertex2); i++;

			if (edge->m_hasVertex3) {
				vertices[i] = b2Mul(xf, edge->m_vertex3);
				i++;
			}

			vertexCount = i;

			float pointStart[2] = { SCALE * vertices[0].x, SCALE * vertices[0].y };
			float pointFinish[2] = { SCALE * vertices[1].x, SCALE * vertices[1].y };

			renderer->DrawLine(vertexCount, pointStart, pointFinish, window, sf::Color::Black);
		}

		break;

		}
	}
}

float32 Game::WorldToBox(float32 value)
{
	value /= SCALE;
	return value;
}

b2Vec2 Game::WorldToBox(b2Vec2 vector)
{
	vector.x /= SCALE;
	vector.y /= SCALE;
	return vector;
}

b2Vec2 * Game::WorldToBox(b2Vec2 *vectors, float32 size)
{
	for (int i = 0; i < size; i++) {
		vectors[i].x /= SCALE;
		vectors[i].y /= SCALE;
	}

	return vectors;
}

b2Vec2 Game::CalculateVector(float32 magnitude, float32 angle)
{
	float32 angleRadians = angle * b2_pi / 180;
	float32 vx = magnitude * cos(angleRadians);
	float32 vy = magnitude * sin(angleRadians);
	b2Vec2 vec(vx, vy);
	return vec;
}

void Game::Dominoes() {
	
	b2Vec2 groundCoords1 = { 5.f, 595.f };
	b2Vec2 groundCoords2 = { 795.f, 595.f };

	b2Body *firstDomino;

	b2Vec2 domDimensions = { 20.f, 100.f };
	b2Vec2 domPosition = { 250.f, 545.f };

	b2Vec2 forceVector = { 150.f, 0.f };
	b2Vec2 forcePoint = { -1.f, -1.f };

	// Creating the ground
	world->CreateLine(b2_staticBody, WorldToBox(groundCoords1), WorldToBox(groundCoords2), 0.f, 0.f, 1.f);

	//Creating the first domino
	firstDomino = world->CreateBox(b2_dynamicBody, WorldToBox(domPosition), WorldToBox(domDimensions), 1.f, 0.f, 1.f);
	domPosition.x += 50.f;

	//Creating the dominoes
	for (int i = 0; i < 5; i++) {
		world->CreateBox(b2_dynamicBody, WorldToBox(domPosition), WorldToBox(domDimensions), 1.f, 0.f, 1.f);
		domPosition.x += 50.f;
	}

	while (window->isOpen()) {
		//Checking for exercise-specific events
		while (window->pollEvent(sfEvent)) {
			if (sfEvent.type == sf::Event::Closed) window->close();

			else if (sfEvent.type == sf::Event::KeyPressed) {
				if(sfEvent.key.code == sf::Keyboard::F) firstDomino->ApplyForce(forceVector, firstDomino->GetWorldPoint(forcePoint), true);
			}
		}

		Update();
		Render();
	}
}

void Game::Projectile()
{
	b2Vec2 bulletPosition = { 10.f, 590.f };
	float32 forceMagnitude = 10.f;
	float32 forceAngle = -45.f;
	b2Vec2 forceVector = CalculateVector(forceMagnitude, forceAngle);
	b2Body *bullet = NULL;

	while (window->isOpen()) {
		while (window->pollEvent(sfEvent)) {
			if (sfEvent.type == sf::Event::Closed) window->close();

			else if (sfEvent.type == sf::Event::KeyPressed) {
				if (sfEvent.key.code == sf::Keyboard::Space) {
					forceVector = CalculateVector(forceMagnitude, forceAngle);
					bullet = NULL;
					bullet = world->CreateCircle(b2_dynamicBody, WorldToBox(bulletPosition), WorldToBox(10.f), 1.f, 0.f, 1.f);
					bullet->ApplyLinearImpulse(forceVector, bullet->GetWorldCenter(), true);
				}
				else if (sfEvent.key.code == sf::Keyboard::Left) {
					if (forceMagnitude > 0.f) forceMagnitude -= 1.f;
				}
				else if (sfEvent.key.code == sf::Keyboard::Right) {
					if (forceMagnitude < 20.f) forceMagnitude += 1.f;
				}
				else if (sfEvent.key.code == sf::Keyboard::Up) {
					if (forceAngle > -90.f) forceAngle -= 1.f;
				}
				else if (sfEvent.key.code == sf::Keyboard::Down) {
					if (forceAngle < 0.f) forceAngle += 1.f;
				}
			}
		}

		Update();
		Render();
	}
}

void Game::Run()
{
	bool exit = 0;
	int number = -1;

	while (!exit) {
		std::cout << "Digite o numero do exercicio:\n";
		std::cout << "0 - Sair do programa\n";
		std::cout << "1 - Dominos\n";
		std::cout << "2 - Projetil\n";
		std::cout << "3 - Controller\n\n";
		std::cout << "Digite o numero: ";
		std::cin >> number;
		std::cout << std::endl;

		if (std::cin.fail()) {
			number = -1;
			std::cin.clear();
			std::cin.ignore(256, '\n');
		}

		switch (number) {
		case 0:
			exit = 1;
			break;
		case 1:
			createWindow();
			Dominoes();
			delete world;
			number = -1;
			break;
		case 2:
			createWindow();
			Projectile();
			delete world;
			number = -1;
			break;
			/*case 3:
			createWindow();
			Exercicio5();
			delete world;
			number = -1;
			break;
		case 4:
			createWindow();
			Exercicio6();
			delete world;
			number = -1;
			break;
		case 5:
			createWindow();
			Exercicio7();
			delete world;
			number = -1;
			break;
		case 6:
			createWindow();
			Exercicio8();
			delete world;
			number = -1;
			break;*/
		default:
			break;
		}
	}
}

void Game::createWindow()
{
	b2Vec2 gravity(0.f, 9.8f);

	world = new PhysicsWorld(gravity);

	window = new sf::RenderWindow(sf::VideoMode(800, 600), "Física - Lista 1");
	window->setFramerateLimit(60);

	renderer = new Renderer();
}
