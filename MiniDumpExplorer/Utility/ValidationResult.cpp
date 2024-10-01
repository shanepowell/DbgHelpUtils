#include "pch.h"
#include "ValidationResult.h"

namespace Utility
{
    ValidationResult::ValidationResult() = default;

    ValidationResult::ValidationResult(std::wstring errorMessage)  // NOLINT(performance-unnecessary-value-param)
        : errorMessage_{std::move(errorMessage)}
    {
    }

    ValidationResult::ValidationResult(std::wstring errorMessage, std::vector<std::wstring> memberNames)  // NOLINT(performance-unnecessary-value-param)
        : errorMessage_{ std::move(errorMessage) }
        , memberNames_{ std::move(memberNames) }
    {
    }

    ValidationResult::ValidationResult(std::wstring errorMessage, std::wstring const& memberName)  // NOLINT(performance-unnecessary-value-param)
        : errorMessage_{ std::move(errorMessage) }
    {
        memberNames_.push_back(memberName);
    }

    bool ContainsEqualValidationResult(std::vector<ValidationResult> const& collection, ValidationResult const& target)  // NOLINT(misc-use-internal-linkage)
    {
        return FindEqualValidationResult(collection, target).has_value();
    }

    std::optional<ValidationResult> FindEqualValidationResult(std::vector<ValidationResult> const& collection, ValidationResult const& target)  // NOLINT(misc-use-internal-linkage)
    {
        for(auto const& oldValidationResult : collection)
        {
            if (oldValidationResult.ErrorMessage() == target.ErrorMessage())
            {
                if(oldValidationResult.MemberNames() == target.MemberNames())
                {
                    return oldValidationResult;
                }
            }
        }

        return std::nullopt;
    }

    bool ContainsMemberName(ValidationResult const& validationResult, std::wstring const& target)
    {
        if(validationResult.MemberNames().empty() && target.empty())
        {
            return true;
        }

        return std::ranges::find(validationResult.MemberNames(), target) != validationResult.MemberNames().end();
    }

    void AddIfNew(std::vector<ValidationResult>& collection, ValidationResult const& target)  // NOLINT(misc-use-internal-linkage)
    {
        if(std::ranges::find(collection, target) == collection.end())
        {
            collection.push_back(target);
        }
    }

    void Erase(std::vector<ValidationResult>& collection, ValidationResult const& target)  // NOLINT(misc-use-internal-linkage)
    {
        if(auto const it = std::ranges::find(collection, target);
            it != collection.end())
        {
            collection.erase(it);
        }
    }
}
