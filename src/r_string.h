R_bool
R_IsAlpha(R_u8 c)
{
	return ((R_u8)((c & 0xDF) - 'A') <= 'Z' - 'A');
}

R_bool
R_IsHexAlpha(R_u8 c)
{
	return ((R_u8)((c & 0xDF) - 'A') <= 'F' - 'A');
}

R_bool
R_IsDigit(R_u8 c)
{
	return ((R_u8)(c - '0') < 10);
}

R_bool
R_AlphaToLower(R_u8 c)
{
	return c | 0x20;
}

R_bool
R_AlphaToUpper(R_u8 c)
{
	return c & 0xDF;
}

R_bool
R_String_Match(R_String a, R_String b)
{
	R_bool do_match = (a.size == b.size);

	if (a.data != b.data)
	{
		for (R_umm i = 0; i < a.size && do_match; ++i)
		{
			do_match = (a.data[i] == b.data[i]);
		}
	}

	return do_match;
}
