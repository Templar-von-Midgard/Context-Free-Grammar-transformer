#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Types.hpp"
#include <functional>
#include <algorithm>

using string = std::string;
using ifstream = std::ifstream;
using ostream = std::ostream;

namespace IO {
	namespace Input {
		inline void for_each_line(ifstream& file, std::function<void(std::string&)> function) {
			uint64 pos = file.tellg();
			string token;
			while( std::getline(file, token) ) {
				function(token);
			}
			file.seekg(pos);
		}

		inline uint64 count_lines(ifstream& file) {
			uint64 pos = file.tellg();
			uint64 count = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
			file.seekg(pos);
			return count + 1;
		}
	}

	namespace Output {
		ostream& Logger = std::cout;

		inline void Log_var(byte1* var) {

		}
	}

	namespace String {
		inline void removeCharsFromString(string &str, char* charsToRemove) {
			for( unsigned int i = 0; i < strlen(charsToRemove); ++i ) {
				str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
			}
		}

		inline void eat_whitespace(string& str) {
			removeCharsFromString(str, " \t");
		}
	}
}