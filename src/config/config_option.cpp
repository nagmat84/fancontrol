#include "config_option.h"
#include <regex>

namespace FanControl {

ConfigOption::ConfigOption(std::string const& line) :
	attribute(),
	index(-1),
	value(),
	valid(false),
	failed(false) {
		static std::regex const commentOrEmpty("^\\s*($|#])");
		// This regex has three caputing groups: attribute, index, value
		// The value-group is special, because we want to support values with
		// spaces in the middle, but spaces at the beginning and end shall be removed,
		// i.e. a valid line could look like this
		//
		// Name = John Dear
		//
		// In this case the value is "John Dear" with a space between first and
		// last name, but any preceeding blank (i.e. between the equal sign and the
		// first letter) as well as any spurious blank at the end shall be ignored.
		// The regex `\\s*((?:\\s*\\S+)+)\\s*$`works, because regex are greedy.
		// The first `\\s*\\ catches all preceeding blanks greedily.
		// The actual value is captured by `((?:\\s*\\S+)+)` with a list of
		// inner sub-expressions that allow optional, seperating blanks at the
		// beginning but must at least contain a non-space at the end.
		static std::regex const attrIdxValuePair("^\\s*([_A-Za-z]+)(?:\\.(\\d+))?\\s*=\\s*((?:\\s*\\S+)+)\\s*$");
		std::smatch pieces;

		if( std::regex_match(line, commentOrEmpty) )
				return;
		if( std::regex_match(line, pieces, attrIdxValuePair) ) {
				if( pieces.size() == 2 ) {
						attribute = pieces[0];
						value = pieces[1];
						valid = true;
				} else if( pieces.size() == 3 ) {
						attribute = pieces[0];
						index = std::stoi(pieces[1]);
						value = pieces[2];
						valid = true;
				} else {
						failed = true;
				}
		}
		failed = true;
}

}
