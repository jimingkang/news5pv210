#include "stdio.h"

/*
 ****************************************************************
 * 	enumeration 类型定义
 ****************************************************************
 */ 
/*		// 定义方法1，定义类型和定义变量分离开
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

/*		// 定义方法2,定义类型的同时定义变量
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

/*		// 定义方法3,定义类型的同时定义变量
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

/*		// 定义方法4,用typedef定义枚举类型别名，并在后面使用别名进行变量定义
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

/*		// 定义方法5,用typedef定义枚举类型别名，并在后面使用别名进行变量定义
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
 *	错误类型举例
 */

/*	// 错误1，枚举类型重名，编译时报错：error: conflicting types for ‘DAY’
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

/*	// 错误2，枚举成员重名，编译时报错：redeclaration of enumerator ‘MON’
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
// 结构体中元素可以重名
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

/*		// #define宏可以重复定义(没有error但是有warning)，结果以最后一次定义为准。
#define MACRO1	12
#define MACRO1	24
*/

/*
 *
 * 	测试代码
 */
int main(int argc, char **argv)
{
	printf("%d\n", MACRO1);


/*   		// 测试定义方法4,5
	week today;
	today = WEN;
	printf("today is the %dth day in week\n", today);
*/

/*		// 测试定义方法2
	today = WEN;
	printf("today is the %dth day in week\n", today);
*/


/*		// 测试enum变量的类型
	enum week w1;
	w1 = TUE;
	printf("%d\n", w1);
*/	
} 



/*
 *测试结论记录
 *******************************************************************
 1、编译以下代码
	enum week w1;		
	w1 = TUE;		
	printf("%s\n", w1);		
出现警告，enum.c:28: warning: format ‘%s’ expects type ‘char *’, but argument 2 has type ‘unsigned int’ 
分析：典型的格式化输出和类型不匹配，从警告信息可以看出enum变量的类型为unsigned int
	既然是unsigned int，那自然使用%d打印是正确的了。

2、不能有重名的枚举类型。即在一个文件中不能有两个或两个以上的enum被typedef成相同的别名。
分析：这很好理解，因为将两种不同类型重命名为相同的别名，这会让gcc在还原别名时遇到困惑。比如你定义了
typedef int INT;	typedef char INT; 那么INT到底被译为int还是char呢？
3、不能有重名的枚举成员。
分析：经过测试，两个struct类型内的成员名称可以重名，而两个enum类型中的成员不可以重名。实际上从两者的成员
在访问方式上的不同就可以看出了。struct类型成员的访问方式是：变量名.成员，而enum成员的访问方式为：成员名。
因此若两个enum类型中有重名的成员，那代码中访问这个成员时到底指的是哪个enum中的成员呢？
两个#define宏定义是可以重名的，该宏名真正的值取决于最后一次定义的值。编译器会给出警告但不会error

 ********************************************************************
 */
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 