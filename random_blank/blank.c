#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



void main(int argc, char **argv)
{
	FILE *fp;
	char tmp[1024];
	char tmp1[2048];
	int blank_num = 0;

	srand(time(NULL));

	if (argc != 2)
		return;

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("%s doesn't exist\n", argv[1]);
		return;
	}
#define SKIP 8

	printf("%s\n", argv[1]);
	while(fgets(tmp, 1024, fp) != NULL) {
		memset(tmp1, 0, 2048);
		//printf("%s\n", tmp);
		int i, j, k;
		i = rand() % SKIP;
		j = 1;

		char *ptr = strtok(tmp, " ");
		strcat(tmp1, ptr);
		strcat(tmp1, " ");

		while((ptr = strtok(NULL, " ")) != NULL) {

			if (strlen(ptr)<2)
				continue;
			if (ptr[0] < 0x41 || ptr[0] > 0x7a)
				continue;
			if (ptr[1] < 0x61 && ptr[1] > 0x5a)
				continue;


			if (j == i) {
				for (k = 0; k < strlen(ptr); k++) {
					if (k < 5)
						strcat(tmp1, "__");
					else
						strcat(tmp1, "_");
				}
				strcat(tmp1, " ");
				i = rand() % SKIP;
				j = 0;
				blank_num++;
			} else {
				strcat(tmp1, ptr);
				strcat(tmp1, " ");
			}
			j = (j+1) % SKIP;
		}
		printf("%s", tmp1);
	}
	printf("blank = %d\n", blank_num);

err0:
	fclose(fp);

}

