//
// Created by 顾俊 on 2018/10/16.
//

#include "upload.h"
#include <dirent.h>
#include <curl/curl.h>
#include <stdlib.h>

std::list <std::string> g_filelist;

std::list<std::string>& getFileList(std::string filepath,std::vector<std::string> delimeter)
{
    DIR *dir;
    struct dirent *ent;

    if((dir = opendir(filepath.c_str()))!= NULL)
    {

        while ((ent =readdir(dir)) != NULL) {
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;

            std::string ext = getPostfix(ent->d_name);
             bool isExit = false;

             for(std::vector<std::string>::const_iterator it = delimeter.begin();it != delimeter.end();it++)
             {
                 if(*it == ext)
                     isExit = true;
                 continue;
             }

             if(isExit)
                g_filelist.push_back(ent->d_name);

        }
      closedir(dir);
    }

    return g_filelist;
}

std::string getPostfix(std::string filename) {
    std::string extension;

    if (filename.rfind('.') != std::string::npos) {

        extension = filename.substr(filename.rfind('.') + 1);

    }
    return extension;
}

void performPost(struct PostContent & icontent)
{
   CURL *curl;
   CURLcode res;

   curl_global_init(CURL_GLOBAL_ALL);


    struct curl_httppost*formpost = NULL;
    struct curl_httppost * lastptr = NULL;

    char  string[10];


    sprintf(string,"%d",icontent.groupId);



    /*****body*****/
    //fill in the filename upload field
    curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME,"name",CURLFORM_COPYCONTENTS,icontent.filename.c_str(),CURLFORM_END);

    // fill in the file upload field
    curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME,"photo",CURLFORM_FILE,(icontent.path + icontent.filename).c_str(),CURLFORM_END);

    curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME,"group_id",CURLFORM_COPYCONTENTS,string,CURLFORM_END);


    /*****headers****/

  // curl_formadd()

    //curl_slist * plist = curl_slist_append(NULL,NULL);

    curl_slist * plist = curl_slist_append(NULL,"Content-Type:multipart/form-data;charset=UTF-8");

   curl = curl_easy_init();  // get a curl handle

   if(curl)
   {
       curl_easy_setopt(curl,CURLOPT_URL,icontent.url.c_str());

       curl_easy_setopt(curl,CURLOPT_HTTPPOST,formpost);

       curl_easy_setopt(curl,CURLOPT_HTTPHEADER,plist);


       //curl_easy_setopt(curl,)



       res = curl_easy_perform(curl);

       if(res != CURLE_OK)
           fprintf(stderr,"curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

       //always cleanup
       curl_easy_cleanup(curl);

       curl_slist_free_all(plist);

   }

  curl_global_cleanup();

}

size_t write_callback(void * ptr,size_t size,size_t nmemb,void*stream)
{

   return size * nmemb;
}


void dealBus(std::string & filepath,std::vector <std::string> delimeter,std::string & url,int group_id)
{
       //获取文件列表，进行过滤
       getFileList(filepath,delimeter);

       struct PostContent icontent;
       for(auto & list:g_filelist)
       {
           icontent.path = filepath;
           icontent.filename = list;
           icontent.url = url;
           icontent.groupId = group_id;

           //执行文件上传
           performPost(icontent);

       }



}