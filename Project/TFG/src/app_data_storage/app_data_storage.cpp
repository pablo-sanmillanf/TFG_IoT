/**
  ******************************************************************************
  * @file   app_data_storage.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  App data storage handler Module.
  *
  * @note   End-of-degree work.
  *         This module handles the write and the read of the application data
  *         in the application folder.
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "app_data_storage.h" // Module header

const std::string Storage::data_directory = "./weatherApp";
const std::string Storage::app_data_file_path = data_directory + "/data.conf";

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
  * @brief Class constructor. Download all the information from previous times.
  */
Storage::AppDataStorage::AppDataStorage(){
  file_path = std::filesystem::path(app_data_file_path);
  download_data();
}

/**
  * @brief Class destroyer. Upload all the information for next times.
  */
Storage::AppDataStorage::~AppDataStorage(){
  upload_data();
}

/**
  * @brief Gets the app parameter with the supplied key and return its value. If the parameter doesn't
  *        exist, the defaultValue will be returned.
  *
  * @param[in] key The data key string.
  * @param[in] defaultValue The value returned if the data doesn't exist.
  *
  * @return The value associated to the key or the default value if the data doesn't exist.
  */
std::string Storage::AppDataStorage::read_data_param(std::string key, std::string defaultValue){
  return root.get(key, defaultValue).asString();
}

/**
  * @brief Creates a new data entry with the key and the value supplied. This function doesn't upload the value in
  * the app data file.
  *
  * @param[in] key The data key of the new entry.
  * @param[in] value The data value of the new entry.
  *
  */
void Storage::AppDataStorage::write_data_param(std::string key, std::string value){
  root[key] = value;
}

/**
  * @brief If the app data file exists, download all the information and delete all the previous non-saved changes.
  *        Otherwise, do nothing.
  *
  */
void Storage::AppDataStorage::download_data(){
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

/**
  * @brief Upload all the data stored in memory in the app data file and overwrites it. If the app data file
  *        doesn't exist, this functions creates it.
  *
  */
void Storage::AppDataStorage::upload_data(){
  if(!std::filesystem::is_directory(file_path.parent_path())){
    std::filesystem::create_directories(data_directory);
  }
  std::ofstream file_stream;
  file_stream.open(file_path);
  file_stream << root;
}
