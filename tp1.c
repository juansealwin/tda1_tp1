#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "types.h"
#include "tp1.h"
#define MIN_ARGS 3
#define FIRST_ARG 1
#define SECOND_ARG 2
#define MIN_CONTACTS 4
#define END_LINE '\n'
#define DELIM ','

status_t validate_arguments(int argc, char *argv[], params_t *args_values)
{
	if (argv == NULL)
		return ERROR_NULL_POINTER;

	if (argc != MIN_ARGS)
	{
		return ERROR_INVALID_ARGS;
	}
	else
	{
		char *ptr;

		args_values->number = strtol(argv[FIRST_ARG], &ptr, 10);
		args_values->file_name = argv[SECOND_ARG];

		if (args_values->number)
			return OK;
		else
			return ERROR_INVALID_ARGS;
	}
}

void load_contacts(members_t *guests, char *contacts_str, size_t n_members)
{
	char *delim = ",";
	char *ptr;
	int contact_num;
	contact_t *prev_contact = NULL;

	guests->added_uninvited_list = FALSE;
	char *contact_str = strtok(contacts_str, delim);
	
	if (contact_str != NULL && contact_str[0] >= '0' && contact_str[0] <= '9')
	{
		guests->contacts = (contact_t *)malloc(sizeof(contact_t) * n_members);
		memset(guests->contacts, 0, n_members * sizeof(contact_t));
	}
	else
	{
		guests->contacts = NULL;
		guests->total_contacts = 0;
		return;
	}

	contact_t *contacts = guests->contacts;

	for (size_t i = 0; contact_str != NULL && i < n_members; i++)
	{
		contact_num = strtol(contact_str, &ptr, 10) - 1;

		// As long as the number of contacts is less than the total number of guests to be taken.
		if (contact_num < (int)n_members)
		{
			contacts[contact_num].contact_num = contact_num;

			contacts[contact_num].prev = prev_contact;

			// Check if it is not the first contact
			if (prev_contact != NULL)
			{
				prev_contact->next = &contacts[contact_num];

			// First contact
			}
			else
			{
				guests->head_contact = contact_num;
			}

			prev_contact = &contacts[contact_num];

			contacts[contact_num].next = NULL;

			guests->total_contacts++;
		}

		contact_str = strtok(NULL, delim);
	}
}

size_t len_split(char *line, char delim, size_t encounters)
{

	size_t i = 0;

	for (size_t counter = 0; line[i]; i++)
	{
		if (line[i] == delim)
			counter++;
		if (counter == encounters)
			return i;
	}

	return i;
}

void remove_contact(members_t *guest, int contact_num)
{
	contact_t *prev_contact = NULL, *next_contact = NULL;

	contact_t *contacts = guest->contacts;

	// If it is zero, there is no such contact, so there is nothing to delete.
	if (contacts[contact_num].contact_num != 0) {

		if (contacts[contact_num].prev != NULL)
		{
			prev_contact = contacts[contact_num].prev;
			prev_contact->next = contacts[contact_num].next;
		}
		else
		{
			next_contact = contacts[contact_num].next;

			// Check if there is a next contact
			if (next_contact)
			{
				next_contact->prev = NULL;
				guest->head_contact = next_contact->contact_num;
			}
		}
		
		guest->total_contacts--;
	}
}

int main(int argc, char *argv[])
{
	params_t args_values;

	if (validate_arguments(argc, argv, &args_values) != OK)
	{
		perror("Argumentos inválidos \n");
		_exit(1);
	}

	FILE *guest_file;

	if ((guest_file = fopen(args_values.file_name, "r+")) == NULL)
	{
		perror("Error al abrir el archivo \n");
		_exit(1);
	}

	char *read_buffer = NULL;
	size_t len_buff = 0;

	size_t n_members = args_values.number;
	size_t max_digits = 0;
	for (size_t i = n_members; i != 0; max_digits++)
		i /= 10;

	// Case if n > total number of members
	size_t total_members = 0;

	// Prepare the structure to load the members and the list of the members who will not be invited
	members_t *guests = (members_t *)malloc(sizeof(members_t) * n_members);
	size_t *uninvited_list = (size_t *)malloc(sizeof(size_t) * n_members);
	size_t uninvited_index = 0;

	// Read all the lines and store data
	for (size_t i = 0, len_name = 0;
		 getdelim(&read_buffer, &len_buff, END_LINE, guest_file) > 1 && i < n_members;
		 i++)
	{

		len_name = len_split(read_buffer, DELIM, 2);
	
		guests[i].name = (char *)malloc(sizeof(char) * len_name);
		strncpy(guests[i].name, read_buffer, len_name);
		guests[i].name[len_name] = '\0';

		load_contacts(&guests[i], &read_buffer[len_name], n_members);
		
		if (guests[i].total_contacts < MIN_CONTACTS) {
			uninvited_list[uninvited_index++] = i;
			guests[i].added_uninvited_list = TRUE;
		}

		total_members++;
	}

	fclose(guest_file);

	// Start to cross off possible guests
	for (size_t i = 0; i < uninvited_index; i++)
	{

		size_t idx_guest_to_erase = uninvited_list[i];

		members_t *guest_to_erase = &guests[idx_guest_to_erase];

		contact_t *contacts = guest_to_erase->contacts;

		contact_t *contact = &contacts[guest_to_erase->head_contact];
		
		while (contact != NULL)
		{
			size_t idx_contact = contact->contact_num;
	
			remove_contact(&guests[idx_contact], idx_guest_to_erase);

			// if member has less than the minimum number of guests we add it to the list 
			// and if it has not added to the list yet
			if (guests[idx_contact].total_contacts < MIN_CONTACTS && !guests[idx_contact].added_uninvited_list) {
				uninvited_list[uninvited_index++] = idx_contact;
				guests[idx_contact].added_uninvited_list = TRUE;
			}

			contact = contact->next;
		}
	}

	// Print the guests and free the internal data
	for (size_t i = 0; i < n_members && i < total_members; i++)
	{
		if (guests[i].total_contacts >= MIN_CONTACTS)
			printf("%s\n", guests[i].name);

		free(guests[i].name);
		free(guests[i].contacts);
	}

	// Free structures
	free(uninvited_list);
	free(guests);

	_exit(0);
}
