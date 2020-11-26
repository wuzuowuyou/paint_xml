#include<fstream>
#include<iostream>
#include<map>
#include<opencv2/opencv.hpp>
#include"Markup.h"
#include <boost/filesystem.hpp>
#include <algorithm>
using namespace std;
using namespace cv;
//cmarkup xml:: https://www.cnblogs.com/Hisin/archive/2012/02/27/2370649.html

cv::Scalar color_arr[10]={Scalar(255,0,0),Scalar(0,0,255),Scalar(0,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,255,0),Scalar(50,100,200),Scalar(255,255,255),Scalar(0,0,0),Scalar(122,50,200)};

namespace fs = boost::filesystem;

inline int get_filenames(const std::string& dir, std::vector<std::string>& filenames)
{
    fs::path path(dir);
    if (!fs::exists(path))
    {
        return -1;
    }

    fs::directory_iterator end_iter;
    for (fs::directory_iterator iter(path); iter!=end_iter; ++iter)
    {
        if (fs::is_regular_file(iter->status()))
        {
            filenames.push_back(iter->path().string());
        }

        if (fs::is_directory(iter->status()))
        {
            get_filenames(iter->path().string(), filenames);
        }
    }
    return filenames.size();
}

inline bool isPngFile(std::string fileName)
{
    int pos = fileName.rfind(".");
    std::string fileType = fileName.substr(pos+1, fileName.length()-pos-1);
    if(fileType == "png")
        return true;
    return false;
}

inline bool isJpgFile(std::string fileName)
{
    int pos = fileName.rfind(".");
    std::string fileType = fileName.substr(pos+1, fileName.length()-pos-1);
    if(fileType == "jpg")
        return true;
    return false;
}

inline bool isJpegFile(std::string fileName)
{
    int pos = fileName.rfind(".");
    std::string fileType = fileName.substr(pos+1, fileName.length()-pos-1);
    if(fileType == "jpeg")
        return true;
    return false;
}

inline int getImageFiles(const std::string& dir, std::vector<std::string>& filenames)
{
    std::vector<std::string> fileList;
    get_filenames(dir,fileList);
    for(auto fileItem : fileList){
        if(isJpgFile(fileItem) || isPngFile(fileItem) || isJpegFile(fileItem)) {
            filenames.push_back(fileItem);
        } else{
            std::string subs = fileItem.substr(fileItem.length()-5, 5);
            if(-1 == subs.find(".")){
                filenames.push_back(fileItem);
            }
        }
    }
}

//roi_correct
void roi_correct(cv::Rect  &r, cv::Mat &m)
{
    if (r.x < 0) r.x = 0;
    if (r.y < 0) r.y = 0;

    if(r.x >= m.cols-1) r.x=0;
    if(r.y >= m.rows-1) r.y=0;

    if(r.width <= 0) r.width = 1;
    if(r.height <= 0) r.height = 1;

    if(r.x + r.width > m.cols - 1) r.width = m.cols - 1 - r.x;
    if(r.y + r.height > m.rows - 1) r.height = m.rows - 1 - r.y;
}

string str_replace(const string &str,const string &str_find,const string &str_replacee)
{
    string str_tmp=str;
    size_t pos = str_tmp.find(str_find);
    while (pos != string::npos)
    {
        str_tmp.replace(pos, str_find.length(), str_replacee);

        size_t pos_t=pos+str_replacee.length();
        string str_sub=str_tmp.substr(pos_t,str_tmp.length()-pos_t);

        size_t pos_tt=str_sub.find(str_find);
        if(string::npos != pos_tt)
        {
            pos =pos_t + str_sub.find(str_find);
        }else
        {
            pos=string::npos;
        }

    }
    return str_tmp;
}


map<string,int> Label_cnt; //global variable
bool read_xml_draw_img(string path_xml,Mat &img,const string path_save,const int T_save)
{//cmarkup xml:: https://www.cnblogs.com/Hisin/archive/2012/02/27/2370649.html

    map<string,int>::iterator it;
    CMarkup xml;
    bool b_flg=xml.Load(path_xml);
    if(!b_flg)
    {
        cout<<"warning:: load fail\n";
        return 0;
    }

    xml.ResetPos();
    b_flg=xml.FindElem("annotation");
    if(!b_flg)
    {
        cout<<"warning:: 2_load fail\n";
        return 0;
    }

    xml.IntoElem();
    while(xml.FindElem(("object")))
    {
        xml.IntoElem();
        b_flg = xml.FindElem("name");
        string label;
        if(b_flg)
        {
            label=xml.GetData();
            // cout<<"label::"<<label<<endl;
            it = Label_cnt.find(label);
            if(it == Label_cnt.end()) //new label
            {
                Label_cnt[label] = 1;
                string cmd_ = (string)"mkdir -p " + (string)path_save  + label;
                system(cmd_.c_str());
            }else
            {
                Label_cnt[label] += 1;
            }

            if(Label_cnt[label] > T_save)
            {
                continue;
            }
        }
        string path_save_label = (string)path_save + label + (string)"/";

        xml.ResetMainPos();
        b_flg = xml.FindElem("bndbox");
        if(b_flg)
        {
            xml.IntoElem();
            b_flg=xml.FindElem("xmin");
            int xmin=stoi((string)xml.GetData());
            xml.ResetMainPos();

            b_flg=xml.FindElem("ymin");
            int ymin=stoi((string)xml.GetData());
            xml.ResetMainPos();

            b_flg=xml.FindElem("xmax");
            int xmax=stoi((string)xml.GetData());
            xml.ResetMainPos();

            b_flg=xml.FindElem("ymax");
            int ymax=stoi((string)xml.GetData());
            xml.ResetMainPos();

            // cout<<xmin<<" "<<ymin<<" "<<xmax<<" "<<ymax<<endl;
            Point pt_tl(xmin,ymin),pt_br(xmax,ymax);
            Rect rt(pt_tl,pt_br);
            roi_correct(rt,img);
            rectangle(img,rt,Scalar(255,0,0),2);
            cv::putText(img, label, Point(rt.x,rt.y+rt.height/2), cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(0, 0, 255), 2, 8, 0);
            imwrite(path_save_label + std::to_string(Label_cnt[label]) + ".jpg",img);
            xml.OutOfElem();
        }
        xml.OutOfElem();
    }

    return true;
}

string get_xml_path(string path_img,string xml_dir)
{
    int pos_1 = path_img.find_last_of('/');
    string name = path_img.substr(pos_1+1,path_img.size()-pos_1);
    string xml_name = str_replace(name,".jpg",".xml");
    string xml_path = xml_dir + xml_name;
    return xml_path;
}

/*
 by:yhl 2018.05.17
├── xml
├── mark
├── json
*/

int main()
{

    std::string test_folder = "/data_1/2020biaozhushuju/2020_detection_merge/big/JPEGImages/";
    std::string xml_folder = "/data_1/2020biaozhushuju/2020_detection_merge/big/Annotations/";
    std::vector<std::string> imageFiles;
    getImageFiles(test_folder,imageFiles);
    random_shuffle(imageFiles.begin(), imageFiles.end());

    int T_save = 100;
    string path_save = "/data_1/2020biaozhushuju/2020_detection_merge/big/SHOW_CLASS_LABEL/";
    for(int i=0;i<imageFiles.size();i++)
    {
        string path_img = imageFiles[i];
        cv::Mat img = cv::imread(path_img);
        std::cout<<i<<" "<<path_img<<std::endl;
        string path_xml =get_xml_path(path_img,xml_folder);
        //        std::cout<<path_img<<std::endl;
        //        std::cout<<path_xml<<std::endl;

        read_xml_draw_img(path_xml,img,path_save, T_save);
    }


    //    //warin: read xml list
    //    fstream XML_infile("/media/data_2/everyday/0724/rf/xml.txt");
    //    string str;

    //    string check_dir;
    //    string root_dir;
    //    int cnt=0;
    //    int T_save = 100;
    //    string path_save = "/data_1/2020biaozhushuju/2020_detection_merge/big/SHOW_CLASS_LABEL/";
    //    while(getline(XML_infile,str))
    //    {


    //        Mat img_draw=img.clone();
    //        read_xml_draw_img(str,img_draw,path_save, T_save);



    //        cout<<"cnt: "<<cnt<<"  ::: "<<img_name<<endl;
    //    }

    return 0;
}
