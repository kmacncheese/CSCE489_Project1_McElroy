/*************************************************************************************
 * shellfuncts.h - header file for your shell functions
 *
 *************************************************************************************/

// Replace these with something useful
int hello(int param1);
void send_msg(const char *param2);
char** parse_command(const char* command);
void exe_command(const char** command);
void exe_create(const char** command);
void exe_update(const char** command);
void exe_list(const char** command);
void exe_dir();
int get_command_params(const char* first_word);
void exe_with_fork(const char** command);
