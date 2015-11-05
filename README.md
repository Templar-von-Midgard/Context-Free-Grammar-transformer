# Context-Free-Grammar-transformer
Console application to make certain transformations on context free grammars

The program asks for an input file whose format must comply the following requirements:
  * First line: List of nonterminals (strings) separated by commas
  * Second line: List of terminals (strings) separated by commas
  * Third line: Start symbol
  * Fourth line: Empty string symbol
  * Fifth to last line: production rules separated by new line characters in following form:
    * \<ProductionRule\> ::= \<NonTerminal\>'->'\<Production\>('|'\<Production\>)*
    * \<Production\> ::= \<NonTerminal\>\<Production\> | \<Terminal\>\<Production\> | \<NonTerminal\> | \<Terminal\>
