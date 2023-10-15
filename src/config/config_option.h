#ifndef _CONFIG_OPTION_H_
#define _CONFIG_OPTION_H_

#include <string>
#include <utility>

namespace FanControl {

class ConfigOption {
	public:
		ConfigOption(std::string const& line);

		ConfigOption(ConfigOption const& other) :
			attribute(other.attribute),
			index(other.index),
			value(other.value),
			valid(false),
			failed(false) {};

		ConfigOption(ConfigOption&& other) :
			attribute(std::move(other.attribute)),
			index(other.index),
			value(std::move(other.value)),
			valid(other.valid),
			failed(other.failed) {
			other.valid = false;
		};

		std::string const& getAttribute() const { return attribute; };

		size_t getIndex() const { return index; };

		std::string const& getValue() const { return value; };

		unsigned long getValueAsUL() const { return std::stoul(value); }

		/**
		 * Indicates whether the associated line has successfully been parsed
		 * as a proper configuration line with an (attribute,value)-pair.
		 *
		 * Note, this is not the inverse of `hasFailed`.
		 * It is possible that `isValid` and ` hasFailed` are both `false`
		 * at the same time, i.e. if the line is empty or a comment.
		 */
		bool isValid() const { return valid; };

		/**
		 * Indicates whether the associated line is syntactically wrong and
		 * could not be parsed.
		 *
		 * Note, this not the inverse of `isValid`.
		 * It is possible that `isValid` and ` hasFailed` are both `false`
		 * at the same time, i.e. if the line is empty or a comment.
		 */
		bool hasFailed() const { return failed; };

	private:
		std::string attribute;
		size_t index;
		std::string value;
		bool valid;
		bool failed;
};
}

#endif
