/**
  ******************************************************************************
  * @file   app_data_storage.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  App data storage handler Module Header.
  *
  * @note   End-of-degree work.
  *         This module handles the write and the read of the application data
  *         in the application folder.
  ******************************************************************************
*/

#ifndef __APP_DATA_STORAGE_H__
#define __APP_DATA_STORAGE_H__


/* Includes ------------------------------------------------------------------*/

#include <json/json.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>


namespace Storage{

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const std::string data_directory;
extern const std::string app_data_file_path;

/* Exported macro ------------------------------------------------------------*/
/* Exported Classes ----------------------------------------------------------*/

class AppDataStorage{
  Json::Value root;
  std::filesystem::path file_path;
public:

  /**
    * @brief Class constructor. Download all the information from previous times.
    */
  AppDataStorage();

  /**
    * @brief Class destroyer. Upload all the information for next times.
    */
  ~AppDataStorage();

  /**
    * @brief Gets the app parameter with the supplied key and return its value. If the parameter doesn't
    *        exist, the defaultValue will be returned.
    *
    * @param[in] key The data key string.
    * @param[in] defaultValue The value returned if the data doesn't exist.
    *
    * @return The value associated to the key or the default value if the data doesn't exist.
    */
  std::string read_data_param(std::string key, std::string defaultValue);

  /**
    * @brief Creates a new data entry with the key and the value supplied. This function doesn't upload the value in
    * the app data file.
    *
    * @param[in] key The data key of the new entry.
    * @param[in] value The data value of the new entry.
    *
    */
  void write_data_param(std::string key, std::string value);

  /**
    * @brief If the app data file exists, download all the information and delete all the previous non-saved changes.
    *        Otherwise, do nothing.
    *
    */
  void download_data();

  /**
    * @brief Upload all the data stored in memory in the app data file and overwrites it. If the app data file
    *        doesn't exist, this functions creates it.
    *
    */
  void upload_data();
};


}



#endif /* __APP_DATA_STORAGE_H__ */
