#pragma once

#include <map>
#include <vector>

static int id = 0;

class Symbol {
public:
	std::string identifier;
	int m_id = -1;

public:
	Symbol(const std::string& str);
	Symbol(const char& ch);
	Symbol();
	Symbol(const Symbol& sym);

	Symbol& operator=(const Symbol& sym );
	operator std::string();
	//operator char();
	//operator int();
	bool operator==( const Symbol& sym );
	bool operator==( Symbol& sym );
	Symbol& operator+=( Symbol& sym );
	friend bool operator<( const Symbol& lhs, const Symbol& rhs );
	friend bool operator==( const Symbol& lhs, const Symbol& rhs );

	Symbol& pop_back();
	Symbol& push_back(const char& ch);
};

using SymbolList = std::vector < Symbol > ;

class Grammar {
private:
	Symbol starting_symbol;
	Symbol epsilon;
	SymbolList non_terminals;
	SymbolList terminals;
	std::map<int, std::vector<SymbolList>> production_rules;

	void as_prod_rules(std::string& str);

	SymbolList get_actives();
	SymbolList get_reachables();

	void pseudo_non_terminals();
	void length_reduction();

	int get_nt_index(const Symbol& sym);
	SymbolList those_that_product(SymbolList& sl);
	SymbolList to_pseudo_non_terminals(const SymbolList& sl);

	SymbolList find_as_list(const SymbolList& sl);
public:
	std::string m_fname;

	Grammar();
	~Grammar();

	void load(const std::string& filename, std::ifstream& file);
	void print();
	void print_file(std::ostream& os);

	void reduce();

	//EPSILON
	void kill_epsilon_rules();

	//CHAIN
	void kill_chains();

	//NORMAL FORM
	void to_normal_form();

	bool cyk(const std::string& input);
};

