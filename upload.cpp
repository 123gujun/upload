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

    curl_slist * plist = curl_slist_append(NULL,"Content-Type:multipart/form-data;charset=UTF-8");

   curl = curl_easy_init();  // get a curl handle

   if(curl)
   {
       curl_easy_setopt(curl,CURLOPT_URL,icontent.url.c_str());

       curl_easy_setopt(curl,CURLOPT_HTTPPOST,formpost);

       curl_easy_setopt(curl,CURLOPT_HTTPHEADER,plist);


       curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
       curl_easy_setopt(curl,CURLOPT_WRITEDATA,UPLOAD_RES);


       res = curl_easy_perform(curl);

       if(res != CURLE_OK)
           fprintf(stderr,"curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

       //always cleanup
       curl_easy_cleanup(curl);

       curl_slist_free_all(plist);

   }

  curl_global_cleanup();

}

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::string Instr = static_cast< char * >(stream);

    FILE *fp = NULL;

    char * ResStr = static_cast<char*>(ptr);

    std::string str = ResStr;
    Json::Value root ;
    Json::Reader reader;
    std::string res;

    if(reader.parse(str,root)) {
        int value = root["code"].asInt();

        char ss[12];
        sprintf(ss, "%d", value);
        res = ss;
        res += (res == "0" ? "success\n" : "get the failed result!\n");
    }
    else
        res = "write_callback parse failed !\n";


    fp = fopen((char *)(Instr.c_str()), "at+");

    if (fp)
        fwrite(res.c_str(), strlen(res.c_str()), 1, fp);

    fclose(fp);
    return size * nmemb;
}


void dealBus(std::string & filepath,std::vector <std::string> delimeter,std::string & url,int group_id)
{
       //get filename list
       getFileList(filepath,delimeter);


       struct PostContent icontent;
       memset(&icontent,0, sizeof(struct PostContent));
       for(auto & list:g_filelist)
       {
           icontent.path = filepath;
           icontent.filename = list;
           icontent.url = url;
           icontent.groupId = group_id;

           //perform to upload
           performPost(icontent);

           memset(&icontent,0, sizeof(struct PostContent));

       }

}

size_t query_callback(void*ptr,size_t size,size_t nmemb,void * stream)
{
    char* filename = static_cast<char*>(stream);

    char*restr = static_cast<char*>(ptr);

    Json::Value value;

    Json::Reader reader;

    std::string Res;


    if(reader.parse(restr,value)){

        int total = value["data"]["total"].asInt();

        int res = (value["code"].asInt() == 0 && total >= 1) ? total : -1 ;

        char tmpstr[128];

        if(res == -1)
            Res = "get the failed result!\n";
        else
            Res = "success!\n";
    }else{

         Res = "write_callback parse failed !\n";
    }

    FILE * fp = NULL;
     fp = fopen(filename, "at+");

    if (fp)
        fwrite(Res.c_str(), strlen(Res.c_str()), 1, fp);

    fclose(fp);
    return size * nmemb;

}

void performGet(struct QueryContains & query)
{

    CURL *curl;
    CURLcode res;

    char GetStr[128];
    memset(GetStr,0,128);

    sprintf(GetStr,"%s?group_id=%d&keyword=%s",query.url.c_str(),query.groupId,query.filename.c_str());

    //curl_slist *plist = curl_slist_append(NULL, GetStr);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();  // get a curl handle


    if(curl) {
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,query_callback);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,QUERY_RES);
        curl_easy_setopt(curl,CURLOPT_URL,GetStr);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "performGet curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        //always cleanup
        curl_easy_cleanup(curl);

        //curl_slist_free_all(plist);

    }

    curl_global_cleanup();
}

void dealQueryBus(std::string & url,int group_id)
{
    struct QueryContains query;
    memset(&query,0,sizeof(struct QueryContains));
    for(auto list : g_filelist)
    {
        query.filename = list;
        query.url = url;
        query.groupId = group_id;
        performGet(query);
        memset(&query,0,sizeof(struct QueryContains));
    }

}