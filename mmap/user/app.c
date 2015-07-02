#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <sys/time.h>


int main(int argc, char **argv)
{
#define SIZE 1024*4096

	int fd;
	int i = 100;
	void *vaddr = NULL;
	char test_buf[SIZE];

	struct timeval prv_tv;
	struct timeval cur_tv;
	struct timezone tz;
	float loop_timeuse;

	if (test_buf == NULL)
		return 0;

	fd = open("/dev/test", O_RDWR);
	if (fd == -1) {
		printf("open /dev/test failure (%s)\n", strerror(errno));
		return 0;
	}
	vaddr = mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (vaddr == MAP_FAILED) {
		printf("mmap fails (%s)\n", strerror(errno));
		return 0;
	}
	close(fd);

	gettimeofday(&prv_tv, NULL);
	memcpy(test_buf, vaddr, SIZE);
	gettimeofday(&cur_tv, NULL);
	loop_timeuse = 1000.0 * (cur_tv.tv_sec - prv_tv.tv_sec) + (cur_tv.tv_usec - prv_tv.tv_usec) / 1000.0;

	for (i=10;i<20;i++) 
		printf("%c ", test_buf[i]);
	printf("\n");

	printf("### [%s:%d] %f\n", __FUNCTION__, __LINE__, loop_timeuse);

	munmap(vaddr, SIZE);
	return 0;
}
