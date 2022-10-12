#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void	    log_wrapper(FILE *fd_log, char *message, char *argv_1)
{
	char	full_log[264];

	strcpy(full_log, message);
	snprintf(&full_log[strlen(full_log)], 254 - strlen(full_log), argv_1);
	full_log[strcspn(full_log, "\n")] = 0;

	fprintf(fd_log, "LOG: %s\n", full_log);
	return;
}

int         main(int argc, char **argv)
{
	char	c = EOF;
	FILE	*fd_log;
    FILE    *fd_arg;
	char	filename_backup_path[100];
	int		fd_dest;

	if (argc != 2)
		printf("Usage: %s filename\n", argv[0]);

    fd_log = fopen("./backups/.log", "w");
	if (fd_log == 0)
	{
		printf("ERROR: Failed to open %s\n", "./backups/.log");
		exit(1);
	}
	log_wrapper(fd_log, "Starting back up: ", argv[1]);

	fd_arg = fopen(argv[1], "r");
	if (arg_file == 0)
	{
		printf("ERROR: Failed to open %s\n", argv[1]);
		exit(1);
	}

	strcpy(filename_backup_path, "./backups/");
	strncat(filename_backup_path, argv[1], 100 - strlen(filename_backup_path));

	fd_dest = open(filename, 0xc1, 0x1b0);
	if (fd < 0)
	{
		printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
		exit(1);
	}

    do
    {
		write(fd_dest, &c, 1);
        c = fgetc(fd_arg);
    } while (c != EOF);

	log_wrapper(fd_log, "Finished back up ", argv[1]);

	fclose(fd_arg);
	close(fd);
	return (0);
}