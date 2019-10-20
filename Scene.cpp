#include "Scene.h"
#include "TilesGenerator.h"
#include "RectMovable.h"
#include "RectCollisionComponent.h"
#include "DynamicPositionComponent.h"
#include <iostream>
void Scene::addObject(std::shared_ptr<Object> toAdd)
{
	objects.push_back(toAdd);
}

Scene::Scene(std::string sceneSourceName, sf::Vector2i resolution) :
	window(sf::VideoMode(resolution.x, resolution.y), "do sth", sf::Style::Default), debugger(window), gravity(1000.f), collidable(500, 6, {0,0, 3200,1500 })
{
	sf::Vector2i size{ 55,40 };
	sf::Vector2i textureSize{200, 145};
	mp::TilesGenerator generator("pierwsza mapa.tmx");
	generator.generate(*this);
	auto object = std::make_shared<Object>(size);
	sf::Vector2f forPos{ 0.f,0.f };
	sf::Vector2f speed{ 200.f,750.f };
	auto position = std::make_unique<DynamicPositionComponent>(object, forPos, speed);
	object->setPosition(std::move(position));
	std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
	if (!texture->loadFromFile("Viking.png"))
		throw std::invalid_argument("No such texture to load file for map.");
	sf::Vector2i temp{ 0,0 };
	std::unique_ptr<GraphicComponent> gComponent =
		std::make_unique<GraphicComponent>(object, texture, temp, textureSize);
	std::shared_ptr<KeyboardComponent> keyboard = std::make_shared<KeyboardComponent>(menager,object,gravity);
	object->setGraphic(std::move(gComponent));
	objects.push_back(object); 
	object->addComponent(keyboard);
	std::shared_ptr<RectCollisionComponent> col = std::make_shared<RectCollisionComponent>(std::move(std::make_unique<RectMovable>(gravity)));
	col->setOwner(object);
	object->addComponent(col);
	myHero = col;
	debugger.addCollidable(col);
	view = std::make_unique<View>(object, 1920.f, 1080.f);
	window.setView(view->getView());
	for (auto coll : movable)
	{
		auto rect = std::dynamic_pointer_cast<RectCollisionComponent>(coll);
		if (rect != nullptr)
			debugger.addCollidable(rect);
	}
	gravity.addObject(object);
	clock.restart(); // to start measuring time after scene's creation
	std::cout << collidable.getSize() << std::endl;
}

bool Scene::update()
{
	sf::Time time = clock.restart();
	gravity.update(time.asSeconds());
	for (auto& object : objects)
	{
		object->update(time.asSeconds());
	}
	std::vector<std::shared_ptr<CollisionComponent>> temp = collidable.possibleOverlaps(myHero);
	//std::cout << temp.size() << std::endl;
	for (auto& collidable : temp)
	{
		collidable->resolveCollision(*myHero);
		auto rect = std::dynamic_pointer_cast<RectCollisionComponent>(collidable);
		if (rect != nullptr)
			debugger.setSpecial(rect, true);
	}
		for (auto& object : objects)
		object->draw(window);
	debugger.drawCollidableSquares();
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			return false;
		}
		
	}
	view->update();
	window.setView(view->getView());
	window.display();
	window.clear(sf::Color::Cyan);
	return true;
}

void Scene::addCollidable(std::shared_ptr<CollisionComponent> toAdd)
{
	//movable.push_back(toAdd);
	collidable.add(toAdd);
	auto rect = std::dynamic_pointer_cast<RectCollisionComponent>(toAdd);
	if (rect != nullptr)
		debugger.addCollidable(rect);
}
