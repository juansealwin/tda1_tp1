#include "types.h"

typedef enum { FALSE=0, TRUE=1 } bool_t;

typedef struct params {
	char *file_name;
	size_t number;
}params_t;


typedef struct contact {
	size_t contact_num;
	struct contact *prev;
	struct contact *next;
}contact_t;

typedef struct members {
	size_t total_contacts;
	size_t head_contact;
	bool_t added_uninvited_list;
	char *name;
	contact_t *contacts; 
}members_t;

status_t validate_arguments(int , char*[], params_t*);

size_t len_split(char* ,char , size_t);

void count_contacts(members_t*, char*);

void remove_contact(members_t*, int);

void load_contacts(members_t *, char*, size_t);