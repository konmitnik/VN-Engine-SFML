#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Config.h"
#include "Script.h"
#include "Sprite.h"

Parameters opts;
std::string root;

sf::Music music;

sf::Texture backgroundTex;
sf::Sprite background;

sf::RectangleShape fadeRect;
int fade;

std::size_t instruct;
std::vector<Statement> statements;

sf::RectangleShape textRect;

sf::Text text;
sf::Font textFont;

sf::Clock textClock;
sf::Clock delayClock;

std::wstring textStr;
bool hasPrintedText;
std::string::size_type textPos;

int delay;

sf::RectangleShape rectPause;
sf::Text textPause;
sf::Font pauseFont;
bool pause;

MySprite list_sprites[TOTAL_POSITION];

bool hideText;
bool fadeOn;

sf::String wrapText(sf::String string, unsigned width, const sf::Font& font, unsigned charicterSize, bool bold);
void nextStatement();
void update();

int main() {
	Parameters opts;
	Config conf;
	opts = conf.parse("game/config.cfg");

	Script script;
	if (!script.open("game/script.txt")) {
		std::cout << "Error of open script file" << std::endl;
	}
	statements = script.parse();

	sf::RenderWindow window(sf::VideoMode(opts.width, opts.height), opts.title, sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	instruct = -1;
	hasPrintedText = false;
	textPos = 0;
	hideText = false;
	delay = 0;
	fade = 0;
	fadeOn = false;
	pause = false;

	textRect.setPosition(sf::Vector2f(16.f, opts.height - 16 - 200 /*должно быть 128 вместо 200*/));
	textRect.setSize(sf::Vector2f(opts.width - 32.f, 200.f)); //было 128 на высоте
	textRect.setFillColor(sf::Color(255, 255, 255, 255)); //последний параметр был 100
	textRect.setOutlineThickness(2);
	textRect.setOutlineColor(sf::Color(0, 0, 0, 255));

	fadeRect.setSize(sf::Vector2f(opts.width, opts.height));
	fadeRect.setPosition(sf::Vector2f(0, 0));
	fadeRect.setFillColor(sf::Color::Black);

	rectPause.setPosition(sf::Vector2f(0, 0));
	rectPause.setSize(sf::Vector2f(opts.width, opts.height));
	rectPause.setFillColor(sf::Color(0, 0, 0, 175));

	if (!textFont.loadFromFile("game/font/script.ttf")) {
		std::cout << "Error of load font /font/script.ttf" << std::endl;
	} else if (!pauseFont.loadFromFile("game/font/pause.ttf")) {
		std::cout << "Error of load font /font/pause.ttf";
	} else {
		text.setFont(textFont);
		text.setFillColor(sf::Color::Black);
		text.setPosition(textRect.getPosition() + sf::Vector2f(7, 5));
		text.setCharacterSize(32);

		textPause.setFont(pauseFont);
		textPause.setFillColor(sf::Color::White);
		textPause.setCharacterSize(48); // было 24 вместо 48
		textPause.setPosition(opts.width / 2 - 70, opts.height / 2 - 20);
		textPause.setString("PAUSE");
	}

	nextStatement();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) && delay == 0 && !fadeOn) {
				if ((event.key.code == sf::Keyboard::Enter || event.mouseButton.button == sf::Mouse::Left) && !pause) {
					nextStatement();
				} else if (event.key.code == sf::Keyboard::Escape || event.mouseButton.button == sf::Mouse::Right) {
					if (!pause) {
						pause = true;
						if(music.getStatus() == sf::SoundSource::Status::Playing) music.pause();
					} else {
						pause = false;
						if(music.getStatus() == sf::SoundSource::Status::Paused) music.play();
					}
				}
			}
		}

		
		update();

		window.clear();

		window.draw(background);
		for (int i = LEFT; i < TOTAL_POSITION; i++) {
			if (list_sprites[i].getShow()) {
				window.draw(list_sprites[i].getSprite());
			}
		}
		if (!hideText) {
			window.draw(textRect);
			window.draw(text);
		}
		if (fadeOn) {
			window.draw(fadeRect);
		}
		if (pause) {
			window.draw(rectPause);
			window.draw(textPause);
		}

		window.display();
	}

	return 0;
}

sf::String wrapText(sf::String string, unsigned width, const sf::Font& font, unsigned charicterSize, bool bold = false) {
	unsigned currentOffset = 0;
	bool firstWord = true;
	std::size_t wordBegining = 0;

	for (std::size_t pos(0); pos < string.getSize(); ++pos) {
		auto currentChar = string[pos];
		if (currentChar == '\n') {
			currentOffset = 0;
			firstWord = true;
			continue;
		}
		else if (currentChar == ' ') {
			wordBegining = pos;
			firstWord = false;
		}

		auto glyph = font.getGlyph(currentChar, charicterSize, bold);
		currentOffset += glyph.advance;

		if (!firstWord && currentOffset > width) {
			pos = wordBegining;
			string[pos] = '\n';
			firstWord = true;
			currentOffset = 0;
		}
	}

	return string;
}

void nextStatement() {
	Parameters opts;
	Config conf;
	opts = conf.parse("game/config.cfg");

	if (instruct + 1 == statements.size()) return;

	instruct++;

	Statement s = statements.at(instruct);
	if (s.type == StatementType::BACKGROUND) {
		std::string path = "game/background/" + s.content;
		if (!backgroundTex.loadFromFile(path)) {
			std::cout << "Error of load background from file " << s.content << std::endl;
		}
		background.setTexture(backgroundTex, true);
		background.setPosition(sf::Vector2f(0, 0));
		background.setScale(sf::Vector2f(opts.width / background.getLocalBounds().width, opts.height / background.getLocalBounds().height));

		nextStatement();
		return;
	}

	if (s.type == StatementType::TEXT) {
		textStr = s.wtext;
		text.setString(textStr[0]);
		hasPrintedText = false;
		textPos = 0;
		return;
	}

	if (s.type == StatementType::DELAY) {
		delay = atoi(s.content.c_str());
		hideText = true;
		delayClock.restart();
	}

	if (s.type == StatementType::FONT) {
		if (!textFont.loadFromFile("game/font/" + s.content)) {
			std::cout << "Error of load font from file" << s.content << std::endl;
		}
		nextStatement();
		return;
	}

	if (s.type == StatementType::SPRITE_LEFT) {
		list_sprites[LEFT].loadSprite("game/sprite/" + s.content, LEFT);
		nextStatement();
		return;
	}

	if (s.type == StatementType::SPRITE_CENTER) {
		list_sprites[CENTER].loadSprite("game/sprite/" + s.content, CENTER);
		nextStatement();
		return;
	}

	if (s.type == StatementType::SPRITE_RIGHT) {
		list_sprites[RIGHT].loadSprite("game/sprite/" + s.content, RIGHT);
		nextStatement();
		return;
	}

	if (s.type == StatementType::CLEAR) {
		if (s.content == "left") {
			list_sprites[LEFT].hide();
		} else if (s.content == "center") {
			list_sprites[CENTER].hide();
		} else if (s.content == "right") {
			list_sprites[RIGHT].hide();
		}
		nextStatement();
	}

	if (s.type == StatementType::MUSIC) {
		if (s.content == "stop") {
			music.stop();
		} else {
			std::string path = "game/music/" + s.content;
			if (!music.openFromFile(path)) {
				std::cout << "Error of load music file " << s.content << std::endl;
			}
			music.setLoop(true);
			music.play();
		}

		nextStatement();
		return;
	}

	if (s.type == StatementType::FADE) {
		std::stringstream ss(s.content);
		int n = 0;
		std::string way;

		ss >> way; //in or out
		ss >> n; //fade speed

		if (way == "in") {
			fade = n;
			fadeOn = true;
			fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
			hideText = true;
		} else if (way == "out") {
			fade = -n;
			fadeOn = true;
			fadeRect.setFillColor(sf::Color(0, 0, 0, 255));
			hideText = true;
		} else {
			std::cout << "Unknown fade statement at " << instruct << " line" << std::endl;
		}
	}
}

void update() {
	//Text printing animation
	if (!hasPrintedText && textClock.getElapsedTime().asMilliseconds() > 30) {
		textPos++;
		if (textPos >= textStr.size()) {
			hasPrintedText = false;
		} else {
			std::wstring newStr = text.getString().toWideString();
			newStr += textStr[textPos];
			sf::String newsfStr(newStr);
			sf::String wrappedStr = wrapText(newsfStr, static_cast<unsigned int>(textRect.getSize().x), textFont, 36.f); //было 24 вместо 36
			text.setString(wrappedStr);
		}

	}

	//Delay logic
	if (delay > 0 && delayClock.getElapsedTime().asMilliseconds() > delay) {
		hideText = false;
		delay = 0;
		nextStatement();
	}

	if (fadeOn) {
		sf::Color c = fadeRect.getFillColor();

		//Change fadeRect alpha


		//If fade animation is finished, continue executing statements
		if (fade < 0 && fade + c.a < 0) {
			fadeOn = false;
			hideText = false;
			nextStatement();
			return;
		} else if (fade > 0 && fade + c.a > 255) {
			fade = 0; // We do not set fadeOn to false to allow executing next statements without losing fade rect state
			hideText = false;
			nextStatement();
			return;
		} else {
			//If animation is in process, set rect color
			c.a += fade;
			fadeRect.setFillColor(c);
		}
	}
}