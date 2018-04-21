#include "comando.h"

#include <stddef.h>
#include <stdlib.h>
#include <commons/string.h>

static bool _check_arg_count(char **args, size_t expected_count);
static struct command_t *_find_command(struct command_t *commands, int commands_size, char *command_name);
static void _free_string_array(char **str_arr);

int execute_command_line(struct command_t *command_list,
							int commands_list_size,
							char *command_line)
{
	char **tokens = string_split(command_line, " ");
	char *command_name = tokens[0];

	struct command_t *command = _find_command(command_list, commands_list_size, command_name);
	if (command == NULL) {
		_free_string_array(tokens);
		return NO_COMMAND_ERROR;
	}

	char **args = &tokens[1];

	if (_check_arg_count(args, command->n_args)) {
		command->function(args);
		_free_string_array(tokens);
		return 1;
	} else {
		_free_string_array(tokens);
		return ARGUMENT_COUNT_ERROR;
	}
}

static bool _check_arg_count(char **args, size_t expected_count)
{
	int i;
	for (i = 0; i < expected_count; i++) {
		if (args[i] == NULL) {
			return false;
		}
	}

	if (args[i] != NULL) {
		return false;
	}

	return true;
}

static struct command_t *_find_command(struct command_t *commands,
										  int commands_size,
										  char *command_name)
{
	if (command_name == NULL) {
		return NULL;
	}

	int i;
	for (i = 0; i < commands_size; i++) {
		if (string_equals_ignore_case(command_name, commands[i].name)) {
			return &commands[i];
		}
	}

	return NULL;
}

static void _free_string_array(char **str_arr)
{
	int i;
	for (i = 0; str_arr[i] != NULL; i++) {
		free(str_arr[i]);
	}
	free(str_arr);
}
