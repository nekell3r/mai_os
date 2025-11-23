template <typename... Args>
std::string err::CreateReport(const char* file, int line, const char* condition, const char* fmt, Args...)
{
    std::stringstream stream;
    stream << "==================================================================\n";
    stream << "ASSERTION AT " << file << ": " << line << "\n";
    stream << "Condition: " << condition << "\n";
    stream << "Message: " << fmt << "\n";
    stream << "==================================================================\n";
    return stream.str();
}

