#pragma once

typedef struct manager
{
	unsigned id;
	char name[20];
	int client_next;
	unsigned client_count;
}Manager;
typedef struct client
{
	unsigned id;
	unsigned manager_id;
	char name[20];
	char address[30];
	int client_next;
	bool exists;
} Client;
typedef struct manager_index
{
	unsigned key; // manager id
	unsigned position; // record position
} Manager_Index;
struct meta
{
	unsigned manager_count;
	unsigned client_count;
	unsigned total_clients;
	unsigned free_client_pos[20];
	unsigned fcp_size;
};
struct meta MetaData;
void insert_m(unsigned id, char* name);
void insert_s(unsigned id, const char* name, const char* address, unsigned manager_id);
void insert_m_index(unsigned key, unsigned position);

int get_m_pos_and_index_by_id(int id, int* mid);


int get_m_pos_by_id(int id);


void load_data_base();

void close_data_base();

void get_all_s_of_m(int id);

void delete_s(int id);

void delete_m(int id);

Manager get_m(int id);

Client get_s_and_pos(int id, int* i);

void display_m_index_file();

void display_m_index_struct();

void display_s_file();

void display_m_file();

void display_free_s_space();

void display_m(unsigned id);

void display_s(unsigned id);

void update_m(int id, char* newName);

void update_s(int id, char* newName, char* newAddress);

void delete_all_s_of_m(unsigned id);
