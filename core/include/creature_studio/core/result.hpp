#pragma once

#include <string>
#include <utility>
#include <variant>

namespace creature_studio::core
{

template <typename T>
class Result
{
public:
    static Result success(T value)
    {
        return Result(std::move(value));
    }

    static Result failure(std::string error)
    {
        return Result(std::move(error));
    }

    [[nodiscard]] bool isSuccess() const
    {
        return std::holds_alternative<T>(m_value);
    }

    [[nodiscard]] bool isFailure() const
    {
        return !isSuccess();
    }

    [[nodiscard]] const T& value() const
    {
        return std::get<T>(m_value);
    }

    [[nodiscard]] T& value()
    {
        return std::get<T>(m_value);
    }

    [[nodiscard]] const std::string& error() const
    {
        return std::get<std::string>(m_value);
    }

private:
    explicit Result(T value)
        : m_value(std::move(value))
    {
    }

    explicit Result(std::string error)
        : m_value(std::move(error))
    {
    }

    std::variant<T, std::string> m_value;
};

template <>
class Result<void>
{
public:
    static Result success()
    {
        return Result(true, {});
    }

    static Result failure(std::string error)
    {
        return Result(false, std::move(error));
    }

    [[nodiscard]] bool isSuccess() const
    {
        return m_success;
    }

    [[nodiscard]] bool isFailure() const
    {
        return !m_success;
    }

    [[nodiscard]] const std::string& error() const
    {
        return m_error;
    }

private:
    Result(bool success, std::string error)
        : m_success(success),
          m_error(std::move(error))
    {
    }

    bool m_success;
    std::string m_error;
};

} // namespace creature_studio::core