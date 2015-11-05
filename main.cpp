// FONYA_szorgalmi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Grammar.hpp"

void get_input(Grammar& g, std::string& in);
void print_instructions();
void load_grammar(Grammar& g, std::string& fname);
void print_file(Grammar& g);

int _tmain(int argc, _TCHAR* argv[])
{
	std::string in;
	Grammar g;
	load_grammar(g, in);
	print_instructions();
	do {
		std::cout << g.m_fname << "*>>> ";
		get_input(g, in);
	} while( in != "exit" );
	return 0;
}

void get_input(Grammar& g, std::string& in) {
	std::cin >> in;
	if( in == "reduce" ) { g.reduce(); return; }
	if( in == "print" ) { g.print(); return; }
	if( in == "epsilon" ) { g.kill_epsilon_rules(); return; }
	if( in == "chain" ) { g.kill_chains(); return; }
	if( in == "normal" ) { g.to_normal_form(); return; }
	if( in == "load" ) { load_grammar(g, in); return; }
	if( in == "all" ) {
		g.reduce();
		g.kill_epsilon_rules();
		g.kill_chains();
		g.to_normal_form();
		g.print();
	}
	if( in == "printf" ) { print_file(g); return; }
	/*if( in == "cyk" ) {
		std::cout << "Input string: ";
		std::cin >> in;
		g.cyk(in);
	}*/
	if( in == "help" ) { print_instructions(); return; }
}

void print_instructions() {
	std::cout << "input \t-> function\n"
		<< "help \t-> show this message\n"
		<< "print \t-> print the grammar\n"
		<< "printf \t-> print grammar to a file\n"
		<< "reduce \t-> reduce the grammar\n"
		<< "epsilon -> kill epsilon rules\n"
		<< "chain \t-> kill chain rules\n"
		<< "normal \t-> get to normal form\n"
		<< "load \t-> load another file\n"
		<< "all \t-> run all and print\n"
		//<< "cyk \t-> CYK algorithm\n"
		<< "exit \t-> exit the program\n";
}

void load_grammar(Grammar& g, std::string& fname) {
	std::ifstream inf;
	do {
		std::cout << "File name: ";
		std::cin >> fname;
		inf.open(fname.c_str());
		if( inf.is_open() ) break;
		std::cout << "Invalid file name!\n";
	} while( true );
	g.load(fname, inf);
	inf.close();
}

void print_file(Grammar& g) {
	std::ofstream of;
	std::string in;
	std::cout << "File name: ";
	std::cin >> in;
	of.open(in);
	g.print_file(of);
	of.close();
}