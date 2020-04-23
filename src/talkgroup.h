

#ifndef _TALK_GROUP_H_
#define _TALK_GROUP_H_

#define TGID_SIZE 12
#define TGNAME_SIZE 12

typedef struct tgData{
	uint32_t id;
	uint8_t name[TGNAME_SIZE];
} talkgroupData_t;

void talkgroup_init(void);
bool talkgroups_load(void);
void talkgroupAdd(talkgroupData_t *data);
void talkgroup_select_by_index(int index);
#endif
