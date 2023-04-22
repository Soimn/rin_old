#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>

typedef signed __int8  R_i8;
typedef signed __int16 R_i16;
typedef signed __int32 R_i32;
typedef signed __int64 R_i64;

typedef unsigned __int8  R_u8;
typedef unsigned __int16 R_u16;
typedef unsigned __int32 R_u32;
typedef unsigned __int64 R_u64;

typedef signed __int64 R_imm;
typedef unsigned __int64 R_umm;

#define R_I8_MIN  ((R_i8) 0x80)
#define R_I16_MIN ((R_i16)0x8000)
#define R_I32_MIN ((R_i32)0x80000000)
#define R_I64_MIN ((R_i64)0x8000000000000000)

#define R_I8_MAX  ((R_i8) 0x7F)
#define R_I16_MAX ((R_i16)0x7FFF)
#define R_I32_MAX ((R_i32)0x7FFFFFFF)
#define R_I64_MAX ((R_i64)0x7FFFFFFFFFFFFFFF)

#define R_U8_MAX  ((R_u8) 0xFF)
#define R_U16_MAX ((R_u16)0xFFFF)
#define R_U32_MAX ((R_u32)0xFFFFFFFF)
#define R_U64_MAX ((R_u64)0xFFFFFFFFFFFFFFFF)

typedef float R_f32;
typedef double R_f64;

typedef R_u8 R_bool;

#define R_false 0
#define R_true 1

typedef struct R_String
{
	R_u32 size;
	R_u8* data;
} R_String;

#define R_STRING(str) (R_String){ .data = (R_u8*)(str), .size = sizeof(str) - 1 }

typedef R_u8* R_ZString;

#define R_ASSERT(E) ((E) ? 1 : *(volatile int*)0)
#define R_NOT_IMPLEMENTED R_ASSERT(!"NOT_IMPLEMENTED")

#include "r_memory.h"
#include "r_string.h"
#include "r_lexer.h"

int
main(int argc, char** argv)
{
	FILE* test_file = fopen("test_num.txt", "rb");
	fseek(test_file, 0, SEEK_END);
	int test_file_size = ftell(test_file);
	rewind(test_file);

	char* test_file_data = malloc(test_file_size + 1);
	memset(test_file_data, 0, test_file_size + 1);

	fread(test_file_data, test_file_size, 1, test_file);

	R_Lexer lexer = R_Lexer_Init(test_file_data);

	for (;;)
	{
		R_Token token = R_Lexer_NextToken(&lexer);

		if (token.kind == R_Token_EndOfFile) break;
		else
		{
			if      (token.kind == R_Token_Integer)       printf("Integer: %llu\n", token.integer);
			else if (token.kind == R_Token_BinaryInteger) printf("BinInt: 0x%llX\n", token.integer);
			else if (token.kind == R_Token_HexInteger)    printf("HexInt: 0x%llX\n", token.integer);
			else if (token.kind == R_Token_Float64)       printf("Float: %.30F\n", token.float64);
			else if (token.kind == R_Token_HexFloat32)    printf("HexFloat32: %.30f\n", token.float32);
			else if (token.kind == R_Token_HexFloat64)    printf("HexFloat64: %.30F\n", token.float64);
		}
	}

	fclose(test_file);

	return 0;
}
