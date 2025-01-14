#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include "../include/cmd_c_cfgtools.h"

union cfg_value {
	int    ivalue;
	double dvalue;
	char   svalue[128];
	char   cvalue;
	void* value;
};

struct cfg_item {
	const char* cfg_item_name;
	enum ITEM_TYPE  cfg_item_type;
	union cfg_value cfg_item_value;
};

/***************************************************************************
  函数名称：
  功    能：显示提示信息
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
static void to_be_continued(const char* prompt)
{
	if (prompt)
		printf("%s，按回车键继续...", prompt);
	else
		printf("按回车键继续...");

	/* 忽略除回车键外的所有输入（注意：_getch的回车是\r，而getchar是\n）*/
	while (_getch() != '\r')
		;
	printf("\n");
	return;
}

/***************************************************************************
  函数名称：
  功    能：因为cfg_item中含有共用体，共用体只能初始化第一项，因此用函数初始化
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
static void set_item(struct cfg_item* item)
{
	item[0].cfg_item_value.ivalue = 123;
	item[1].cfg_item_value.dvalue = 123.45;
	strcpy(item[2].cfg_item_value.svalue, "Hello");
	item[3].cfg_item_value.cvalue = 'Y';
	//	item[4].cfg_item_value.ivalue = 0;
	item[5].cfg_item_value.ivalue = 99999;
	//	item[6].cfg_item_value.ivalue = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int test_readconfig_c(const char* cfgname, const char** group_name, struct cfg_item* item)
{
	FILE* fp;
	int ret, i, j;
	char tmp[256];

	/* 第一次模拟，添加若干组 */
	if (1) {
		remove(cfgname); //删除文件

		fp = open_cfgfile(cfgname, OPEN_OPT_RDWR);
		if (fp == NULL) {
			printf("无法打开配置文件(%s).\n", cfgname);
			return -1;
		}

		for (i = 0; group_name[i]; i++) {
			if ((ret = group_add(fp, group_name[i])) <= 0)
				printf("增加[%s]失败.\n", group_name[i]);
			else
				printf("增加[%s]成功.\n", group_name[i]);
		}
		close_cfgfile(fp); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察是否仅有8个空组", cfgname);
		to_be_continued(tmp);
	}

	/* 第二次模拟，每组添加若干项 */
	if (1) {
		fp = open_cfgfile(cfgname, OPEN_OPT_RDWR);
		if (fp == NULL) {
			printf("无法打开配置文件(%s).\n", cfgname);
			return -1;
		}

		for (i = 0; group_name[i]; i++) {
			for (j = 0; item[j].cfg_item_name; j++) {
				sprintf(tmp, "%s-%s", group_name[i], item[j].cfg_item_name);
				/* 说明：item_add的第3个参数是void*，因此不管什么类型，均传入union第一个成员的地址，即其它成员地址 */
				if ((ret = item_add(fp, group_name[i], tmp, &(item[j].cfg_item_value.ivalue), item[j].cfg_item_type)) <= 0)
					printf("增加[%s]/%s 失败.\n", group_name[i], tmp);
				else
					printf("增加[%s]/%s 成功.\n", group_name[i], tmp);
			}
		}
		close_cfgfile(fp); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察是否8个组均有6个配置项", cfgname);
		to_be_continued(tmp);
	}

	/* 第三次模拟，删除组及项 */
	if (1) {
		sprintf(tmp, "请打开配置文件%s，将[配置组3]复制若干份，配置组2-设置项B复制若干份，且后面的值类型不同", cfgname);
		to_be_continued(tmp);

		fp = open_cfgfile(cfgname, OPEN_OPT_RDWR);
		if (fp == NULL) {
			printf("无法打开配置文件(%s).\n", cfgname);
			return -1;
		}

		printf("删除[配置组3]共%d组\n", group_del(fp, "配置组3"));
		printf("删除[配置组5]共%d组\n", group_del(fp, "配置组5"));

		printf("删除<配置组2-设置项B>共%d项\n", item_del(fp, "配置组2", "配置组2-设置项B"));

		close_cfgfile(fp); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察[配置组3]/[配置组5]/<配置组2-设置项B>是否已被删除", cfgname);
		to_be_continued(tmp);
	}

	/* 第四次模拟，修改项 */
	if (1) {
		fp = open_cfgfile(cfgname, OPEN_OPT_RDWR);
		if (fp == NULL) {
			printf("无法打开配置文件(%s).\n", cfgname);
			return -1;
		}

		int a = 54321;
		double b = 1023.4567;
		char c = 'N';

		/* 已存在项，更新 */
		item_update(fp, "配置组4", "配置组4-设置项B", &a, TYPE_INT);
		item_update(fp, "配置组1", "配置组1-设置项D", "helloworld", TYPE_STRING);
		item_update(fp, "配置组8", "配置组8-设置项E", &b, TYPE_DOUBLE);
		item_update(fp, "配置组6", "配置组6-设置项F", NULL, TYPE_NULL);
		item_update(fp, "配置组7", "配置组7-设置项C", &c, TYPE_CHARACTER);

		/* 不存在项，新增 */
		item_update(fp, "配置组6", "配置组6-设置项U", NULL, TYPE_NULL);
		item_update(fp, "配置组7", "配置组7-设置项V", &c, TYPE_CHARACTER);
		item_update(fp, "配置组7", "配置组6-设置项W", NULL, TYPE_NULL);
		item_update(fp, "配置组4", "配置组4-设置项X", &a, TYPE_INT);
		item_update(fp, "配置组1", "配置组1-设置项Y", "helloworld", TYPE_STRING);
		item_update(fp, "配置组8", "配置组8-设置项Z", &b, TYPE_DOUBLE);

		close_cfgfile(fp); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察上述配置是否已修改", cfgname);
		to_be_continued(tmp);
	}

	/* 第五次模拟，读 */
	if (1) {
		fp = open_cfgfile(cfgname, OPEN_OPT_RDONLY);
		if (fp == NULL) {
			printf("无法打开配置文件(%s).\n", cfgname);
			return -1;
		}

		int a;
		double b;
		char c, s[80];

		/* 已存在项，更新 */
		if (item_get_value(fp, "配置组4", "配置组4-设置项B", &a, TYPE_INT) > 0)
			printf("[配置组4]/配置组4-设置项B=%d\n", a);
		if (item_get_value(fp, "配置组1", "配置组1-设置项F", &a, TYPE_INT) > 0)
			printf("[配置组1]/配置组1-设置项F=%d\n", a);
		if (item_get_value(fp, "配置组6", "配置组6-设置项D", &c, TYPE_CHARACTER) > 0)
			printf("[配置组6]/配置组6-设置项D=%c\n", c);
		if (item_get_value(fp, "配置组8", "配置组8-设置项C", s, TYPE_STRING) > 0)
			printf("[配置组8]/配置组8-设置项C=%s\n", s);
		if (item_get_value(fp, "配置组7", "配置组7-设置项E", NULL, TYPE_NULL) > 0)
			printf("[配置组7]/配置组7-设置项E=<NULL>\n");
		if (item_get_value(fp, "配置组6", "配置组6-设置项B", &b, TYPE_DOUBLE) > 0)
			printf("[配置组6]/配置组6-设置项B=%f\n", b);
		if (item_get_value(fp, "配置组1", "设置项100", NULL, TYPE_NULL) > 0)
			printf("[配置组1]/设置项100=<NULL>\n");
		else
			printf("[配置组1]/设置项100 不存在\n");

		close_cfgfile(fp); //关闭文件，模拟本次设置完成

		sprintf(tmp, "请打开配置文件%s，观察上述配置读取是否正确", cfgname);
		to_be_continued(tmp);
	}

	return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int main()
{
	const char* cfgname = "c1.cfg";
	const char* group_name[] = {
		"配置组1",
		"配置组2",
		"配置组3",
		"配置组4",
		"配置组5",
		"配置组6",
		"配置组7",
		"配置组8",
		NULL
	};

	struct cfg_item item[] =
	{
		{ "设置项A", TYPE_INT, 0 },		//共用体union的初始化，只能初始化第一个元素
		{ "设置项B", TYPE_DOUBLE, 0 },
		{ "设置项C", TYPE_STRING, 0 },
		{ "设置项D", TYPE_CHARACTER, 0 },
		{ "设置项E", TYPE_NULL, 0 },
		{ "设置项F", TYPE_INT, 0 },
		{ NULL,     TYPE_NULL, 0 }
	};
	set_item(item); //给union赋初值

	test_readconfig_c(cfgname, group_name, item);

	return 0;
}