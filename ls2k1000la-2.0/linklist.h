#ifndef __LIST_LIST_H____
#define __LIST_LIST_H____
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*无线传感采集节点的数据结构体*/
struct WireLess_SensorsNode
{
    unsigned char id;   //无线传感节点的id
    char SensorType[24];   //无线传感节点的采集类型：温度/湿度/etc..
};
/*存储无线传感节点信息的链表节点*/
typedef struct LinkNode
{
    struct WireLess_SensorsNode data;  //无线传感节点数据
    struct LinkNode *next;     //下一个链表节点的地址
}linknode, *linklist;
/*创建空链表:初始化时使用__返回值：链表头结点*/
extern linklist CreateEmptyLinklist ();
/*插入结点（头插法）：输入参数为待插入的结构体，成功返回0，失败返回-1;当无线传感节点上线后插入*/
extern void InsertLinkNode(linklist header,struct WireLess_SensorsNode data);
/*删除结点：成功返回0，失败返回-1；无线传感节点下线后删除*/
extern int DeleteLinkNode(linklist header,unsigned char id);
/*遍历结点：获取在线无线传感结点的信息；参数：链表的头结点（指针类型）；功能：输出当前在线的无线传感结点*/
extern int TraverseLinknode(linklist header);
/*查询结点：获取某个无线传感节点的信息、在线情况*/
extern linklist SearchLinkNode(linklist header,unsigned char id);
/*销毁链表:程序结束时将链表的内存空间全部释放*/
extern void DestroyLinkList(linklist header);

#endif