#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <nlohmann/json.hpp>
using namespace std;

using json = nlohmann::json;

struct Preset {
  string name;
  string description;
  vector<string> packages;
};

int log_success(const string& message) {
  cout << "\033[32m✔ " << message << "\033[0m" << endl;
  return 0;
};

int log_error(const string& message) {
  cerr << "\033[31m✘ " << message << "\033[0m" << endl;
  return 1;
};

int log_msg(const string& message) {
  cout << "\033[34mℹ " << message << "\033[0m" << endl;
  return 0;
};

int log_info(const string& message) {
  cout << "\033[34m→ " << message << "\033[0m" << endl;
  return 0;
};

int log_normal(const string& message) {
  cout << message << endl;
  return 0;
};

int initPresetFile() {
  string homeDir = getenv("HOME");
  string devenvDir = homeDir + "/.local/share/devenv";
  string presetsPath = devenvDir + "/presets.json";
  ifstream devenvDirCheck(devenvDir);
  if (!devenvDirCheck.good()) {
    if (mkdir(devenvDir.c_str(), 0755) != 0) {
      log_error("Failed to create devenv directory.");
      return 1;
    }
  };
  ifstream presetsFile(presetsPath);
  if (!presetsFile.good()) {
    ofstream outFile(presetsPath);
    if (!outFile.is_open()) {
      log_error("Failed to create presets file.");
      return 1;
    }
    outFile << "[]";
    outFile.close();
  }
  return 0;
};

int addPreset(const string& name, const string& description, const vector<string>& packages) {
  string homeDir = getenv("HOME");
  string presetsPath = homeDir + "/.local/share/devenv/presets.json";
  json presetsJson;
  ifstream presetsFile(presetsPath);
  if (presetsFile.is_open()) {
    presetsFile >> presetsJson;
    presetsFile.close();
  }
  json newPreset;
  newPreset["name"] = name;
  newPreset["description"] = description;
  newPreset["packages"] = packages;
  presetsJson.push_back(newPreset);
  ofstream outFile(presetsPath);
  if (!outFile.is_open()) {
    log_error("Failed to open presets file for writing.");
    return 1;
  }
  outFile << presetsJson.dump(4);
  outFile.close();
  return 0;
};

int removePreset(const string& name) {
  string homeDir = getenv("HOME");
  string presetsPath = homeDir + "/.local/share/devenv/presets.json";
  json presetsJson;
  ifstream presetsFile(presetsPath);
  if (presetsFile.is_open()) {
    presetsFile >> presetsJson;
    presetsFile.close();
  }
  json updatedPresets;
  bool foundPreset = false;
  for (const auto& preset : presetsJson) {
    if (preset["name"] != name) {
      updatedPresets.push_back(preset);
    } else {
      foundPreset = true;
    }
  }
  if (!foundPreset) {
    log_error("Preset not found: " + name);
    return 1;
  }
  ofstream outFile(presetsPath);
  if (!outFile.is_open()) {
    log_error("Failed to open presets file for writing.");
    return 1;
  }
  outFile << updatedPresets.dump(4);
  outFile.close();
  return 0;
};

// Presets file on Linux will be .local/share/devenv/presets.json
vector<Preset> getPresets() {
  string homeDir = getenv("HOME");
  string presetsPath = homeDir + "/.local/share/devenv/presets.json";
  ifstream presetsFile(presetsPath);
  vector<Preset> presets;
  if (!presetsFile.is_open()) {
    return presets;
  };
  json presetsJson;
  presetsFile >> presetsJson;
  for (const auto& presetJson : presetsJson) {
    Preset preset;
    preset.name = presetJson["name"];
    preset.description = presetJson["description"];
    for (const auto& package : presetJson["packages"]) {
      preset.packages.push_back(package);
    }
    presets.push_back(preset);
  }
  return presets;
};

Preset getPreset(const string& name) {
  vector<Preset> presets = getPresets();
  for (const auto& preset : presets) {
    if (preset.name == name) {
      return preset;
    }
  }
  return Preset();
};

int presetExists(const string& name) {
  vector<Preset> presets = getPresets();
  for (const auto& preset : presets) {
    if (preset.name == name) {
      return true;
    }
  }
  return false;
};

int handle_presets(int argc, char** argv) {
  vector<Preset> presets = getPresets();
  if (presets.empty()) {
    log_error("No presets found. Use 'preset add <presetName>' to create a new preset.");
    return 0;
  }
  for (const auto& preset : presets) {
    log_normal("Name: " + preset.name);
    log_normal("Description: " + preset.description);
    log_normal("Packages: ");
    for (const auto& package : preset.packages) {
      log_normal("  - " + package);
    }
  }
  return 0;
};

int handle_preset(int argc, char** argv) {
  if (argc < 4) {
    if (argc == 3 && (string(argv[2]) == "list" || string(argv[2]) == "ls")) {
      return handle_presets(argc, argv);
    }
    cout << "Preset commands:" << endl;
    cout << "  add <name>    Add a new preset" << endl;
    cout << "  rm <name>     Remove a preset" << endl;
    cout << "  cat <name>    Show preset details" << endl;
    return 1;
  }
  string action = argv[2];
  string presetName = argv[3];
  if (action == "add") {
    if(presetExists(presetName)) {
      log_error("Preset already exists: " + presetName);
      return 1;
    }
    string description;
    vector<string> packages;
    cout << "Enter description: " << endl;
    cout << "> ";
    getline(cin, description);
    cout << "Enter packages (comma separated): " << endl;
    cout << "> ";
    string packagesInput;
    getline(cin, packagesInput);
    size_t pos = 0;
    while ((pos = packagesInput.find(',')) != string::npos) {
      packages.push_back(packagesInput.substr(0, pos));
      packagesInput.erase(0, pos + 1);
    }
    if (!packagesInput.empty()) {
      packages.push_back(packagesInput);
    }
    return addPreset(presetName, description, packages);
  } else if (action == "rm") {
    if(removePreset(presetName) != 0) {
      log_error("Failed to remove preset: " + presetName);
      return 1;
    }
    log_success("Preset removed: " + presetName);
    return 0;
  } else if (action == "cat") {
    vector<Preset> presets = getPresets();
    for (const auto& preset : presets) {
      if (preset.name == presetName) {
        log_normal("Name: " + preset.name);
        log_normal("Description: " + preset.description);
        log_normal("Packages: ");
        for (const auto& package : preset.packages) {
          log_normal("  - " + package);
        }
        return 0;
      }
    }
    log_error("Preset not found: " + presetName);
    return 1;
  } else {
    log_error("Unknown action: " + action);
    return 1;
  }
}

int dirExists(const string& path) {
  struct stat info;
  if (stat(path.c_str(), &info) != 0) {
    return false;
  }
  return (info.st_mode & S_IFDIR) != 0;
};

string promptUser(const string& prompt) {
  cout << prompt << endl;
  cout << "> ";
  string input;
  getline(cin, input);
  return input;
};

int handle_init(int argc, char** argv) {
  string presetName;
  string directory;
  // if it's less than 3, we need to prompt for both preset name and directory
  if (argc < 3) {
    presetName = promptUser("Enter preset name:");
    if (!presetExists(presetName)) {
      log_error("Preset not found: " + presetName);
      return 1;
    }
    directory = promptUser("Enter directory to initialize in:");
    if (dirExists(directory)) {
      log_error("Directory already exists: " + directory);
      return 1;
    }
  } else if (argc == 3) {
    presetName = argv[2];
    if (!presetExists(presetName)) {
      log_error("Preset not found: " + presetName);
      return 1;
    }
    directory = promptUser("Enter directory to initialize in:");
    if (dirExists(directory)) {
      log_error("Directory already exists: " + directory);
      return 1;
    }
  } else {
    presetName = argv[2];
    directory = argv[3];
    if (!presetExists(presetName)) {
      log_error("Preset not found: " + presetName);
      return 1;
    }
    if (dirExists(directory)) {
      log_error("Directory already exists: " + directory);
      return 1;
    }
  }
  // create directory
  if (mkdir(directory.c_str(), 0755) != 0) {
    log_error("Failed to create directory: " + directory);
    return 1;
  }
  log_success("Creating project directory...");
  Preset preset = getPreset(presetName);
  string shellNixPath = directory + "/shell.nix";
  ofstream shellNixFile(shellNixPath);
  if (!shellNixFile.is_open()) {
    log_error("Failed to create shell.nix file.");
    return 1;
  }
  shellNixFile << "{ pkgs ? import <nixpkgs> {} }:" << endl;
  shellNixFile << "pkgs.mkShell {" << endl;
  shellNixFile << "  packages = with pkgs; [" << endl;
  for (const auto& package : preset.packages) {
    shellNixFile << "    " << package << endl;
  }
  shellNixFile << "  ];" << endl;
  shellNixFile << "}" << endl;
  shellNixFile.close();
  log_success("Writing shell.nix file... (" + preset.name + " preset)");
  // try and run git init in the directory but don't fail if it doesn't work, just log a message also make the system call quiet
  string gitInitCommand = "git -C " + directory + " init > /dev/null 2>&1";
  int gitInitResult = system(gitInitCommand.c_str());
  if (gitInitResult != 0) {
    log_error("Failed to initialize git repository");
  } else {
    log_success("Initializing git repository...");
  }
  log_success("Done.");
  cout << endl;
  log_msg("To start developing, run the following commands:");
  log_info("cd " + directory);
  log_info("nix-shell");
  log_msg("Happy hacking! 🚀");
  return 0;
};

int log_commands() {
  log_normal("Available commands:");
  log_normal("  presets             List all presets");
  log_normal("  preset add <name>    Add a new preset");
  log_normal("  preset rm <name>     Remove a preset");
  log_normal("  preset cat <name>    Show preset details");
  log_normal("  preset list          List all presets (or ls)");
  log_normal("  init <preset> <dir>  Initialize a new project");
  return 0;
}

int main(int argc, char** argv) {
  string command;
  initPresetFile();
  if(argc > 1) {
    command = argv[1];
  } else {
    command = "";
  };
  if(command.empty()) {
    log_error("No command provided.");
    log_commands();
    return 1;
  }
  if (command == "presets") {
    return handle_presets(argc, argv);
  } else if (command == "preset") {
    return handle_preset(argc, argv);
  } else if (command == "init") {
    return handle_init(argc, argv);
  } else {
    log_error("Unknown command: " + command);
    log_commands();
    return 1;
  }
  return 0;
}