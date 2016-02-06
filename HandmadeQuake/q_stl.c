#include "quakedef.h"

void Q_strcpy(char *dest, const char *src)
{
	while (*src)
	{
		*dest++ = *src++;
	}
	*dest = 0;
}

void Q_strncpy(char *dest, const char *src, size_t count)
{
	if (count < 0)
		return;

	while (*src && count)
	{
		*dest++ = *src++;
		--count;
	}

	while (count)
	{
		*dest++ = 0;
		--count;
	}
}

size_t Q_strlen(const char *str)
{
	size_t count = 0;
	while (str[count])
		++count;

	return count;
}

int Q_strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2)
	{
		if (!*s1)
			return 0;
		++s1;
		++s2;
	}

	return ((*s1 < *s2) ? -1 : 1);
}

// decimal or hexadecimal
// negative and positive
int Q_atoi(const char *str)
{
	int sign = 1;
	int val = 0;
	char c;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}

	// hexadecimal
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;
		for (;;)
		{
			c = *str;
			str++;
			if (c >= '0' && c <= '9')
				val = val * 16 + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = val * 16 + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = val * 16 + c - 'A' + 10;
			else
				return val*sign;
		}
	}

	// decimal
	for (;;)
	{
		c = *str;
		str++;
		if (c < '0' || c > '9')
			return sign * val;
		val = val * 10 + c - '0';
	}
}
