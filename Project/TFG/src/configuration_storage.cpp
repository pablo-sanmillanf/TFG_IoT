/*
 * configuration_storage.cpp
 *
 *  Created on: May 18, 2023
 *      Author: ubuntu
 */

#include <json/json.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace Configuration{
const std::string conf_file_path = "/etc/app/conf.conf";

class ConfStorage{
  Json::Value root;
  std::filesystem::path file_path;
public:
  ConfStorage(){
    file_path = std::filesystem::path(conf_file_path);
    download_confs();
  }

  ~ConfStorage(){
    upload_confs();
  }

  std::string read_conf_param(std::string key, std::string defaultValue){
    return root.get(key, defaultValue).asString();
  }

  void write_conf_param(std::string key, std::string value){
    root[key] = value;
  }

  void download_confs(){
    if(std::filesystem::exists(file_path)){
      std::ifstream file_stream;
      file_stream.open(file_path);
      std::string file_content( (std::istreambuf_iterator<char>(file_stream) ),
                                (std::istreambuf_iterator<char>()            ) );

      Json::Reader reader;
      if(!reader.parse(file_content, root)){
        root.clear();
      }
    }
  }

  void upload_confs(){
    if(!std::filesystem::is_directory(file_path.parent_path())){
      std::filesystem::create_directories("/etc/app");
    }
    std::ofstream file_stream;
    file_stream.open(file_path);
    file_stream << root;
  }


};
}
