#ifndef CMD_ARGER_H
#define CMD_ARGER_H

//
// cmd arger: a minimal command line argument parsing library.
//
// features:
// - simple easy to use API.
// - no dependancies other than libc.
// - linux & windows support.
// - C99 compatible.
// - parse boolean, integers and strings with error checking.
// - auto generates a help message when parsing fails and can be invoked manually with --help
// - unix style optional arguments
// - optional arguments are allowed to be before, after and inbetween required arguments.
// - terminal colors
//
// non features:
// - short hand optional arguments (with a single hyphen: tar -xvf): as you cannot clearly read what they mean.
//
// USAGE:
//
// everywhere you need to use this library, put this at the top of the file:
// #include "cmd_arger.h"
//
// in a single compilation unit, include the source file.
// #include "cmd_arger.c"
//
// EXAMPLE PROGRAM:
//
// below is an example program. than can be compiled with "clang -o example example.c"
//
// it has 2 required arguments who's values must be set with a command like so:
// ./example string_value 88
//
// it has 3 optional arguments who's values can be set with a command like so:
// ./example --flag --string "overwritten value" --integer 2048 string_value 88
//

/* EXAMPLE PROGRAM

int main(int argc, char** argv) {
	//
	// these are the optional arguments, so you need to provide them with a default value.
	CmdArgerBool flag = cmd_arger_false;
	char* string = "default value";
	int64_t integer = 1024;
	CmdArgerDesc optional_arg_descs[] = {
		cmd_arger_desc_flag(&flag, "flag", "a boolean value"),
		cmd_arger_desc_string(&string, "string", "a string value"),
		cmd_arger_desc_integer(&integer, "integer", "a 64 bit signed integer value"),
	};

	//
	// these are the required arguments, they guaranteed to be initalized with a value after cmd_arger_parse
	char* required_string = NULL;
	int64_t required_integer = 0;
	CmdArgerDesc required_arg_descs[] = {
		cmd_arger_desc_string(&required_string, "string", "a string value"),
		cmd_arger_desc_integer(&required_integer, "integer", "a 64 bit signed integer value"),
	};

	//
	// will parse the arguments from argc and argv.
	// if parsing fails, the program will terminate with an error and/or help message printed to stdout.
	static char* app_name_and_version = "example cmd arger";
	static CmdArgerBool colors = cmd_arger_true;
	cmd_arger_parse(
		optional_arg_descs, sizeof(optional_arg_descs) / sizeof(*optional_arg_descs),
		required_arg_descs, sizeof(required_arg_descs) / sizeof(*required_arg_descs),
		argc, argv, app_name_and_version, colors);
}

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>

#ifndef noreturn
#define noreturn _Noreturn
#endif

typedef uint8_t CmdArgerBool;
#define cmd_arger_true 1
#define cmd_arger_false 0

typedef enum {
	CmdArgerDescKind_flag,
	CmdArgerDescKind_string,
	CmdArgerDescKind_integer,
} CmdArgerDescKind;

typedef struct {
	char* name;
	char* info;
	void* value_out;
	CmdArgerDescKind kind;
} CmdArgerDesc;

//
// these function instantiate a CmdArgerDesc for a particularly typed argument.
//
// @param value_out:
//     a pointer to the data that will have it's value set after calling cmd_arger_parse.
//     if this is a required argument: after cmd_arger_parse returns, the value would have been set.
//     if this is a optional argument: after cmd_arger_parse returns, the value may have been set.
//                                     but be sure to initialize the value to it's default value before parsing.
//
// @param name:
//     the name of the argument.
//     if this is a required argument: then the name will only be shown in the help message.
//     if this is a optional argument: then this should contain no spaces.
//
// @param info:
//     information about the argument, like what it is used for.
//     this is what is displayed in the help message along side the argument.
//
// @return: the initialized CmdArgerDesc that is ready to be passed into cmd_arger_parse.
//
extern CmdArgerDesc cmd_arger_desc_flag(CmdArgerBool* value_out, char* name, char* info);
extern CmdArgerDesc cmd_arger_desc_string(char** value_out, char* name, char* info);
extern CmdArgerDesc cmd_arger_desc_integer(int64_t* value_out, char* name, char* info);

//
// parses the command line arguments by using the argument descriptions passed into the function.
// if parsing fails, the program will terminate with an error and/or help message printed to stdout.
//
// @param optional_args:
//     a pointer to an array of argument descriptions that describe the optional arguments.
//     on the command line, when providing a value for an optional argument:
//         the argument name with a prefix of a double hyphen must come before it's value.
//     an example of an optional argument is: --argument_name argument_value
//
// @param optional_args_count: the number of elements in the array pointed to by @param(optional_args)
//
// @param required_args:
//     a pointer to an array of argument descriptions that describe the required arguments.
//     on the command line, values for required arguments do not required any argument names.
//     they only require that they are provided in the same order of the array of argument descriptions.
//     optional arguments are allowed to be before, after and inbetween required arguments.
//
// @param required_args_count: the number of elements in the array pointed to by @param(required_args)
//
// @param argc: the argc argument from the main function
//
// @param argv: the argv argument from the main function
//
// @param app_name_and_version: a string used to identify the program. this is used in the help message.
//
extern void cmd_arger_parse(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, int argc, char** argv, char* app_name_and_version, CmdArgerBool colors);

//
// prints out the help message and terminates the program. this is automatically called when cmd_arger_parse fails.
//
// @param optional_args: a pointer to an array of argument descriptions that describe the optional arguments.
//
// @param optional_args_count: the number of elements in the array pointed to by @param(optional_args)
//
// @param required_args: a pointer to an array of argument descriptions that describe the required arguments.
//
// @param required_args_count: the number of elements in the array pointed to by @param(required_args)
//
// @param exe_name: the name of the executable that is stored in argv[0].
//
// @param app_name_and_version: a string used to identify the program. this is used in the help message.
//
extern noreturn void cmd_arger_show_help_and_exit(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, char* exe_name, char* app_name_and_version, CmdArgerBool colors);

#endif

