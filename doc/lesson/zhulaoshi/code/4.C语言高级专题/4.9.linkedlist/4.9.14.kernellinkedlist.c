#include <linux/list.h>



struct driver_info
{
	int data;
};

// driver�ṹ�����������ں��е�����
struct driver
{
	char name[20];				// ��������
	int id;						// ����id���
	struct driver_info info;	// ������Ϣ
	struct list_head head;		// ��Ƕ���ں������Ա
};

struct driver2
{
	char name[20];				// ��������
	int id;						// ����id���
	struct driver_info info;	// ������Ϣ
	//struct list_head head;		// ��Ƕ���ں������Ա
	struct driver *prev;
	struct driver *next;
};

// ����driver�ṹ�壬��֪��ǰ������Ա�������������Ա����������֮ǰ��Ϊint data�Ķ���������4����Ա��һ��struct list_head���͵ı����������һ��������
// ����driver�ṹ����û������ģ�Ҳ�޷���������������������driver��Ƕ��head��Ա�������һ������������driverͨ��head��Ա���Լ���չ������Ĺ��ܡ�
// driverͨ����Ƕ�ķ�ʽ��չ�����Ա������ֻ������һ�������Ա���ؼ��ǿ���ͨ������list_head��������ʵ�ֵ�����ĸ��ֲ�������������head��

// ����Ч�������ǿ���ͨ������head��ʵ��driver�ı���������head�ĺ�����list.h���Ѿ�����д���ˣ����������ں���ȥ����driverʱ�Ͳ����ظ�ȥд�ˡ�
// ͨ������head������driver��ʵ���Ͼ���ͨ�������ṹ���ĳ����Ա���������������ṹ�������������Ҫ����container_of��































