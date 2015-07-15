#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void main(int argc, char **argv)
{
	FILE *fp;
	char tmp[1024];
	char tmp1[1024];
	int blank_num = 0;
	int SKIP = 10;
	int total = 0;

	srand(time(NULL));

	if (argc < 2) {
		printf("usage: a.out input_file [SKIP]\n");
		return;
	}
	if (argc == 3)
		SKIP = atoi(argv[2]);

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("%s doesn't exist\n", argv[1]);
		return;
	}

	printf("%s\n\n", argv[1]);

	while(fgets(tmp, 1024, fp) != NULL) {
		memset(tmp1, 0, 1024);
		memcpy(tmp1, tmp, 1024);
		int i, j, k;
		i = rand() % SKIP;
		j = 1;

		char *ptr = strtok(tmp, " ");

		while((ptr = strtok(NULL, " ")) != NULL) {

			if ((strlen(ptr)<5) || 
				(ptr[0] < 0x41 || ptr[0] > 0x7a) ||
				(ptr[1] < 0x61 && ptr[1] > 0x5a)) {
				continue;
			}

			total++;

			if (j == i) {
				for (k = 0; k < strlen(ptr); k++) {
					tmp1[ptr-tmp+k] = '_';
				}
				for (k = 0; k < 10; k++) {
					i = rand() % SKIP;
					if (i > 2)
						break;
				}
				j = 0;
				blank_num++;
			}
			j = (j+1) % SKIP;
		}
		printf("%s", tmp1);
	}
	printf("\nblank/total = %d/%d\n", blank_num, total);
err0:
	fclose(fp);

}

