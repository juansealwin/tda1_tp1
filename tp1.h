#include "types.h"

typedef enum { FALSE=0, TRUE=1 } bool_t;

typedef struct params {
	char *file_name;
	size_t number;
}params_t;

typedef struct members {
	size_t total_contacts;
	char *name;
	char *contacts ;
}members_t;

status_t validate_arguments(int , char*[], params_t*);

size_t len_split(char* ,char , size_t);

size_t count_contacts(char*, char);

void remove_contact(members_t*, char*, size_t, size_t*, size_t*);

