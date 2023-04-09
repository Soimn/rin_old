#include <stdio.h>
#include <stdlib.h>

typedef unsigned __int8  R_u8;
typedef unsigned __int32 R_u32;

typedef R_u8 R_bool;

typedef struct R_String
{
	R_u32 size;
	R_u8* data;
} R_String;

typedef R_u8* R_ZString;

R_bool
R_IsAlpha(R_u8 c)
{
	return ((R_u8)((c & 0xDF) - 'A') < 26);
}

R_bool
R_IsDigit(R_u8 c)
{
	return ((R_u8)(c - '0') < 10);
}

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

	R_Token__FirstKeyword,
	R_Token_If = R_Token__FirstKeyword,
	R_Token_Else,
	R_Token_While,
	R_Token_Break,
	R_Token_Continue,
	R_Token_Return,
	R_Token_Proc,
	R_Token_Struct,
	R_Token__LastKeyword = R_Token_Struct,

	
	R_Token__FirstAssignment            = R_TOKEN_BLOCK(5),
	R_Token_Equals                      = R_Token_FirstAssignment,

	R_Token__FirstMulLevelAssignment    = R_TOKEN_BLOCK(6),
	R_Token_MulEq                       = R_Token__FirstMulLevelAssignment,
	R_Token_DivEq,
	R_Token_RemEq,
	R_Token_AndEq,
	R_Token_LessLessEq,
	R_Token_GreaterGreaterEq,
	R_Token_TripleGreaterEq,
	R_Token__LastMulLevelAssignment     = R_TOKEN_BLOCK(7) - 1,

	R_Token__FirstAddLevelAssignment    = R_TOKEN_BLOCK(7)
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

	R_Token__FirstBinary                = R_TOKEN_BLOCK(11)
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
	R_Token_EqualsEq                    = R_Token_FirstCmpLevel,
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

typedef struct R_Token
{
	R_Token_Kind kind;
} R_Token;

typedef struct R_Lexer
{
	R_ZString string;
	R_ZString cursor;
	R_u32 line;
	R_u32 line_start_offset;
} R_Lexer;

R_Lexer
R_Lexer_Init(R_ZString string)
{
	return (R_Lexer){
		.string            = string,
		.cursor            = string,
		.line              = 0,
		.line_start_offset = 0,
	};
}

R_Token
R_Lexer_NextToken(R_Lexer* lexer)
{
	R_Token token = { .kind = R_Token_Invalid };

	int comment_depth = 0;
	while (*lexer->cursor)
	{
		if (lexer->cursor[0] == '\n')
		{
			lexer->line   += 1;
			lexer->cursor += 1;

			lexer->line_start_offset = (R_u32)(lexer->cursor - lexer->string);
		}
		else if (lexer->cursor[0] == ' '  || lexer->cursor[0] == '\t' ||
						 lexer->cursor[0] == '\v' || lexer->cursor[0] == '\f' ||
						 lexer->cursor[0] == '\r')
		{
			lexer->cursor += 1;
		}
		else if (lexer->cursor[0] == '/' && lexer->cursor[1] == '/')
		{
			while (*lexer->cursor && lexer->cursor[0] != '\n') ++lexer->cursor;
		}
		else if (lexer->cursor[0] == '/' && lexer->cursor[1] == '*')
		{
			comment_depth += 1;
			lexer->cursor += 2;
		}
		else if (comment_depth > 0 && lexer->cursor[0] == '*' && lexer->cursor[1] == '/')
		{
			comment_depth -= 1;
			lexer->cursor += 2;
		}
		else
		{
			if (comment_depth > 0)
			{
				lexer->cursor += 1;
				continue;
			}
			else break;
		}
	}

	if (lexer->current[0] == 0) token.kind = R_Token_EndOfFile;
	else
	{
		R_u8 c = lexer->current[0];
		lexer->current += 1;

		swicth (c)
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

	R_Token_Bang
	R_Token_BangEq

	R_Token_Mul
	R_Token_MulEq

	R_Token_Div
	R_Token_DivEq

	R_Token_Rem
	R_Token_RemEq

	R_Token_Add
	R_Token_AddEq

	R_Token_Sub
	R_Token_SubEq
	R_Token_Arrow,

	R_Token_Tilde
	R_Token_TildeEq

	R_Token_Equals
	R_Token_EqualsEq

	R_Token_Less
	R_Token_LessEq
	R_Token_LessLess
	R_Token_LessLessEq

	R_Token_Greater
	R_Token_GreaterEq
	R_Token_GreaterGreater
	R_Token_GreaterGreaterEq
	R_Token_TripleGreater
	R_Token_TripleGreaterEq

	R_Token_And
	R_Token_AndEq
	R_Token_AndAnd

	R_Token_Or
	R_Token_OrEq
	R_Token_OrOr

			default:
			{
				if (R_IsAlpha(c) || c == '_')
				{
					//R_Token_Identifier
					R_NOT_IMPLEMENTED;
				}
				else if (R_IsDigit(c))
				{
					// number
					R_NOT_IMPLEMENTED;
				}
				else if (c == '"')
				{
				//R_Token_String
					R_NOT_IMPLEMENTED;
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

int
main(int argc, char** argv)
{
	FILE* test_file = fopen("test_c.txt", "rb");
	fseek(test_file, 0, SEEK_END);
	int test_file_size = ftell(test_file);
	rewind(test_file);

	char* test_file_data = malloc(test_file_size + 1);
	memset(test_file_data, 0, test_file_size + 1);

	fread(test_file_data, test_file_size, 1, test_file);

	fclose(test_file);

	return 0;
}
