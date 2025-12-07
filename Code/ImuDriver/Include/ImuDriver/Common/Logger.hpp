#pragma once

#include <format>
#include <string>

namespace Log
{

using Message = std::string;

void Debug(const Message& message);
void Info(const Message& message);
void Error(const Message& message);

constexpr auto FileName = "logs.txt";

}
