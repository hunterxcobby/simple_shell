#include "shell.h"
/**
 * our_strcmp - copies the string pointed to by src
 * including the terminating null byte (\0)
 * to the buffer pointed to by dest
 * @string1: string to copy into
 * @string2: string to be copied
 *
 * Return: final string
 */
int our_strcmp(const char *string1, const char *string2)
{
	while (*string1 && *string2 && *string1 == *string2)
	{
		string1++;
		string2++;
	}

	return ((int)(*string1 - *string2));
}
