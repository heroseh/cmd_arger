#include "../cmd_arger.h"
#include "../cmd_arger.c"

#define example_src_file "example.c"
#ifdef _WIN32
#define example_out_file "example.exe"
#else
#define example_out_file "example"
#endif

int main(int argc, char** argv) {
	CmdArgerBool debug = cmd_arger_false;
	CmdArgerBool debug_address = cmd_arger_false;
	CmdArgerBool debug_memory = cmd_arger_false;
	CmdArgerBool clean = cmd_arger_false;

#ifdef _WIN32
	char* compiler = "clang.exe";
#else
	char* compiler = "clang";
#endif
	int64_t opt = 0;
	CmdArgerDesc desc[] = {
		cmd_arger_desc_flag(&debug, "debug", "compile in debuggable executable"),
		cmd_arger_desc_flag(&clean, "clean", "remove any built binaries"),
		cmd_arger_desc_flag(&debug_address, "debug_address", "turns on address sanitizer"),
		cmd_arger_desc_flag(&debug_memory, "debug_memory", "turns on address memory sanitizer"),
		cmd_arger_desc_string(&compiler, "compiler", "the compiler command"),
		cmd_arger_desc_integer(&opt, "opt", "compiler code optimization level, 0 none, 3 max"),
	};

	char* app_name_and_version = "example build script";
	cmd_arger_parse(desc, sizeof(desc) / sizeof(*desc), NULL, 0, argc, argv, app_name_and_version);

	int exe_res;
	if (clean) {
#ifdef _WIN32
		exe_res = system("del "example_out_file);
#else
		exe_res = system("rm "example_out_file);
#endif
		if (exe_res != 0) { return exe_res; }
		return exe_res;
	}

#define buf_count 1024

	char buf[buf_count];
	char cflags[512];
	size_t cflags_idx = 0;
	cflags_idx += snprintf(cflags + cflags_idx, sizeof(cflags) - cflags_idx, "-O%zd -mstackrealign", opt);
	if (debug) {
		cflags_idx += snprintf(cflags + cflags_idx, sizeof(cflags) - cflags_idx, " -g3 -DDEBUG");
	}
	if (debug_address) {
		cflags_idx += snprintf(cflags + cflags_idx, sizeof(cflags) - cflags_idx, " -fsanitize=address");
	}
	if (debug_memory) {
		cflags_idx += snprintf(cflags + cflags_idx, sizeof(cflags) - cflags_idx, " -fsanitize=memory");
	}
	cflags_idx += snprintf(cflags + cflags_idx, sizeof(cflags) - cflags_idx, "");
	char* include_paths = "-I./";

	// compile example
	snprintf(buf, buf_count, "%s %s -o %s %s %s", compiler, cflags, example_out_file, example_src_file, include_paths);
	exe_res = system(buf);
	if (exe_res != 0) { return exe_res; }

	return exe_res;
}

