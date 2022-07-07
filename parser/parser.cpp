#include "parser.h"
#include <string>

parser::Command::ArgumentInfo::ArgumentInfo()
:   type(ArgumentType::tString),
    optional(false)
{}

parser::Command::ArgumentInfo::ArgumentInfo(ArgumentType type)
:   type(type),
    optional(false)
{}

parser::Command::ArgumentInfo::ArgumentInfo(ArgumentType type, std::string defaultValue)
:   type(type),
    optional(true),
    defaultValue(std::move(defaultValue))
{
    if(!isType(this->defaultValue)) throw std::exception("Default value cannot be parsed to the type entered");
}

bool parser::Command::ArgumentInfo::isType(const std::string &value) const
{
    switch (type) {
        case ArgumentType::tString:
            return true;
        case ArgumentType::tBool:
            return value == "true"
                || value == "TRUE"
                || value == "false"
                || value == "FALSE";
        case ArgumentType::tInt:
            if(value.find(' ') != value.size())
                return false;

            std::string::size_type position;

            try {
                std::stoi(value, &position);
                if (position != value.size())
                    return false;
            } catch (...) {
                return false;
            }

            return true;
        case ArgumentType::tFloat:
            if(value.find(' ') != value.size())
                return false;

            try {
                std::stof(value, &position);
                if (position != value.size()) return false;
            } catch (...) {
                return false;
            }

            return true;
        case ArgumentType::tChar:
            return value.size() == 1;
    }

    return false;
}

parser::Command::Command(char id, FunctorType function, int numArguments)
:   identificator(id),
    numArgumentsTotal(numArguments),
    numArgumentsOptional(0),
    function(function),
    ArgumentsInfo(std::vector<ArgumentInfo>(numArguments))
{}

parser::Command::Command(char id, std::vector<ArgumentInfo> argumentsInfo, FunctorType function)
:   identificator(id),
    function(function),
    ArgumentsInfo(std::move(argumentsInfo))
{
    auto firstOptional = std::find_if(
            this->ArgumentsInfo.begin(),
            this->ArgumentsInfo.end(),
            [](auto ai) { return ai.optional;
            });

    int argn_nec = std::distance(this->ArgumentsInfo.begin(), firstOptional);
    auto firstbad = std::find_if(firstOptional, this->ArgumentsInfo.end(), [](auto ai) { return !ai.optional; });
    if(firstbad != this->ArgumentsInfo.end())
        throw std::exception("All optional arguments must go after all necessary");

    numArgumentsTotal = this->ArgumentsInfo.size();
    numArgumentsOptional = numArgumentsTotal - argn_nec;
}

void parser::Command::execute(ArgumentGeneralType first, ArgumentGeneralType last) const
{
    if (std::distance(first, last) > numArgumentsTotal)
        throw std::exception("Too many arguments");

    auto iter = ArgumentsInfo.begin();
    std::vector<std::string> result;
    for(auto iterGiven = first; iterGiven != last; iterGiven++, iter++) {
        if (!iter->isType(*iterGiven))
            throw std::exception("Given argument cannot be parsed into necessary type");

        result.push_back(*iterGiven);
    }

    for(; iter != ArgumentsInfo.end(); iter++) {
        if(!iter->optional)
            throw std::exception("Not enough arguments");

        result.push_back(iter->defaultValue);
    }

    function(result.cbegin(), result.cend());
}

void parser::addCommand(const Command &command)
{
    if(m_commands.contains(command.identificator))
        throw std::exception("Command already exists");

    m_commands.insert({command.identificator, command});
}

void parser::parse(int argc, char **argv) const
{
    std::vector<std::string> args(argc);
    for(int i = 0; i < argc; i++)
        args[i] = argv[i];

    for(auto iterator = args.begin(); iterator != args.end();) {
        if(!iterator->starts_with('-'))
            throw std::exception("Expected a Command starting with \'-\'");

        if(iterator->length() == 2) {
            if(!m_commands.contains((*iterator)[1]))
                throw std::exception("Command not found");

            auto cmd = m_commands.at((*iterator)[1]);
            if(cmd.numArgumentsTotal == 0) {
                cmd.execute(args.end(), args.end());
                iterator++;
                continue;
            }

            if(std::distance(iterator, args.end()) < cmd.numArgumentsTotal - cmd.numArgumentsOptional)
                throw std::exception("Not enough arguments");

            iterator++;

            auto end = std::min(iterator + cmd.numArgumentsTotal, args.end());
            cmd.execute(iterator, end);
            iterator = end;
        }
        else if (iterator->length() > 2) {
            for(auto characterIterator = iterator->begin() + 1; characterIterator != iterator->end(); characterIterator++) {
                if(!m_commands.contains(*characterIterator))
                    throw std::exception("Command not found");

                auto cmd = m_commands.at(*characterIterator);
                if(cmd.numArgumentsTotal != 0)
                    throw std::exception("Cannot pack commands with arguments");

                cmd.execute(args.end(), args.end());
            }
            iterator++;
        }
    }
}
