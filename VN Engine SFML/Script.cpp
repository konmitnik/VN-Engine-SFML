#include "Script.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>

bool Script::open(std::string filename) {
	file.open(filename);
	return file.good();
}

std::vector<Statement> Script::parse() {
	file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10ffffUL, std::codecvt_mode::consume_header>));

	std::vector<Statement> v;

	std::wstring stStr;
	int line = 0;

	while (std::getline(file, stStr)) {
		std::wstring type;
		Statement s;
		line++;

		for (int i = 0; i < stStr.size(); i++) {
			if (stStr[i] == L' ') {
				type = stStr.substr(0, i);

				if (type == L"txt") {
					s.wtext = stStr.substr(i + 1);
				} else {
					char mbstr[255];
					std::wcstombs(mbstr, stStr.substr(i + 1).c_str(), 255);
					std::string cstr(mbstr);
					s.content = cstr;
				}
				break;
			}
		}

		if (type == L"music") {
			s.type = StatementType::MUSIC;
		} else if (type == L"bg") {
			s.type = StatementType::BACKGROUND;
		} else if (type == L"delay") {
			s.type = StatementType::DELAY;
		} else if (type == L"txt") {
			s.type = StatementType::TEXT;
		} else if (type == L"font") {
			s.type = StatementType::FONT;
		} else if (type == L"sprite_left") {
			s.type = StatementType::SPRITE_LEFT;
		} else if (type == L"sprite_center") {
			s.type = StatementType::SPRITE_CENTER;
		} else if (type == L"sprite_right") {
			s.type = StatementType::SPRITE_RIGHT;
		} else if (type == L"clear") {
			s.type = StatementType::CLEAR;
		} else if (type == L"fade") {
			s.type = StatementType::FADE;
		} else {
			std::cout << "Unknown script token at line" << line <<  std::endl;
			continue;
		}

		v.push_back(s);
	}

	file.close();
	return v;
}