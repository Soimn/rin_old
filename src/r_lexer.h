#define R_TOKEN_BLOCK_SIZE 16
#define R_TOKEN_BLOCK_INDEX_FROM_KIND(kind) ((kind) >> 4)
#define R_TOKEN_BLOCK_OFFSET_FROM_KIND(kind) ((kind) & 0xF)
#define R_TOKEN_OP_OF_BINARY_OP_ASSIGNMENT(kind) ((kind) + (R_Token__FirstBinary - R_Token__FirstAssignment))
#define R_TOKEN_BLOCK(index) ((index) << 4)

typedef enum R_Token_Kind
{
	R_Token_Invalid = 0,
	R_Token_EndOfFile,

	R_Token_Identifier,
	R_Token_String,

	R_Token__FirstNumber,
	R_Token__FirstInteger               = R_Token__FirstNumber,
	R_Token_Integer                     = R_Token__FirstInteger,
	R_Token_BinaryInteger,
	R_Token_HexInteger,
	R_Token__LastInteger                = R_Token_HexInteger,

	R_Token__FirstFloat,
	R_Token_Float                       = R_Token__FirstFloat,
	R_Token_HexFloat32,
	R_Token_HexFloat64,
	R_Token__LastFloat                  = R_Token_HexFloat64,
	R_Token__LastNumber                 = R_Token__LastFloat,

	R_Token_OpenParen,
	R_Token_CloseParen,
	R_Token_OpenBracket,
	R_Token_CloseBracket,
	R_Token_OpenBrace,
	R_Token_CloseBrace,
	R_Token_Colon,
	R_Token_Semicolon,
	R_Token_Comma,
	R_Token_Hat,
	R_Token_Period,
	R_Token_Bang,
	R_Token_Arrow,
	R_Token_Blank,
	R_Token_Qmark,

	R_Token__FirstKeyword,
	R_Token_If                          = R_Token__FirstKeyword,
	R_Token_Else,
	R_Token_While,
	R_Token_Break,
	R_Token_Continue,
	R_Token_Return,
	R_Token_Proc,
	R_Token_Struct,
	R_Token_True,
	R_Token_False,
	R_Token__LastKeyword                = R_Token_False,

	
	R_Token__FirstAssignment            = R_TOKEN_BLOCK(5),
	R_Token_Equals                      = R_Token__FirstAssignment,

	R_Token__FirstMulLevelAssignment    = R_TOKEN_BLOCK(6),
	R_Token_MulEq                       = R_Token__FirstMulLevelAssignment,
	R_Token_DivEq,
	R_Token_RemEq,
	R_Token_AndEq,
	R_Token_LessLessEq,
	R_Token_GreaterGreaterEq,
	R_Token_TripleGreaterEq,
	R_Token__LastMulLevelAssignment     = R_TOKEN_BLOCK(7) - 1,

	R_Token__FirstAddLevelAssignment    = R_TOKEN_BLOCK(7),
	R_Token_AddEq                       = R_Token__FirstAddLevelAssignment,
	R_Token_SubEq,
	R_Token_OrEq,
	R_Token_TildeEq,
	R_Token__LastAddLevelAssignment     = R_TOKEN_BLOCK(8) - 1,

	R_Token__FirstCmpLevelAssignment    = R_TOKEN_BLOCK(8),
	R_Token__LastCmpLevelAssignment     = R_TOKEN_BLOCK(9) - 1,

	R_Token__FirstAndAndLevelAssignment = R_TOKEN_BLOCK(9),
	R_Token__LastAndAndLevelAssignment  = R_TOKEN_BLOCK(10) - 1,

	R_Token__FirstOrOrLevelAssignment   = R_TOKEN_BLOCK(10),
	R_Token__LastOrOrLevelAssignment    = R_TOKEN_BLOCK(11) - 1,

	R_Token__LastAssignment             = R_Token__LastOrOrLevelAssignment,

	R_Token__FirstBinary                = R_TOKEN_BLOCK(11),
	R_Token__FirstMulLevel              = R_Token__FirstBinary,
	R_Token_Mul                         = R_Token__FirstMulLevel,
	R_Token_Div,
	R_Token_Rem,
	R_Token_And,
	R_Token_LessLess,
	R_Token_GreaterGreater,
	R_Token_TripleGreater,
	R_Token__LastMulLevel               = R_TOKEN_BLOCK(12) - 1,

	R_Token__FirstAddLevel              = R_TOKEN_BLOCK(12),
	R_Token_Add                         = R_Token__FirstAddLevel,
	R_Token_Sub,
	R_Token_Or,
	R_Token_Tilde,
	R_Token__LastAddLevel               = R_TOKEN_BLOCK(13) - 1,

	R_Token__FirstCmpLevel              = R_TOKEN_BLOCK(13),
	R_Token_EqualsEq                    = R_Token__FirstCmpLevel,
	R_Token_BangEq,
	R_Token_Less,
	R_Token_LessEq,
	R_Token_Greater,
	R_Token_GreaterEq,
	R_Token__LastCmpLevel               = R_TOKEN_BLOCK(14) - 1,

	R_Token__FirstAndAndLevel           = R_TOKEN_BLOCK(14),
	R_Token_AndAnd                      = R_Token__FirstAndAndLevel,
	R_Token__LastAndAndLevel            = R_TOKEN_BLOCK(15) - 1,

	R_Token__FirstOrOrLevel             = R_TOKEN_BLOCK(15),
	R_Token_OrOr                        = R_Token__FirstOrOrLevel,
	R_Token__LastOrOrLevel              = R_TOKEN_BLOCK(16) - 1,
	R_Token__LastBinary                 = R_Token__LastOrOrLevel,
} R_Token_Kind;

#define R_TOKEN_KEYWORD_COUNT ((R_Token__LastKeyword - R_Token__FirstKeyword) + 1)

// NOTE: MSVC is stupid
#define R_TOKEN_KEYWORDSTRING(k, str) [(k) - R_Token__FirstKeyword] = { .data = (R_u8*)(str), .size = sizeof(str) - 1 }
static R_String R_Token_KeywordStrings[R_TOKEN_KEYWORD_COUNT] = {
	R_TOKEN_KEYWORDSTRING(R_Token_If,       "if"),
	R_TOKEN_KEYWORDSTRING(R_Token_Else,     "else"),
	R_TOKEN_KEYWORDSTRING(R_Token_While,    "while"),
	R_TOKEN_KEYWORDSTRING(R_Token_Break,    "break"),
	R_TOKEN_KEYWORDSTRING(R_Token_Continue, "continue"),
	R_TOKEN_KEYWORDSTRING(R_Token_Return,   "return"),
	R_TOKEN_KEYWORDSTRING(R_Token_Proc,     "proc"),
	R_TOKEN_KEYWORDSTRING(R_Token_Struct,   "struct"),
	R_TOKEN_KEYWORDSTRING(R_Token_True,     "true"),
	R_TOKEN_KEYWORDSTRING(R_Token_False,    "false"),
};
#undef R_TOKEN_KEYWORDSTRING

typedef struct R_Token
{
	R_Token_Kind kind;

	union
	{
		R_u64 integer;
		R_f64 floating;
		R_String string;
	};
} R_Token;

typedef struct R_Lexer
{
	R_ZString string;
	R_ZString current;
	R_u32 line;
	R_u32 line_start_offset;
	R_Arena* string_arena;
} R_Lexer;

R_Lexer
R_Lexer_Init(R_ZString string, R_Arena* string_arena)
{
	return (R_Lexer){
		.string            = string,
		.current           = string,
		.line              = 0,
		.line_start_offset = 0,
		.string_arena      = string_arena,
	};
}

R_Token
R_Lexer_NextToken(R_Lexer* lexer)
{
	R_Token token = { .kind = R_Token_Invalid };

	int comment_depth = 0;
	while (*lexer->current)
	{
		if (*lexer->current == '\n')
		{
			lexer->line   += 1;
			lexer->current += 1;

			lexer->line_start_offset = (R_u32)(lexer->current - lexer->string);
		}
		else if (*lexer->current == ' '  || *lexer->current == '\t' ||
						 *lexer->current == '\v' || *lexer->current == '\f' ||
						 *lexer->current == '\r')
		{
			lexer->current += 1;
		}
		else if (lexer->current[0] == '/' && lexer->current[1] == '/')
		{
			while (*lexer->current && lexer->current[0] != '\n') ++lexer->current;
		}
		else if (lexer->current[0] == '/' && lexer->current[1] == '*')
		{
			comment_depth += 1;
			lexer->current += 2;
		}
		else if (comment_depth > 0 && lexer->current[0] == '*' && lexer->current[1] == '/')
		{
			comment_depth -= 1;
			lexer->current += 2;
		}
		else
		{
			if (comment_depth > 0)
			{
				lexer->current += 1;
				continue;
			}
			else break;
		}
	}

	if (*lexer->current == 0) token.kind = R_Token_EndOfFile;
	else
	{
		R_u8 c = *lexer->current;
		lexer->current += 1;

		switch (c)
		{
			case '(': token.kind = R_Token_OpenParen;    break;
			case ')': token.kind = R_Token_CloseParen;   break;
			case '[': token.kind = R_Token_OpenBracket;  break;
			case ']': token.kind = R_Token_CloseBracket; break;
			case '{': token.kind = R_Token_OpenBrace;    break;
			case '}': token.kind = R_Token_CloseBrace;   break;
			case ':': token.kind = R_Token_Colon;        break;
			case ';': token.kind = R_Token_Semicolon;    break;
			case ',': token.kind = R_Token_Comma;        break;
			case '^': token.kind = R_Token_Hat;          break;
			case '.': token.kind = R_Token_Period;       break;
			case '?': token.kind = R_Token_Qmark;        break;

#define R_TOKEN_OP_OR_OPEQ(c, op, op_eq) \
			case c:                            \
			{                                  \
				token.kind = op;                 \
				if (*lexer->current == '=')      \
				{                                \
					token.kind = op_eq;            \
					lexer->current += 1;           \
				}                                \
			} break

			R_TOKEN_OP_OR_OPEQ('!', R_Token_Bang, R_Token_BangEq);
			R_TOKEN_OP_OR_OPEQ('*', R_Token_Mul, R_Token_MulEq);
			R_TOKEN_OP_OR_OPEQ('/', R_Token_Div, R_Token_DivEq);
			R_TOKEN_OP_OR_OPEQ('%', R_Token_Rem, R_Token_RemEq);
			R_TOKEN_OP_OR_OPEQ('+', R_Token_Add, R_Token_AddEq);
			R_TOKEN_OP_OR_OPEQ('~', R_Token_Tilde, R_Token_TildeEq);
			R_TOKEN_OP_OR_OPEQ('=', R_Token_Equals, R_Token_EqualsEq);

#undef R_TOKEN_OP_OR_OPEQ

			case '-':
			{
				token.kind = R_Token_Sub;
				if (*lexer->current == '=')
				{
					token.kind = R_Token_SubEq;
					lexer->current += 1;
				}
				else if (*lexer->current == '>')
				{
					token.kind = R_Token_Arrow;
					lexer->current += 1;
				}
			} break;

			case '&':
			{
				token.kind = R_Token_And;
				if (*lexer->current == '=')
				{
					token.kind = R_Token_AndEq;
					lexer->current += 1;
				}
				else if (*lexer->current == '&')
				{
					token.kind = R_Token_AndAnd;
					lexer->current += 1;
				}
			} break;

			case '|':
			{
				token.kind = R_Token_Or;
				if (*lexer->current == '=')
				{
					token.kind = R_Token_OrEq;
					lexer->current += 1;
				}
				else if (*lexer->current == '|')
				{
					token.kind = R_Token_OrOr;
					lexer->current += 1;
				}
			} break;

			case '<':
			{
				token.kind = R_Token_Less;
				if (*lexer->current == '=')
				{
					token.kind = R_Token_LessEq;
					lexer->current += 1;
				}
				else if (*lexer->current == '<')
				{
					token.kind = R_Token_LessLess;
					lexer->current += 1;

					if (*lexer->current == '=')
					{
						token.kind = R_Token_LessLessEq;
						lexer->current += 1;
					}
				}
			} break;

			case '>':
			{
				token.kind = R_Token_Greater;
				if (*lexer->current == '=')
				{
					token.kind = R_Token_GreaterEq;
					lexer->current += 1;
				}
				else if (*lexer->current == '>')
				{
					token.kind = R_Token_GreaterGreater;
					lexer->current += 1;

					if (*lexer->current == '=')
					{
						token.kind = R_Token_GreaterGreaterEq;
						lexer->current += 1;
					}
					else if (*lexer->current == '>')
					{
						token.kind = R_Token_TripleGreater;
						lexer->current += 1;

						if (*lexer->current == '=')
						{
							token.kind = R_Token_TripleGreaterEq;
							lexer->current += 1;
						}
					}
				}
			} break;

			default:
			{
				if (R_IsAlpha(c) || c == '_')
				{
					R_String identifier;
					identifier.data = lexer->current - 1;

					while (*lexer->current == '_' || R_IsAlpha(*lexer->current) || R_IsDigit(*lexer->current))
					{
						lexer->current += 1;
					}

					identifier.size = (R_u32)(lexer->current - identifier.data);

					if (identifier.size == 1 && *identifier.data == '_')
					{
						token.kind = R_Token_Blank;
					}
					else
					{
						token.kind   = R_Token_Identifier;
						token.string = identifier;

						// TODO: perfect hash table
						for (R_u32 i = 0; i < R_TOKEN_KEYWORD_COUNT; ++i)
						{
							if (R_String_Match(identifier, R_Token_KeywordStrings[i]))
							{
								token.kind = R_Token__FirstKeyword + i;
								break;
							}
						}
					}
				}
				else if (R_IsDigit(c))
				{
					R_umm base      = 10;
					R_bool is_float = R_false;

					if (c == '0')
					{
						if      (*lexer->current == 'b')                    base = 2,  ++lexer->current;
						else if (*lexer->current == 'x')                    base = 16, ++lexer->current;
						else if (*lexer->current == 'h') is_float = R_true, base = 16, ++lexer->current;
					}

					R_u64 value          = c & 0xF;
					R_bool has_overflown = R_false;
					R_umm digit_count    = (base == 10);

					for (;; ++lexer->current)
					{
						c = *lexer->current;

						R_umm digit;
						if      (R_IsDigit(c))                  digit = c & 0xF;
						else if (base == 16 && R_IsHexAlpha(c)) digit = (c & 0xF) + 9;
						else if (c == '_')                      continue;
						else                                    break;

						if (digit >= base)
						{
							//// ERROR: digit is too large for base n literal
							R_NOT_IMPLEMENTED;
						}
						else
						{
							R_u64 high_product;
							R_u64 low_product = _mulx_u64(value, base, &high_product);
							R_u8 carry        = _addcarry_u64(0, low_product, digit, &value);
							has_overflown = (has_overflown || high_product != 0 || carry != 0);
							digit_count  += 1;
						}
					}

					// TODO:
					// HACK:
					if (base == 10 && (*lexer->current == '.' || R_AlphaToLower(*lexer->current) == 'e'))
					{
						if (has_overflown)
						{
							//// ERROR: Floating point literal has too many digits
							R_NOT_IMPLEMENTED;
						}
						else
						{
							R_ASSERT(digit_count != 0);

							R_u64 integral_part    = value;
							R_u64 fractional_part  = 0;
							R_f64 fractional_adj   = 1;
							R_u64 exponent_part    = 0;
							R_bool exponent_is_neg = R_false;

							if (*lexer->current == '.')
							{
								lexer->current += 1;

								if (!R_IsDigit(*lexer->current))
								{
									//// ERROR: Missing digits after decimal point
									R_NOT_IMPLEMENTED;
								}
								else
								{
									for (; R_IsDigit(*lexer->current); ++lexer->current)
									{
										R_umm digit = *lexer->current & 0xF;

										R_u64 high_product;
										R_u64 low_product = _mulx_u64(fractional_part, 10, &high_product);
										R_u8 carry        = _addcarry_u64(0, low_product, digit, &fractional_part);

										fractional_adj /= 10;

										if (high_product != 0 || carry != 0)
										{
											//// ERROR: Too many digits after decimal point
											R_NOT_IMPLEMENTED;
										}
									}
								}
							}

							if (R_AlphaToLower(*lexer->current) == 'e')
							{
								lexer->current += 1;

								exponent_is_neg = (*lexer->current == '-');
								lexer->current += (*lexer->current == '+' || *lexer->current == '-');

								if (!R_IsDigit(*lexer->current))
								{
									//// ERROR: Missing digits after exponent prefix
									R_NOT_IMPLEMENTED;
								}
								else
								{
									for (; R_IsDigit(*lexer->current); ++lexer->current)
									{
										exponent_part = exponent_part*10 + (*lexer->current & 0xF);

										if (exponent_part > 307) // NOTE: for f64: 307 = floor(log10(2^emax)) = floor(abs(log10(2^emin)))
										{
											//// ERROR: Exponent is too large in magnitude
											R_NOT_IMPLEMENTED;
										}
									}
								}
							}

							R_f64 floating = (R_f64)integral_part + fractional_adj*(R_f64)fractional_part;

							for (R_umm i = 0; i < exponent_part; ++i)
							{
								floating *= (exponent_is_neg ? 0.1 : 10);
							}

							token.kind     = R_Token_Float;
							token.floating = floating;
						}
					}
					else if (base == 16 && is_float)
					{
						if (digit_count == 16)
						{
							union
							{
								R_f64 f;
								R_u64 bits;
							} typepun;

							typepun.bits = value;

							token.kind     = R_Token_HexFloat64;
							token.floating = typepun.f;
						}
						else if (digit_count == 8)
						{
							union
							{
								R_f32 f;
								R_u32 bits;
							} typepun;

							typepun.bits = (R_u32)value;

							token.kind     = R_Token_HexFloat32;
							token.floating = (R_f64)typepun.f;
						}
						else
						{
							//// ERROR: Invalid digit count for hex float literal
							R_NOT_IMPLEMENTED;
						}
					}
					else
					{
						if (digit_count == 0)
						{
							//// ERROR: Missing digits after based integer literal prefix
							R_NOT_IMPLEMENTED;
						}
						else if (has_overflown)
						{
							//// ERROR: Integer literal is too large in magnitude
							R_NOT_IMPLEMENTED;
						}
						else
						{
							token.kind    = (base == 16 ? R_Token_HexInteger : (base == 2 ? R_Token_BinaryInteger : R_Token_Integer));
							token.integer = value;
						}
					}
				}
				else if (c == '"')
				{
					R_String raw_string = {
						.data = lexer->current,
						.size = 0,
					};

					while (*lexer->current != '"')
					{
						if (*lexer->current == '\\' && *lexer->current != 0) lexer->current += 2;
						else                                                 lexer->current += 1;
					}

					if (*lexer->current != '"')
					{
						//// ERROR: Unterminated string literal
						R_NOT_IMPLEMENTED;
					}
					else
					{
						raw_string.size = (R_u32)(lexer->current - raw_string.data);
						lexer->current += 1; // NOTE: Skip past the terminating quote

						R_String string;
						string.data = R_Arena_Push(lexer->string_arena, raw_string.size, R_ALIGNOF(R_u8));
						string.size = 0;

						for (R_umm i = 0; i < raw_string.size; )
						{
							if (raw_string.data[i] != '\\')
							{
								string.data[string.size] = raw_string.data[i];
								string.size += 1;
								i           += 1;
							}
							else
							{
								i += 1;
								R_ASSERT(i < raw_string.size);

								if (R_AlphaToLower(raw_string.data[i]) != 'u')
								{
									switch (raw_string.data[i])
									{
										case '\\': string.data[string.size++] = '\\'; break;
										case 'a':  string.data[string.size++] = '\a'; break;
										case 'b':  string.data[string.size++] = '\b'; break;
										case 'f':  string.data[string.size++] = '\f'; break;
										case 'n':  string.data[string.size++] = '\n'; break;
										case 'r':  string.data[string.size++] = '\r'; break;
										case 't':  string.data[string.size++] = '\t'; break;
										case 'v':  string.data[string.size++] = '\v'; break;
										case '\'': string.data[string.size++] = '\''; break;
										case '\"': string.data[string.size++] = '\"'; break;
										default:
										{
											//// ERROR: Illegal escape sequence
											R_NOT_IMPLEMENTED;
										} break;
									}

									i += 1; // NOTE: Skip symbol after backslash in escape sequence
								}
								else
								{
									R_umm expected_digit_count = (raw_string.data[i] == 'u' ? 4 : 6);
									i += 1;

									R_umm past_end = i + expected_digit_count;

									if (past_end > raw_string.size)
									{
										//// ERROR: Missing digits of unicode escape sequence
										R_NOT_IMPLEMENTED;
									}
									else
									{
										R_u32 codepoint = 0;
										for (; i < past_end; ++i)
										{
											c = raw_string.data[i];

											R_u8 digit;
											if      (R_IsDigit(c))    digit = c & 0xF;
											else if (R_IsHexAlpha(c)) digit = (c & 0xF) + 9;
											else
											{
												//// ERROR: Illegal hexadecimal digit
												R_NOT_IMPLEMENTED;
											}

											codepoint <<= 4;
											codepoint  |= digit;
										}

										if (codepoint <= 0x7F)
										{
											string.data[string.size++] = (R_u8)codepoint;
										}
										else if (codepoint <= 0x7FF)
										{
											string.data[string.size++] = 0xC0 | (R_u8)(codepoint >> 6);
											string.data[string.size++] = 0x80 | (R_u8)(codepoint & 0x3F);
										}
										else if (codepoint <= 0xFFFF)
										{
											string.data[string.size++] = 0xE0 | (R_u8)(codepoint >> 12);
											string.data[string.size++] = 0x80 | (R_u8)((codepoint >> 6) & 0x3F);
											string.data[string.size++] = 0x80 | (R_u8)(codepoint & 0x3F);
										}
										else if (codepoint <= 0x10FFFF)
										{
											string.data[string.size++] = 0xF0 | (R_u8)(codepoint >> 18);
											string.data[string.size++] = 0x80 | (R_u8)((codepoint >> 12) & 0x3F);
											string.data[string.size++] = 0x80 | (R_u8)((codepoint >>  6) & 0x3F);
											string.data[string.size++] = 0x80 | (R_u8)(codepoint & 0x3F);
										}
										else
										{
											//// ERROR: UTF8 codepoint is out of range
											R_NOT_IMPLEMENTED;
										}
									}
								}
							}
						}

						token.kind   = R_Token_String;
						token.string = string;
					}
				}
				else
				{
					//// ERROR: Invalid token
					R_NOT_IMPLEMENTED;
				}
			} break;
		}
	}

	return token;
}
