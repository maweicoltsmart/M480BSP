#ifndef __MINI_JSON_H__
#define __MINI_JSON_H__

char json_check(char const *str); //JSON 校验函数
char json_get_value(char const *json,char const *json_key , char *json_value, int len); //JSON获取键值

#endif