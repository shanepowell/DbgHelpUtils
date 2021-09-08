// AllocationSetupTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#define NOMINMAX 1
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable : 26812 26495)
#include <boost/program_options.hpp>
#pragma warning(pop)

#include "ResultSet.h"

int LfhAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator, ResultSet& set);
int VirtualAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator, ResultSet& set);
int AllocateSizeRanges(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator, ResultSet& set);
template<size_t N>
bool AllocateBuffers(std::wostream& log, std::function<void*(size_t size)> const& allocator, std::array<void*, N>& allocations, char& fill_value, size_t allocation_size, size_t increase_amount, char const* type, ResultSet& set);
template<size_t N>
void DeallocateSomeBuffers(std::wostream& log, ResultSet& set, std::function<void(void*)> const& deallocator, std::array<void*, N>& allocations);

template<size_t N, typename ...Args>
void DeallocateSomeBuffers(std::wostream& log, ResultSet& set, std::function<void(void*)> const& deallocator, std::array<void*, N>& allocations, Args... args)
{
    DeallocateSomeBuffers(log, set, deallocator, allocations);
    DeallocateSomeBuffers(log, set, deallocator, args...);
}

void DeallocateSomeBuffer(std::wostream& log, ResultSet& set, std::function<void(void*)> const& deallocator, void* allocation);

void FreeAllocationInResultSet(ResultSet& set, void* allocation);

void CreateOutput(std::wostream& log, std::wstring const& dump_filename);
void GenerateDumpFile(std::wostream& log, std::wstring const& dump_filename);

template<typename T, typename D>
std::unique_ptr<T, D> make_handle(T* handle, D deleter)
{
    return std::unique_ptr<T, D>{handle, deleter};
}


int main(int const argc, char* argv[])
{
    namespace po = boost::program_options;
    try
    {
        try
        {
            po::options_description options;
            options.add_options()
                ("help,h", "produce help message")
                ("lfh", "generate lfh allocations")
                ("virtual", "generate virtual allocations")
                ("sizes", "generate range allocation sizes allocations")
                ("useheapalloc", "use HeapAlloc/HeapFree")
                ("usemalloc", "use malloc/free")
                ("usenew", "use new/delete")
                ("dmp", po::wvalue<std::wstring>(), "dump filename")
                ("log", po::wvalue<std::wstring>(), "log filename")
                ("json", po::wvalue<std::wstring>(), "json filename")
                ;

            po::variables_map vm;
            store(parse_command_line(argc, argv, options), vm);
            notify(vm);

            auto const do_lfh_allocations = vm.count("lfh") != 0;
            auto const do_virtual_allocations = vm.count("virtual") != 0;
            auto const do_sizes = vm.count("sizes") != 0;
            auto const use_malloc = vm.count("usemalloc") != 0;
            auto const use_new = vm.count("usenew") != 0;
            auto const dump_filename = vm.count("dmp") > 0 ? vm["dmp"].as<std::wstring>() : std::wstring{};
            auto const log_filename = vm.count("log") > 0 ? vm["log"].as<std::wstring>() : std::wstring{};
            auto const json_filename = vm.count("json") > 0 ? vm["json"].as<std::wstring>() : std::wstring{};

            if (vm.count("help") || (!do_lfh_allocations && !do_virtual_allocations && !do_sizes))
            {
                std::cout << options << "\n";
                return EXIT_SUCCESS;
            }

            std::function allocator = [](size_t const size) { return HeapAlloc(GetProcessHeap(), 0x0, size); };
            std::function deallocator = [](void* memory) { HeapFree(GetProcessHeap(), 0x0, memory); };

            if(use_malloc)
            {
                allocator = [](size_t const size) { return malloc(size); };
                deallocator = [](void* memory) { free(memory); };
            }

            if(use_new)
            {
                allocator = [](size_t const size) { return new char[size]; };
                deallocator = [](void* memory) { delete[] static_cast<char*>(memory); };
            }

            std::unique_ptr<std::wfstream> log;
            std::wostream* o_log{&std::wcout};
            if(!log_filename.empty())
            {
                log = std::make_unique<std::wfstream>(log_filename, std::ios_base::out | std::ios_base::trunc);
                if(log->bad())
                {
                    std::wcout << "failed to open log file: " << log_filename << '\n';
                    return EXIT_FAILURE;
                }
                o_log = log.get();
            }

            auto result = EXIT_SUCCESS;
            ResultSet set;
            if(do_lfh_allocations)
            {
                result = LfhAllocations(*o_log, dump_filename, allocator, deallocator, set);
            }

            if(result == EXIT_SUCCESS && do_virtual_allocations)
            {
                result = VirtualAllocations(*o_log, dump_filename, allocator, deallocator, set);
            }

            if(result == EXIT_SUCCESS && do_sizes)
            {
                result = AllocateSizeRanges(*o_log, dump_filename, allocator, deallocator, set);
            }

            
            if(!json_filename.empty())
            {
                std::fstream json_file{json_filename, std::ios_base::out | std::ios_base::trunc};
                if(json_file.bad())
                {
                    std::wcout << "failed to open json result set file: " << json_filename << '\n';
                    return EXIT_FAILURE;
                }
                json_file << JS::serializeStruct(set);
                json_file.close();
            }

            log->close();
            return result;
        }
        catch (std::exception const& e)
        {
            std::cout << "fatal error: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "fatal error: Unknown exception\n";
        }
    }
    catch(...)
    {
    }
    return EXIT_FAILURE;
}

int LfhAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator, ResultSet& set)
{
    std::array<void*, 0x12> backend_allocations{};
    constexpr size_t allocation_size = 0x10;

    char fill_value = 'A';
    if (!AllocateBuffers(log, allocator, backend_allocations, fill_value, allocation_size, 0, "backend", set))
    {
        return EXIT_FAILURE;
    }

    std::array<void*, 0x12> frontend_allocations{};
    if (!AllocateBuffers(log, allocator, frontend_allocations, fill_value, allocation_size, 0, "frontend", set))
    {
        return EXIT_FAILURE;
    }

    DeallocateSomeBuffers(log, set, deallocator, backend_allocations, frontend_allocations);

    CreateOutput(log, dump_filename);

    return EXIT_SUCCESS;
}


int VirtualAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator, ResultSet& set)
{
    std::array<void*, 0x5> virtual_allocations{};
    
    if (char fill_value = 'A';
        !AllocateBuffers(log, allocator, virtual_allocations, fill_value, 0x100000, 0, "virtual", set))
    {
        return EXIT_FAILURE;
    }

    DeallocateSomeBuffers(log, set, deallocator, virtual_allocations);

    CreateOutput(log, dump_filename);

    return EXIT_SUCCESS;
}

int AllocateSizeRanges(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator, ResultSet& set)
{
    size_t allocation_size = 0x1;

    auto constexpr max_rounds = 6;

    std::array<std::array<void*, 0xF>, max_rounds> allocation_groups{};
    std::array<std::array<void*, 0xE>, max_rounds - 1> small_allocation_groups{};

    for(auto index = 0; index < max_rounds; ++index)
    {
        char fill_value = 'A';
        if (!AllocateBuffers(log, allocator, allocation_groups[index], fill_value, allocation_size, allocation_size, "sizes", set))
        {
            return EXIT_FAILURE;
        }
        if(allocation_size > 1)
        {
            if (!AllocateBuffers(log, allocator, small_allocation_groups[index - 1], fill_value, allocation_size + 1, 1, "sizes", set))
            {
                return EXIT_FAILURE;
            }
        }
        allocation_size *= 0x10;
    }

    for(auto& allocations : allocation_groups)
    {
        DeallocateSomeBuffers(log, set, deallocator, allocations);
    }

    for(auto& allocations : small_allocation_groups)
    {
        DeallocateSomeBuffers(log, set, deallocator, allocations);
    }

    CreateOutput(log, dump_filename);
    return EXIT_SUCCESS;
}

template<size_t N>
bool AllocateBuffers(std::wostream& log, std::function<void*(size_t size)> const& allocator, std::array<void*, N>& small_allocations, char& fill_value, size_t allocation_size, size_t const increase_amount, char const* type, ResultSet& set)
{
    for (auto& virtual_allocation : small_allocations)
    {
        virtual_allocation = allocator(allocation_size);
        log << type << " allocation [0x" << virtual_allocation <<  " / " << reinterpret_cast<uint64_t>(virtual_allocation) << "] fill value [" << fill_value << "]  buffer size [0x" << std::hex << allocation_size << std::dec << " | " << allocation_size << "]\n";
        if(virtual_allocation == nullptr)
        {
            log << "Failed to allocate " << type << " allocation\n";
            return false;
        }
        memset(virtual_allocation, fill_value, allocation_size);

        set.allocations.emplace_back(Allocation{reinterpret_cast<uint64_t>(virtual_allocation), allocation_size, fill_value, true});

        ++fill_value;
        allocation_size += increase_amount;
    }
    return true;
}

template<size_t N>
void DeallocateSomeBuffers(std::wostream& log, ResultSet& set, std::function<void(void*)> const& deallocator, std::array<void*, N>& allocations)
{
    DeallocateSomeBuffer(log, set, deallocator, allocations[1]);
    DeallocateSomeBuffer(log, set, deallocator, allocations[3]);
}

void DeallocateSomeBuffer(std::wostream& log, ResultSet& set, std::function<void(void*)> const& deallocator, void* allocation)
{
    deallocator(allocation);
    log << "deallocate [0x" << allocation <<  " / " << reinterpret_cast<uint64_t>(allocation) << "]\n";
    FreeAllocationInResultSet(set, allocation);
}

void FreeAllocationInResultSet(ResultSet& set, void* allocation)
{
    if(auto const it = std::ranges::find_if(set.allocations, [allocation](Allocation const& a) { return a.allocated && a.pointer == reinterpret_cast<uint64_t>(allocation); });
        it != set.allocations.end())
    {
        it->allocated = false;
    }
}

void CreateOutput(std::wostream& log, std::wstring const& free_dump_filename)
{
    if(free_dump_filename.empty())
    {
        std::cout << "press enter to exit\n";
        [[maybe_unused]] auto const ch2 = getchar();
    }
    else
    {
        GenerateDumpFile(log, free_dump_filename);
    }
}

void GenerateDumpFile(std::wostream& log, std::wstring const& dump_filename)
{
    std::wostringstream ss;

    // ReSharper disable once StringLiteralTypo
    ss << "procdump -ma " << GetCurrentProcessId() << " \"" << dump_filename << "\"";
    auto const command = std::move(ss).str();


    SECURITY_ATTRIBUTES security_attributes;

    // Set the bInheritHandle flag so pipe handles are inherited. 
    security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    security_attributes.bInheritHandle = TRUE;
    security_attributes.lpSecurityDescriptor = nullptr;

    // Create a pipe for the child process's STDOUT.
    HANDLE out_write_pipe_handle{nullptr};
    HANDLE out_read_pipe_handle{nullptr};
    if (!CreatePipe(&out_read_pipe_handle, &out_write_pipe_handle, &security_attributes, 0))
    {
        log << "Failed to create pipe : 0x" << std::hex << GetLastError() << std::dec << '\n';
        return;
    }

    auto const out_write_handle = make_handle(out_write_pipe_handle, CloseHandle);
    auto const out_read_handle = make_handle(out_read_pipe_handle, CloseHandle);

    STARTUPINFOW startup_info;
    PROCESS_INFORMATION process_info;
    memset(&startup_info, 0, sizeof(startup_info));
    memset(&process_info, 0, sizeof(process_info));
    startup_info.cb = sizeof(startup_info);
    startup_info.hStdError = out_write_pipe_handle;
    startup_info.hStdOutput = out_write_pipe_handle;
    startup_info.hStdInput = out_read_pipe_handle;
    startup_info.dwFlags |= STARTF_USESTDHANDLES;

    if(command.size() >= MAX_PATH)
    {
        log << "Command line [" << command << "] to long : " << command.size() << " >= " << MAX_PATH << '\n';
        return;
    }

    wchar_t temp[MAX_PATH];
    wcsncpy_s(temp, command.c_str(), command.size());
    temp[command.size()] = 0;

    if (!CreateProcessW(nullptr, temp, nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info))
    {
        log << "CreateProcessW [" << command << "] failed : 0x" << std::hex << GetLastError() << std::dec << '\n';
        return;
    }

    //Close thread handle. 
    CloseHandle(process_info.hThread);

    // Wait for dump to complete...
    WaitForSingleObject(process_info.hProcess, INFINITE);

    // Close process handle.
    CloseHandle(process_info.hProcess);

    log << "Dump file created: " << dump_filename << '\n';
}
