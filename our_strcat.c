#include "shell.h"
/**
 * our_strcat - concatenates two strings
 * @final_str: string to append to
 * @str_to: string to add
 *
 * Return: a pointer to the resulting string
 */
char *our_strcat(char *final_str, char *str_to)
{
	int i, j;

	i = 0;
	while (final_str[i] != '\0')
		i++;

	j = 0;
	while (str_to[j] != '\0')
	{
		final_str[i] = str_to[j];
		j++;
		i++;
	}
	final_str[i] = '\0';

	return (final_str);
}
