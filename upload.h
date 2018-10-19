//
// Created by 顾俊 on 2018/10/16.
//

#ifndef UPLOAD_UPLOAD_H
#define UPLOAD_UPLOAD_H

#include <string>
#include <list>
#include <vector>
#include <json/json.h>


#define  UPLOAD_RES "upload.txt"


#define QUERY_RES  "query.txt"

struct PostContent{

    std::string path;
    std::string filename;
    std::string url;
    int groupId;
};


struct QueryContains{

    std::string url;
    int groupId;
    std::string filename;
};

extern std::list <std::string> g_filelist;
//filepath 路径
//返回值 文件列表
std::list<std::string>& getFileList(std::string filepath,std::vector<std::string> delimeter);

//获取文件名后缀
std::string getPostfix(std::string filename);

size_t write_callback(void * ptr,size_t size,size_t nmemb,void*stream);


//处理具体的业务逻辑
void dealBus(std::string & filepath,std::vector <std::string> delimeter,std::string & url,int group_id);

//执行照片入库
void performPost(struct PostContent & icontent);


size_t query_callback(void*ptr,size_t size,size_t nmemb,void * stream);
//照片查询
void performGet(struct QueryContains & query);

void dealQueryBus(std::string & url,int group_id);

#endif //UPLOAD_UPLOAD_H
