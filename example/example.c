#include "../cmd_arger.h"
#include "../cmd_arger.c"

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
#ifdef _WIN32
	static CmdArgerBool colors = cmd_arger_false;
#else
	static CmdArgerBool colors = cmd_arger_true;
#endif
	cmd_arger_parse(
		optional_arg_descs, sizeof(optional_arg_descs) / sizeof(*optional_arg_descs),
		required_arg_descs, sizeof(required_arg_descs) / sizeof(*required_arg_descs),
		argc, argv, app_name_and_version, colors);

	printf("successfully parsed arguments...\n\n");

	printf(
		"optional argument values:\n"
		"flag = %s\n"
		"string = %s\n"
		"integer = %zd\n"
		"\n",
		flag ? "true" : "false",
		string,
		integer
	);

	printf(
		"required argument values:\n"
		"string = %s\n"
		"integer = %zd\n"
		"\n",
		required_string,
		required_integer
	);

	return 0;
}

