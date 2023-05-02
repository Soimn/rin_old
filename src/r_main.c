#include <windows.h>

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

#define R_KB(n) (1024ULL * (n))
#define R_MB(n) (1024ULL * R_KB(n))
#define R_GB(n) (1024ULL * R_MB(n))

#define R_ALIGNOF(T) ((R_u8)&((struct { char c; T t; }*)0)->t)

void* R_System_ReserveMemory(R_u64 size);
void  R_System_CommitMemory (void* base, R_u64 size);
R_u32 R_System_PageSize     ();
void  R_System_ReleaseMemory(void* base);

#include "r_memory.h"
#include "r_string.h"
#include "r_lexer.h"
#include "r_ast.h"
#include "r_parser.h"

void*
R_System_ReserveMemory(R_u64 size)
{
	void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
	R_ASSERT(result != 0); // TODO:
	return result;
}

void
R_System_CommitMemory(void* base, R_u64 size)
{
	void* result = VirtualAlloc(base, size, MEM_COMMIT, PAGE_READWRITE);
	R_ASSERT(result != 0); // TODO:
}

R_u32
R_System_PageSize()
{
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);

	return sys_info.dwPageSize;
}

void
R_System_ReleaseMemory(void* base)
{
	VirtualFree(base, 0, MEM_RELEASE);
}

int
main(int argc, char** argv)
{
	R_Arena* ast_arena    = R_Arena_Create();
	R_Arena* string_arena = R_Arena_Create();

	FILE* file = fopen("demo.rin", "rb");
	
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	rewind(file);

	R_u8* file_data = R_Arena_Push(string_arena, file_size, R_ALIGNOF(R_u8));

	fread(file_data, 1, file_size, file);
	fclose(file);

	R_Declaration* decls = 0;
	R_bool succeeded = R_Parser_ParseFile(file_data, ast_arena, string_arena, &decls);

	printf("succeeded: %s\n", (succeeded ? "true" : "false"));

	return 0;
}

// TODO:
// - identifiers are strings using the file memory for backing, consider hash consing (ties into multi threading)
// - should parsing be multi threaded? How should e.g. hash consed strings be unified?
