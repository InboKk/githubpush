#include "linklist.h"

/*创建空链表:初始化时使用__返回值：链表头结点*/
linklist CreateEmptyLinklist ()
{
    linklist header;  //创建头结点
    header = (linklist)malloc (sizeof (linknode));//为头结点分配内存空间
    /*初始化头结点的数据*/
    header->data.id = -1;
    strcpy(header->data.SensorType, "-1");
    /*初始化头结点的指针域*/
    header->next = NULL;   
    return header;   //返回链表的头结点，根据头结点可以获得整个链表的信息
}
/*插入结点（头插法）：输入参数为待插入的结构体，成功返回0，失败返回-1;当无线传感节点上线后插入*/
void InsertLinkNode(linklist header,struct WireLess_SensorsNode data)
{
    /*为待插入的节点分配内存*/
    linklist NewNode = (linklist)malloc (sizeof (linknode));
    /*将数据填充进新结点中*/
    NewNode->data = data;
    NewNode->next = header->next;
    header->next = NewNode;
}
/*删除结点：成功返回0，失败返回-1；无线传感节点下线后删除*/
int DeleteLinkNode(linklist header,unsigned char id)
{
    linklist DelNode;  //待删除结点
    linklist DelNodeNext;  //待删除结点的下一个结点
    /*如果链表为空*/
    if(header == NULL)
    {
        return -1;
    }
    /*找到当前id对应的结点*/
    DelNode = SearchLinkNode(header,id);
    /*如果待删除结点为尾结点*/
    if(DelNode->next == NULL)
    {
        /*只需要使待删除结点的前一个结点next->NULL*/
        while(header->next != DelNode)
        {
            header = header->next;//得到的header为DelNode的前一个结点
        }
        header->next = DelNode->next;//DelNode->next = NULL;   
        free(DelNode);//释放删除结点的内存空间
    }
    /*如果链表在中间*/
    else
    {
        /*先保存DelNode的下一个结点*/
        DelNodeNext = DelNode->next;
        /*方法一：直接用DelNode替换DelNodeNext，然后删除DelNodeNext*/
        DelNode->data = DelNodeNext->data;
        DelNode->next = DelNodeNext->next;
        free(DelNodeNext);
    }
}
/*遍历结点：获取在线无线传感结点的信息；参数：链表的头结点（指针类型）；功能：输出当前在线的无线传感结点*/
int TraverseLinknode(linklist header)
{
    /*如果链表为空*/
    if(header == NULL)
    {
        return -1;
    }
    /*不为空，头结点是空的，因此直接指向下一个结点*/
    header = header->next;
    /*指针指向NULL时表示到了尾部*/
    while(header)
    {
        printf("Online_ID:%d\n\r",header->data.id);
        printf("Online_ID_Type:%s\n\r",header->data.SensorType);
        header = header->next;
    }
    return 0;
}
/*查询结点：获取某个无线传感节点的信息、在线情况*/
linklist SearchLinkNode(linklist header,unsigned char id)
{
    /*因为头结点为空，所以不用查*/
    header = header->next;
    while(header)
    {
        if(header->data.id == id)
            break;
        header = header->next;
    }
    return header;
}
/*销毁链表:程序结束时将链表的内存空间全部释放*/
void DestroyLinkList(linklist header)
{
    linklist headerNext; //保存删除结点的下一个结点
    while(header)
    {
        headerNext = header->next;
        free(header);
        header = headerNext;
    }
}