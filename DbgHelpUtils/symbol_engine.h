#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <experimental/generator>

#include "i_stack_walk_callback.h"
#include "i_symbol_load_callback.h"
#include "symbol_type_info.h"

namespace dlg_help_utils
{
    class stream_thread_context;
}

namespace dlg_help_utils::dbg_help
{
    extern HANDLE fake_process;
    extern HANDLE fake_thread;

    class i_symbol_callback
    {
    public:
        i_symbol_callback() = default;
        virtual ~i_symbol_callback() = default;

        i_symbol_callback(i_symbol_callback const&) = default;
        i_symbol_callback(i_symbol_callback &&) = default;

        i_symbol_callback& operator=(i_symbol_callback const&) = default;
        i_symbol_callback& operator=(i_symbol_callback &&) = default;

        [[nodiscard]] virtual DWORD loading_module_check_sum() const = 0;
        [[nodiscard]] virtual bool loading_module() const = 0;
        virtual void set_downloading_module(std::wstring module_name) = 0;
        [[nodiscard]] virtual std::wstring_view downloading_module() const = 0;
        [[nodiscard]] virtual i_symbol_load_callback& callback() const = 0;
    };

    class symbol_engine;
    class callback_handle
    {
    private:
        callback_handle(std::function<void()> on_complete)
        : on_complete_{std::move(on_complete)}
        {
        }

    public:
        ~callback_handle()
        {
            on_complete_();
        }

        callback_handle(callback_handle const&) = delete;
        callback_handle(callback_handle &&) = default;

        callback_handle& operator=(callback_handle const&) = delete;
        callback_handle& operator=(callback_handle &&) = default;

    private:
        std::function<void()> on_complete_;

        friend class symbol_engine;
    };

    class symbol_engine
        : private i_symbol_callback
    {
    public:
        symbol_engine(i_symbol_load_callback& callback);
        ~symbol_engine() override;

        symbol_engine(symbol_engine const&) = delete;
        symbol_engine(symbol_engine&&) = default;

        symbol_engine& operator=(symbol_engine const&) = delete;
        symbol_engine& operator=(symbol_engine&&) = default;

        void clear_modules();
        void load_module(std::wstring module_name, DWORD64 module_base, DWORD module_size, DWORD module_time_stamp,
                         DWORD module_check_sum, void const* cv_record, DWORD cv_record_size, void const* misc_record,
                         DWORD misc_record_size, VS_FIXEDFILEINFO const& version_info);
        void load_module(std::wstring module_name, DWORD64 module_base, DWORD module_size, DWORD module_time_stamp,
                         DWORD module_check_sum);
        void unload_module(std::wstring const& module_name);
        [[nodiscard]] bool is_module_loaded(std::wstring const& module_name) const;
        [[nodiscard]] bool is_module_symbols_available(std::wstring const& module_name) const;
        [[nodiscard]] IMAGEHLP_MODULEW64 get_module_information(std::wstring const& module_name) const;
        [[nodiscard]] IMAGEHLP_MODULEW64 get_module_information(DWORD64 module_base) const;
        [[nodiscard]] std::wstring const& get_module_image_path(std::wstring const& module_name) const;
        [[nodiscard]] std::optional<symbol_address_info> address_to_info(DWORD64 address);
        [[nodiscard]] std::optional<symbol_address_info> address_to_info(STACKFRAME_EX const& frame);

        [[nodiscard]] std::optional<symbol_type_info> get_type_info(std::wstring const& type_name) const;
        [[nodiscard]] std::optional<symbol_type_info> get_type_info(std::wstring const& module_name, std::wstring const& type_name) const;
        [[nodiscard]] std::vector<symbol_type_info> module_types(std::wstring const& module_name);

        [[nodiscard]] static std::optional<symbol_type_info> get_symbol_info(std::wstring const& symbol_name);
        [[nodiscard]] static std::vector<symbol_type_info> symbol_walk(std::wstring const& find_mask = {});

        [[nodiscard]] static std::experimental::generator<symbol_address_info> stack_walk(stream_thread_context const& thread_context);

        [[nodiscard]] i_symbol_load_callback& callback() const override { return *callback_; }

        [[nodiscard]] static std::tuple<std::wstring, std::wstring> parse_type_info(std::wstring const& type_name);

        static callback_handle set_callback(i_stack_walk_callback& callback);

    private:
        [[nodiscard]] DWORD loading_module_check_sum() const override { return loading_module_check_sum_; }
        [[nodiscard]] bool loading_module() const override { return loading_module_; }
        void set_downloading_module(std::wstring module_name) override { downloading_module_name_ = std::move(module_name); }
        [[nodiscard]] std::wstring_view downloading_module() const override { return downloading_module_name_; }

    private:
        struct module_info
        {
            DWORD64 handle;
            DWORD64 base;
            DWORD size;
            std::wstring module_image_path;
        };

        [[nodiscard]] std::map<std::wstring, module_info>::const_iterator find_module_name(DWORD64 address);

        void dump_loaded_module_information(DWORD64 handle, DWORD64 module_base,
                                            std::wstring const& module_image_path) const;
        [[nodiscard]] std::wstring load_module_image_path(DWORD module_size, DWORD module_time_stamp, DWORD module_check_sum,
                                            std::wstring const& module_name, DWORD64 handle);
        [[nodiscard]] DWORD64 load_module(std::wstring const& module_name, DWORD64 module_base, DWORD module_size,
                            MODLOAD_DATA* module_load_info);

        [[nodiscard]] static std::optional<symbol_type_info> load_type_info(DWORD64 module_base, std::wstring const& type_name);

    private:
        i_symbol_load_callback* callback_;
        std::unique_ptr<SYMBOL_INFOW> symbol_;
        IMAGEHLP_LINEW64 line_{};
        std::map<std::wstring, module_info> modules_{};
        DWORD loading_module_check_sum_{};
        bool loading_module_{};
        std::wstring downloading_module_name_{};
    };
}
