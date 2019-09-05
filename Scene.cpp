#include "Scene.h"
#include "TilesGenerator.h"
#include "RectStopAnother.h"
#include "RectCollisionComponent.h"
#include <iostream>

void Scene::addObject(std::shared_ptr<Object> toAdd)
{
	objects.push_back(toAdd);
}

Scene::Scene(std::string sceneSourceName) :
	window(sf::VideoMode(1920, 1080), "do sth", sf::Style::Default), debugger(window), gravity(50.f)
{
	sf::Vector2i size{ 200,145 };
	mp::TilesGenerator generator("pierwsza mapa.tmx");
	generator.generate(*this);
	auto object = std::make_shared<Object>(size);
	sf::Vector2f forPos{ 0.f,0.f };
	sf::Vector2f speed{ 200.f,200.f };
	auto position = std::make_unique<PositionComponent>(*object, forPos, speed);
	object->setPosition(std::move(position));
	std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
	if (!texture->loadFromFile("Viking.png"))
		throw std::invalid_argument("No such texture to load file for map.");
	sf::Vector2i temp{ 0,0 };
	std::unique_ptr<GraphicComponent> gComponent =
		std::make_unique<GraphicComponent>(*object, texture, temp,0.5f);
	std::shared_ptr<KeyboardComponent> keyboard = std::make_shared<KeyboardComponent>(menager,*object);
	object->setGraphic(std::move(gComponent));
	objects.push_back(object); 
	object->addComponent(keyboard);
	std::shared_ptr<RectCollisionComponent> col = std::make_shared<RectCollisionComponent>(std::move(std::make_unique<RectStopAnother>()));
	col->setOwner(*object);
	object->addComponent(col);
	myHero = col;
	debugger.addCollidable(col);
	for (auto coll : tempCollidableCollection)
	{
		auto rect = std::dynamic_pointer_cast<RectCollisionComponent>(coll);
		if (rect != nullptr)
			debugger.addCollidable(rect);
	}
	gravity.addObject(object);
}

bool Scene::update()
{
	sf::Time time = clock.restart();
	gravity.update(time.asSeconds());
	for (auto& object : objects)
	{
		object->update(time.asSeconds());
	}
	for (auto& collidable : tempCollidableCollection)
	{
		collidable->resolveCollision(*myHero);
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
	std::cout << myHero->getOwner().getPosition().getCurrentSpeed().y << std::endl;
	window.display();
	window.clear(sf::Color::Cyan);
	return true;
}
