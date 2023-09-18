#include "shell.h"
size_t our_strlen(const char *str);
/**
 * our_strdup - duplicating string
 * @str: string to be duplicated
 * Return: string
 */
char *our_strdup(const char *str)
{
	size_t length = our_strlen(str) + 1;
	char *new_str = (char *)malloc(length);
	size_t i = 0;

	if (new_str == NULL)
		return (NULL);

	for (; i < length; i++)
		new_str[i] = str[i];

	return (new_str);
}
