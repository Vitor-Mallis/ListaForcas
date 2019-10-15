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

void Game::Run()
{
	bool exit = 0;
	int number = -1;

	while (!exit) {
		std::cout << "Digite o numero do exercicio. Os exercicios 1 a 3 estao juntos no numero 1:\n";
		std::cout << "0 - Sair do programa\n";
		std::cout << "1 - Criar caixas, circulos e linhas e alterar a gravidade\n";
		std::cout << "2 - Para cada caixa criada, aumenta a restituicao\n";
		std::cout << "3 - Para cada caixa criada na rampa, aumenta a friccao\n";
		std::cout << "4 - 6 caixas empilhadas e 6 circulos empilhados\n";
		std::cout << "5 - Muro de 10x10 caixas\n";
		std::cout << "6 - Objeto com multiplas fixtures\n\n";
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
		/*case 1:
			createWindow();
			Exercicio1_3();
			delete world;
			number = -1;
			break;
		case 2:
			createWindow();
			Exercicio4();
			delete world;
			number = -1;
			break;
		case 3:
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
	world = new PhysicsWorld();

	window = new sf::RenderWindow(sf::VideoMode(800, 600), "Física - Lista 1");
	window->setFramerateLimit(60);

	renderer = new Renderer();
}
