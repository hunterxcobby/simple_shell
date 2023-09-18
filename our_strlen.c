#include "shell.h"
/**
 * our_strlen - length of string
 * @str: string
 * Return: number of characters
 */
size_t our_strlen(const char *str)
{
	size_t i = 0;

	while (str[i] != '\0')
		i++;
	return (i);
}
