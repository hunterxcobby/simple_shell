#include "shell.h"
/**
 * our_strcpy - copies the string pointed to by src
 * including the terminating null byte (\0)
 * to the buffer pointed to by dest
 * @dest: string to copy into
 * @src: string to be copied
 *
 * Return: final string
 */
char *our_strcpy(char *dest, char *src)
{
	int len = 0, i = 0;

	len = 0;
	while (src[len] != '\0')
		len++;

	for (; i < len; i++)
		dest[i] = src[i];

	dest[i] = '\0';

	return (dest);
}
