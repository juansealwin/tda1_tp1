#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "tp1.h"
#define MIN_ARGS 3
#define FIRST_ARG 1
#define SECOND_ARG 2
#define MIN_CONTACTS 4
#define END_LINE '\n'
#define DELIM ','

status_t validate_arguments(int argc , char* argv[], params_t* args_values){

    if (argv == NULL)
        return ERROR_NULL_POINTER;

    if (argc != MIN_ARGS) {
		return ERROR_INVALID_ARGS;

	} else {
		char *ptr;

		args_values->number = strtol(argv[FIRST_ARG], &ptr, 10);
		args_values->file_name = argv[SECOND_ARG];

		if (args_values->number)
			return OK;
		else
			return ERROR_INVALID_ARGS;
	}	   
}



size_t count_contacts(char* contacts, char delim){
	size_t total_contacts = 1;

	for(size_t i = 1; contacts[i] ; i++){
		if (contacts[i] == delim) total_contacts++;
	}
	return total_contacts;
}

size_t len_split(char* line, char delim, size_t encounters){
	
	size_t i = 0;

	for(size_t counter = 0; line[i] ; i++){
		if (line[i] == delim) counter++;
		if (counter == encounters) return i;
	}

	return i;
}

void remove_contact(members_t* member, char* contact, size_t index_member, size_t* not_invited_list, size_t* not_invited_index) {

	size_t len_contact = strlen(contact);
	size_t len_contacts = strlen(member->contacts);
	size_t delete = 0;
	size_t i, j;
	//printf("Del miembro %zu remuevo %s y con lista de contactos %s", index_member+1, contact, member->contacts);
	//printf("Largo del contacto a eliminar %zu\n", len_contact);
    for (i = 0; member->contacts[i] ; i++){

		if (member->contacts[i] == contact[0]) {
			
			delete = 1;
			
			for (j = 1 ; j < len_contact ; j++) {
				//printf("Te elimino... %zu\n", j);
				if (member->contacts[i + j] != contact[j]) {
					//printf("Safaste\n");
					delete = 0;
				}
			}
			if (delete) {
				//printf("Te voy a eliminar\n");
				// +1 to grab what follows the comma
				memmove(&member->contacts[i], &member->contacts[i + j] + 1, len_contacts - i );
				member->total_contacts--;
				
				if (member->total_contacts < MIN_CONTACTS) 
					not_invited_list[(*not_invited_index)++] = index_member;
				//printf("Eliminado\n");
				break;
			}
		}
    }
}


int main(int argc, char* argv[]) {
	params_t args_values;

	if (validate_arguments(argc, argv, &args_values) != OK) {
        perror("Argumentos inválidos \n");
        _exit(1);
    }

	FILE *guest_file;

	if((guest_file = fopen(args_values.file_name, "r+")) == NULL){
        perror("Error al abrir el archivo \n");
        _exit(1);
    }

	char *read_buffer = NULL;

	char* delim = ",";
	size_t len_buff = 0;

	size_t len_name = 0;
	size_t len_contacts = 0;

	size_t total_digits = 0; 
	for (size_t i = 0; i != 0;) {
		i /= 10;
		total_digits++;
	}
	
	size_t n_members = args_values.number;
	size_t total_members = 0;
	members_t* guests = (members_t*) malloc(sizeof(members_t*) *  n_members);
	size_t* not_invited_list = (size_t*) malloc(sizeof(size_t*) * n_members);
	size_t not_invited_index = 0;
	char* str_member_idx = (char*) malloc(sizeof(char*) * total_digits);

	// Read all the lines and store data
	for (size_t i = 0 ; getdelim(&read_buffer, &len_buff, END_LINE, guest_file) > 1 && i < n_members; i++) {
	
		len_name = len_split(read_buffer, DELIM, 2);
		len_contacts = strlen(read_buffer) - len_name;
			
		guests[i].name = (char*) malloc(sizeof(char*) * len_name);
		strncpy(guests[i].name, read_buffer, len_name);

		if (len_contacts > MIN_CONTACTS) {
			guests[i].contacts = (char*) malloc(sizeof(char*) * (len_contacts));
			strncpy(guests[i].contacts, &read_buffer[len_name + 1], len_contacts);

		} else {
			guests[i].contacts = "";
		}

		guests[i].total_contacts = count_contacts(guests[i].contacts, DELIM);

		if (guests[i].total_contacts < MIN_CONTACTS) {
			not_invited_list[not_invited_index++] = i;
		}
		total_members++;
	}

	for (size_t i = 0 ; i < not_invited_index ; i++) {
		
		size_t member_idx = not_invited_list[i];
		char* temp;
		
		// +1 para que se adecue desde la estructura de datos que comienza en 0
		snprintf(str_member_idx ,sizeof(member_idx) ,"%zu" ,(member_idx + 1));
		
		char* contact = strtok(guests[member_idx].contacts, delim);
		
		while (contact != NULL) {
			
			// -1 para que se adecue a la estructura de datos que comienza en 0
			size_t index_contact = strtol(contact, &temp, 10) - 1;
			
			remove_contact(&guests[index_contact], str_member_idx, index_contact, not_invited_list, &not_invited_index);

			contact = strtok(NULL, delim);
		}
	}

	

	for (size_t i = 0 ; i < n_members && i < total_members ; i++) {
		if (guests[i].total_contacts >= MIN_CONTACTS) {
			printf("%s\n", guests[i].name);
			free(guests[i].name);
			free(guests[i].contacts);
		}
	}
		
	free(str_member_idx);
	free(not_invited_list);
	free(guests);

	_exit(0);
}