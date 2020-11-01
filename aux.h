#ifndef AUX_H
#define AUX_H

#define ARGS_SIZE 5

int process_input(char * line);
char * read_user_input_line(void);
char ** split_input_into_arguments(char * line);
int validate_input(char * input);
void image_subs_exec(char ** args);
void generate_signal_file(void);
int check_comment(char * line);
void read_commands_file(char * filename);
int check_if_file_exists(char * filename);

#endif