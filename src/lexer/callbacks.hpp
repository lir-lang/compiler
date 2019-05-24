#pragma once

#include <utils/logger.hpp>
#include <structures/token_types.hpp>
#include <structures/token.hpp>
#include <structures/view.hpp>
#include <preprocessor/preprocessor.hpp>

namespace lir::lexer {
	namespace Type = lir::Tokens;


	// Consume a character literal. ('.')
	inline lir::Token on_char(lir::View& view) {
		++view;

		auto chr = view.get();

		if (view.match('\''))
			return {Type::Char, chr};

		// lir::errorln("Unterminated char literal.");

		return {};
	}



	// (+= | ++ | +)
	inline lir::Token on_plus(lir::View& view) {
		if (view.match('='))
			return {Type::Op_PlusEq};

		else if (view.match('+'))
			return {Type::Op_Incr};

		else
			return {Type::Op_Plus};
	}


	// (-= | -- | -)
	inline lir::Token on_minus(lir::View& view) {
		if (view.match('='))
			return {Type::Op_MinusEq};

		else if (view.match('-'))
			return {Type::Op_Decr};

		else
			return {Type::Op_Minus};
	}




	// (*= | *)
	inline lir::Token on_multiply(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Op_MultiplyEq}
			: lir::Token{Type::Op_Multiply});
	}

	// (/= | /)
	inline lir::Token on_divide(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Op_DivideEq}
			: lir::Token{Type::Op_Divide});
	}

	// (%= | %)
	inline lir::Token on_mod(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Op_ModEq}
			: lir::Token{Type::Op_Mod});
	}






	// ( &= | & )
	inline lir::Token on_ampersand(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Op_AndEq}
			: lir::Token{Type::Op_And});
	}

	// ( \|= | \|)
	inline lir::Token on_bar(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Op_OrEq}
			: lir::Token{Type::Op_Or});
	}

	// ( ~= | ~ )
	inline lir::Token on_tilde(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Op_NegEq}
			: lir::Token{Type::Op_Neg});
	}

	// ( == | = )
	inline lir::Token on_equal(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Cmp_Eq}
			: lir::Token{Type::Assign});
	}





	// ( <= | << | < )
	inline lir::Token on_less(lir::View& view) {
		if (view.match('=')) {
			return {Type::Cmp_LessEq};

		} else if (view.match('<')) {
			return (view.match('=')
				? lir::Token{Type::Op_ShiftLeftEq}
				: lir::Token{Type::Op_ShiftLeft});

		} else {
			return {Type::Cmp_Less};
		}
	}



	// ( >= | >> | > )
	inline lir::Token on_more(lir::View& view) {
		if (view.match('=')) {
			return {Type::Cmp_MoreEq};

		} else if (view.match('>')) {
			return (view.match('=')
				? lir::Token{Type::Op_ShiftRightEq}
				: lir::Token{Type::Op_ShiftRight});

		} else {
			return {Type::Cmp_More};
		}
	}



	// ( :: | : )
	inline lir::Token on_colon(lir::View& view) {
		return (view.match(':')
			? lir::Token{Type::Namespace}
			: lir::Token{Type::Colon});
	}


	// ( != | ! )
	inline lir::Token on_exclaim(lir::View& view) {
		return (view.match('=')
			? lir::Token{Type::Cmp_NotEq}
			: lir::Token{Type::Exclaim});
	}





	// Handle all callbacks.
	lir::Token lexer_callback(lir::FileStack& files) {
		char current = *files.view();



		// Special.
		if (lir::common_whitespace(current)) {
			// Skip whitespace...
			++files.view();
			return lexer_callback(files);
		}

		else if (current == '/' and files.view().match('/')) {
			// Skip comments...
			lir::lexer::on_comment(files);
			// ++files.view();
			return lexer_callback(files);
		}

		else if (current == '#') {
			// Handle preprocessor directives...
			lir::preprocessor(files);
			return lexer_callback(files);
		}

		else if (not files.view().remaining())
			// Handle EOF...
			return lir::lexer::on_eof(files, lexer_callback);




		// Identifier & Number.
		else if (lir::alpha(current)) return {Type::Identifier, lir::lexer::on_alpha(files)};
		else if (lir::digit(current)) return {Type::Number, lir::lexer::on_num(files)};


		// String & Char.
		else if (current == '"')  return {Type::String, lir::lexer::on_string(files)};
		else if (current == '\'') return on_char(files.view());


		// Grouping.
		else if (current == '(') return {Type::ParenLeft};
		else if (current == ')') return {Type::ParenRight};
		else if (current == '{') return {Type::BraceLeft};
		else if (current == '}') return {Type::BraceRight};
		else if (current == '[') return {Type::BracketLeft};
		else if (current == ']') return {Type::BracketRight};


		// Comparison operators.
		else if (current == '>') return on_more(files.view());
		else if (current == '<') return on_less(files.view());
		else if (current == '!') return on_exclaim(files.view());
		else if (current == '=') return on_equal(files.view());


		// Numerical operators.
		else if (current == '+') return on_plus(files.view());
		else if (current == '-') return on_minus(files.view());
		else if (current == '*') return on_multiply(files.view());
		else if (current == '/') return on_divide(files.view());
		else if (current == '%') return on_mod(files.view());


		// Logical operators.
		else if (current == '&') return on_ampersand(files.view());
		else if (current == '|') return on_bar(files.view());
		else if (current == '~') return on_tilde(files.view());


		// Seperators
		else if (current == ':') return on_colon(files.view());
		else if (current == ';') return {Type::Semicolon};
		else if (current == '?') return {Type::Question};
		else if (current == ',') return {Type::Comma};
		else if (current == '.') return {Type::Dot};




		lir::errorln("unknown character: '", current, "'");
		++files.view();
		return lexer_callback(files);
	};
}
