#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

enum StatementType {
	BACKGROUND,
	MUSIC,
	SPRITE_LEFT,
	SPRITE_CENTER,
	SPRITE_RIGHT,
	CLEAR,
	TEXT,
	FONT,
	FADE,
	DELAY
};

struct Statement {
	StatementType type;
	std::string content;
	std::wstring wtext;
};

class Script {
	private:
		std::wifstream file;
	public:
		bool open(std::string path);
		std::vector<Statement> parse();
};

#endif