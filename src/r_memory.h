void
R_Copy(void* src, void* dst, R_umm size)
{
	R_u8* bsrc = src;
	R_u8* bdst = dst;

	for (R_umm i = 0; i < size; ++i)
	{
		bdst[i] = bsrc[i];
	}
}

void
R_Move(void* src, void* dst, R_umm size)
{
	R_u8* bsrc = src;
	R_u8* bdst = dst;

	if (bsrc > bdst) R_Copy(bsrc, bdst, size);
	else
	{
		for (R_i64 i = size - 1; i >= 0; ++i)
		{
			bdst[i] = bsrc[i];
		}
	}
}

#define R_ARENA_RESERVE_SIZE R_GB(32)

typedef struct R_Arena
{
	R_u64 offset;
	R_u32 page_size;
} R_Arena;

R_Arena*
R_Arena_Create()
{
	R_Arena* arena = R_System_ReserveMemory(R_ARENA_RESERVE_SIZE);

	R_u32 page_size = R_System_PageSize();
	R_ASSERT((page_size & (page_size - 1)) == 0);

	R_System_CommitMemory(arena, page_size);

	*arena = (R_Arena){
		.offset    = sizeof(R_Arena),
		.page_size = page_size,
	};

	return arena;
}

void*
R_Arena_Push(R_Arena* arena, R_umm size, R_u8 alignment)
{
	R_umm aligned_offset = (arena->offset + (alignment - 1)) & ~(alignment - 1);

	R_umm page_boundary = (arena->offset + arena->page_size) & ~(arena->page_size - 1);
	if (aligned_offset + size >= page_boundary)
	{
		R_umm overflow = (aligned_offset + size) - page_boundary;

		R_umm commit_size = (overflow + arena->page_size) & ~(arena->page_size - 1);

		R_System_CommitMemory((R_u8*)arena + page_boundary, commit_size);
	}

	arena->offset = aligned_offset + size;

	return (R_u8*)arena + aligned_offset;
}

void
R_Arena_Pop(R_Arena* arena, R_umm size)
{
	R_ASSERT(arena->offset - sizeof(R_Arena) >= size);
	arena->offset -= size;
}

void
R_Arena_Reset(R_Arena* arena)
{
	arena->offset = sizeof(R_Arena);
}
