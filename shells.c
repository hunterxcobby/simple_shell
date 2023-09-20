#include "shell.h"

int our_strcmp(const char *string1, const char *string2);
void signal_handler(int signal);
size_t our_strlen(const char *str);
char *our_strdup(const char *str);
char *our_strcat(char *final_str, char *str_to);
char *our_strcpy(char *dest, char *src);
ssize_t our_getline(char **line, size_t *len, FILE *stream);

/**
 * main - building my own shell
 * @ac: argument count
 * @av: argument vector
 *
 * desc: program does the following
 * 1. prompts a user to enter a line (command)
 * 2. command is passed into the program through main ()
 * 3. tokenizes the command into smaller strings
 * 4. if first token (program name) contains '/' means it is a path
 * 5. if fist token (program name) does not contain
 * '/' means it is a raw program name
 *
 * 6. if path () {execve(path, arguments, NULL)}
 * 7. if (!path) {search through all directories in $PATH environment
 * variable to find a program,
 * 8. that has the same name as the program using strcmp. if strcmp == 0,
 * execve(strcat(directory, line), arguments, NULL);
 * 4. smaller strings will represent the arguments parameter of execve function
 * 5. forks program to create a new process (child)
 * 6. uses execve function to execute the command in the child process
 * 7. parent process waits for another command to be inputed by user
 *
 * Return: always 0
 */

int main(int ac, char **av)
{
	/* for prompting user */
	char *prompt = "$ ";
	ssize_t characters_read = 0;
	/* for testing */
	ssize_t prompt_bytes_written = 0;
	/* for getline() */
	char *line = NULL;
	size_t len = 0;
	size_t i = 0;
	/* for freeing dir*/
	size_t free_dir_i = 0;
	/* for strtok() */
	const char *delimiters = NULL;
	char *command_token = NULL;
	/* for execve */
	char **command = NULL;
	/* for allocating space for command array */
	size_t max_command = 0;
	/* for fork */
	int child_pid = 0;
	int status = 0;
	/* for getenv() */
	char *path_env_var = NULL;
	/* for checking if string is path */
	int slash_i = 0;
	char *path = NULL;
	/* for allocating space for command array */
	size_t max_directories = 0;
	/* for tokenized path env variable to directories */
	char **directories = NULL;
	char *directories_token = NULL;
	size_t directories_i = 0;
	/* for opening directories in PATH */
	DIR *directory_stream = NULL;
	size_t searching_i = 0;
	/* for reading entries in directory */
	struct dirent *entry = NULL;
	/* for concatenating directory with program name */
	char *concat_path = NULL;
	/* for if program is found in an entry */
	int found = 0;
	/* for printing error message */
	char *err_msg = NULL;
	/* for malloc */
	size_t size = 0;

	(void)ac;

	/* handling signals */
	signal(SIGINT, signal_handler);

	while (1)
	{
		line = NULL;
		delimiters = " \n\t\r\a";
		path = NULL;

		/** 1. prompts a user to enter a line (command) */
		if (isatty(STDIN_FILENO))
		{
			prompt_bytes_written = write(1, prompt, our_strlen(prompt));
			if (prompt_bytes_written == -1)
			{
				perror("write");
				exit(1);
			}
		}

		/* flushing stdout */
		fflush(stdout);

		/** get command from user */
		characters_read = our_getline(&line, &len, stdin);
		/*printf("line is: %s\n", line);*/
		if (characters_read == EOF)
		{
			break;
		}
		else if (characters_read == -1)
		{
			break;
		}

		/* if command is exit */
		if (our_strcmp(line, "exit") == 0)
		{
			free(line);
			exit(0);
		}

		/* if command is env */
		if (our_strcmp(line, "env") == 0)
		{
			int i = 0;

			for (; environ[i]; i++)
			{
				/*print each environment variable*/
				write(1, environ[i], our_strlen(environ[i]));
				write(1, "\n", 1);
			}
			free(line);
			continue;
		}

		/* 3. tokenizes the command into smaller strings */
		/**
		 * 4. smaller strings will represent the arguments
		 * parameter of execve function
		 */
		command_token = strtok(line, delimiters);
		if (command_token == NULL)
		{
			free(line);
			continue;
		}

		/* allocating space for command array */
		max_command = 10;
		command = malloc(sizeof(char *) * max_command);
		i = 0;
		while (command_token != NULL)
		{
			/* reallocate memory when max commands reach */
			if (i >= max_command)
			{
				max_command *= 2;
				command = realloc(command, sizeof(char *) * max_command);
				if (command == NULL)
				{
					perror("./hsh");
				}
			}

			/* put tokens into command array */
			command[i] = our_strdup(command_token);
			if (command[i] == NULL)
				continue;
			command_token = strtok(NULL, delimiters);
			i++;
		}
		/* set last element of command array to NULL */
		command[i] = NULL;

		/* check if program name is a path or just a file name */
		slash_i = 0;
		while (line[slash_i] != '\0')
		{
			if (line[slash_i] == '/')
			{
				path = line;
				break;
			}
			slash_i++;
		}

		/* if program name is not a path */
		if (path == NULL)
		{
			/* taking environment variable PATH */
			char *PATH = getenv("PATH");

			if (PATH == NULL)
				continue;

			/*CHECK IF STRDUP SUCCEEDED*/
			path_env_var = our_strdup(PATH);
			if (path_env_var == NULL)
				continue;

			/* tokenize PATH into array directories*/
			directories_token = strtok(path_env_var, ":");
			if (directories_token == NULL)
			{
				perror("av[0]");/*PROGRAM NAME ./hsh*/
			}

			/* allcoating space for directories array */
			max_directories = 100;/*max_path = 4096*/
			directories = malloc(sizeof(char *) * max_directories);
			directories_i = 0;

			while (directories_token != NULL)
			{
				if (directories_i >= max_directories)
				{
					max_directories *= 2;
					directories = realloc(directories, sizeof(char *) * max_directories);
					if (directories == NULL)
					{
						perror("realloc");
					}
				}
				/*what if strdup fails?*/
				directories[directories_i] = our_strdup(directories_token);
				if (PATH == NULL)
					continue;
				directories_token = strtok(NULL, ":");
				directories_i++;
			}
			/* null terminate directories array */
			directories[directories_i] = NULL;

			/* opening all directories to look for line(e.g "ls") */
			searching_i = 0;
			while (directories[searching_i] != NULL)
			{
				directory_stream = opendir(directories[searching_i]);

				if (directory_stream == NULL)
				{
					searching_i++;
					continue;
				}

				found = 0;
				entry = readdir(directory_stream);
				while (entry != NULL)
				{
					if (our_strcmp(command[0], entry->d_name) == 0)
					{
						found = 1;
						break;
					}

					entry = readdir(directory_stream);
				}

				closedir(directory_stream);

				if (found == 1)
					break;

				searching_i++;
				if (searching_i == directories_i)/*directories[searching_i] == NULL*/
				{
					/*ERRONO = 2: NO SUCH FILE*/
					/*concat(command[0], command_count)*/
					/*PERROR("LOCATION" ":" "NO SUCH FILE FOUND")*/
					/*char *err_msg = ": command not found\n";*/

					/*write(2, line, our_strlen(line));*/
					/*write(2, err_msg, our_strlen(err_msg));*/
					/*found = -1;*/
					/* err_msg = NULL;*/
					perror(command[0]);
				}
			}
		} /* END OF if (path == NULL) */

		/** 5. forks program to create a new process (child) */
		/*check if i found executatble path use found*/
		child_pid = fork();
		if (child_pid == -1)
		{
			perror("fork");
		}
		else if (child_pid == 0)
		{
			/* this is the child */
			if (path == NULL)/*if not a path*/
			{
				size = our_strlen(directories[searching_i]) + our_strlen(line) + 2;
				concat_path = malloc(size);
				if (concat_path == NULL)
				{
					perror("malloc");
				}

				/* forming a path to be executed */
				our_strcpy(concat_path, directories[searching_i]);
				our_strcat(concat_path, "/");
				our_strcat(concat_path, command[0]/*line*/);

				/* executing the program */
				command[0] = concat_path;
				execve(command[0], command, environ);

				/* if execve fails */
				perror("execve");
				free(concat_path);
				free(command);
			}
			else
			{
				(void)av;
				(void)err_msg;
				execve(command[0], command, environ);

				/* handle execve failure */
				perror("execve");
			}

		}

		/* continuing what happens in the parent (this program) */
		wait(&status);

		/* freeing directories_i */
		if (path == NULL)
		{
			for (free_dir_i = 0; directories[free_dir_i] != NULL; free_dir_i++)
			{
				free(directories[free_dir_i]);
			}
		}

		/* freeing command[i] */
		for (i = 0; command[i] != NULL; i++)
		{
			free(command[i]);
		}

		free(directories);
		free(path_env_var);
		free(concat_path);
		free(command);
		free(line);
	} /*end of loop;*/
	free(line);
	if (isatty(STDIN_FILENO) == 1)
		write(1, "\n", 1);
	return (0);
}
