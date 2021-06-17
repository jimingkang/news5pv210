#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>


#define DEVICE_KEY			"/dev/input/event1"
#define DEVICE_MOUSE		"/dev/input/event3"

#define X210_KEY			"/dev/input/event1"


int main(void)
{
	int fd = -1, ret = -1;
	struct input_event ev;
	
	// ��1�������豸�ļ�
	fd = open(X210_KEY, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	
	while (1)
	{
		// ��2������ȡһ��event�¼���
		memset(&ev, 0, sizeof(struct input_event));
		ret = read(fd, &ev, sizeof(struct input_event));
		if (ret != sizeof(struct input_event))
		{
			perror("read");
			close(fd);
			return -1;
		}
		
		// ��3��������event������֪��������ʲô���������¼�
		printf("-------------------------\n");
		printf("type: %hd\n", ev.type);
		printf("code: %hd\n", ev.code);
		printf("value: %d\n", ev.value);
		printf("\n");
	}
	
	
	// ��4�����ر��豸
	close(fd);
	
	return 0;
}







