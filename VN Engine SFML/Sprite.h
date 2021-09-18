#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <string>
#include <SFML/Graphics.hpp>

#include "Config.h"

enum Position {
	LEFT,
	CENTER,
	RIGHT,
	TOTAL_POSITION
};

class MySprite {
	private:
		sf::Texture spriteText;
		sf::Sprite sprite;
		bool isShow;
	public:
		MySprite();
		void loadSprite(std::string path, Position pos);
		sf::Sprite getSprite();
		bool getShow();
		void hide();
};

#endif // !_SPRITE_H_
