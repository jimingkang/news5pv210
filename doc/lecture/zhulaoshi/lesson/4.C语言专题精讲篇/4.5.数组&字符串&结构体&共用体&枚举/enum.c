#include "stdio.h"

/*
 ****************************************************************
 * 	enumeration ���Ͷ���
 ****************************************************************
 */ 
/*		// ���巽��1���������ͺͶ���������뿪
enum week
{
	SUN,		// SUN = 0
	MON,		// MON = 1;
	TUE,
	WEN,
	THU,
	FRI,
	SAT,
};

enum week today;
*/

/*		// ���巽��2,�������͵�ͬʱ�������
enum week
{
	SUN,		// SUN = 0
	MON,		// MON = 1;
	TUE,
	WEN,
	THU,
	FRI,
	SAT,
}today,yesterday;
*/

/*		// ���巽��3,�������͵�ͬʱ�������
enum 
{
	SUN,		// SUN = 0
	MON,		// MON = 1;
	TUE,
	WEN,
	THU,
	FRI,
	SAT,
}today,yesterday;
*/

/*		// ���巽��4,��typedef����ö�����ͱ��������ں���ʹ�ñ������б�������
typedef enum week
{
	SUN,		// SUN = 0
	MON,		// MON = 1;
	TUE,
	WEN,
	THU,
	FRI,
	SAT,
}week;
*/

/*		// ���巽��5,��typedef����ö�����ͱ��������ں���ʹ�ñ������б�������
typedef enum 
{
	SUN,		// SUN = 0
	MON,		// MON = 1;
	TUE,
	WEN,
	THU,
	FRI,
	SAT,
}week;
*/


/*
 ******************************************************************	
 *	�������;���
 */

/*	// ����1��ö����������������ʱ����error: conflicting types for ��DAY��
typedef enum workday
{
	MON,		// MON = 1;
	TUE,
	WEN,
	THU,
	FRI,
}DAY;

typedef enum weekend
{
	SAT,
	SUN,
}DAY;
*/

/*	// ����2��ö�ٳ�Ա����������ʱ����redeclaration of enumerator ��MON��
typedef enum workday
{
	MON,		// MON = 1;
	TUE,
	WEN,
	THU,
	FRI,
}workday;

typedef enum weekend
{
	MON,
	SAT,
	SUN,
}weekend;
// �ṹ����Ԫ�ؿ�������
typedef struct 
{
	int a;
	char b;
}st1;

typedef struct 
{
	int a;
	char b;
}st2;
*/

/*		// #define������ظ�����(û��error������warning)����������һ�ζ���Ϊ׼��
#define MACRO1	12
#define MACRO1	24
*/

/*
 *
 * 	���Դ���
 */
int main(int argc, char **argv)
{
	printf("%d\n", MACRO1);


/*   		// ���Զ��巽��4,5
	week today;
	today = WEN;
	printf("today is the %dth day in week\n", today);
*/

/*		// ���Զ��巽��2
	today = WEN;
	printf("today is the %dth day in week\n", today);
*/


/*		// ����enum����������
	enum week w1;
	w1 = TUE;
	printf("%d\n", w1);
*/	
} 



/*
 *���Խ��ۼ�¼
 *******************************************************************
 1���������´���
	enum week w1;		
	w1 = TUE;		
	printf("%s\n", w1);		
���־��棬enum.c:28: warning: format ��%s�� expects type ��char *��, but argument 2 has type ��unsigned int�� 
���������͵ĸ�ʽ����������Ͳ�ƥ�䣬�Ӿ�����Ϣ���Կ���enum����������Ϊunsigned int
	��Ȼ��unsigned int������Ȼʹ��%d��ӡ����ȷ���ˡ�

2��������������ö�����͡�����һ���ļ��в������������������ϵ�enum��typedef����ͬ�ı�����
��������ܺ���⣬��Ϊ�����ֲ�ͬ����������Ϊ��ͬ�ı����������gcc�ڻ�ԭ����ʱ�������󡣱����㶨����
typedef int INT;	typedef char INT; ��ôINT���ױ���Ϊint����char�أ�
3��������������ö�ٳ�Ա��
�������������ԣ�����struct�����ڵĳ�Ա���ƿ���������������enum�����еĳ�Ա������������ʵ���ϴ����ߵĳ�Ա
�ڷ��ʷ�ʽ�ϵĲ�ͬ�Ϳ��Կ����ˡ�struct���ͳ�Ա�ķ��ʷ�ʽ�ǣ�������.��Ա����enum��Ա�ķ��ʷ�ʽΪ����Ա����
���������enum�������������ĳ�Ա���Ǵ����з��������Աʱ����ָ�����ĸ�enum�еĳ�Ա�أ�
����#define�궨���ǿ��������ģ��ú���������ֵȡ�������һ�ζ����ֵ����������������浫����error

 ********************************************************************
 */
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 