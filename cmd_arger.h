#ifndef CMD_ARGER_H
#define CMD_ARGER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

CmdArgerDesc cmd_arger_desc_flag(CmdArgerBool* value_out, char* name, char* info);
CmdArgerDesc cmd_arger_desc_string(char** string_out, char* name, char* info);
CmdArgerDesc cmd_arger_desc_integer(int64_t* integer_out, char* name, char* info);

void cmd_arger_parse(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, int argc, char** argv, char* app_name_and_version);

_Noreturn void cmd_arger_show_help_and_exit(CmdArgerDesc* optional_args, uint32_t optional_args_count, CmdArgerDesc* required_args, uint32_t required_args_count, char* exe_name, char* app_name_and_version);

#endif
