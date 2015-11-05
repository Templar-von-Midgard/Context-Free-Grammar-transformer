#include "stdafx.h"
#include "Grammar.hpp"
#include <functional>
#include <cmath>

std::vector<SymbolList> permutations(const SymbolList& str, const SymbolList& of);

SymbolList string_to_symbols(const std::string& str) {
	SymbolList ret;
	//std::cout << str << '\n';
	std::istringstream iss(str);
	std::string tmp;
	while( std::getline(iss,tmp, ',') ) { 
		//std::cout << tmp << '\n';
		ret.push_back(Symbol(tmp));
	}
	return ret;
}

std::ostream& operator<<( std::ostream& os, const Symbol& sym ) {
	os << sym.identifier;
	return os;
}

std::ostream& operator<<( std::ostream& os, const SymbolList& sl ) {
	for( auto it = sl.begin(); it != sl.end(); ++it ) {
		os << (*it);
		if( it != sl.end() - 1 ) os << ",";
	}
	return os;
}

Grammar::Grammar() {
	m_fname = "";
}

void Grammar::load(const std::string& filename, std::ifstream& file) {

	non_terminals.clear();
	terminals.clear();
	for( auto p : production_rules ) {
		p.second.clear();
	}
	production_rules.clear();

	m_fname = filename;
	std::string token;
	std::getline(file, token);
	non_terminals = string_to_symbols(token);
	starting_symbol = non_terminals[0];
	std::getline(file, token);
	terminals = string_to_symbols(token);
	epsilon = terminals[0];
	std::string tokenizer;
	while( std::getline(file, tokenizer) ) {
		as_prod_rules(tokenizer);
	}
}

Grammar::~Grammar() {
	non_terminals.clear();
	terminals.clear();
	for( auto p : production_rules ) {
		p.second.clear();
	}
	production_rules.clear();
}


void Grammar::as_prod_rules(string& str) {
	IO::String::eat_whitespace(str);
	std::istringstream tokenizer(str);
	std::string tmp_str;
	std::getline(tokenizer, tmp_str, '-');
	Symbol non_term(tmp_str);
	//std::cout << "as_prod_rules: non_term " << non_term.m_id << " " << non_term << '\n';
	tokenizer.get(); // remove >
	string token;
	while( std::getline(tokenizer, token, '|') ) {
		//std::cout << "as_prod_rules while \t" << token << '\n';
		production_rules[get_nt_index(non_term)].push_back(string_to_symbols(token));
	}
}

SymbolList Grammar::those_that_product(SymbolList& sl) {
	SymbolList ret;
	for( auto CV : sl ) {
		for( auto nT : non_terminals ) {
			for( auto rule : production_rules[get_nt_index(nT)] ) {
				if( sl.size() != rule.size() ) { continue; }
				for( unsigned int i = 0; i < rule.size(); ++i ) {
					if( !(sl[i] == rule[i]) ) { break; }
					if( i == rule.size() - 1 ) {
						ret.push_back(nT);
					}
				}
			}
		}
	}
	return ret;
}

SymbolList Grammar::find_as_list(const SymbolList& sl) {
	SymbolList ret;
	for( auto nT : non_terminals ) {
		for( auto rule : production_rules[get_nt_index(nT)] ) {
			if( rule.size() != 2 ) continue;
			if( rule[0] == sl[0] && rule[1] == sl[1] ) {
				ret.push_back(nT);
			}
		}
	}
	return ret;
}

bool Grammar::cyk(const std::string& input) {
	unsigned int n = input.size();
	SymbolList in_sl;
	for( auto ch : input ) {
		in_sl.push_back(ch);
	}
	in_sl = to_pseudo_non_terminals(in_sl);

	std::vector<SymbolList> pairs;
	for( auto it = in_sl.begin(); it != in_sl.end() - 1; ++it ) {
		SymbolList temp_sl;
		temp_sl.push_back(*it);
		temp_sl.push_back(*( it + 1 ));
		pairs.push_back(temp_sl);
	}

	std::vector<SymbolList> sl_vect;
	for( auto t : pairs ) {
		SymbolList temp = find_as_list(t);
		if( temp.size() == 0 ) {
			Symbol temp_sym("empty");
			temp.push_back(temp_sym);
		}
		sl_vect.push_back(temp);
	}

	for( auto x : sl_vect ) {
		for( auto t : x ) {
			std::cout << t << '|';
		}
		std::cout << '\n';
	}

	return true;
}

void Grammar::print() {
	ostream& log = std::cout;
	log << "Starting symbol: " << starting_symbol << '\n';
	log << "Non terminals: " << non_terminals << '\n';
	log << "Terminals: " << terminals << '\n';
	log << "Epsilon: " << epsilon << '\n';
	log << "Production rules: \n";
	for( auto nT : non_terminals ) {
		log << nT << " -> ";
		for( unsigned int i = 0; i < production_rules[get_nt_index(nT)].size(); ++i ) {
			log << production_rules[get_nt_index(nT)][i];
			if( i != production_rules[get_nt_index(nT)].size() - 1 ) log << " | ";
		}
		log << '\n';
	}
	log << std::endl;
}

void Grammar::print_file(std::ostream& os) {
	ostream& log = os;
	log << non_terminals << '\n';
	log << terminals << '\n';
	for( auto it = non_terminals.begin(); it != non_terminals.end(); ++it ) {
		log << (*it) << " -> ";
		for( unsigned int i = 0; i < production_rules[get_nt_index(*it)].size(); ++i ) {
			log << production_rules[get_nt_index(*it)][i];
			if( i != production_rules[get_nt_index(*it)].size() - 1 ) log << " | ";
		}
		if(it != non_terminals.end() -1) log << '\n';
	}
}

bool contains_char(const SymbolList& str, const Symbol& ch) {
	for( auto &cha : str ) {
		if( cha == ch ) { return true; }
	}
	return false;
}

SymbolList intersection(const SymbolList& str1, const SymbolList& str2) {
	SymbolList ret;
	for( auto &ch : str1 ) {
		if( contains_char(str2, ch) ) {
			ret.push_back(ch);
		}
	}
	return ret;
}

bool subset_of(const SymbolList& in, const SymbolList& of) {
	return intersection(in, of).size() == in.size();
}

bool empty_intersection(const SymbolList& str1, const SymbolList& str2) {
	for( auto &ch : str1 ) {
		if( contains_char(str2, ch) ) {
			return false;
		}
	}
	return true;
}

SymbolList unite(const SymbolList& str1, const SymbolList& str2) {
	SymbolList ret_val = str2;
	for( auto &ch : str1 ) {
		if( contains_char(str2, ch) ) { continue; }
		ret_val.push_back(ch);
	}
	return ret_val;
}

SymbolList Grammar::get_actives() {
	SymbolList actives(terminals);
	//std::vector<char> active_vector;
	for( auto &nT : non_terminals ) {
		for( auto rule : production_rules[get_nt_index(nT)] ) {
			if( subset_of(rule, terminals) ) {
				actives.push_back(nT);
				break;
			}
		}
	}
	SymbolList prev_actives;
	do {
		//std::cout << "get_actives do-while\n";
		prev_actives = actives;
		for( auto& nT : non_terminals ) {
			if( contains_char(actives, nT) ) { continue; }
			for( auto rule : production_rules[get_nt_index(nT)] ) {
				if( subset_of(rule, actives) ) {
					actives.push_back(nT);
					break;
				}
			}
		}
	} while( prev_actives != actives );
	return intersection(actives, non_terminals);
}

SymbolList Grammar::get_reachables() {
	SymbolList reachables;
	reachables.push_back(starting_symbol);
	SymbolList prev_reachables;
	do {
		prev_reachables = reachables;
		for( auto &ch : prev_reachables ) {
			for( auto rule : production_rules[get_nt_index(ch)] ) {
				for( auto &symbol : rule ) {
					if( contains_char(reachables, symbol) || contains_char(terminals, symbol) ) { continue; }
					reachables.push_back(symbol);
				}
			}
		}
	} while( prev_reachables != reachables );
	return reachables;
}

void Grammar::reduce() {
	non_terminals = intersection(get_reachables(), get_actives());
	SymbolList all_symbols = unite(non_terminals, terminals);
	auto temp_rules = production_rules;
	production_rules.clear();
	for( auto &nT : non_terminals ) {
		for( auto rule : temp_rules[get_nt_index(nT)] ) {
			if( subset_of(rule, all_symbols) ) {
				production_rules[get_nt_index(nT)].push_back(rule);
			}
		}
	}
}

unsigned int factorial(unsigned int n) {
	unsigned int ret = 1;
	for( unsigned int k = n; k > 1; --k ) ret *= k;
	return ret;
}

std::vector<SymbolList> permutations(const SymbolList& str, const SymbolList& of) {
	std::vector<SymbolList> ret;
	unsigned int size = of.size();
	unsigned int n = factorial(size);
	SymbolList temp = str;
	//std::cout << temp << '\n';
	while( !empty_intersection(temp, of) && temp.size() > 1 ) {
		SymbolList copy = temp;
		for( unsigned int i = 0; i < copy.size(); ++i ) {
			if( contains_char(of, copy[i]) ) {
				copy.erase(copy.begin() + i);
				ret.push_back(copy);
				//std::cout << copy << std::endl;
				break;
			}
		}
		temp = ret.back();
	}
	return ret;
}

void Grammar::kill_epsilon_rules() {
	SymbolList epsilon_rules;
	epsilon_rules.push_back(epsilon);
	SymbolList prev;
	do {
		prev = epsilon_rules;
		for( auto &nT : non_terminals ) {
			if( contains_char(epsilon_rules, nT) ) { continue; }
			for( auto rule : production_rules[get_nt_index(nT)] ) {
				if( subset_of(rule, epsilon_rules) ) {
					epsilon_rules.push_back(nT);
				}
			}
		}
	} while( prev != epsilon_rules );
	
	auto map_copy = production_rules;
	for( auto &nT : non_terminals ) {
		//std::cout << "running for " << nT << std::endl;
		for( auto rule : map_copy[get_nt_index(nT)] ) {
			//std::cout << "running for " << rule << std::endl;
			if( !empty_intersection(rule, epsilon_rules) ) {
				for( auto str : permutations(rule, epsilon_rules) ) {
					production_rules[get_nt_index(nT)].push_back(str);
				}
			}
			//std::cout << "finished for " << rule << std::endl;
		}
		//std::cout << "finished for " << nT << std::endl;
	}
	
	//map_copy = production_rules;
	SymbolList str_epsilon;
	str_epsilon.push_back(epsilon);
	for( auto &nT : non_terminals ) {
		for( auto it = production_rules[get_nt_index(nT)].begin(); it != production_rules[get_nt_index(nT)].end(); ++it ) {
			if( str_epsilon == *it ) {
				production_rules[get_nt_index(nT)].erase(it);
				break;
			}
		}
	}
}

void Grammar::kill_chains() {
	std::vector<SymbolList> temp;
	for( auto &nT : non_terminals ) {
		SymbolList set;
		set.push_back(nT);
		for( auto rule : production_rules[get_nt_index(nT)] ) {
			if( rule.size() == 1 ) {
				if( contains_char(set, rule[0]) ) {
					continue;
				} else {
					set.push_back(rule[0]);
				}
			}
		}
		for( auto &ch : set ) {
			for( auto rule : production_rules[get_nt_index(ch)] ) {
				if( rule.size() == 1 && subset_of(rule, non_terminals) ) {
					continue;
				} else {
					temp.push_back(rule);
				}
			}
		}
		production_rules[get_nt_index(nT)] = temp;
		temp.clear();
	}
}

Symbol get_first(const SymbolList& str, char start) {
	Symbol sym(start);
	sym.identifier.push_back('0');
	return sym;
}

std::string int_as_str(unsigned int n) {
	std::string ret;
	unsigned int length = std::log10(n) +1;
	unsigned int mod_tmp;
	unsigned int div_tmp = n;
	for( unsigned int i = 0; i < length; ++i ) {
		unsigned int pow_tmp = pow(10, i);
		mod_tmp = div_tmp % 10;
		div_tmp /= pow_tmp*10;
		//std::cout << div_tmp << " " << mod_tmp << " " << pow_tmp << '\n';
		ret.push_back('0' + mod_tmp );
		//std::cout << ret.back() << " ";
	}
	//std::cout << ret << '\n';
	return ret;
}

void get_next(Symbol& ch, unsigned int counter) {
	//std::cout << "get_next\n";
	unsigned int temp_c = counter > 1 ? counter : 2;
	//std::cout << "temp_c: " << temp_c << '\n';
	unsigned int length = floor(std::log10(temp_c -1)) +1;
	//std::cout << "length: " << length << '\n';
	//std::cout << ch << '\n';
	for( unsigned int i = 0; i < length; ++i ) {
		ch.pop_back();
	}
	//std::cout << ch << '\n';
	auto tmp = int_as_str(counter);
	for( unsigned int i = 0; i < tmp.size(); ++i ) {
		ch.push_back(tmp[tmp.size() - i - 1]);
	}
	//std::cout << ch << '\n';
}

void replace_first_of(SymbolList& sl, const Symbol& of, const Symbol& with) {
	//std::cout << "replace_first_with\n";
	for( unsigned int i = 0; i < sl.size(); ++i ) {
		if( sl[i] == of ) {
			sl[i] = with;
			return;
		}
	}
}

SymbolList& operator+=( SymbolList& lhs, SymbolList& rhs ) {
	for( auto sym : rhs ) {
		lhs.push_back(sym);
	}
	return lhs;
}

void Grammar::pseudo_non_terminals() {
	auto current = get_first(unite(non_terminals, terminals), 'Q');
	SymbolList temp_non_terms = non_terminals;
	SymbolList pseudo_non_terms;
	unsigned int c = 0;
	for( auto T : terminals ) {
		SymbolList temp;
		temp.push_back(T);
		non_terminals.push_back(current);
		pseudo_non_terms.push_back(current);
		production_rules[get_nt_index(current)].push_back(temp);
		//std::cout << current << " " << production_rules[get_nt_index(current)].back() << " " << production_rules[get_nt_index(current)].size() << '\n';
		get_next(current, ++c);
	}

	for( auto nT : temp_non_terms ) {
		std::vector<SymbolList> temp_rules;
		for( auto rule : production_rules[get_nt_index(nT)] ) {
			//std::cout << "rule: " << rule << " => ";
			if( rule.size() == 1 ) {
				//std::cout << "unchanged\n";
				temp_rules.push_back(rule);
				continue;
			}
			for( auto ch : rule ) {
				//std::cout << "ch: " << ch << '\n';
				if( contains_char(terminals, ch) ) {
					unsigned int i = pseudo_non_terms.size();
					while( i --> 0 ) {
						//std::cout << "while( "<<i<<" --> 0)\n";
						auto x = production_rules[get_nt_index(pseudo_non_terms[i])];
						//std::cout << production_rules[get_nt_index(pseudo_non_terms[i])][0];
						//std::cout << x[0][0] << " =?= " << ch << '\n';
						if( ch == x[0][0] ) {
							//std::cout << " :: TRUE\n";
							break;
						}
					}
					auto t = pseudo_non_terms[i];
					replace_first_of(rule, ch, t);
				}
			}
			//std::cout << rule << '\n';
			temp_rules.push_back(rule);
		}
		production_rules[get_nt_index(nT)] = temp_rules;
	}

}

void Grammar::length_reduction() {
	auto current = get_first(unite(non_terminals, terminals), 'W');
	SymbolList temp_nts = non_terminals;
	auto new_map = production_rules;
	new_map.clear();
	unsigned int c = 0;
	for( auto nT : temp_nts ) {
		std::vector<SymbolList> temp_vector;
		for( auto rule : production_rules[get_nt_index(nT)] ) {
			while( rule.size() > 2 ) {
				SymbolList temp_str;
				temp_str.push_back(*(rule.end() -2));
				temp_str.push_back(*( rule.end() - 1 ));
				rule.pop_back();
				rule.pop_back();
				rule.push_back(current);
				non_terminals.push_back(current);
				new_map[get_nt_index(current)].push_back(temp_str);
				//std::cout << current << "(id: " << get_nt_index(nT) << ") -> " << temp_str << '\n';
				get_next(current, ++c);
			}
			new_map[get_nt_index(nT)].push_back(rule);
		}
	}
	production_rules = new_map;
}

void Grammar::to_normal_form() {
	pseudo_non_terminals();
	length_reduction();
}

int Grammar::get_nt_index(const Symbol& sym) {
	int i = 0;
	for( i = 0; i < non_terminals.size(); ++i ) {
		if( sym == non_terminals[i] ) {
			return i;
		}
	}
	return ++i;
}

Symbol::operator std::string() {
	return identifier;
}

bool Symbol::operator==( const Symbol& sym ) { return identifier == sym.identifier; }
bool Symbol::operator==( Symbol& sym ) { return identifier == sym.identifier; }

Symbol& Symbol::operator=( const Symbol& sym ) {
	this->identifier = sym.identifier;
	return *this;
}

Symbol::Symbol() {
	identifier = "";
	if( m_id == -1 ) m_id = id++;
	//std::cout << "new symbol\n";
}
Symbol::Symbol(const char& ch) {
	identifier.push_back(ch);
	if( m_id == -1 ) m_id = id++;
}
Symbol::Symbol(const std::string& str) {
	//std::cout << "Symbol by str\n" << m_id;
	identifier = str;
	if( m_id == -1 ) m_id = id++;
}
Symbol::Symbol(const Symbol& sym) {
	identifier = sym.identifier;
	m_id = sym.m_id;
	//std::cout << "symbol copy\n";
}

Symbol& Symbol::operator+=( Symbol& sym ) {
	this->identifier += sym.identifier;
	return *this;
}

bool operator<( const Symbol& lhs, const Symbol& rhs ) {
	return lhs.m_id < rhs.m_id;
}

bool operator==( const Symbol& lhs, const Symbol& rhs ) {
	return lhs.identifier == rhs.identifier;
}

/*Symbol::operator int() {
	return m_id;
}*/

Symbol& Symbol::pop_back() {
	identifier.pop_back();
	return *this;
}

Symbol& Symbol::push_back(const char& ch) {
	identifier.push_back(ch);
	return *this;
}

SymbolList Grammar::to_pseudo_non_terminals(const SymbolList& str) {
	SymbolList ret = str;
	for( auto nT : non_terminals ) {
		//std::cout << nT << " ";
		if( production_rules[get_nt_index(nT)].begin() != production_rules[get_nt_index(nT)].end() - 1 ) { continue; }
		//std::cout << "still going... " << nT << '\n';
		auto& ruleL = production_rules[get_nt_index(nT)].front();
		if( ruleL.begin() != ruleL.end() - 1 ) { continue; }
		//std::cout << "still going... " << nT << '\n';
		auto &rule = ruleL.front();
		for( auto& sym : ret ) {
			if( sym == rule ) {
				sym = nT;
			}
		}
	}
	return ret;
}