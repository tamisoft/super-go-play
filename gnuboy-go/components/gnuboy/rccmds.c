#include <stdlib.h>
#include <string.h>

#include "gnuboy.h"
#include "defs.h"
#include "rc.h"
#include "hw.h"
#include "loader.h"



/*
 * define the command functions for the controller pad.
 */

#define CMD_PAD(b, B) \
static int (cmd_ ## b)(int c, char **v) \
{ (void) c; /* avoid warning about unused parameter */ pad_set((PAD_ ## B), v[0][0] == '+'); return 0; } \
static int (cmd_ ## b)(int c, char **v)

CMD_PAD(up, UP);
CMD_PAD(down, DOWN);
CMD_PAD(left, LEFT);
CMD_PAD(right, RIGHT);
CMD_PAD(a, A);
CMD_PAD(b, B);
CMD_PAD(start, START);
CMD_PAD(select, SELECT);


/*
 * the set command is used to set rc-exported variables.
 */

static int cmd_set(int argc, char **argv)
{
	if (argc < 3)
		return -1;
	return rc_setvar(argv[1], argc-2, argv+2);
}



/*
 * the following commands allow keys to be bound to perform rc commands.
 */

static int cmd_bind(int argc, char **argv)
{
	if (argc < 3)
		return -1;
	return rc_bindkey(argv[1], argv[2]);
}

static int cmd_unbind(int argc, char **argv)
{
	if (argc < 2)
		return -1;
	return rc_unbindkey(argv[1]);
}

static int cmd_unbindall()
{
	rc_unbindall();
	return 0;
}

static int cmd_source(int argc, char **argv)
{
	if (argc < 2)
		return -1;
	return rc_sourcefile(argv[1]);
}

static int cmd_quit()
{
	exit(0);
	/* NOT REACHED */
}

static int cmd_reset()
{
	emu_reset();
	return 0;
}

static int cmd_savestate(int argc, char **argv)
{
	state_save(argc > 1 ? atoi(argv[1]) : -1);
	return 0;
}

static int cmd_loadstate(int argc, char **argv)
{
	state_load(argc > 1 ? atoi(argv[1]) : -1);
	return 0;
}

#ifndef GNUBOY_NO_SCREENSHOT
static int cmd_screenshot(int argc, char **argv)
{
	int i=0;
	char *filename=NULL;

	if (argc >= 2)
		filename = argv[1];

	return vid_screenshot(filename);
}
#endif /* GNUBOY_NO_SCREENSHOT */


/*
 * table of command names and the corresponding functions to be called
 */

rccmd_t rccmds[] =
{
	RCC((char *)"+up", cmd_up),
	RCC((char *)"-up", cmd_up),
	RCC((char *)"+down", cmd_down),
	RCC((char *)"-down", cmd_down),
	RCC((char *)"+left", cmd_left),
	RCC((char *)"-left", cmd_left),
	RCC((char *)"+right", cmd_right),
	RCC((char *)"-right", cmd_right),
	RCC((char *)"+a", cmd_a),
	RCC((char *)"-a", cmd_a),
	RCC((char *)"+b", cmd_b),
	RCC((char *)"-b", cmd_b),
	RCC((char *)"+start", cmd_start),
	RCC((char *)"-start", cmd_start),
	RCC((char *)"+select", cmd_select),
	RCC((char *)"-select", cmd_select),
	
#ifndef GNUBOY_NO_SCREENSHOT
	RCC((char *)"screenshot", cmd_screenshot),
#endif /* GNUBOY_NO_SCREENSHOT */
	
	RCC((char *)"set", cmd_set),
	RCC((char *)"bind", cmd_bind),
	RCC((char *)"unbind", cmd_unbind),
	RCC((char *)"unbindall", cmd_unbindall),
	RCC((char *)"source", cmd_source),
	RCC((char *)"reset", cmd_reset),
	RCC((char *)"quit", cmd_quit),
	RCC((char *)"savestate", cmd_savestate),
	RCC((char *)"loadstate", cmd_loadstate),
	
	RCC_END
};





int rc_command(char *line)
{
	int i, argc, ret;
	char *argv[128], *linecopy;

	linecopy = malloc(strlen(line)+1);
	strcpy(linecopy, line);
	
	argc = splitline(argv, (sizeof argv)/(sizeof argv[0]), linecopy);
	if (!argc)
	{
		free(linecopy);
		return -1;
	}
	
	for (i = 0; rccmds[i].name; i++)
	{
		if (!strcmp(argv[0], rccmds[i].name))
		{
			ret = rccmds[i].func(argc, argv);
			free(linecopy);
			return ret;
		}
	}

	/* printf("unknown command: %s\n", argv[0]); */
	free(linecopy);
	
	return -1;
}
