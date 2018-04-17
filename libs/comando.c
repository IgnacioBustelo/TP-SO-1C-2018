#include "comando.h"

#include <stddef.h>
#include <commons/string.h>


static void _apply_command(struct command_t command, char **args);
static void _free_string_array(char **str_arr);

bool execute_command_line(struct command_t *commands,
							 int commands_size,
							 const char *command_line)
{
	char **tokens = string_split(command_line, " ");
	char *command_name = tokens[0];
	if (command_name == NULL) {
		_free_string_array(tokens);
		return false;
	}

	char **args = &tokens[1];

	int i;
	for (i = 0; i < commands_size; i++) {
		if (string_equals_ignore_case(command_name, commands[i].name)) {
			_apply_command(commands[i], args);
			return true;
		}
	}

	return false;
}

static void _apply_command(struct command_t command, char **args)
{
	if (command.has_args) {
		command_func_with_args command_function = command.function;
		command_function(args);
	} else {
		command_func command_function = command.function;
		command_function();
	}
}

static void _free_string_array(char **str_arr)
{
	int i;
	for (i = 0; str_arr[i] != NULL; i++) {
		free(str_arr[i]);
	}
	free(str_arr);
}
