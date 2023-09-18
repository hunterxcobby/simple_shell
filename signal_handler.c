#include "shell.h"
size_t our_strlen(const char *str);
/**
 * signal_handler - handles signals for ctrl c
 * @signal: signal number
 * Return: nothing
 */
void signal_handler(int signal)
{
	char *prompt = "\n$ ";

	(void)signal;
	write(1, prompt, our_strlen(prompt));
	fflush(stdout);
}
