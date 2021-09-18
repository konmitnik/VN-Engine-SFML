#include "Sprite.h"
#include "Config.h"

#include <iostream>

MySprite::MySprite() {}

void MySprite::loadSprite(std::string path, Position pos) {
	this->isShow = true;

	Parameters opts;
	Config conf;
	opts = conf.parse("game/config.cfg");

	double x, y, scalex = 1, scaley = 1;
	if (pos == LEFT) {
		x = 0;
	}
	else if (pos == CENTER) {
		x = opts.width / 3;
	}
	else if (pos == RIGHT) {
		x = opts.width / 3 * 2;
	}

	if (!this->spriteText.loadFromFile(path)) {
		std::cout << "Error of load sprite from image " << path << std::endl;
	}

	this->sprite.setTexture(this->spriteText, true);
	scalex = (opts.width / 3) / this->sprite.getLocalBounds().width;
	scaley = (opts.height - 200 - 32) / this->sprite.getLocalBounds().height;
	this->sprite.setScale(sf::Vector2f(scalex, scaley));
	y = opts.height - this->sprite.getGlobalBounds().height - 200 - 32;
	this->sprite.setPosition(sf::Vector2f(x, y));
}

sf::Sprite MySprite::getSprite() {
	return this->sprite;
}

bool MySprite::getShow() {
	return this->isShow;
}

void MySprite::hide() {
	this->isShow = false;
}