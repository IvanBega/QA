#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "DataBase.h"
#include "DBInterface.c"
//m is manager

const char* MANAGER_FILE = "manager.fl";
const char* MANAGER_INDEX_FILE = "manager_index.ind";
const char* CLIENT_FILE = "client.fl";
const char* METADATA_FILE = "metadata.dat";
Manager_Index MANAGER_INDEX[20];
void insert_m(unsigned id, char* name)
{
	Manager m;
	m.id = id;
	strcpy(m.name, name);
	m.client_count = 0;
	m.client_next = -1;
	insert_m_index(m.id, MetaData.manager_count);

	FILE* f = fopen(MANAGER_FILE, "rb+");
	fseek(f, (long)(MetaData.manager_count * sizeof(Manager)), SEEK_SET);
	fwrite(&m, sizeof(Manager), 1, f);
	fclose(f);
	MetaData.manager_count++;
}
void insert_s(unsigned id, const char* name, const char* address, unsigned manager_id)
{
	Client c;
	strcpy(c.name, name);
	strcpy(c.address, address);
	c.manager_id = manager_id;
	c.id = id;
	c.exists = true;
	int free_pos = -1;
	int position = get_m_pos_by_id(manager_id);
	if (position >= 0)
	{
		Manager m;

		FILE* fm = fopen(MANAGER_FILE, "rb+");
		fseek(fm, (long)(position * sizeof(Manager)), SEEK_SET);
		fread(&m, sizeof(Manager), 1, fm);
		c.client_next = m.client_next;
		m.client_count++;
		if (MetaData.fcp_size > 0)
		{
			free_pos = MetaData.free_client_pos[MetaData.fcp_size - 1];
			m.client_next = free_pos;
		}
		else
		{
			m.client_next = MetaData.client_count;
		}

		fseek(fm, (long)(position * sizeof(Manager)), SEEK_SET);
		fwrite(&m, sizeof(Manager), 1, fm);
		fclose(fm);

	}

	FILE* f = fopen(CLIENT_FILE, "rb+");
	if (MetaData.fcp_size)
	{
		fseek(f, (long)(free_pos * sizeof(Client)), SEEK_SET);
		MetaData.fcp_size--;
	}
	else
	{
		fseek(f, (long)(MetaData.client_count * sizeof(Client)), SEEK_SET);
		MetaData.total_clients++;
	}
	fwrite(&c, sizeof(c), 1, f);
	MetaData.client_count++;
	fclose(f);
}
void insert_m_index(unsigned key, unsigned position)
{
	int i = 0;
	while (i < MetaData.manager_count)
	{
		if (key > MANAGER_INDEX[i].key)
			i++;
		else
			break;
	}
	for (int j = MetaData.manager_count; j > i; j--)
	{
		MANAGER_INDEX[j].position = MANAGER_INDEX[j - 1].position;
		MANAGER_INDEX[j].key = MANAGER_INDEX[j - 1].key;
	}
	MANAGER_INDEX[i].key = key;
	MANAGER_INDEX[i].position = position;
}
int get_m_pos_and_index_by_id(int id, int* mid)
{
	int low = 0;
	int high = MetaData.manager_count - 1;
	*mid = 0;
	while (low <= high)
	{
		*mid = (high + low) / 2;
		if (id > MANAGER_INDEX[*mid].key)
		{
			low = *mid + 1;
		}
		else
		{
			if (id < MANAGER_INDEX[*mid].key)
			{
				high = *mid;
			}
			else
			{
				if (id == MANAGER_INDEX[*mid].key)
					return MANAGER_INDEX[*mid].position;
			}
		}
	}
	return -1;
}
int get_m_pos_by_id(int id)
{
	int mid;
	return get_m_pos_and_index_by_id(id, &mid);
}
void load_data_base()
{
	FILE* f;
	if (f = fopen(METADATA_FILE, "rb"))
	{
		fread(&MetaData, sizeof(MetaData), 1, f);
		fclose(f);
	}
	else
	{
		MetaData.manager_count = 0;
		MetaData.client_count = 0;
		MetaData.fcp_size = 0;
		MetaData.total_clients = 0;
	}

	if (f = fopen(MANAGER_INDEX_FILE, "rb"))
	{
		printf("Opened MANAGER_INDEX file!\n");

		for (int i = 0; i < MetaData.manager_count; i++)
		{
			fread(&MANAGER_INDEX[i], sizeof(Manager_Index), 1, f);
		}
		fclose(f);
	}
	if (!(f = fopen(MANAGER_FILE, "rb")))
	{
		f = fopen(MANAGER_FILE, "wb");
		fclose(f);
	}


	if (!(f = fopen(CLIENT_FILE, "rb")))
	{
		f = fopen(CLIENT_FILE, "wb");
		fclose(f);
	}
}
void close_data_base()
{
	FILE* f;
	f = fopen(METADATA_FILE, "wb");
	fwrite(&MetaData, sizeof(MetaData), 1, f);
	fclose(f);

	f = fopen(MANAGER_INDEX_FILE, "wb");
	for (int i = 0; i < MetaData.manager_count; i++)
	{
		fwrite(&MANAGER_INDEX[i], sizeof(Manager_Index), 1, f);
	
	}
	fclose(f);
}
void get_all_s_of_m(int id)
{
	int position = get_m_pos_by_id(id);
	if (position >= 0)
	{
		Manager m;
		Client c;
		int next = 0;
		FILE* f = fopen(MANAGER_FILE, "rb");
		fseek(f, (long)(position * sizeof(Manager)), SEEK_SET);
		fread(&m, sizeof(Manager), 1, f);
		next = m.client_next;
		fclose(f);
		printf("**** DISPLAYING ALL CLIENTS OF MANAGER %d\n\n", id);
		FILE* fc = fopen(CLIENT_FILE, "rb");
		while (next >= 0)
		{
			fseek(fc, (long)(next * sizeof(Client)), SEEK_SET);
			fread(&c, sizeof(Client), 1, fc);
			if (c.exists)
			{
				printf("Client position %d id %d, name %s, next client pos %d\n",next, c.id, c.name, c.client_next);
			}
			next = c.client_next;
		}
		fclose(fc);
	}
	printf("\n");
}
void delete_s(int id)
{
	int c_pos;
	FILE* f;
	Client c_todelete = get_s_and_pos(id, &c_pos);
	int m_pos = get_m_pos_by_id(c_todelete.manager_id);
	Manager m = get_m(c_todelete.manager_id);
	MetaData.free_client_pos[MetaData.fcp_size++] = c_pos;

	if (m.client_next == c_pos)
	{
		m.client_next = c_todelete.client_next;
	}
	else
	{
		f = fopen(CLIENT_FILE, "rb+");
		int search_pos = m.client_next;
		Client c;
		bool flag = true;
		while (flag)
		{
			fseek(f, (long)(search_pos * sizeof(Client)), SEEK_SET);
			fread(&c, sizeof(Client), 1, f);
			if (c.client_next == c_pos)
			{
				c.client_next = c_todelete.client_next;
				flag = false;
			}
			else
				search_pos = c.client_next;
		}
		fseek(f, (long)(search_pos * sizeof(Client)), SEEK_SET);
		fwrite(&c, sizeof(Client), 1, f);
		c_todelete.exists = false;
		fseek(f, (long)(c_pos * sizeof(Client)), SEEK_SET);
		fwrite(&c_todelete, sizeof(Client), 1, f);
		fclose(f);
	}
	m.client_count--;
	f = fopen(MANAGER_FILE, "wb+");
	fseek(f, (long)m_pos, SEEK_SET);
	fwrite(&m, sizeof(Manager), 1, f);
	fclose(f);
	MetaData.client_count--;
}
void delete_m(int id)
{
	delete_all_s_of_m(id);
	int index;
	int position = get_m_pos_and_index_by_id(id, &index);
	printf("Index of deleting manager %d is %d\n", id, index);
	//MANAGER_INDEX[index].exists = false;
	if (MetaData.manager_count == 1 || MetaData.manager_count - 1 == position)
	{
		for (int i = index; i < MetaData.manager_count - 1; i++)
		{
			MANAGER_INDEX[i].position = MANAGER_INDEX[i + 1].position;
			MANAGER_INDEX[i].key = MANAGER_INDEX[i + 1].key;
		}
		MetaData.manager_count--;
		return;
	}

	Manager m;
	FILE* f = fopen(MANAGER_FILE, "rb+");
	fseek(f, (long)((MetaData.manager_count - 1) * sizeof(Manager)), SEEK_SET);
	fread(&m, sizeof(Manager), 1, f);
	fseek(f, (long)(position * sizeof(Manager)), SEEK_SET);
	fwrite(&m, sizeof(Manager), 1, f);
	fclose(f);

	int edited_index;
	get_m_pos_and_index_by_id(m.id, &edited_index);
	MANAGER_INDEX[edited_index].position = position;
	for (int i = index; i < MetaData.manager_count - 1; i++)
	{
		MANAGER_INDEX[i].position = MANAGER_INDEX[i + 1].position;
		MANAGER_INDEX[i].key = MANAGER_INDEX[i + 1].key;
	}
	MetaData.manager_count--;

}
Manager get_m(int id)
{
	int position = get_m_pos_by_id(id);
	if (position >= 0)
	{
		Manager m;
		FILE* f = fopen(MANAGER_FILE, "rb");
		fseek(f, (long)(position * sizeof(Manager)), SEEK_SET);
		fread(&m, sizeof(Manager), 1, f);
		fclose(f);
		//if (m.exists)
		return m;
		printf("Manager id %d, name %s, total clients %d, next client id %d\n", id, m.name, m.client_count, m.client_next);
		//printf("Manager was deleted!");	
	}
	else
	{
		printf("Position of manager %d is %d", id, position);
	}
}
Client get_s(int id)
{
	int i = 0;
	return get_s_and_pos(id, &i);
}
Client get_s_and_pos(int id, int* i)
{
	FILE* f = fopen(CLIENT_FILE, "rb");
	Client c;
	for (*i = 0; *i < MetaData.total_clients; (*i)++)
	{
		fread(&c, sizeof(c), 1, f);
		if (c.exists && (c.id == id))
		{
			fclose(f);
			return c;
		}
	}
	fclose(f);
	printf("ERROR! Client with id %d was not found", id);
}
void display_m_index_file()
{
	FILE* f;
	if (!(f = fopen(MANAGER_INDEX_FILE, "rb")))
	{
		printf("No managers found yet!\n");
		return;
	}
	Manager_Index temp;
	printf("**** DISPLAYING MANAGER INDEX FILE ****\n\n");
	for (int i = 0; i < MetaData.manager_count; i++)
	{
		fread(&temp, sizeof(Manager_Index), 1, f);
		printf("key = %d, position = %d\n", temp.key, temp.position);
	}
	fclose(f);
	printf("-----------------\n");
	printf("Total managers %d\n\n", MetaData.manager_count);
}
void display_m_index_struct()
{
	printf("**** DISPLAYING MANAGER INDEXSTRUCT ****\n\n");
	for (int i = 0; i < MetaData.manager_count; i++)
	{
		printf("Manager id %d, position %d\n", MANAGER_INDEX[i].key, MANAGER_INDEX[i].position);
	}
	printf("\n");
}
void display_s_file()
{
	FILE* f = fopen(CLIENT_FILE, "rb");
	Client c;
	printf("**** DISPLAYING CLIENT FILE\n\n");
	for (int i = 0; i < MetaData.total_clients; i++)
	{
		fread(&c, sizeof(c), 1, f);
		printf("Position %d, Client id %d, name %s, address %s, next %d, Manager %d, exists %d\n", i, c.id, c.name, c.address, c.client_next, c.manager_id, c.exists);
	}
	printf("---------------------\n");
	printf("Existing clients %d, total clients %d\n\n", MetaData.client_count, MetaData.total_clients);
	fclose(f);
}
void display_m_file()
{
	FILE* f = fopen(MANAGER_FILE, "rb");
	Manager m;
	printf("**** DISPLAYING MANAGER FILE **** \n\n");
	for (int i = 0; i < MetaData.manager_count; i++)
	{
		fread(&m, sizeof(Manager), 1, f);
		printf("Position %d, Manager id %d, name %s, total clients %d, next %d\n", i, m.id, m.name, m.client_count, m.client_next);
	}
	fclose(f);
	printf("\n");
}
void display_free_s_space()
{
	printf("**** DISPLAYING GARBAGE POSITIONS OF CLIENT FILE ****\n\n");
	for (int i = 0; i < MetaData.fcp_size; i++)
	{
		printf("%d, ", MetaData.free_client_pos[i]);
	}
	printf("- total %d garbage positions\n\n",MetaData.fcp_size);
}
void display_m(unsigned id)
{
	Manager m = get_m(id);
	printf("Manager id %d, name %s, clients %d, next client %d\n", m.id, m.name, m.client_count, m.client_next);
}
void delete_all_s_of_m(unsigned id)
{
	Manager m = get_m(id);
	Client c;
	int current_pos = m.client_next;
	FILE* f = fopen(CLIENT_FILE, "rb+");
	while (current_pos >= 0)
	{
		fseek(f, (long)(current_pos * sizeof(Client)), SEEK_SET);
		fread(&c, sizeof(Client), 1, f);
		c.exists = false;
		fseek(f, (long)(current_pos * sizeof(Client)), SEEK_SET);
		fwrite(&c, sizeof(Client), 1, f);

		MetaData.free_client_pos[MetaData.fcp_size++] = current_pos;
		MetaData.client_count--;
		current_pos = c.client_next;
	}
	fclose(f);
}
void display_s(unsigned id)
{
	Client c = get_s(id);
	if (c.exists)
		printf("Client id %d, name %s, address %s, manager id %d", c.id, c.name, c.address, c.manager_id);
	else
		printf("Client %d does not exist!");
}
void update_m(int id, char* newName)
{
	int pos = get_m_pos_by_id(id);
	FILE* f = fopen(MANAGER_FILE, "rb+");
	Manager m;
	m.id = id;
	strcpy(m.name, newName);
	fseek(f, (long)(pos * sizeof(Manager)), SEEK_SET);
	fwrite(&m, sizeof(Manager), 1, f);
	fclose(f);
	
}
void update_s(int id, char* newName, char* newAddress)
{
	int pos;
	Client c = get_s_and_pos(id, &pos);
	
	FILE* f = fopen(CLIENT_FILE, "rb+");

	strcpy(c.name, newName);
	strcpy(c.address, newAddress);

	
	fseek(f, (long)(pos * sizeof(Client)), SEEK_SET);
	fwrite(&c, sizeof(Client), 1, f);
	fclose(f);
}
int main()
{
	load_data_base();
	user_interface();
	close_data_base();
	printf("Total workers %d\n", MetaData.manager_count);
	system("pause");
	return 0;
}