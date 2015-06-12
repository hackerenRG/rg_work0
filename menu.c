/****************************************************/
/* FILE NAME             : menu.c                   */
/* PRINCIPLE AUTHOR      : renlanhui                */
/* SUBSYSTEM NAME        : MENU                     */
/* MODULE    NAME        : MENU                     */
/* LANGUAGE              : C                        */
/* TARGET ENVIRONMENT    : ANY                      */
/* DATE OF FIRST RELEASE : 2015/03/14               */
/* DESCRIPTION           : MENU PRJ                 */
/****************************************************/

/*
* Revision log:
*
* Created by renlanhui, 2015/03/14
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "list.h"
/****************************************************/
#define MAX_CMD_ARGV_NUM 32
#define MAX_CMD_LEN 100
#define FAILURE (-1)
#define SUCCESS (0)
/****************************************************/
/* data node definition */
typedef struct tdata_node{
	int number;//data
	struct list_head list;
}data_node;

/****************************************************/
/* command node */
typedef struct tnode{
	char* cmd;
	int flag;
	int (*handler)(int argc, char* argv[]);
	struct tnode* next;
}node;

/****************************************************/
/* command function declaration */
static int show(int argc, char *argv[]);
static int stop(int argc, char *argv[]);
static int add(int argc, char *argv[]);
static int del(int argc, char *argv[]);

/****************************************************/
/* global variable definition */

LIST_HEAD(data_list);//head

node cmd_list[] = {
	{"show", 0, show, &(cmd_list[1])},
	{"stop", 0, stop, &(cmd_list[2])},
	{"add",  1, add,  &(cmd_list[3])},
	{"del",  1, del, NULL}
};

/****************************************************/


/*
* according to cmd, find the cmd nodea
*/
static node* find_cmd(node* head, char* cmd)
{
	node* p = head;
	while(NULL != p){
		if(strcmp(p->cmd, cmd) == 0){
			return p;
		}
		p = p->next;
	}
	return NULL;
} 
/*
* application entry function 
*/
int Execute(void)
{
	while(1){
		int argc = 0;
		char *argv[MAX_CMD_ARGV_NUM];
		char cmd[MAX_CMD_LEN];
		char *pcmd = NULL;

		pcmd = fgets(cmd, MAX_CMD_LEN, stdin);
		if(cmd[strlen(cmd) - 1] == '\n')
			cmd[strlen(cmd) - 1] = '\0';
		
		pcmd = strtok(pcmd, " ");
		while(argc < MAX_CMD_ARGV_NUM && NULL != pcmd){
			argv[argc++] = pcmd;
			pcmd = strtok(NULL, " ");
		}

		node* p = find_cmd(&(cmd_list[0]), argv[0]);	

		if(NULL != p){
			if(NULL != p->handler && p->flag == (argc - 1)){
				p->handler(argc, argv);
			}else{
				continue;
			}
		}else{
			printf("error: this is a wrong cmd!\n");
		}
	}
}
/*
* show the data_list 
*/
static int show(int argc, char *argv[])
{
	data_node *pos;
	list_for_each_entry(pos, (&data_list), list, data_node){
		printf("%d\n", pos->number);
	}
	return SUCCESS;	
}

/*
* add a number to data_list
*/
static int add(int argc, char *argv[])
{
	int num;
	if(argc == 2){
		num = atoi(argv[1]);
		if(num == 0 && !isdigit(*(argv[1]))){
			return FAILURE;
		}
	}else{
		return FAILURE;
	}
	
	data_node* new = (data_node*)malloc(sizeof(data_node));
	if(NULL == new){
		printf("error: memory malloc error!");
		return FAILURE;
	}
	
	new->number = num;

	if(list_empty(&data_list)){
		list_add((&(new->list)), (&data_list));
		return SUCCESS;
	}else{
		data_node* pos;
		list_for_each_entry(pos, (&data_list), list, data_node){
			if(pos->number == new->number){
				free(new);
				return SUCCESS;
			}else if(pos->number > new->number){
				list_add((&(new->list)), ((pos->list).prev));
				return SUCCESS;
			}
		}
		list_add_tail(&(new->list), (&data_list));
		return SUCCESS;
	}
}

/*
* delete a number from data_list
*/
static int del(int argc, char *argv[])
{
	int num;
	if(argc == 2){
		num = atoi(argv[1]);
	}else{
		return FAILURE;
	}	
	
	if(list_empty((&data_list))){
		return FAILURE;
	}else{
		data_node* pos;
		list_for_each_entry(pos, (&data_list), list, data_node){
			if(pos->number == num){
				list_del_init((&(pos->list)));
				free(pos);
				return SUCCESS;
			}		
		}
		return FAILURE;
	}
}

/*
* quit from the application
*/
static int stop(int argc, char *argv[])
{
	data_node* pos;
	data_node* tmp;

	list_for_each_entry(pos, (&data_list), list, data_node){
		tmp = pos;
		pos = list_entry(((pos->list).prev), data_node, list);
		list_del_init((&(tmp->list)));
		free(tmp);
	}
	exit(0);	
}

