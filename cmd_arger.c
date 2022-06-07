
CmdArgerDesc cmd_arger_desc_flag(CmdArgerBool* value_out, const char* name, const char* info) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = value_out,
		.kind = CmdArgerDescKind_flag,
	};
}

CmdArgerDesc cmd_arger_desc_string(char** value_out, const char* name, const char* info) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = value_out,
		.kind = CmdArgerDescKind_string,
	};
}

CmdArgerDesc cmd_arger_desc_integer(int64_t* value_out, const char* name, const char* info) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = value_out,
		.kind = CmdArgerDescKind_integer,
	};
}

CmdArgerDesc cmd_arger_desc_float(double* value_out, const char* name, const char* info) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = value_out,
		.kind = CmdArgerDescKind_float,
	};
}

CmdArgerDesc cmd_arger_desc_enum(int64_t* value_out, const char* name, const char* info, CmdArgerEnumDesc* enum_descs, uint32_t enum_descs_count) {
	return (CmdArgerDesc) {
		.name = name,
		.info = info,
		.value_out = value_out,
		.kind = CmdArgerDescKind_enum,
		.enum_descs = enum_descs,
		.enum_descs_count = enum_descs_count,
	};
}

void cmd_arger_parse(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, int argc, char** argv, const char* app_name_and_version, CmdArgerBool colors) {
	// argument index 0 (the first argument) is the name of the program.
	// so we are going to start after that.
	int arg_idx = 1;

	uint32_t required_args_idx = 0;

	//
	// iterate over the arguments and parse them one by one.
	while (arg_idx < argc) {
		char* name_or_value = argv[arg_idx];
		uint32_t name_or_value_len = strlen(name_or_value);

		CmdArgerDesc* desc = NULL;
		CmdArgerBool is_optional_arg = name_or_value_len > 2 && name_or_value[0] == '-' && name_or_value[1] == '-';
		if (is_optional_arg) {
			//
			// move after the --
			name_or_value += 2;
			name_or_value_len -= 2;

			//
			// if we find --help, then just ignore everything, print the help message and exist the program
			if (name_or_value_len == 4 && memcmp(name_or_value, "help", sizeof("help")) == 0) {
				goto PRINT_HELP;
			}

			//
			// locate the argument description that matches this name
			for (uint32_t i = 0; i < optional_args_count; i += 1) {
				CmdArgerDesc* arg = &optional_args[i];
				if (strcmp(arg->name, name_or_value) == 0) {
					desc = arg;
					break;
				}
			}

			//
			// stop if we do not have an argument description that matches the name
			if (desc == NULL) {
				const char* fmt = colors
					? "\x1b[91merror:\x1b[0m unsupported optional argument '--%.*s'\n\n"
					: "error: unsupported optional argument '--%.*s'\n\n";
				printf(fmt, (int)name_or_value_len, name_or_value);
				goto PRINT_HELP;
			}
		} else {
			//
			// we have a required argument
			//

			//
			// stop if we have too many required arguments passed in
			if (required_args_idx >= required_args_count) {
				const char* fmt = colors
					? "\x1b[91merror:\x1b[0m more than %u required arguments have been provided\n\n"
					: "error: more than %u required arguments have been provided\n\n";
				printf(fmt, required_args_count);
				goto PRINT_HELP;
			}

			//
			// get the argument description based on how many requirements have already been parsed
			desc = &required_args[required_args_idx];
			required_args_idx += 1;

			//
			// flags are not allowed in argument descriptions so abort
			if (desc->kind == CmdArgerDescKind_flag) {
				const char* msg = colors
					? "\x1b[91mcmd arger usage error:\x1b[0m cannot have a flag as a required argument"
					: "cmd arger usage error: cannot have a flag as a required argument";
				puts(msg);
				abort();
			}
		}

		//
		// assign the values back out.
		if (desc->kind == CmdArgerDescKind_flag) {
			*(CmdArgerBool*)desc->value_out = cmd_arger_true;
		} else {
			if (is_optional_arg) {
				//
				// we parsed the optional argument name earlier.
				// so move off the optional argument name to get the value
				arg_idx += 1;
				if (arg_idx >= argc) {
					const char* fmt = colors
						? "\x1b[91merror:\x1b[0m argument '%s' must have a value\n\n"
						: "error: argument '%s' must have a value\n\n";
					printf(fmt, desc->name);
					goto PRINT_HELP;
				}
				name_or_value = argv[arg_idx];
				name_or_value_len = strlen(name_or_value);
			}

			switch (desc->kind) {
				case CmdArgerDescKind_flag: break;
				case CmdArgerDescKind_string:
					*(char**)desc->value_out = name_or_value;
					break;
				case CmdArgerDescKind_integer: {
					char* end_ptr = NULL;
					long v = strtol(name_or_value, &end_ptr, 10);
					if ((v == LONG_MIN || v == LONG_MAX) && errno == ERANGE) {
						//
						// overflow
						char* prefix = is_optional_arg ? "--" : "";
						const char* fmt = colors
							? "\x1b[91merror:\x1b[0m argument '%s%s' has overflowed a 64-bit signed integer for it's value '%s'\n\n"
							: "error: argument '%s%s' has overflowed a 64-bit signed integer for it's value '%s'\n\n";
						printf(fmt, prefix, desc->name, name_or_value);
						goto PRINT_HELP;
					} else if (end_ptr - name_or_value != name_or_value_len) {
						//
						// not an integer
						char* prefix = is_optional_arg ? "--" : "";
						const char* fmt = colors
							? "\x1b[91merror:\x1b[0m argument '%s%s' expected an integer value but got '%s'\n\n"
							: "error: argument '%s%s' expected an integer value but got '%s'\n\n";

						printf(fmt, prefix, desc->name, name_or_value);
						goto PRINT_HELP;
					}
					*(int64_t*)desc->value_out = v;
					break;
				};
				case CmdArgerDescKind_float: {
					char* end_ptr = NULL;
					double v = strtod(name_or_value, &end_ptr);
					if ((v == -HUGE_VAL || v == HUGE_VAL) && errno == ERANGE) {
						//
						// overflow
						char* prefix = is_optional_arg ? "--" : "";
						const char* fmt = colors
							? "\x1b[91merror:\x1b[0m argument '%s%s' has overflowed a 64-bit floating point for it's value '%s'\n\n"
							: "error: argument '%s%s' has overflowed a 64-bit floating point for it's value '%s'\n\n";
						printf(fmt, prefix, desc->name, name_or_value);
						goto PRINT_HELP;
					} else if (end_ptr - name_or_value != name_or_value_len) {
						//
						// not a float
						char* prefix = is_optional_arg ? "--" : "";
						const char* fmt = colors
							? "\x1b[91merror:\x1b[0m argument '%s%s' expected an floating point value but got '%s'\n\n"
							: "error: argument '%s%s' expected an floating point value but got '%s'\n\n";

						printf(fmt, prefix, desc->name, name_or_value);
						goto PRINT_HELP;
					}
					*(double*)desc->value_out = v;
					break;
				};
				case CmdArgerDescKind_enum: {
					//
					// find the enum that matches the name and use it's value
					uint32_t count = desc->enum_descs_count;
					uint32_t i = 0;
					for (; i < count; i += 1) {
						CmdArgerEnumDesc* enum_desc = &desc->enum_descs[i];
						if (strcmp(enum_desc->name, name_or_value) == 0) {
							*(int64_t*)desc->value_out = enum_desc->value;
							break;
						}
					}

					//
					// check for if there was no name that matches
					if (i == count) {
						char* prefix = is_optional_arg ? "--" : "";
						const char* fmt = colors
							? "\x1b[91merror:\x1b[0m argument '%s%s' has an unexpected enumeration value of '%s'\n\n"
							: "error: argument '%s%s' has an unexpected enumeration value of '%s'\n\n";

						printf(fmt, prefix, desc->name, name_or_value);
						goto PRINT_HELP;
					}
					break;
				};
			}
		}

		arg_idx += 1;
	}

	//
	// make sure we have all the required arguments
	if (required_args_idx < required_args_count) {
		const char* fmt = colors
			? "\x1b[91merror:\x1b[0m expected %u required arguments but got %u\n\n"
			: "error: expected %u required arguments but got %u\n\n";
		printf(fmt, required_args_count, required_args_idx);
		goto PRINT_HELP;
	}

	//
	// success, so lets return
	return;

PRINT_HELP:
	cmd_arger_show_help_and_exit(optional_args, optional_args_count, required_args, required_args_count, argv[0], app_name_and_version, colors);
}

void _cmd_arger_print_help_enum_values(CmdArgerDesc* desc) {
	uint32_t count = desc->enum_descs_count;
	uint32_t i = 0;
	for (; i < count; i += 1) {
		CmdArgerEnumDesc* enum_desc = &desc->enum_descs[i];
		printf("\t\t%s: %s\n", enum_desc->name, enum_desc->info);
	}
}

void cmd_arger_show_help_and_exit(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, const char* exe_name, const char* app_name_and_version, CmdArgerBool colors) {
	const char* fmt = colors
		? "\x1b[1m------ %s help ------\n\x1b[0m"
		: "------ %s help ------\n";
	printf(fmt, app_name_and_version);

    int opt_pad = 0, req_pad = 0;
	for (uint32_t i = 0; i < required_args_count; i += 1) {
        int arg_len = strlen(required_args[i].name);
        if (arg_len > req_pad)
        {   req_pad = arg_len;   }
    }
	for (uint32_t i = 0; i < optional_args_count; i += 1) {
        int arg_len = strlen(optional_args[i].name);
        if (arg_len > opt_pad)
        {   opt_pad = arg_len;   }
    }


	//
	// print usage line
	fmt = colors
		? "\x1b[1musage:\x1b[0m %s"
		: "usage: %s";
	printf(fmt, exe_name);
	if (optional_args_count > 0) {
		printf(" [OPTIONAL_ARGS...]");
	}
	for (uint32_t i = 0; i < required_args_count; i += 1) {
		CmdArgerDesc* arg = &required_args[i];
		printf(" %s", arg->name);
	}
	putchar('\n');

	//
	// print required args help
	if (required_args_count)
		putchar('\n');

	fmt = colors
		? "\t\x1b[91m%-*s\x1b[0m : %s\n"
		: "\t%-*s : %s\n";
	for (uint32_t i = 0; i < required_args_count; i += 1) {
		CmdArgerDesc* arg = &required_args[i];
		printf(fmt, req_pad, arg->name, arg->info);
		if (arg->kind == CmdArgerDescKind_enum)
			_cmd_arger_print_help_enum_values(arg);
	}

	//
	// print optional args help
	puts("\nOPTIONAL_ARGS:");
	fmt = colors
		? "\t\x1b[93m--%-*s\x1b[0m : this help screen\n"
		: "\t--%-*s : this help screen\n";
	printf(fmt, opt_pad, "help");

	fmt = colors
		? "\t\x1b[93m--%-*s\x1b[0m : %s\n"
		: "\t--%-*s : %s\n";
	for (uint32_t i = 0; i < optional_args_count; i += 1) {
		CmdArgerDesc* arg = &optional_args[i];
		printf(fmt, opt_pad, arg->name, arg->info);
		if (arg->kind == CmdArgerDescKind_enum)
			_cmd_arger_print_help_enum_values(arg);
	}

	exit(1);
}

