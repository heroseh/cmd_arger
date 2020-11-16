
CmdArgerDesc cmd_arger_desc_flag(CmdArgerBool* value_out, char* name, char* info) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = value_out,
		.kind = CmdArgerDescKind_flag,
	};
}

CmdArgerDesc cmd_arger_desc_string(char** string_out, char* name, char* info) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = string_out,
		.kind = CmdArgerDescKind_string,
	};
}

CmdArgerDesc cmd_arger_desc_integer(int64_t* integer_out, char* name, char* info) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = integer_out,
		.kind = CmdArgerDescKind_integer,
	};
}

void cmd_arger_parse(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, int argc, char** argv, char* app_name_and_version) {
	// the first arg is the name of the program
	int arg_idx = 1;

	//
	// print help if we dont have enough of the required args
	// less than or equal because the first arg is the name of the program and not an argument
	if (argc <= required_args_count) {
		goto PRINT_HELP_REQUIRED_ARGS;
	}

	int required_args_idx = 0;

	while (arg_idx < argc) {
		char* name_or_value = argv[arg_idx];
		uint32_t name_or_value_len = strlen(name_or_value);

		CmdArgerDesc* desc = NULL;
		CmdArgerBool is_optional_arg = name_or_value_len > 2 && name_or_value[0] == '-' && name_or_value[1] == '-';
		if (is_optional_arg) {
			// true when we still require an argument and there is not enough room for them all
			CmdArgerBool should_be_required_arg = argc - arg_idx <= required_args_count - required_args_idx;
			if (should_be_required_arg) { goto PRINT_HELP_REQUIRED_ARGS; }

			// move after the --
			name_or_value += 2;
			name_or_value_len -= 2;

			if (name_or_value_len == 4 && memcmp(name_or_value, "help", 4) == 0) {
				goto PRINT_HELP;
			}

			// locate the argument description that matches this name
			for (int i = 0; i < optional_args_count; i += 1) {
				CmdArgerDesc* arg = &optional_args[i];
				if (strcmp(arg->name, name_or_value) == 0) {
					desc = arg;
					break;
				}
			}

			if (desc == NULL) {
				printf("error: unsupported argument '%.*s'\n\n", (int)name_or_value_len, name_or_value);
				goto PRINT_HELP;
			}
		} else {
			if (required_args_idx >= required_args_count) {
				printf("error: all required arguments have already been met\n\n");
				goto PRINT_HELP;
			}

			// get the argument description based on how far from the end we are
			desc = &required_args[required_args_idx];
			required_args_idx += 1;
		}

		//
		// assign the values
		switch (desc->kind) {
			case CmdArgerDescKind_flag:
				*(CmdArgerBool*)desc->value_out = cmd_arger_true;
				break;
			case CmdArgerDescKind_string:
			case CmdArgerDescKind_integer: {
				if (is_optional_arg) {
					// move off the argument name to get the value
					arg_idx += 1;
					if (arg_idx >= argc) {
						printf("error: argument '%s' must have a value\n\n", desc->name);
						goto PRINT_HELP;
					}
					name_or_value = argv[arg_idx];
				}

				if (desc->kind == CmdArgerDescKind_string) {
					*(char**)desc->value_out = name_or_value;
				} else {
					*(int64_t*)desc->value_out = strtol(name_or_value, NULL, 10);
				}
				break;
			};
		}

		arg_idx += 1;
	}

	return;
PRINT_HELP_REQUIRED_ARGS:
	printf("\e[91merror:\e[0m expected atleast %u required arguments\n\n", required_args_count);
PRINT_HELP:
	cmd_arger_show_help_and_exit(optional_args, optional_args_count, required_args, required_args_count, argv[0], app_name_and_version);
}

_Noreturn void cmd_arger_show_help_and_exit(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, char* exe_name, char* app_name_and_version) {
	printf("\e[97m------ %s help ------\e[0m\n", app_name_and_version);
	//
	// print usage line
	printf("\e[92musage:\e[0m %s", exe_name);
	if (optional_args_count > 0) {
		printf(" [OPTIONAL_ARGS...]");
	}
	for (int i = 0; i < required_args_count; i += 1) {
		CmdArgerDesc* arg = &required_args[i];
		printf(" %s", arg->name);
	}
	putchar('\n');

	//
	// print required args help
	for (int i = 0; i < required_args_count; i += 1) {
		CmdArgerDesc* arg = &required_args[i];
		printf("\t%s: %s\n", arg->name, arg->info);
	}

	//
	// print optional args help
	puts("OPTIONAL_ARGS:");
	puts("\t--help: this help screen");
	for (int i = 0; i < optional_args_count; i += 1) {
		CmdArgerDesc* arg = &optional_args[i];
		printf("\t--%s: %s\n", arg->name, arg->info);
	}

	exit(1);
}
