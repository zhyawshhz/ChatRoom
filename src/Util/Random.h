/*
 * Random.h
 *
 *  Created on: Jan 26, 2016
 *      Author: brian
 */

#ifndef CHATROOM_UTIL_RANDOM_H_
#define CHATROOM_UTIL_RANDOM_H_




char *get_random_str(char *str,const int len)
{
    int i;
    for(i=0;i<len;++i)
        str[i]='A'+rand()%26;
    str[++i]='\0';
    return str;
}


#endif /* CHATROOM_UTIL_RANDOM_H_ */
