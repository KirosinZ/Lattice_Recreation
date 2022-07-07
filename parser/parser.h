#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <functional>

class parser
{
public:
    struct Command
    {
        struct ArgumentInfo
        {
            enum class ArgumentType
            {
                tBool,
                tInt,
                tFloat,
                tChar,
                tString
            };

            ArgumentType type = ArgumentType::tString;
            bool optional = false;
            std::string defaultValue;

            ArgumentInfo();

            explicit ArgumentInfo(ArgumentType type);

            ArgumentInfo(ArgumentType type, std::string defaultValue);

            [[nodiscard]]
            bool isType(const std::string &value) const;
        };

        using ArgumentGeneralType = const std::vector<std::string>::const_iterator&;
        using FunctorType = void(ArgumentGeneralType, ArgumentGeneralType);
        char identificator;
        int numArgumentsTotal;
        int numArgumentsOptional;
        std::vector<ArgumentInfo> ArgumentsInfo;
        std::function<FunctorType> function;

        Command(char id, FunctorType function, int numArguments);

        Command(char id, std::vector<ArgumentInfo> argumentsInfo, FunctorType function);

        void execute(ArgumentGeneralType first, ArgumentGeneralType last) const;
    };

    void addCommand(const Command &command);

    void parse(int argc, char *argv[]) const;

private:
    std::unordered_map<char, Command> m_commands;
};

#endif //PARSER_PARSER_H
