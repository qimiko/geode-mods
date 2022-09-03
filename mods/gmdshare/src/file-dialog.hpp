#pragma once

#include <string>
#include <functional>

using FileDialogCallback = std::function<void(const std::string& path)>;

void showSaveDialogCxx(const std::string& name, FileDialogCallback cb);
void showOpenDialogCxx(FileDialogCallback cb);
void outputException(const char* what);
