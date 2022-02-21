void cmd_list()
{
	printf("0 - show command list\n");
	printf("1 - insert Manager\n");
	printf("2 - insert Client\n");
	printf("3 - get Manager\n");
	printf("4 - get Client\n");
	printf("5 - get all clients of Manager\n");
	printf("6 - delete Client\n");
	printf("7 - delete Manager(with all his Clients)\n");
	printf("8 - display Manager Index struct\n");
	printf("9 - display Manager File\n");
	printf("10 - display Client File\n");
	printf("11 - update Manager\n");
	printf("12 - update Client\n");
	printf("13 - display garbage position of Client file\n");
	printf("14 - quit\n");
}
void user_interface()
{
	int input = 0;
	int i1 = 0, i2 = 0;
	double d1 = 0;
	char s1[20], s2[20];
	while (true)
	{	
		switch (input)
		{
		case 0:
			cmd_list();
			break;
		case 1:
			printf("Manager id: ");
			scanf("%d", &i1);
			printf("Manager name: ");
			scanf("%s", s1);
			insert_m(i1, s1);
			break;
		case 2:
			printf("Client id: ");
			scanf("%d", &i1);
			printf("Client name: ");
			scanf("%s", s1);
			printf("Client address: ");
			scanf("%s", s2);
			printf("Manager's id: ");
			scanf("%d", &i2);
			insert_s(i1, s1, s2, i2);
			break;
		case 3:
			printf("Manager id: ");
			scanf("%d", &i1);
			display_m(i1);
			break;
		case 4:
			printf("Client id: ");
			scanf("%d", &i1);
			display_s(i1);
			break;
		case 5:
			printf("Manager id: ");
			scanf("%d", &i1);
			get_all_s_of_m(i1);
			break;
		case 6:
			printf("Client id: ");
			scanf("%d", &i1);
			delete_s(i1);
			break;
		case 7:
			printf("Manager id: ");
			scanf("%d", &i1);
			delete_m(i1);
			break;
		case 8:
			display_m_index_struct();
			break;
		case 9:
			display_m_file();
			break;
		case 10:
			display_s_file();
			break;
		case 11:
			printf("Manager id: ");
			scanf("%d", &i1);
			printf("New Manager name: ");
			scanf("%s", s1);
			update_m(i1, s1);
			break;
		case 12:
			printf("Client id: ");
			scanf("%d", &i1);
			printf("New Client name: ");
			scanf("%s", s1);
			printf("New Client address: ");
			scanf("%s", s2);
			update_s(i1, s1, s2);
			break;
		case 13:
			display_free_s_space();
			break;
		case 14:
			return;
		}
		printf("Enter next command: \n");
		scanf("%d", &input);
	}
	
}