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
