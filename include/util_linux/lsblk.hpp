/**
 * @file lsblk.hpp
 * @author jungkwang.lee (ntoskrnl7@gmail.coom)
 * @brief This module implements  lsblk helper features.
 *
 */

#pragma once

#include <ext/process>
#include <ext/string>
#include <ext/version>
#include <string>

#include <nlohmann/json.hpp>
#include <unordered_map>

#if __cplusplus >= 201703L
#include <filesystem>
#endif
#include <sys/statvfs.h>

namespace util_linux {
inline const ext::version lsblk_version() {
  ext::process lsblk("lsblk", {"-V"});
  std::string version;
  try {
    version =
        std::string(std::istreambuf_iterator<char>(lsblk.out().rdbuf()), {});
  } catch (const std::exception &e) {
    std::cerr << "lsblk_version : " << e.what() << '\n';
  }
  if (lsblk.joinable())
    lsblk.join();

std::size_t pos = version.find_first_of("123456789") ;
  if (pos == std::string::npos)
    throw std::runtime_error("Invalid lsblk version : " + version);

  return ext::rtrim(version.substr(pos));
}

inline std::pair<const ext::version, const char *> &lsblk_available_outputs() {
  static std::pair<const ext::version, const char *> available_ouputs = []() {
    std::unordered_map<ext::version, const char *> outputs = {
        {ext::version("2.33.1"),
         "-oNAME,TYPE,TRAN,MODEL,HOTPLUG,PARTTYPE,FSTYPE,"
         "MOUNTPOINT,VENDOR,REV,"
         "SERIAL,PATH,FSSIZE,FSAVAIL,FSUSE%"}};

    auto it = outputs.find(lsblk_version());
    if (it != outputs.end())
      return *it;

    for (auto output : outputs) {
      ext::process process("lsblk", {"-J", output.second});
      if (process.joinable())
        process.join();
      if (process.exit_code() == 0)
        return output;
    }

    return std::pair<const ext::version, const char *>(
        "2.29.2", "-oNAME,TYPE,TRAN,MODEL,HOTPLUG,PARTTYPE,FSTYPE,"
                  "MOUNTPOINT,VENDOR,REV,"
                  "SERIAL");
  }();
  return available_ouputs;
}

inline std::string lsblk_json_str() {
  ext::process process("lsblk", {"-J", lsblk_available_outputs().second});
  auto result =
      std::string(std::istreambuf_iterator<char>(process.out().rdbuf()), {});
  if (process.joinable())
    process.join();
  return result;
}

inline void adjust_object(nlohmann::json &object) {
  // lsblk 2.29.2에서는 FSSIZE, FSAVAIL, FSUSE%가 없으므로 해당 경우 마운트
  // 경로를 사용하여 크기 정보를 획득합니다.
  if ((object.contains("mountpoint")) &&
      ((!object.contains("fssize")) || (!object.contains("fsavail")) ||
       (!object.contains("fsuse%")))) {
    size_t fssize = 0;
    size_t fsavail = 0;
#if __cplusplus >= 201703L
    std::filesystem::space_info spaceInfo;
    try {
      spaceInfo = std::filesystem::space(object["mountpoint"]);
      fssize = spaceInfo.capacity;
      fsavail = spaceInfo.available;
    } catch (...) {
#endif
      struct statvfs stat = {};
      statvfs(((std::string)object["mountpoint"]).c_str(), &stat);
      fssize = stat.f_bsize * stat.f_blocks;
      fsavail = stat.f_bavail * stat.f_bsize;
#if __cplusplus >= 201703L
    }
#endif
    if (!object.contains("fssize"))
      object["fssize"] = std::to_string(fssize);
    if (!object.contains("fsavail"))
      object["fsavail"] = std::to_string(fsavail);
    if (fssize && (!object.contains("fsuse%")))
      object["fsuse%"] =
          std::to_string((1 - ((double)fsavail / (double)fssize)) * 100) + "%";
  }

  // lsblk 2.33.1에서는 HOTPLUG boolean자료형이지만 lsblk 2.29.2에서는 "0",
  // "1"으로 표현되어있어서 해당 상황에 대한 처리를 했습니다.
  if (!object["hotplug"].is_boolean())
    object["hotplug"] = object["hotplug"] == "1";

  // lsblk 2.29.2에는 PATH가 없으므로 직접 구합니다. (아직 확실한 방법이
  // 없으므로 name을 설정하였으며 추후 변경하시기 바랍니다.)
  if (!object.contains("path"))
    object["path"] = "/dev/ " + (std::string)object["name"];

  // lsblk 2.29.2에서 모델명에 공백이 붙은것으로 확인되어 좌우공백을
  // 제거했습니다.
  if (object["model"].is_string())
    object["model"] = ext::trim((std::string)object["model"]);
}

inline nlohmann::json lsblk(const std::string &dev_path) {
  static const std::string lsblk_outputs = lsblk_available_outputs().second;
  ext::process lsblk("lsblk", {"-J", "--inverse", lsblk_outputs, dev_path});
  nlohmann::json data;
  try {
    lsblk.out() >> data;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  if (lsblk.joinable())
    lsblk.join();
  nlohmann::json ret;

  auto devs = *data.begin();
  auto dev = *devs.begin();
  std::string name = dev["name"];
  std::string path = dev["path"];

  while (dev.contains("children")) {
    auto parent = dev;
    dev = parent["children"][0];
    adjust_object(dev);
    parent.erase("children");

    for (auto it = dev.begin(); it != dev.end();)
      if (it->is_null())
        it = dev.erase(it);
      else
        ++it;

    parent.merge_patch(dev);
    dev = std::move(parent);
  }
  adjust_object(dev);

  dev["name"] = name;
  dev["path"] = path;
  return dev;
}

inline nlohmann::json lsblk() {
  static const std::string lsblk_outputs = lsblk_available_outputs().second;
  ext::process lsblk("lsblk", {"-J", lsblk_outputs});
  nlohmann::json data;
  try {
    lsblk.out() >> data;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  if (lsblk.joinable())
    lsblk.join();
  for (auto &devs : data) {
    for (auto &d : devs) {
      if (d.contains("children")) {
        for (auto &child : d["children"])
          adjust_object(child);
      } else {
        adjust_object(d);
      }
    }
  }
  return data;
}
} // namespace util_linux