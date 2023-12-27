#pragma once

namespace Utility
{

    class ValidationResult
    {
    public:
        ValidationResult();
        ValidationResult(std::wstring errorMessage);
        ValidationResult(std::wstring errorMessage, std::vector<std::wstring> memberNames);
        ValidationResult(std::wstring errorMessage, std::wstring const& memberName);

        [[nodiscard]] bool Success() const { return errorMessage_.empty(); }
        [[nodiscard]] std::wstring const& ErrorMessage() const { return errorMessage_; }
        [[nodiscard]] std::vector<std::wstring> const& MemberNames() const { return memberNames_; }

        auto operator<=>(ValidationResult const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)

    private:
        std::wstring errorMessage_;
        std::vector<std::wstring> memberNames_;
    };


    bool ContainsEqualValidationResult(std::vector<ValidationResult> const& collection, ValidationResult const& target);
    std::optional<ValidationResult> FindEqualValidationResult(std::vector<ValidationResult> const& collection, ValidationResult const& target);

    bool ContainsMemberName(ValidationResult const& validationResult, std::wstring const& target);

    void AddIfNew(std::vector<ValidationResult>& collection, ValidationResult const& target);
    void Erase(std::vector<ValidationResult>& collection, ValidationResult const& target);
}